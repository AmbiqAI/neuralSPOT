#!/usr/bin/env python
"""Refactored neuralSPOT autodeploy driver
========================================
A fully compatible rewrite of `tools/ns_autodeploy.py` with identical public
behaviour and side‑effects.  All heavy‑lifting has been extracted into helper
classes so that the control‑flow is explicit, easy to read and unit‑testable.
"""
from __future__ import annotations

import logging as log
import os
import warnings
from pathlib import Path
from time import sleep
import shutil
from typing import Callable, List
import importlib

import numpy as np
import argparse
import yaml
from pydantic import BaseModel, Field

# Suppress Pydantic warnings about protected namespace conflicts
warnings.filterwarnings("ignore", message="Field.*has conflict with protected namespace.*")

# ---------------------------------------------------------------------------
# Optional HeliosAOT import --------------------------------------------------
# ---------------------------------------------------------------------------
HeliosConvertArgs = None  # type: ignore  # populated only if import succeeds
AotModel = None  # type: ignore
try:
    import helios_aot  # noqa: F401  – side‑effect import for pkg resources
    from helios_aot.defines import ConvertArgs as HeliosConvertArgs  # type: ignore
    from helios_aot.aot_model import AotModel  # type: ignore

    helios_aot_available = True
    print("[NS] HeliosAOT module is available")
except (ImportError, OSError, RuntimeError) as e:
    helios_aot_available = False
    # print(f"Helios AOT support is not available: {e}")
    

# External modules – behaviour must stay identical; keep import locations
from neuralspot.tools.autodeploy.gen_library import generateModelLib
from neuralspot.tools.autodeploy.measure_power import (
    generatePowerBinary,
    joulescope_power_on,
    measurePower,
)
from neuralspot.tools.autodeploy.validator import (
    ModelConfiguration,
    configModel,
    create_validation_binary,
    get_interpreter,
    getModelStats,
    getPMUStats,
    printStats,
    validateModel,
)
from neuralspot.tools.ns_utils import (
    ModelDetails,
    reset_dut,
    rpc_connect_as_client,
    get_armclang_version,
)
import neuralspot.tools.ns_platform as ns_platform

# Suppress TensorFlow logging messages
log.getLogger("tensorflow").setLevel(log.ERROR)
log.getLogger("tflite").setLevel(log.ERROR)
log.getLogger("tensorflow.lite").setLevel(log.ERROR)

# Suppress specific TensorFlow messages
warnings.filterwarnings("ignore", message=".*Created TensorFlow Lite XNNPACK delegate.*")

# More comprehensive TensorFlow logging suppression
import logging
for logger_name in ["tensorflow", "tflite", "tensorflow.lite", "tensorflow.python"]:
    logging.getLogger(logger_name).setLevel(logging.ERROR)

# Also suppress at the root logger level for TensorFlow messages
class TensorFlowFilter(logging.Filter):
    def filter(self, record):
        return not any(msg in record.getMessage() for msg in [
            "Created TensorFlow Lite XNNPACK delegate",
            "XNNPACK delegate"
        ])

# Apply the filter to the root logger
logging.getLogger().addFilter(TensorFlowFilter())

__all__ = ["main", "Params"]  # For external reuse & unit‑test import

# ---------------------------------------------------------------------------
# 0. Utility helpers ---------------------------------------------------------
# ---------------------------------------------------------------------------

