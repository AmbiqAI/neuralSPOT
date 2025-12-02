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
# Set multiple environment variables to ensure XNNPACK is disabled
os.environ.setdefault("TF_LITE_DISABLE_XNNPACK", "1")
os.environ["TF_LITE_DISABLE_XNNPACK"] = "1"
os.environ["TF_LITE_DISABLE_DELEGATES"] = "1"
os.environ["TF_LITE_DISABLE_DEFAULT_DELEGATES"] = "1"
os.environ["TF_LITE_DISABLE_DELEGATE_PLUGINS"] = "1"
os.environ["TF_LITE_USE_XNNPACK"] = "0"
os.environ["TF_LITE_EXPERIMENTAL_USE_XNNPACK"] = "0"
import subprocess
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
helios_aot_version = None
try:
    import helios_aot  # noqa: F401  – side‑effect import for pkg resources
    from helios_aot.cli.defines import ConvertArgs as HeliosConvertArgs  # type: ignore
    from helios_aot.converter import AotConverter as AotModel   # type: ignore
    from helios_aot.utils import get_version as get_helios_aot_version
    helios_aot_version = get_helios_aot_version()
    # Make sure the version is at least 0.5.0 (semantic versioning)
    if helios_aot_version < "0.5.0":
        print(f"[NS] HeliosAOT version is {helios_aot_version}, but must be at least 0.5.0")
        helios_aot_available = False
        print(f"[NS] HeliosAOT support is not available, version is too old")
    else:
        helios_aot_available = True
        print(f"[NS] HeliosAOT module is available, version: {get_helios_aot_version()}")
