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
from pathlib import Path
from time import sleep
from typing import Callable, List

import numpy as np
import pydantic_argparse
import yaml
from pydantic import BaseModel, Field

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

__all__ = ["main", "Params"]  # For external reuse & unit‑test import

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
        """Return a *pydantic‑argparse* parser that mirrors the legacy CLI."""
        return pydantic_argparse.ArgumentParser(
            model=cls,
            prog="Evaluate TFLite model against EVB instantiation",
            description="Evaluate TFLite model",
        )

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

    merged = default_params.dict()
    merged.update(yaml_params)
    # CLI wins last
    merged.update(cli.dict(exclude_unset=True))
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
        self.powerIterations = p.runs_power
        # self.stats_filename = p.stats_filename
        self.model_name = p.model_name
        self.p = p
        self.toolchain = p.toolchain
        self.model_size = 0
        self.arena_size = 0

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
                with open(self.p.resultlog_file, "w") as f:
                    f.write(
                        "Model Filename, Platform, Compiler, TF Version, Model Size (KB), Arena Size (KB), Model Location, Arena Location, Est MACs, HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW), AS Version, Date Run, ID\n"
                    )
            with open(self.p.resultlog_file, "a") as f:
                f.write(
                    f"{self.model_name},{self.p.platform},{self.p.toolchain} {compiler_version},{self.p.tensorflow_version},{self.model_size},{self.arena_size},{self.p.model_location},{self.p.arena_location},{self.profileTotalEstimatedMacs},{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts},{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts},{self.p.ambiqsuite_version},{d1}, {self.p.run_log_id}\n"
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

    def setPower(self, cpu_mode, mSeconds, uJoules, mWatts):
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

        # adResults is untouched – keep legacy import‑side‑effects intact
        # from ns_autodeploy import adResults  # noqa: WPS433 – cyclic import okay (legacy)
        # from neuralspot.tools.autodeploy.ns_autodeploy import adResults  # noqa: WPS433 – cyclic import okay (legacy)

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
            print(f"[NS] Max {self.p.arena_location if self.p.arena_location != 'auto' else ''} Arena Size for {self.p.platform}: {self.p.max_arena_size} KB")
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
            create_validation_binary(self.p, True, self.mc)
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

        # if not self.p.nocompile_mode:
        #     create_validation_binary(self.p, False, self.mc)
        # client = rpc_connect_as_client(self.p)
        # configModel(self.p, client, self.md)

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
            create_validation_binary(self.p, False, self.mc)
        client = rpc_connect_as_client(self.p)
        configModel(self.p, client, self.md)

        differences = validateModel(self.p, client, get_interpreter(self.p), self.md, self.mc)
        stats = getModelStats(self.p, client)
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
        for mode in cpu_modes:
            generatePowerBinary(self.p, self.mc, self.md, mode)
            if self.p.joulescope:
                td, i, v, p_, c, e = measurePower(self.p)
                energy_uJ = (e["value"] / self.p.runs_power) * 1_000_000
                time_ms = (td.total_seconds() * 1000) / self.p.runs_power
                power_mW = (e["value"] / td.total_seconds()) * 1000
                self.results.setPower(cpu_mode=mode, mSeconds=time_ms, uJoules=energy_uJ, mWatts=power_mW)
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