def _fetch_ns_cmsis_nn(destination_rootdir: str) -> Path:
    """Clone **ns‑cmsis‑nn** into *destination_rootdir* if missing.

    Parameters
    ----------
    destination_rootdir: str
        The directory where the repository should be cloned (typically
        *Params.destination_rootdir*).

    Returns
    -------
    Path
        The path to the local *ns‑cmsis‑nn* checkout.
    """
    ns_cmsis_nn_path = Path(destination_rootdir) / "ns-cmsis-nn"
    if ns_cmsis_nn_path.exists():
        print(f"[NS] ns-cmsis-nn repository already exists at {ns_cmsis_nn_path}")
        return ns_cmsis_nn_path

    print(f"[NS] Cloning ns-cmsis-nn into {ns_cmsis_nn_path}…")
    result = subprocess.run(
        [
            "git",
            "clone",
            "--depth",
            "1",
            "https://github.com/AmbiqAI/ns-cmsis-nn.git",
            str(ns_cmsis_nn_path),
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        log.error("Failed to clone ns-cmsis-nn: %s", result.stderr.strip())
        raise RuntimeError("ns-cmsis-nn clone failed")
    return ns_cmsis_nn_path


def _load_yaml_config(path: str | Path | None) -> dict:
    """Load a YAML file if provided, else return an empty dict."""
    if not path:
        return {}
    with open(path, "r", encoding="utf-8") as handle:
        return yaml.safe_load(handle) or {}


# ---------------------------------------------------------------------------
# 1. Declarative configuration model (unchanged)
# ---------------------------------------------------------------------------
class Params(BaseModel):
    """Exact copy of the original `Params` model (pydantic v1)."""

    # ------------------------------------------------------------------
    #   General Parameters
    # ------------------------------------------------------------------
    seed: int = Field(42, description="Random Seed")
    platform: str = Field(
        "apollo510_evb",
        description="Platform to deploy model on (e.g. apollo4p_evb, apollo510_evb...)",
    )
    toolchain: str = Field("gcc", description="Compiler to use (supported: gcc, arm)")

    # ------------------------------------------------------------------
    #   Stage Selection Flags
    # ------------------------------------------------------------------
    create_binary: bool = Field(
        True,
        description="Create a neuralSPOT Validation EVB image based on TFlite file",
    )
    create_profile: bool = Field(True, description="Profile the performance of the model on the EVB")
    create_library: bool = Field(False, description="Create minimal static library based on TFlite file")
    create_ambiqsuite_example: bool = Field(
        False, description="Create AmbiqSuite example based on TFlite file"
    )

    if helios_aot_available:
        create_aot_profile: bool = Field(
            False,
            description="Add a Helios AOT profiling and benchmarking pass",
        )

        helios_aot_config: str = Field(
            "auto",
            description="Helios AOT configuration YAML file (or 'auto')",
        ) 

    joulescope: bool = Field(
        False,
        description="Measure power consumption of the model on the EVB using Joulescope",
    )
    onboard_perf: bool = Field(False, description="Capture and print performance measurements on EVB")
    full_pmu_capture: bool = Field(
        False,
        description="Capture full PMU data during performance measurements on EVB",
    )

    # ------------------------------------------------------------------
    #   General Configuration
    # ------------------------------------------------------------------
    tflite_filename: str = Field("undefined", description="Name of tflite model to be analyzed")
    configfile: str = Field("", description="Optional configuration file for parameters")
    pmu_config_file: str = Field(
        "default",
        description="M55 PMU configuration override file for peformance profiling",
    )

    model_location: str = Field(
        "auto", description="Where the model is stored on the EVB (Auto, TCM, SRAM, MRAM, or PSRAM)"
    )
    tflm_location: str = Field(
        "auto", description="Where the TFLM library is stored on the EVB (auto, MRAM, or ITCM (M55 only))"
    )
    arena_location: str = Field(
        "auto", description="Where the arena is stored on the EVB (auto, TCM, SRAM, or PSRAM)"
    )

    max_arena_size: int = Field(0, description="Maximum KB to be allocated for TF arena, 0 for auto")
    arena_size_scratch_buffer_padding: int = Field(
        0,
        description="(TFLM Workaround) Padding to be added to arena size to account for scratch buffer (in KB)",
    )

    resource_variable_count: int = Field(
        0,
        description="Maximum ResourceVariables needed by model (typically used by RNNs)",
    )

    # ------------------------------------------------------------------
    #   Validation Parameters
    # ------------------------------------------------------------------
    random_data: bool = Field(True, description="Use random input tensor data")
    dataset: str = Field("dataset.pkl", description="Name of dataset if --random_data is not set")
    runs: int = Field(10, description="Number of inferences to run for characterization")
    runs_power: int = Field(200, description="Number of inferences to run for power measurement")

    # ------------------------------------------------------------------
    #   Joulescope & Onboard Perf
    # ------------------------------------------------------------------
    cpu_mode: str = Field(
        "auto",
        description="CPU Mode for joulescope and onboard_perf modes - can be auto, LP (low power), or HP (high performance)",
    )

    # ------------------------------------------------------------------
    #   Library / Example Generation
    # ------------------------------------------------------------------
    model_name: str = Field(
        "auto", description="Name of model to be used in generated library, 'auto' to use TFLite filename base"
    )
    destination_rootdir: str = Field(
        "auto",
        description="Directory where generated library will be placed, 'auto' to place in neuralSPOT/projects/autodeploy",
    )
    neuralspot_rootdir: str = Field(
        "auto", description="Path to root neuralSPOT directory, 'auto' to autodetect if run within neuralSPOT"
    )

    resultlog_file: str = Field(
        "none", description="Path and Filename to store running log results. If none, result is not recorded."
    )
    profile_results_path: str = Field(
        "none",
        description="Path to store per-model profile results in addition to the file in the working directory. If none, the additional result file is not generated.",
    )

    # ------------------------------------------------------------------
    #   Platform Parameters
    # ------------------------------------------------------------------
    ambiqsuite_version: str = Field(
        "auto", description="AmbiqSuite version used to generate minimal example, 'auto' for latest"
    )
    tensorflow_version: str = Field(
        "auto", description="Tensorflow version used to generate minimal example, 'auto' for latest"
    )

    # ------------------------------------------------------------------
    #   Profile Parameters
    # ------------------------------------------------------------------
    profile_warmup: int = Field(1, description="How many inferences to profile")

    # ------------------------------------------------------------------
    #   Logging / Misc
    # ------------------------------------------------------------------
    verbosity: int = Field(1, description="Verbosity level (0-4)")
    nocompile_mode: bool = Field(False, description="Prevents compile and flash, meant for GDB debug")
    run_log_id: str = Field("none", description="Run ID for the run log. If none, no ID is included in report.")

    # ------------------------------------------------------------------
    #   RPC & Transport
    # ------------------------------------------------------------------
    transport: str = Field("auto", description="RPC transport, 'auto' for autodetect. Can set to USB or UART.")
    tty: str = Field("auto", description="Serial device, 'auto' for autodetect")
    baud: str = Field("auto", description="Baud rate, 'auto' for autodetect")

    # ------------------------------------------------------------------
    #   Convenience helpers
    # ------------------------------------------------------------------
    @classmethod
    def parser(cls):
        """
        Return an `argparse.ArgumentParser` with a **parse_typed_args()** helper
        so that the public API (and the test‑suite) stay identical.
        """
        class _TypedParser(argparse.ArgumentParser):
            def parse_typed_args(self, args=None) -> "Params":          # noqa: D401
                ns = super().parse_args(args=args)
                return cls.model_validate(vars(ns))                     # ← Pydantic v2

        p = _TypedParser(
            prog="Evaluate TFLite model against EVB instantiation",
            description="Evaluate TFLite model",
        )

        # Auto‑generate flags from the model fields
        for name, field in cls.model_fields.items():                    # v2 API
            arg = f"--{name.replace('_', '-')}"
            help_txt = field.description or ""

            # Boolean flags – support `--flag` / `--no-flag`
            if field.annotation is bool:
                if field.default is True:
                    p.add_argument(
                        f"--no-{name.replace('_', '-')}",
                        dest=name,
                        action="store_false",
                        default=argparse.SUPPRESS,
                        help=f"Disable {help_txt}" if help_txt else argparse.SUPPRESS,
                    )
                else:   # default == False
                    p.add_argument(
                        arg,
                        dest=name,
                        action="store_true",
                        default=argparse.SUPPRESS,
                        help=help_txt,
                    )
                continue

            # Non‑bools – let Pydantic handle coercion, so keep the raw string
            p.add_argument(arg, default=argparse.SUPPRESS, help=help_txt)

        return p

# ---------------------------------------------------------------------------
# 2. Pure helper functions
# ---------------------------------------------------------------------------

def _load_yaml_config(path: str | Path | None) -> dict:
    """Load a YAML file if provided, else return an empty dict."""
    if not path:
        return {}
    with open(path, "r", encoding="utf-8") as handle:
        return yaml.safe_load(handle) or {}


def _merge_params(cli: Params) -> Params:
    """Merge CLI params with YAML config and sensible defaults (matches legacy)."""
    yaml_params = _load_yaml_config(cli.configfile)
    default_params = Params()  # defaults

    merged = default_params.model_dump()
    merged.update(yaml_params)
    # CLI wins last
    merged.update(cli.model_dump(exclude_unset=True))
    return Params(**merged)


def _setup_logging(verbosity: int) -> None:
    """Configure root logger mapping legacy verbosity levels."""
    level = log.DEBUG if verbosity > 2 else log.INFO if verbosity > 1 else log.WARNING
    log.basicConfig(level=level, format="%(levelname)s: %(message)s")


# ---------------------------------------------------------------------------
# Results class – encapsulates all profiling results
# ---------------------------------------------------------------------------

class adResults:
    def __init__(self, p) -> None:
        self.profileTotalInferenceTime = 0
        self.profileTotalEstimatedMacs = 0
        self.profileTotalCycles = 0
        self.profileTotalLayers = 0
        self.powerMaxPerfInferenceTime = 0
        self.powerMinPerfInferenceTime = 0
        self.powerMaxPerfJoules = 0
        self.powerMinPerfJoules = 0
        self.powerMaxPerfWatts = 0
        self.powerMinPerfWatts = 0
        self.powerAotMaxPerfInferenceTime = 0
        self.powerAotMinPerfInferenceTime = 0
        self.powerAotMaxPerfJoules = 0
        self.powerAotMinPerfJoules = 0
        self.powerAotMaxPerfWatts = 0
        self.powerAotMinPerfWatts = 0
        self.powerIterations = p.runs_power
        # self.stats_filename = p.stats_filename
        self.model_name = p.model_name
        self.p = p
        self.toolchain = p.toolchain
        self.model_size = 0
        self.arena_size = 0

        # HeliosAOT extras --------------------------------------------------
        self.aot_success: bool = False
        self.aot_module_path: Optional[str] = None
        self.aot_error: Optional[str] = None

    def print(self):
        print("")
        print((f"Characterization Report for {self.model_name}:"))
        print(
            f"[Profile] Per-Layer Statistics file:         {self.model_name}_stats.csv"
        )
        print(
            f"[Profile] Model Size:                        {self.model_size} KB"
        )
        print(
            f"[Profile] Arena Size:                        {self.arena_size} KB")
        print(
            f"[Profile] Total Estimated MACs:              {self.profileTotalEstimatedMacs}"
        )
        print(f"[Profile] Total Model Layers:                {self.profileTotalLayers}")

            # print(f"[Profile] Total CPU Cycles:                  {self.profileTotalCycles}")
        # print(
        #     f"[Profile] Cycles per MAC:                    {(self.profileTotalCycles/self.profileTotalEstimatedMacs):0.3f}"
        # )
        if self.p.joulescope or self.p.onboard_perf:
            print(
                f"[Power]   HP Inference Time (ms):      {self.powerMaxPerfInferenceTime:0.3f}"
            )
            print(
                f"[Power]   HP Inference Energy (uJ):    {self.powerMaxPerfJoules:0.3f}"
            )
            print(
                f"[Power]   HP Inference Avg Power (mW): {self.powerMaxPerfWatts:0.3f}"
            )
            print(
                f"[Power]   LP Inference Time (ms):      {self.powerMinPerfInferenceTime:0.3f}"
            )
            print(
                f"[Power]   LP Inference Energy (uJ):    {self.powerMinPerfJoules:0.3f}"
            )
            print(
                f"[Power]   LP Inference Avg Power (mW): {self.powerMinPerfWatts:0.3f}"
            )
            if self.p.create_aot_profile:
                # Both HP and LP are measured
                print(
                    f"[Power]   AOT HP Inference Time (ms):      {self.powerAotMaxPerfInferenceTime:0.3f}"
                )
                print(
                    f"[Power]   AOT HP Inference Energy (uJ):    {self.powerAotMaxPerfJoules:0.3f}"
                )
                print(
                    f"[Power]   AOT HP Inference Avg Power (mW): {self.powerAotMaxPerfWatts:0.3f}"
                )
                print(
                    f"[Power]   AOT LP Inference Time (ms):      {self.powerAotMinPerfInferenceTime:0.3f}"
                )
                print(
                    f"[Power]   AOT LP Inference Energy (uJ):    {self.powerAotMinPerfJoules:0.3f}"
                )
                print(
                    f"[Power]   AOT LP Inference Avg Power (mW): {self.powerAotMinPerfWatts:0.3f}"
                )
        print(
            f"""
Notes:
        - Statistics marked with [Profile] are collected from the first inference, whereas [Power] statistics
          are collected from the average of the {self.powerIterations} inferences. This will lead to slight
          differences due to cache warmup, etc.
        - CPU cycles are captured via Arm ETM traces
        - MACs are estimated based on the number of operations in the model, not via instrumented code
"""
        )
        log.info(
            f"{self.model_name},LP,{self.profileTotalEstimatedMacs},{self.profileTotalCycles},{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts}"
        )
        log.info(
            f"{self.model_name},HP,{self.profileTotalEstimatedMacs},{self.profileTotalCycles},{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts}"
        )

        # if params.resultlog_file is specified write the results to the file
        # if the file doesn't exist, create it
        # if it does exist, add to the file
        # The contents of the file are CSV formatted, and the header is
        # Model Filename, Platform,	Compiler, TF Verssion, Model Size (KB), Arena Size (KB), Model Location, Arena Location, Est MACs, HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW), AS Version, Date Run
        
        # Given p.toolchain, find the version of the compiler
        if self.toolchain == "gcc":
            # Format for gcc --version is "arm-none-eabi-gcc (Arm GNU Toolchain 13.2.rel1 (Build arm-13.7)) 13.2.1 20231009"
            compiler_version = os.popen("arm-none-eabi-gcc --version").read().split(" ")[4]
        elif self.toolchain == "arm":
            compiler_version = "armclang " + get_armclang_version()
        # print (f"[NS] Compiler Version: {compiler_version}")
        # get today's date
        from datetime import date
        today = date.today()
        # dd/mm/YY
        d1 = today.strftime("%d/%m/%Y")

        if self.p.resultlog_file != "none":
            if not os.path.exists(self.p.resultlog_file):
                # Create AOT columns if aot is enabled
                aot_columns = ""
                if self.p.create_aot_profile:
                    aot_columns = ", AOT HP(ms), AOT HP(uJ), AOT HP(mW), AOT LP(ms), AOT LP(uJ), AOT LP(mW)"
                with open(self.p.resultlog_file, "w") as f:
                    f.write(
                        f"Model Filename, Platform, Compiler, TF Version, Model Size (KB), Arena Size (KB), Model Location, Arena Location, Est MACs, HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW), AS Version, Date Run, ID{aot_columns}\n"
                    )
            with open(self.p.resultlog_file, "a") as f:
                f.write(
                    f"{self.model_name},{self.p.platform},{self.p.toolchain} {compiler_version},{self.p.tensorflow_version},{self.model_size},{self.arena_size},{self.p.model_location},{self.p.arena_location},{self.profileTotalEstimatedMacs},{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts},{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts},{self.powerAotMaxPerfInferenceTime},{self.powerAotMaxPerfJoules},{self.powerAotMaxPerfWatts},{self.powerAotMinPerfInferenceTime},{self.powerAotMinPerfJoules},{self.powerAotMinPerfWatts},{self.p.ambiqsuite_version},{d1},{self.p.run_log_id}\n"
                )

    def setProfile(
        self,
        profileTotalInferenceTime,
        profileTotalEstimatedMacs,
        profileTotalCycles,
        profileTotalLayers,
    ):
        self.profileTotalInferenceTime = profileTotalInferenceTime
        self.profileTotalEstimatedMacs = profileTotalEstimatedMacs
        self.profileTotalCycles = profileTotalCycles
        self.profileTotalLayers = profileTotalLayers

    def setPower(self, cpu_mode, mSeconds, uJoules, mWatts, aot=False):
        if aot:
            if cpu_mode == "LP":
                self.powerAotMinPerfInferenceTime = mSeconds
                self.powerAotMinPerfJoules = uJoules
                self.powerAotMinPerfWatts = mWatts
            else:
                self.powerAotMaxPerfInferenceTime = mSeconds
                self.powerAotMaxPerfJoules = uJoules
                self.powerAotMaxPerfWatts = mWatts
        else:
            if cpu_mode == "LP":
                self.powerMinPerfInferenceTime = mSeconds
                self.powerMinPerfJoules = uJoules
                self.powerMinPerfWatts = mWatts
            else:
                self.powerMaxPerfInferenceTime = mSeconds
                self.powerMaxPerfJoules = uJoules
                self.powerMaxPerfWatts = mWatts

    def setModelSize(self, model_size):
        self.model_size = model_size

    def setArenaSize(self, arena_size):
        self.arena_size = arena_size

    def setAot(self, success: bool, module_path: Optional[str] = None, error: Optional[str] = None):
        self.aot_success = success
        self.aot_module_path = module_path
        self.aot_error = error

# ---------------------------------------------------------------------------
# 3. Core runner class – encapsulates *all* mutating state
# ---------------------------------------------------------------------------
class AutoDeployRunner:
    """Stateful orchestration – each stage is an *explicit* method."""

    def __init__(self, params: Params):
        self.p = params
        np.random.seed(self.p.seed)
        self.platform_cfg = ns_platform.AmbiqPlatform(self.p)
        self._total_stages = self._count_enabled_stages()
        self._stage = 1

        # Derived fields mutated during execution – keep parity with legacy
        self.stash_arena_location: str | None = None
        self.move_model_back_to_sram: bool = False
        self.results = None  # set later when adResults is constructed (unchanged behaviour)

    # ------------------------------------------------------------------
    #   Top‑level control‑flow (identical to legacy order)
    # ------------------------------------------------------------------
    def run(self) -> None:  # noqa: C901 – high complexity mirrors legacy spec
        """Execute the enabled stages in the same order as the original script."""
        self._prepare_environment()

        if self.p.create_binary:
            self._create_and_finetune_binary()

        if self.p.create_profile:
            self._characterize_model()

        if not self.p.create_binary and not self.p.create_profile:
            self._load_pickled_artifacts()

        if self.p.create_aot_profile and not self.p.create_profile:
            self._generate_helios_aot()

        if self.p.joulescope or self.p.onboard_perf:
            self._characterize_power_or_onboard_perf()

        if self.p.create_library:
            self._generate_library()

        if self.p.create_ambiqsuite_example:
            self._generate_ambiqsuite_example()

        # Final report is produced by adResults – identical behaviour
        # Get the arena size from the platform config
        self.results.setArenaSize(self.mc.arena_size_k)
        self.results.print()

    # ------------------------------------------------------------------
    #   Private helpers – all side‑effects are delegated to extracted funcs
    # ------------------------------------------------------------------
    def _prepare_environment(self) -> None:
        """Replicates *all* legacy pre‑flight logic: paths, auto‑detects etc."""
        # --- logging first so subsequent helpers can emit output ----------
        _setup_logging(self.p.verbosity)

        # --- Root directory autodetect -----------------------------------
        if self.p.neuralspot_rootdir == "auto":
            root = Path.cwd().resolve()
            while root != root.root:
                if (root / "neuralspot" / "ns-core" / "module.mk").exists():
                    self.p.neuralspot_rootdir = str(root)
                    break
                root = root.parent
            else:
                log.error("NeuralSPOT root directory not found. Please specify a valid path.")
                raise SystemExit("Autodeploy failed")
            print(f"[NS] NeuralSPOT root directory found at {self.p.neuralspot_rootdir}")

        # --- Destination directory ---------------------------------------
        if self.p.destination_rootdir == "auto":
            projects_autodeploy = Path(self.p.neuralspot_rootdir) / "projects" / "autodeploy"
            projects_autodeploy.mkdir(parents=True, exist_ok=True)
            self.p.destination_rootdir = str(projects_autodeploy)
            print(f"[NS] Destination Root Directory automatically set to: {self.p.destination_rootdir}")

        # --- Model name auto ---------------------------------------------
        if self.p.model_name == "auto":
            self.p.model_name = Path(self.p.tflite_filename).stem.replace("-", "_")
            print(f"[NS] Model Name automatically set to: {self.p.model_name}")

        # --- Canonicalise absolute paths ---------------------------------
        for attr in ("destination_rootdir", "neuralspot_rootdir"):
            value = getattr(self.p, attr)
            if not os.path.isabs(value):
                setattr(self.p, attr, os.path.abspath(value))

        # --- Mutually exclusive flags ------------------------------------
        if self.p.onboard_perf and self.p.joulescope:
            raise ValueError("Cannot run onboard performance and Joulescope at the same time")
        if self.p.tflite_filename == "undefined":
            raise ValueError("TFLite filename must be specified")

        # --- If aot_config is auto, set it to tools/base_aot.yaml
        if self.p.helios_aot_config == "auto":
            self.p.helios_aot_config = str(importlib.resources.files(__name__) / "base_aot.yaml")

        # --- Stage count for pretty progress -----------------------------
        print(f"[NS] Running {self._total_stages} Stage Autodeploy for Platform: {self.p.platform}")

        # --- Joulescope sanity check -------------------------------------
        if self.p.joulescope and not joulescope_power_on():
            raise SystemExit("Autodeploy failed – Joulescope power on failed")

        # --- Arena sizing decisions (delegates to platform cfg) ----------
        self._apply_memory_policy()

        # --- Interpreter & model configuration objects -------------------
        interpreter = get_interpreter(self.p)
        self.mc = ModelConfiguration(self.p)
        self.md = ModelDetails(interpreter)

        self.results = adResults(self.p)
        self.results.setModelSize(self.model_size)

        # Create pickle paths once (used by several stages)
        self._pkl_dir = Path(self.p.destination_rootdir) / self.p.model_name
        self._pkl_dir.mkdir(parents=True, exist_ok=True)
        self._mc_pkl = self._pkl_dir / f"{self.p.model_name}_mc.pkl"
        self._md_pkl = self._pkl_dir / f"{self.p.model_name}_md.pkl"
        self._results_pkl = self._pkl_dir / f"{self.p.model_name}_results.pkl"

    # ------------------------------------------------------------------
    def _apply_memory_policy(self) -> None:
        """Replicates the *exact* heuristics from the legacy script."""
        pc = self.platform_cfg  # alias – matches legacy var name
        self.model_size = int(os.path.getsize(self.p.tflite_filename) / 1024 + 0.999)

        # Arena max size auto‑fill
        if self.p.max_arena_size == 0:
            if self.p.arena_location == "PSRAM":
                self.p.max_arena_size = pc.GetMaxPsramArenaSize()
            else:
                self.p.max_arena_size = pc.GetMaxArenaSize()
            print(f"[NS] Max {self.p.arena_location if self.p.arena_location != 'auto' else 'SRAM'} Arena Size for {self.p.platform}: {self.p.max_arena_size} KB")
        else:
            pc.CheckArenaSize(self.p.max_arena_size, self.p.arena_location)

        # Model location auto decision (+ SRAM temporary move logic)
        if self.p.model_location == "auto":
            self.p.model_location = pc.GetModelLocation(self.model_size, "auto")
            print(f"[NS] Best {self.model_size}KB model location for {self.p.platform}: {self.p.model_location}")
        elif self.p.create_binary and self.p.model_location == "SRAM":
            print("[NS] Model location set to MRAM for first pass, will be moved to SRAM after first pass")
            self.p.model_location = "MRAM"
            self.move_model_back_to_sram = True

        # Auto‑fill AS/TF versions
        if self.p.ambiqsuite_version == "auto":
            self.p.ambiqsuite_version = pc.platform_config["as_version"]
            print(f"[NS] Using AmbiqSuite Version: {self.p.ambiqsuite_version}")
        if self.p.tensorflow_version == "auto":
            self.p.tensorflow_version = pc.platform_config["tflm_version"]
            print(f"[NS] Using TensorFlow Version: {self.p.tensorflow_version}")

        # Transport default USB vs UART
        if self.p.transport == "auto":
            self.p.transport = "USB" if pc.GetSupportsUsb() else "UART"
        log.info(f"[NS] Using transport: {self.p.transport}")

        # Large model edge‑case (needs MRAM even if user requested PSRAM)
        if pc.GetModelLocation(self.model_size, "auto") == "PSRAM":
            print("[NS WARNING] Model is too large for performance or example generation – disabling those stages")
            self.p.joulescope = self.p.onboard_perf = self.p.create_ambiqsuite_example = self.p.create_library = False

    # ------------------------------------------------------------------
    #   Stage helpers (names map 1‑to‑1 with original comments)
    # ------------------------------------------------------------------
    def _create_and_finetune_binary(self) -> None:  # Stage 1
        print(f"[NS] *** Stage [{self._stage}/{self._total_stages}]: Create and fine‑tune EVB model characterization image")
        self._stage += 1
        # Logic identical – delegate to original functions
        stash_arena_location = self.p.arena_location

        # First pass arena location adjustment
        if self.p.arena_location != "PSRAM":
            self.p.arena_location = "SRAM"

        if not self.p.nocompile_mode:
            create_validation_binary(self.p, self.mc, baseline=True, aot=False)
        client = rpc_connect_as_client(self.p)
        configModel(self.p, client, self.md)
        stats = getModelStats(self.p, client)
        self.mc.update_from_stats(stats, self.md)

        # Second pass (tuned arena / model loc)
        self.p.arena_location = stash_arena_location
        if self.p.arena_location == "auto":
            self.p.arena_location = self.platform_cfg.GetArenaLocation(self.mc.arena_size_k, "auto")
        arena_size = self.mc.arena_size_k + self.p.arena_size_scratch_buffer_padding
        self.platform_cfg.CheckArenaSize(arena_size, self.p.arena_location)

        if self.move_model_back_to_sram:
            self.p.model_location = "SRAM"
            print("[NS] Model location set to SRAM for profiling pass")

        if self.p.model_location == "TCM" and self.p.arena_location == "TCM":
            if self.model_size + self.mc.arena_size_k > self.platform_cfg.GetDTCMSize():
                print("[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                self.p.model_location = "MRAM"

        # Persist pickles
        for path, obj in ((self._mc_pkl, self.mc), (self._md_pkl, self.md)):
            with open(path, "wb") as fh:
                import pickle
                pickle.dump(obj, fh)

    # ------------------------------------------------------------------
    def _characterize_model(self) -> None:  # Stage 2
        print(f"\n[NS] *** Stage [{self._stage}/{self._total_stages}]: Characterize model performance on EVB")
        self._stage += 1
        import pickle

        if not self.p.create_binary:  # load pickled metadata from previous runs
            with open(self._mc_pkl, "rb") as fh:
                self.mc = pickle.load(fh)
            with open(self._md_pkl, "rb") as fh:
                self.md = pickle.load(fh)

        # Ensure arena/model loc finalised
        if self.p.arena_location == "auto":
            self.p.arena_location = self.platform_cfg.GetArenaLocation(self.mc.arena_size_k, "auto")
        if self.p.model_location == "TCM" and self.p.arena_location == "TCM":
            if self.model_size + self.mc.arena_size_k > self.platform_cfg.GetDTCMSize():
                print("[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                self.p.model_location = "MRAM"

        if not self.p.nocompile_mode:
            create_validation_binary(self.p, self.mc, baseline=False, aot=False)
        client = rpc_connect_as_client(self.p)
        configModel(self.p, client, self.md)

        differences = validateModel(self.p, client, get_interpreter(self.p), self.md, self.mc)
        stats = getModelStats(self.p, client)
        # pretty-print the differences
        log.info("Model Output Comparison: Mean difference per output label in tensor(0): %s", repr(np.array(differences).mean(axis=0)))
        stats_file_base = Path(self.p.destination_rootdir) / self.p.model_name / f"{self.p.model_name}_stats"
        pmu_csv_header = ""
        overall_pmu_stats: List[List[int]] = []
        if self.p.full_pmu_capture:
            events_per_layer = stats[3]
            layers = stats[5]
            for layer in range(layers):
                csv_header, pmu_stats = getPMUStats(self.p, client, layer, events_per_layer)
                pmu_csv_header = csv_header  # keep header once
                overall_pmu_stats.append(pmu_stats)

        cycles, macs, time_us, layers, events_per_layer = printStats(
            self.p,
            self.mc,
            stats,
            str(stats_file_base),
            pmu_csv_header,
            overall_pmu_stats,
        )
        self.results.setProfile(time_us, macs, cycles, layers)
        # ----  Run an AOT profiling pass (optional) ----------------------
        if self.p.create_aot_profile:
            if not self.p.nocompile_mode:
                self._generate_helios_aot()
                create_validation_binary(self.p, self.mc, baseline=False, aot=True)

            client = rpc_connect_as_client(self.p)
            configModel(self.p, client, self.md)

            differences_aot = validateModel(self.p, client, get_interpreter(self.p),
                              self.md, self.mc)
            stats_aot = getModelStats(self.p, client)
            
            # pretty-print the differences  
            log.info("Model Output Comparison: Mean difference per output label in tensor(0): %s", repr(np.array(differences_aot).mean(axis=0)))
            
            # Compare TFLM to AOT differences - they should be identical. If they're not, print a warning and the differences
            if not np.array_equal(differences, differences_aot):
                log.warning("Model Output Comparison: TFLM and AOT differences are not identical. TFLM differences: %s, AOT differences: %s", repr(np.array(differences).mean(axis=0)), repr(np.array(differences_aot).mean(axis=0)))
            else:
                print("[NS] AOT/TFLM output tensor comparison: Identical")

            printStats(self.p,           # prints to console / log
                       self.mc,
                       stats_aot,
                       str(stats_file_base) + "_aot",
                       pmu_csv_header,
                       overall_pmu_stats)
            

        # Write updated pickles + result summary
        with open(self._mc_pkl, "wb") as fh:
            pickle.dump(self.mc, fh)
        with open(self._results_pkl, "wb") as fh:
            pickle.dump(self.results, fh)

        # Also log tensor diffs (legacy behaviour – info level)
        for idx, tensor_diffs in enumerate(differences):
            log.info(
                "Model Output Comparison: Mean difference per output label in tensor(%d): %s",
                idx,
                repr(np.array(tensor_diffs).mean(axis=0)),
            )

    def _generate_helios_aot(self) -> None:
        print(f"[NS] Generate HeliosAOT source code")
        # erase any existing AOT source code
        aot_dir = Path(self.p.destination_rootdir) / self.p.model_name / (self.p.model_name +"_aot")
        if aot_dir.exists():
            shutil.rmtree(aot_dir)

        try:
            # Ensure supporting libraries are present --------------------
            _fetch_ns_cmsis_nn(self.p.destination_rootdir)

            # Determine configuration path ------------------------------
            cfg_path = self.p.helios_aot_config
            if cfg_path == "auto":
                cfg_path = os.path.splitext(self.p.tflite_filename)[0] + ".yaml"
                if not Path(cfg_path).exists():
                    raise FileNotFoundError(
                        "HeliosAOT config file not provided and 'auto' path does not exist."
                    )

            # Prepare ConvertArgs instance -------------------------------
            convert_args = HeliosConvertArgs(path=Path(cfg_path))  # type: ignore
            # print(convert_args)
            # Override model_path/output/module_name dynamically ---------
            convert_args.model_path = Path(self.p.tflite_filename)
            
            # Put code in same directory as validator and perf, but under its own subdirectory
            default_output = Path(self.p.destination_rootdir) / self.p.model_name / (self.p.model_name +"_aot")
            convert_args.output_path = default_output
            convert_args.module_name = f"{self.p.model_name}"
            convert_args.prefix = f"{self.p.model_name}"

            # put model and arena in specified locations
            from helios_aot.defines import OperatorAttributeRule
            convert_args.operator_attributes = [
                OperatorAttributeRule(
                    type="*",
                    attributes={"weights_memory": self.p.model_location.lower(), "scratch_memory": self.p.arena_location.lower()},
                )
            ]

            # print("new args")
            # print(convert_args)
            # Invoke HeliosAOT programmatically --------------------------
            aot_model = AotModel(config=convert_args)  # type: ignore
            aot_model.initialize()
            aot_model.convert()
            # Get the number of layers
            layers = len(aot_model.operators)
            # store it in mc
            self.mc.aot_layers = layers
            # print("[AOT] Operations:")
            # for op in aot_model.operators:
            #     print(f"  {op.ident}, {op.name}")

            self.results.setAot(True, module_path=str(convert_args.output_path.resolve()))
            print("[NS] HeliosAOT generation completed successfully")
        except Exception as exc:  # broad catch → propagate clean error message
            err_msg = str(exc)
            self.results.setAot(False, error=err_msg)
            print(f"[NS] HeliosAOT generation failed – {err_msg}")

    # ------------------------------------------------------------------
    def _load_pickled_artifacts(self) -> None:
        import pickle

        paths = (self._mc_pkl, self._md_pkl, self._results_pkl)
        for path in paths:
            if not path.exists():
                raise ValueError(f"Required pickle {path} not found – run with --create_binary or --create_profile first")
        with open(self._mc_pkl, "rb") as fh:
            self.mc = pickle.load(fh)
        with open(self._md_pkl, "rb") as fh:
            self.md = pickle.load(fh)
        with open(self._results_pkl, "rb") as fh:
            self.results = pickle.load(fh)

        if self.p.arena_location == "auto":
            self.p.arena_location = self.platform_cfg.GetArenaLocation(self.mc.arena_size_k, "auto")
        if self.p.model_location == "TCM" and self.p.arena_location == "TCM":
            if self.model_size + self.mc.arena_size_k > self.platform_cfg.GetDTCMSize():
                print("[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                self.p.model_location = "MRAM"

    # ------------------------------------------------------------------
    def _characterize_power_or_onboard_perf(self) -> None:  # Stage 3 (or 4)
        print("\n[NS] *** Stage [{} / {}]: Characterize inference {} on EVB".format(
            self._stage,
            self._total_stages,
            "energy consumption using Joulescope" if self.p.joulescope else "performance (onboard)",
        ))
        self._stage += 1

        cpu_modes = ["LP", "HP"] if self.p.cpu_mode == "auto" else [self.p.cpu_mode]
        # runtime_modes = ["tflm"]
        # if self.p.create_aot_profile:
        #     runtime_modes.append("aot")
        runtime_modes = ["aot"]
        for mode in cpu_modes:
            for runtime_mode in runtime_modes:
                generatePowerBinary(self.p, self.mc, self.md, mode, aot=runtime_mode == "aot")
                if self.p.joulescope:
                    td, i, v, p_, c, e = measurePower(self.p)
                    energy_uJ = (e["value"] / self.p.runs_power) * 1_000_000
                    time_ms = (td.total_seconds() * 1000) / self.p.runs_power
                    power_mW = (e["value"] / td.total_seconds()) * 1000
                    self.results.setPower(cpu_mode=mode, mSeconds=time_ms, uJoules=energy_uJ, mWatts=power_mW, aot=runtime_mode == "aot")
                    log.info("Model Power Measurement in %s mode: %.3f ms, %.3f uJ (avg %.3f mW)", mode, time_ms, energy_uJ, power_mW)
                else:
                    sleep(10)  # legacy delay to allow SWO capture
                    log.info("Onboard perf run in %s mode completed (see SWO output)", mode)

    # ------------------------------------------------------------------
    def _generate_library(self) -> None:  # Stage 4/5
        print(f"\n[NS] *** Stage [{self._stage}/{self._total_stages}]: Generate minimal static library")
        generateModelLib(self.p, self.mc, self.md, ambiqsuite=False)
        self._stage += 1

    # ------------------------------------------------------------------
    def _generate_ambiqsuite_example(self) -> None:  # Last stage
        print(f"\n[NS] *** Stage [{self._stage}/{self._total_stages}]: Generate AmbiqSuite Example")
        generateModelLib(self.p, self.mc, self.md, ambiqsuite=True)
        self._stage += 1

    # ------------------------------------------------------------------
    #   Misc
    # ------------------------------------------------------------------
    def _count_enabled_stages(self) -> int:
        flags = (
            self.p.create_binary,
            self.p.create_profile,
            self.p.create_library,
            # self.p.create_aot_profile,
            self.p.joulescope or self.p.onboard_perf,
            self.p.create_ambiqsuite_example,
        )
        return sum(flags)


# ---------------------------------------------------------------------------
# 4. Public entry point
# ---------------------------------------------------------------------------

def main(argv: List[str] | None = None) -> None:  # pragma: no cover – CLI
    """Entry‑point used by the shebang AND by the parity test‐suite."""
    parser = Params.parser()
    cli_params: Params = parser.parse_typed_args(args=argv)
    merged_params = _merge_params(cli_params)
    runner = AutoDeployRunner(merged_params)
    runner.run()


if __name__ == "__main__":
    main()