except (ImportError, OSError, RuntimeError) as e:
    helios_aot_available = False
    print(f"Helios AOT support is not available: {e}")


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

    print(f"[NS] Cloning ns-cmsis-nn into {ns_cmsis_nn_path}")
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

    # Check out specific commit (8a1ae90c9c2ea1f9389ff69b6908099899dc6286)
    # print(f"[NS] Checking out commit 8a1ae90c9c2ea1f9389ff69b6908099899dc6286")
    # result = subprocess.run(
    #     [
    #         "git",
    #         "checkout",
    #         "8a1ae90c9c2ea1f9389ff69b6908099899dc6286",
    #     ],
    #     cwd=ns_cmsis_nn_path,
    # )
    # if result.returncode != 0:
    #     log.error("Failed to checkout commit needed version: %s", result.stderr.strip())
    #     raise RuntimeError("ns-cmsis-nn checkout failed")


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
    else:
        create_aot_profile: bool = Field(
            False,
            description="NOT SUPPORTED - Helios AOT is not available",
        )

        helios_aot_config: str = Field(
            "auto",
            description="NOT SUPPORTED - Helios AOT is not available",
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
        "auto", description="Where the model is stored on the EVB (Auto, TCM, SRAM, MRAM, NVM, or PSRAM)"
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
        # ---- Memory totals (KB) --------------------------------------------
        self.tflm_rom_kb = 0
        self.tflm_ram_kb = 0
        self.aot_rom_kb = 0
        self.aot_ram_kb = 0

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

        # ---- Coalesced memory usage ----------------------------------------
        print(
            f"[Memory]  TFLM: ROM={self.tflm_rom_kb} KB, RAM={self.tflm_ram_kb} KB"
        )
        if self.p.create_aot_profile:
            print(
                f"[Memory]  AOT : ROM={self.aot_rom_kb} KB, RAM={self.aot_ram_kb} KB"
            )

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
        # Model Filename, Platform, Compiler, TF Version, Model Size (KB), Arena Size (KB),
        # Model Location, Arena Location, Est MACs,
        # HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW),
        # [AOT HP/LP columns if enabled],
        # TFLM/AOT section sizes, AS Version, Date Run

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
            print(f"[NS] Resultlog file: {self.p.resultlog_file}")
            if not os.path.exists(self.p.resultlog_file):
                # Create AOT columns if aot is enabled
                #aot_perf_columns = ""
                #if self.p.create_aot_profile:
                aot_perf_columns = ", AOT HP(ms), AOT HP(uJ), AOT HP(mW), AOT LP(ms), AOT LP(uJ), AOT LP(mW), AOT Version"
                # Memory columns (TFLM always; AOT only if enabled)
                mem_tflm_columns = (", TFLM ROM (KB), TFLM RAM (KB)")
                #mem_aot_columns = ""
                #if self.p.create_aot_profile:
                mem_aot_columns = (", AOT ROM (KB), AOT RAM (KB)")
                with open(self.p.resultlog_file, "w") as f:
                    f.write(
                        "Model Filename, Platform, Compiler, TF Version, Model Size (KB), Arena Size (KB), "
                        "Model Location, Arena Location, Est MACs, "
                        "HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW)"
                        f"{aot_perf_columns}"
                        f"{mem_tflm_columns}"
                        f"{mem_aot_columns}"
                        ", AS Version, Date Run, ID\n"                    )
            # Row assembly (match header conditions)
            print(f"[NS] Writing to resultlog file: {self.p.resultlog_file}")
            with open(self.p.resultlog_file, "a") as f:
                base_vals = (
                    f"{self.model_name},{self.p.platform},{self.p.toolchain} {compiler_version},"
                    f"{self.p.tensorflow_version},{self.model_size},{self.arena_size},"
                    f"{self.p.model_location},{self.p.arena_location},{self.profileTotalEstimatedMacs},"
                    f"{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts},"
                    f"{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts}"
                )
                aot_perf_vals = (",0,0,0,0,0,0,0")
                if self.p.create_aot_profile:
                    aot_perf_vals = (
                        f",{self.powerAotMaxPerfInferenceTime},{self.powerAotMaxPerfJoules},{self.powerAotMaxPerfWatts},"
                        f"{self.powerAotMinPerfInferenceTime},{self.powerAotMinPerfJoules},{self.powerAotMinPerfWatts},"
                        f"{helios_aot_version}"
                    )
                mem_tflm_vals = (
                    f",{self.tflm_rom_kb},{self.tflm_ram_kb}"
                )
                mem_aot_vals = ",0,0"
                if self.p.create_aot_profile:
                    mem_aot_vals = (
                        f",{self.aot_rom_kb},{self.aot_ram_kb}"
                    )
                tail = f",{self.p.ambiqsuite_version},{d1},{self.p.run_log_id}\n"
                f.write(base_vals + aot_perf_vals + mem_tflm_vals + mem_aot_vals + tail)


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

    def setAotDifferences(self, golden_output_tensors_aot, differences_np, differences_aot_np, differences):
        self.aot_golden_output_tensors_aot = golden_output_tensors_aot
        self.aot_differences_np = differences_np
        self.aot_differences_aot_np = differences_aot_np
        self.aot_differences = differences

        if self.p.profile_results_path != "none":
            # Create a time-stamped model-unique file with the differences
            import datetime
            diff_file = self.p.profile_results_path + "/aot_diffs_" + self.model_name + "_" + datetime.date.today().strftime("%Y%m%d_%H%M%S") + ".txt"
            with open(diff_file, "a") as f:
                # Print the magnitude of the differences between Golden Output Tensors, TFLM and AOT as a percentage of the Golden Output Tensors values
                # f.write("Magnitude of differences between Expected Outputs and TFLM as percentage of Expected Outputs\n")
                # f.write(f"{differences_np / golden_output_tensors_aot * 100}\n")
                # f.write("Magnitude of differences between Expected Outputs and AOT as percentage of Expected Outputs\n")
                # f.write(f"{differences_aot_np / golden_output_tensors_aot * 100}\n")

                f.write("\nDifferences between TF and TFLM\n")
                f.write(f"{differences_np}\n")
                f.write("\nDifferences between TF and AOT\n")
                f.write(f"{differences_aot_np}\n")
                f.write("\nDifferences between TFLM and AOT\n")
                f.write(f"{differences}\n")

    # ---- New: store coalesced memory totals (in KB) ------------------------
    def setMemoryTotals(self, *, aot: bool, rom_kb: int, ram_kb: int):

        if aot:
            self.aot_rom_kb = rom_kb
            self.aot_ram_kb = ram_kb
        else:
            self.tflm_rom_kb = rom_kb
            if self.p.model_location == "SRAM" or self.p.model_location == "PSRAM" or self.p.model_location == "NVM":
                print(f"[NS] Model Array location is not MRAM or TCM, reducing ROM total by {self.model_size} KB")
                self.tflm_rom_kb = self.tflm_rom_kb - self.model_size
            self.tflm_ram_kb = ram_kb

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
        self.results = None  # set later when adResults is constructed

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
            # force results aot flag to true
            self.results.p.create_aot_profile = True
            if self.p.nocompile_mode:
                client = rpc_connect_as_client(self.p)
                # print("[NS] AOT rpc_connect_as_client done")
                configModel(self.p, client, self.md)
                # print("[NS] AOT configModel done")
                differences_aot, golden_output_tensors_aot = validateModel(self.p, client, get_interpreter(self.p),
                                self.md, self.mc)

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

        # --- If create_aot_profile is True and helios_aot_available = False, set it to False
        if self.p.create_aot_profile and not helios_aot_available:
            self.p.create_aot_profile = False
            print("[WARNING] AOT disabled because HeliosAOT is not available")

        # --- If aot_config is auto, set it to tools/base_aot.yaml
        if self.p.helios_aot_config == "auto":
            self.p.helios_aot_config = str(importlib.resources.files(__package__) / "base_aot.yaml")

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
        pc = self.platform_cfg  # alias – matches legacy var name
        self.model_size = int(os.path.getsize(self.p.tflite_filename) / 1024 + 0.999)

        # Arena max size auto‑fill
        if self.p.max_arena_size == 0:
            if self.p.arena_location == "PSRAM":
                max_allowed = pc.GetMaxPsramArenaSize()
            else:
                max_allowed = pc.GetMaxArenaSize()

            # Respect user padding by reserving it from the auto‑computed ceiling.
            padding = max(0, self.p.arena_size_scratch_buffer_padding)
            self.p.max_arena_size = max_allowed - padding if max_allowed > padding else 0
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

        # AOT can't use PSRAM or NVM, so if PSRAM or NVM is selected, disable AOT
        if self.p.arena_location == "PSRAM" or self.p.model_location == "PSRAM" or self.p.model_location == "NVM":
            self.p.create_aot_profile = False
            print("[WARNING] AOT disabled because PSRAM or NVM is selected")

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
            create_validation_binary(self.p, self.mc, self.md, baseline=True, aot=False)
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
            create_validation_binary(self.p, self.mc, self.md, baseline=False, aot=False)
        client = rpc_connect_as_client(self.p)
        configModel(self.p, client, self.md)

        differences, _ = validateModel(self.p, client, get_interpreter(self.p), self.md, self.mc)
        # print(f"[DEBUG] TFLM differences: {differences}")
        # print(f"[DEBUG] TFLM golden output tensors: {golden_output_tensors}")

        stats = getModelStats(self.p, client)
        # pretty-print the differences

        # Calculate mean differences for each output tensor
        for idx, tensor_diffs in enumerate(differences):
            if tensor_diffs:  # Check if there are any differences for this tensor
                # Convert list of arrays to numpy array and calculate mean
                tensor_diffs_array = np.array(tensor_diffs)
                mean_diff = tensor_diffs_array.mean(axis=0)
                log.info("Model Output Comparison: Mean difference per output label in tensor(%d): %s", idx, repr(mean_diff))
            else:
                log.info("Model Output Comparison: No differences for tensor(%d)", idx)

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
                if self.results.aot_success:
                    create_validation_binary(self.p, self.mc, self.md, baseline=False, aot=True)
                else:
                    print("[NS] AOT generation failed, skipping AOT profiling")

            if self.results.aot_success:
                client = rpc_connect_as_client(self.p)
                print("[NS] AOT rpc_connect_as_client done")
                configModel(self.p, client, self.md)
                print("[NS] AOT configModel done")
                differences_aot, golden_output_tensors_aot = validateModel(self.p, client, get_interpreter(self.p),
                                self.md, self.mc)
                # print(f"[DEBUG] AOT differences: {differences_aot}")
                stats_aot = getModelStats(self.p, client)

                # pretty-print the differences
                # Calculate mean differences for each output tensor (AOT)
                for idx, tensor_diffs in enumerate(differences_aot):
                    if tensor_diffs:  # Check if there are any differences for this tensor
                        # Convert list of arrays to numpy array and calculate mean
                        tensor_diffs_array = np.array(tensor_diffs)
                        mean_diff = tensor_diffs_array.mean(axis=0)
                        log.info("Model Output Comparison (AOT): Mean difference per output label in tensor(%d): %s", idx, repr(mean_diff))
                    else:
                        log.info("Model Output Comparison (AOT): No differences for tensor(%d)", idx)

                # Compare TFLM to AOT differences - they should be identical. If they're not, print a warning and the differences
                # Compare each output tensor separately since they may have different shapes
                all_tensors_equal = True
                for idx, (tflm_tensor_diffs, aot_tensor_diffs) in enumerate(zip(differences, differences_aot)):
                    if len(tflm_tensor_diffs) != len(aot_tensor_diffs):
                        log.warning(f"[NS] Tensor {idx}: Different number of runs (TFLM: {len(tflm_tensor_diffs)}, AOT: {len(aot_tensor_diffs)})")
                        all_tensors_equal = False
                        continue

                    # Compare each run for this tensor
                    for run_idx, (tflm_run, aot_run) in enumerate(zip(tflm_tensor_diffs, aot_tensor_diffs)):
                        if not np.array_equal(tflm_run, aot_run):
                            log.warning(f"[NS] Tensor {idx}, Run {run_idx}: TFLM and AOT outputs differ")
                            all_tensors_equal = False

                if not all_tensors_equal:
                    self.results.setAotDifferences(golden_output_tensors_aot, differences, differences_aot, differences)
                    log.warning("[NS] Model Output Comparison: TFLM and AOT differences are not identical. Report will be generated.")
                else:
                    print("[NS] AOT/TFLM output tensor comparison: Identical")

                overall_pmu_stats_aot = []
                pmu_csv_header_aot = ""
                if self.p.full_pmu_capture:
                    events_per_layer_aot = stats_aot[3]
                    layers_aot = stats_aot[5]
                    for layer in range(layers_aot):
                        csv_header_aot, pmu_stats_aot = getPMUStats(self.p, client, layer, events_per_layer_aot)
                        # print(f"[NS] AOT PMU stats for layer {layer}: {pmu_stats_aot}")
                        pmu_csv_header_aot = csv_header_aot  # keep header once
                        overall_pmu_stats_aot.append(pmu_stats_aot)

                printStats(self.p,           # prints to console / log
                        self.mc,
                        stats_aot,
                        str(stats_file_base) + "_aot",
                        pmu_csv_header_aot,
                        overall_pmu_stats_aot,
                        aot=True)


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

        # Ensure supporting libraries are present --------------------
        _fetch_ns_cmsis_nn(self.p.destination_rootdir)
        # print(f"[NS] HeliosAOT destination rootdir: {self.p.destination_rootdir}")

        # Determine configuration path ------------------------------
        cfg_path = self.p.helios_aot_config
        if cfg_path == "auto":
            cfg_path = os.path.splitext(self.p.tflite_filename)[0] + ".yaml"
            if not Path(cfg_path).exists():
                raise FileNotFoundError(
                    "HeliosAOT config file not provided and 'auto' path does not exist."
                )
        # print(f"[NS] HeliosAOT config file: {cfg_path}")
        # Prepare ConvertArgs instance -------------------------------
        convert_args = HeliosConvertArgs(path=Path(cfg_path))  # type: ignore
        # print("[NS] HeliosAOT convert args:")
        # print(convert_args)
        # Override model_path/output/module_name dynamically ---------
        convert_args.model.path = Path(self.p.tflite_filename)

        # Put code in same directory as validator and perf, but under its own subdirectory
        default_output = Path(self.p.destination_rootdir) / self.p.model_name / (self.p.model_name +"_aot")
        convert_args.module.path = default_output
        convert_args.documentation.html = True
        convert_args.module.name = f"{self.p.model_name}"
        convert_args.module.prefix = f"{self.p.model_name}"
        convert_args.test.enabled = True


        # Memory Config
        # Convert memory type to attribute
        from helios_aot.attributes import AttributeRuleset
        from helios_aot.memory import MemoryPlannerType
        from helios_aot.platforms import MemoryType
        from helios_aot.cli.defines import MemoryArgs, ConvertArgs
        from helios_aot.air import AirModel  # for typing only

        _memory_type_to_attribute = {
            "TCM": MemoryType.DTCM,
            "SRAM": MemoryType.SRAM,
            "MRAM": MemoryType.MRAM,
            "PSRAM": MemoryType.PSRAM,
        }
        arena_memory_type = _memory_type_to_attribute[self.p.arena_location]
        model_memory_type = _memory_type_to_attribute[self.p.model_location]
        # put model and arena in specified locations

        # # override for now
        # arena_memory_type = MemoryType.DTCM

        tensor_rules: list[AttributeRuleset] = [
                # Scratch tensors to DTCM (for now)
                AttributeRuleset(
                    type="scratch",
                    id=None,
                    attributes={"memory": arena_memory_type}
                ),
                # PERSISTENT tensors are the 'arena'
                AttributeRuleset(
                    type="persistent",
                    id=None,
                    attributes={"memory": arena_memory_type}
                ),
                # Specific CONSTANT tensors are the 'model'
                AttributeRuleset(
                    type="constant",
                    id=None,
                    attributes={"memory": model_memory_type}
                )
            ]

        # 2) Attach them to the memory section (choose your planner if you haven’t already).
        convert_args.memory = MemoryArgs(
            planner=MemoryPlannerType.greedy,
            tensors=tensor_rules,
            allocate_arenas=True,
        )
        # print(f"[NS] HeliosAOT convert args: {convert_args}")
        # Invoke HeliosAOT programmatically --------------------------
        aot_model = AotModel(config=convert_args)  # type: ignore
        # print(f"[NS] HeliosAOT model: {aot_model}")

        # Try to initialize and convert, but handle any exceptions gracefully
        # Temporarily override sys.exit to prevent HeliosAOT from killing the process
        import sys
        original_exit = sys.exit

        def safe_exit(code):
            # Don't actually exit, just raise an exception that we can catch
            raise RuntimeError(f"HeliosAOT requested exit with code {code}")

        try:
            sys.exit = safe_exit
            # aot_model.initialize()
            print(f"[NS] HeliosAOT model initialized")
            ctx = aot_model.convert()
            print(f"[NS] HeliosAOT model converted")
        except Exception as e:
            print(f"[NS] HeliosAOT model failed: {e}")
            # Don't re-raise - just set the error and continue
            self.results.setAot(False, error=str(e))
            print(f"[NS] HeliosAOT generation failed – {e}")
            # Disable AOT so the rest of the script can run
            self.p.create_aot_profile = False
            print("[WARNING] AOT disabled because HeliosAOT generation failed")
            return  # Exit this method but continue with the rest of the script
        finally:
            # Restore the original sys.exit
            sys.exit = original_exit

        # --- Pull the AirModel from the context and derive operator info ---
        air_model: AirModel | None = getattr(ctx, "model", None) or getattr(ctx, "air_model", None)
        if air_model is None:
            # If this triggers, we may need to adjust based on your CodeGenContext fields.
            raise RuntimeError("HeliosAOT: CodeGenContext did not expose an AirModel (expected ctx.model).")

        # Prefer a topologically-sorted view if available
        ops = air_model.topo_sort() if hasattr(air_model, "topo_sort") else list(getattr(air_model, "operators", []))

        # Store layer stats
        self.mc.aot_layers = len(ops)
        self.mc.aot_layer_last_identifier = ops[-1].id if ops else None
        self.mc.aot_layer_names = [op.op_type.name for op in ops]        # was: op.name
        self.mc.aot_layer_identifiers = [op.id for op in ops]            # was: op.ident
        # print("[AOT] Operations:")
        # for op in ops:
        #     print(f"  {op.id}, {op.op_type.name}")

        self.results.setAot(True, module_path=str(convert_args.module.path.resolve()))
        print("[NS] HeliosAOT generation completed successfully")

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
            # override loaded results with some commonly overriden values from params
            self.results.p = self.p
            self.results.toolchain = self.p.toolchain

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
        runtime_modes = ["tflm"]
        if self.p.create_aot_profile:
            runtime_modes.append("aot")
        # runtime_modes = ["aot"]
        for mode in cpu_modes:
            for runtime_mode in runtime_modes:
                generatePowerBinary(self.p, self.mc, self.md, mode, aot=runtime_mode == "aot")
                # Collect  memory sizes from generated AXF
                # try:
                #     sizes = self._collect_section_sizes(aot=runtime_mode == "aot")
                #     self.results.setMemorySizes(runtime_mode == "aot", sizes)
                # except Exception as exc:
                #     log.warning("[NS] Memory size collection failed: %s", exc)
                try:
                    rom_kb, ram_kb = self._collect_ram_rom_sizes(aot=runtime_mode == "aot")
                    self.results.setMemoryTotals(aot=runtime_mode == "aot", rom_kb=rom_kb, ram_kb=ram_kb)
                except Exception as exc:
                    log.warning("[NS] Memory size collection failed: %s", exc)
                retries_left = 3
                if self.p.joulescope:
                    while retries_left > 0:
                        td, i, v, p_, c, e = measurePower(self.p)
                        if e != 0:
                            break
                        retries_left -= 1
                        sleep(1)
                    if retries_left == 0:
                        log.error("Joulescope driver failed to load, giving up")
                        continue
                    energy_uJ = (e / self.p.runs_power) * 1_000_000
                    time_ms = (td * 1000) / self.p.runs_power
                    power_mW = (e / td) * 1000
                    self.results.setPower(cpu_mode=mode, mSeconds=time_ms, uJoules=energy_uJ, mWatts=power_mW, aot=runtime_mode == "aot")
                    log.info("Model Power Measurement in %s mode: %.3f ms, %.3f uJ (avg %.3f mW)", mode, time_ms, energy_uJ, power_mW)
                else:
                    sleep(10)  # legacy delay to allow SWO capture
                    log.info("Onboard perf run in %s mode completed (see SWO output)", mode)


    # ---- Helpers: AXF path + size parsing ----------------------------------
    def _axf_path_for(self, aot: bool) -> Path:
        # path is ./build/<platform>/<toolchain>/<destination_rootdir>/<model_name>/<model_name>_power[_aot]/<model_name>_power[_aot].axf
        # for example "build/apollo510b_evb/arm-none-eabi/projects/autodeploy/kws_ref_model_aligned/kws_ref_model_aligned_power/kws_ref_model_aligned_power.axf"
        # if aot is True, then the path is "build/apollo510b_evb/arm-none-eabi/projects/autodeploy/kws_ref_model_aligned/kws_ref_model_aligned_power_aot/kws_ref_model_aligned_power_aot.axf"
        # if toolchain is gcc, the directory is arm-none-eabi, otherwise it is just the toolchain
        if self.p.toolchain == "gcc":
            toolchain = "arm-none-eabi"
        else:
            toolchain = self.p.toolchain

        # the destination is anything after neuralspot_rootdir in destination_rootdir
        destination = self.p.destination_rootdir.replace(self.p.neuralspot_rootdir, "")
        # remove leading slash and convert to string
        destination = destination.lstrip("/")
        destination = str(destination)


        if self.p.platform == "apollo330mP_evb":
            actual_platform = "apollo330P_evb"
        else:
            actual_platform = self.p.platform
        print(f"[NS] Toolchain: {toolchain}")
        print(f"[NS] Destination rootdir: {self.p.destination_rootdir}")
        print(f"[NS] Model name: {self.p.model_name}")
        print(f"[NS] Platform: {actual_platform}")
        print(f"[NS] AOT: {aot}")
        print(f"[NS] Destination: {destination}")
        if aot:
            postfix = "_aot"
        else:
            postfix = ""
        build_path = Path("build") / actual_platform / toolchain / destination / self.p.model_name / f"{self.p.model_name}_power{postfix}" / f"{self.p.model_name}_power{postfix}.axf"
        # if aot:
        #     build_path = build_path.with_suffix(f"{self.p.model_name}_aot.axf")
        print(f"[NS] AXF path: {build_path}")

        return build_path

        # base = Path(self.p.destination_rootdir) / self.p.model_name
        # name = self.p.model_name if not aot else f"{self.p.model_name}_aot"
        # return base / name / f"{name}.axf"

    def _collect_section_sizes(self, aot: bool) -> dict:
        """
        Run `arm-none-eabi-size -A -x` on the generated .axf and return KB (ceil).
        Keys: text, itcm_text, data, bss, sram_bss, shared
        """
        axf = self._axf_path_for(aot)
        targets = {"text", "itcm_text", "data", "bss", "sram_bss", "shared"}
        armclang_targets = {"MCU_MRAM", "MCU_ITCM", "MCU_TCM", "SHARED_SRAM"}
        # Armclang has two MCU_TCM sections, one for .data and one for .bss - map the first to data and the second to bss
        sizes_bytes = {k: 0 for k in targets}
        if not axf.exists():
            raise FileNotFoundError(f"AXF not found: {axf}")
        res = subprocess.run(
            ["arm-none-eabi-size", "-A", "-x", str(axf)],
            capture_output=True,
            text=True,
        )
        if res.returncode != 0:
            raise RuntimeError(res.stderr.strip() or "arm-none-eabi-size failed")
        for line in res.stdout.splitlines():
            parts = line.split()
            if len(parts) < 2:
                continue
            sec = parts[0].lstrip(".")
            if sec in sizes_bytes:
                # Size is hex with -x (e.g., 0x1234); be defensive
                try:
                    val = int(parts[1], 0)
                except ValueError:
                    continue
                if sec in armclang_targets:
                    if sec == "MCU_TCM":
                        sizes_bytes["data"] += val
                        sizes_bytes["bss"] += val
                    else:
                        sizes_bytes[sec] += val
                sizes_bytes[sec] += val
        # Convert to ceil-KB
        def b2kb(x: int) -> int:
            return (x + 1023) // 1024
        return {k: b2kb(v) for k, v in sizes_bytes.items()}

    def _collect_ram_rom_sizes(self, aot: bool) -> tuple[int, int]:
        """
        Coalesce memory into ROM and RAM totals (KB, ceiled).
        - ArmClang: parse `fromelf --info=totals`
        - GCC:      parse `arm-none-eabi-size -A -x` sections (ITCM counted as ROM)
        """
        axf = self._axf_path_for(aot)
        if not axf.exists():
            raise FileNotFoundError(f"AXF not found: {axf}")
        if self.p.toolchain == "arm":
            return self._collect_ram_rom_sizes_armclang(axf)
        return self._collect_ram_rom_sizes_gcc(axf)

    def _collect_ram_rom_sizes_armclang(self, axf_path: Path) -> tuple[int, int]:
        """
        Parse `fromelf --info=totals`:
          - RAM := 'Total RW  Size (RW Data + ZI Data)'
          - ROM := 'Total ROM Size (Code + RO Data + RW Data)'
        """
        import re, subprocess
        res = subprocess.run(
            ["fromelf", "--info=totals", str(axf_path)],
            capture_output=True,
            text=True,
        )
        if res.returncode != 0:
            raise RuntimeError(res.stderr.strip() or "fromelf failed")
        txt = res.stdout
        m_rom = re.search(r"Total RO\s+Size.*?(\d+)\s+\(", txt)
        m_ram = re.search(r"Total RW\s+Size.*?(\d+)\s+\(", txt)
        if not (m_ram and m_rom):
            raise RuntimeError("fromelf totals not found")
        ram_b = int(m_ram.group(1))
        rom_b = int(m_rom.group(1))
        b2kb = lambda x: (x + 1023) // 1024
        return b2kb(rom_b), b2kb(ram_b)

    def _collect_ram_rom_sizes_gcc(self, axf_path: Path) -> tuple[int, int]:
        """
        Heuristically coalesce sections from `arm-none-eabi-size -A -x`:
          - ROM: .text*, *itcm*, .rodata*, .ARM.exidx, .ARM.extab, plus .data load image
          - RAM: .data (init RW), .bss, .s(b)ss, .sram_bss, .shared, .noinit, *ZI*, heap/stack if present
          ITCM is counted as ROM to match ArmClang accounting.
        """
        import subprocess
        res = subprocess.run(
            ["arm-none-eabi-size", "-A", "-x", str(axf_path)],
            capture_output=True,
            text=True,
        )
        if res.returncode != 0:
            raise RuntimeError(res.stderr.strip() or "arm-none-eabi-size failed")
        rom_b = 0
        ram_b = 0
        for line in res.stdout.splitlines():
            parts = line.split()
            if len(parts) < 2:
                continue
            sec = parts[0].lstrip(".").lower()
            # Skip obvious non-loadable noise
            if "debug" in sec or sec.startswith("comment"):
                continue
            try:
                size = int(parts[1], 0)
            except ValueError:
                continue
            if size <= 0:
                continue
            # ROM buckets
            if "rodata" in sec or "exidx" in sec or "extab" in sec:
                rom_b += size
                continue
            if "itcm" in sec or "text" in sec:
                rom_b += size
                continue
            # # .data RAM (runtime)
            # if sec == "data" or sec.startswith("data.") or ".data" in sec:
            #     # rom_b += size
            #     ram_b += size
            #     continue
            # RAM buckets
            if (
                "sram_bss" in sec
                or "shared" == sec
                or "bss" in sec
                or "data" in sec
            ):
                ram_b += size
                continue
            # else: ignore unclassified tiny sections
        b2kb = lambda x: (x + 1023) // 1024
        return b2kb(rom_b), b2kb(ram_b)

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
