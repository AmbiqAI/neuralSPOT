#!/usr/bin/env python
import logging as log
import pickle
import yaml
import os
import numpy as np
import pydantic_argparse
from neuralspot.tools.autodeploy.gen_library import generateModelLib
from neuralspot.tools.autodeploy.measure_power import generatePowerBinary, measurePower
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
from neuralspot.tools.ns_utils import ModelDetails, reset_dut, rpc_connect_as_client
import neuralspot.tools.ns_platform as ns_platform
from pydantic import BaseModel, Field
from pathlib import Path
from time import sleep


class Params(BaseModel):
    # General Parameters
    seed: int = Field(42, description="Random Seed")
    platform: str = Field(
        "apollo510_evb", description="Platform to deploy model on (e.g. apollo4p_evb, apollo510_evb...)"
    )
    toolchain: str = Field(
        "gcc", description="Compiler to use (supported: gcc, arm)"
    )

    # Stage Selection
    create_binary: bool = Field(
        True,
        description="Create a neuralSPOT Validation EVB image based on TFlite file",
    )
    create_profile: bool = Field(
        True, description="Profile the performance of the model on the EVB"
    )
    create_library: bool = Field(
        False, description="Create minimal static library based on TFlite file"
    )
    create_ambiqsuite_example: bool = Field(
        False, description="Create AmbiqSuite example based on TFlite file"
    )
    joulescope: bool = Field(
        False,
        description="Measure power consumption of the model on the EVB using Joulescope",
    )
    onboard_perf: bool = Field(
        False, 
        description="Capture and print performance measurements on EVB"
    )
    full_pmu_capture: bool = Field(
        False,
        description="Capture full PMU data during performance measurements on EVB",
    )

    # General Configuration
    tflite_filename: str = Field(
        "undefined", description="Name of tflite model to be analyzed"
    )
    configfile: str = Field(
        "", description="Optional configuration file for parameters"
    )
    pmu_config_file: str = Field(
        "default", description="M55 PMU configuration override file for peformance profiling"
    )

    model_location: str = Field(
        "auto", description="Where the model is stored on the EVB (Auto, TCM, SRAM, MRAM, or PSRAM)"
    )

    arena_location: str = Field(
        "auto", description="Where the arena is stored on the EVB (auto, TCM, SRAM, or PSRAM)"
    )

    max_arena_size: int = Field(
        0, description="Maximum KB to be allocated for TF arena, 0 for auto"
    )
    arena_size_scratch_buffer_padding: int = Field(
        0,
        description="(TFLM Workaround) Padding to be added to arena size to account for scratch buffer (in KB)",
    )

    resource_variable_count: int = Field(
        0,
        description="Maximum ResourceVariables needed by model (typically used by RNNs)",
    )

    # Validation Parameters
    random_data: bool = Field(True, description="Use random input tensor data")
    dataset: str = Field("dataset.pkl", description="Name of dataset if --random_data is not set")
    runs: int = Field(
        10, description="Number of inferences to run for characterization"
    )
    runs_power: int = Field(
        100, description="Number of inferences to run for power measurement"
    )

    # Joulescope and Onboard Performance Parameters
    cpu_mode: str = Field(
        "auto", 
        description="CPU Mode for joulescope and onboard_perf modes - can be auto, LP (low power), or HP (high performance)"
    )

    # Library Parameters
    model_name: str = Field(
        "auto", description="Name of model to be used in generated library, 'auto' to use TFLite filename base"
    )

    destination_rootdir: str = Field(
        "auto",
        description="Directory where generated library will be placed, 'auto' to place in neuralSPOT/projects/autodeploy",
    )
    neuralspot_rootdir: str = Field(
        "auto",
        description="Path to root neuralSPOT directory, 'auto' to autodetect if run within neuralSPOT",
    )

    resultlog_file: str = Field(
        "none", description="Path and Filename to store running log results. If none, result is not recorded."
    )

    profile_results_path: str = Field(
        "none", description="Path to store per-model profile results in addition to the file in the working directory. If none, the additional result file is not generated."
    )

    # Platform Parameters
    ambiqsuite_version: str = Field(
        "auto",
        description="AmbiqSuite version used to generate minimal example, 'auto' for latest",
    )

    tensorflow_version: str = Field(
        "auto",
        description="Tensorflow version used to generate minimal example, 'auto' for latest",
    )

    # Profile Parameters
    profile_warmup: int = Field(1, description="How many inferences to profile")

    # Logging Parameters
    verbosity: int = Field(1, description="Verbosity level (0-4)")

    # RPC Parameters
    transport: str = Field("auto", description="RPC transport, 'auto' for autodetect. Can set to USB or UART.")
    tty: str = Field("auto", description="Serial device, 'auto' for autodetect")
    baud: str = Field("auto", description="Baud rate, 'auto' for autodetect")


def create_parser():
    """Create CLI argument parser
    Returns:
        ArgumentParser: Arg parser
    """
    return pydantic_argparse.ArgumentParser(
        model=Params,
        prog="Evaluate TFLite model against EVB instantiation",
        description="Evaluate TFLite model",
    )


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
        self.model_size = 0

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
            f"[Profile] Total Estimated MACs:              {self.profileTotalEstimatedMacs}"
        )
        # print(f"[Profile] Total CPU Cycles:                  {self.profileTotalCycles}")
        print(f"[Profile] Total Model Layers:                {self.profileTotalLayers}")
        # print(
        #     f"[Profile] Cycles per MAC:                    {(self.profileTotalCycles/self.profileTotalEstimatedMacs):0.3f}"
        # )
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
    
        if self.p.toolchain == "gcc":
            # Format for gcc --version is "arm-none-eabi-gcc (Arm GNU Toolchain 13.2.rel1 (Build arm-13.7)) 13.2.1 20231009"
            compiler_version = os.popen("arm-none-eabi-gcc --version").read().split(" ")[4]
        elif self.p.toolchain == "arm":
            # Format for armclang --version is "Arm Compiler 6.16"
            compiler_version = os.popen("armclang --version").read().split(" ")[2]

        # get today's date
        from datetime import date
        today = date.today()
        # dd/mm/YY
        d1 = today.strftime("%d/%m/%Y")

        if self.p.resultlog_file != "none":
            if not os.path.exists(self.p.resultlog_file):
                with open(self.p.resultlog_file, "w") as f:
                    f.write(
                        "Model Filename, Platform, Compiler, TF Version, Model Size (KB), Arena Size (KB), Model Location, Arena Location, Est MACs, HP(ms), HP(uJ), HP(mW), LP(ms), LP(uJ), LP(mW), AS Version, Date Run\n"
                    )
            with open(self.p.resultlog_file, "a") as f:
                f.write(
                    f"{self.model_name},{self.p.platform},{self.p.toolchain} {compiler_version},{self.p.tensorflow_version},{self.model_size},{self.p.max_arena_size},{self.p.model_location},{self.p.arena_location},{self.profileTotalEstimatedMacs},{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts},{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts},{self.p.ambiqsuite_version},{d1}\n"
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

def load_yaml_config(configfile):
    with open(configfile, "r") as f:
        try:
            return yaml.safe_load(f)
        except yaml.YAMLError as exc:
            print(exc)

def main():
    # parse cmd parameters
    parser = create_parser()
    cli_params = parser.parse_typed_args()

    # load yaml config
    yaml_params = {}
    if cli_params.configfile:
        yaml_params = load_yaml_config(cli_params.configfile)

    # prepare the default values
    default_params = Params()

    # update from YAML config
    updated_params = default_params.dict()
    updated_params.update(yaml_params)
    
    # override with CLI params
    cli_dict = cli_params.dict(exclude_unset=True)  # exclude unset fields
    updated_params.update(cli_dict)

    # create Params instance with updated values
    params = Params(**updated_params)

    # IF neuralSPOT rootdir is auto, find the root directory assuming we're inside neuralSPOT
    if params.neuralspot_rootdir == "auto":
        # neuralSPOT rootdir has a neuralspot/ns-core/module.mk file, find that to find the rootdir
        # search parent dirs until we find it, starting from the current working directory
        rootdir = Path.cwd().resolve()
        while os.path.normpath(rootdir) != os.path.normpath("/"):
            if os.path.exists(rootdir / "neuralspot" / "ns-core" / "module.mk"):
                params.neuralspot_rootdir = rootdir
                break
            rootdir = rootdir.parent
        if os.path.normpath(rootdir) == os.path.normpath("/"):
            log.error("NeuralSPOT root directory not found. Please specify a valid path.")
            exit("Autodeploy failed")
        
        # convert to string
        print(f"[NS] NeuralSPOT root directory found at {params.neuralspot_rootdir}")

    # if destination_rootdir is auto, set it to the neuralSPOT/projects/autodeploy directory
    if params.destination_rootdir == "auto":
        # check that projects/autodeploy exists
        if not os.path.exists(params.neuralspot_rootdir / "projects" / "autodeploy"):
            log.error(f"{params.neuralspot_rootdir}/projects/autodeploy directory not found. Please specify a valid path using --destination_rootdir")
            exit("Autodeploy failed")
        params.destination_rootdir = params.neuralspot_rootdir / "projects" / "autodeploy"
        # convert to string
        params.destination_rootdir = str(params.destination_rootdir)
        print(f"[NS] Destination Root Directory automatically set to: {params.destination_rootdir}")

    # if model_name is auto, set it to the base of the TFLite filename, replacing dashes with underscores
    if params.model_name == "auto":
        params.model_name = Path(params.tflite_filename).stem.replace("-", "_")
        print(f"[NS] Model Name automatically set to: {params.model_name}")

    params.neuralspot_rootdir = str(params.neuralspot_rootdir)

    # check if params.destination_rootdir is relative path. If it is, make it absolute so that measure_power.py can use it
    if not os.path.isabs(params.destination_rootdir):
        params.destination_rootdir = os.path.abspath(params.destination_rootdir)
    if not os.path.isabs(params.neuralspot_rootdir):
        params.neuralspot_rootdir = os.path.abspath(params.neuralspot_rootdir)

    if params.onboard_perf and params.joulescope:
        raise ValueError("Cannot run onboard performance and Joulescope at the same time")

    if params.tflite_filename == "undefined":
        raise ValueError("TFLite filename must be specified")

    stage = 1
    total_stages = 0
    if params.create_binary:
        total_stages += 1
    if params.create_profile:
        total_stages += 1
    if params.create_library:
        total_stages += 1
    if params.joulescope or params.onboard_perf:
        total_stages += 1
    if params.create_ambiqsuite_example:
        total_stages += 1

    # set logging level
    log.basicConfig(
        level=log.DEBUG
        if params.verbosity > 2
        else log.INFO
        if params.verbosity > 1
        else log.WARNING,
        format="%(levelname)s: %(message)s",
    )

    # check if neuralspot-rootdir is valid
    if not os.path.exists(os.path.abspath(params.neuralspot_rootdir)):
        log.error("NeuralSPOT directory not found. Please specify a valid path.")
        exit("Autodeploy failed")

    # Get platform configuration and check what we can
    pc = ns_platform.AmbiqPlatform(params)
    print (f"[NS] Running {total_stages} Stage Autodeploy for Platform: {params.platform}")

    # Override any 'auto' params with platform defaults (except arena location because 
    # true size isn't known yet)
    if params.max_arena_size == 0:
        params.max_arena_size = pc.GetMaxArenaSize()
        print(f"[NS] Max Arena Size for {params.platform}: {params.max_arena_size} KB")
    else:
        pc.CheckArenaSize(params.max_arena_size, params.arena_location)

    # Get model size based on TFLite file size, round up to next KB
    model_size = os.path.getsize(params.tflite_filename) / 1024
    model_size = int(model_size) + 1 if model_size % 1 > 0 else int(model_size)    

    if params.model_location == "auto":
        params.model_location = pc.GetModelLocation(model_size, params.model_location) # Set best location based on model size
        print(f"[NS] Best {model_size}KB model location for {params.platform}: {params.model_location}")

    if params.ambiqsuite_version == "auto":
        params.ambiqsuite_version = pc.platform_config["as_version"]
        print(f"[NS] Using AmbiqSuite Version: {params.ambiqsuite_version}")
    
    if params.tensorflow_version == "auto":
        params.tensorflow_version = pc.platform_config["tflm_version"]
        print(f"[NS] Using TensorFlow Version: {params.tensorflow_version}")

    if params.transport == "auto":
        # Set it to USB if available, otherwise UART
        params.transport = "USB" if pc.GetSupportsUsb() else "UART"

    log.info(f"[NS] Using transport: {params.transport}")

    # Check to see if model would fit in MRAM even is specified to be PSRAM (power firmware does not enable USB.
    # so if the model doesn't fit in MRAM, we have no way to move it to PSRAM). If the model fits in MRAM but the
    # user asked to run int from PSRAM, the firmware will copy it to PSRAM before running it.
    smallest_fit = pc.GetModelLocation(model_size, "auto")
    # print(f"[NS] Smallest model location for {params.platform}: {smallest_fit}")
    if smallest_fit == "PSRAM":
        print("[NS WARNING] Model is too large for performance or example generation - it needs to fit in MRAM even is target is PSRAM")
        print("[NS WARNING] Disabling Performance and Example Generation")
        params.joulescope = False
        params.onboard_perf = False
        params.create_ambiqsuite_example = False
        params.create_library = False

    interpreter = get_interpreter(params)

    mc = ModelConfiguration(params)
    md = ModelDetails(interpreter)

    # Pickle the model details for later use
    # mc_file = open("model_config.pkl", "wb")
    # pickle.dump(mc, mc_file)
    # mc_file.close()

    # if params.create_binary or params.create_profile:
    #     # Configure the model on the EVB
    #     client = rpc_connect_as_client(params)

    pkl_dir = params.destination_rootdir + "/" + params.model_name + "/"
    mc_name = pkl_dir + params.model_name + "_mc.pkl"
    md_name = pkl_dir + params.model_name + "_md.pkl"
    results_name = pkl_dir + params.model_name + "_results.pkl"

    # If no-create-binary or no-create-profile, print error if pickle files don't exist
    if (params.create_profile == False) and (params.create_binary == False):
        if not os.path.exists(mc_name) or not os.path.exists(md_name):
            raise ValueError(
                f"Model configuration and model details files {mc_name} and {md_name} not found. Please run with --create_binary or --create_profile"
            )

    stash_arena_location = params.arena_location
    results = adResults(params)
    results.setModelSize(model_size)

    if params.create_binary:
        print(
            f"[NS] *** Stage [{stage}/{total_stages}]: Create and fine-tune EVB model characterization image"
        )
        stage += 1

        # If auto, use SRAM as the arena location for the first pass
        if params.arena_location == "auto":
            params.arena_location = "SRAM"

        create_validation_binary(params, True, mc)
        client = rpc_connect_as_client(params)

        configModel(params, client, md)

        stats = getModelStats(params, client)
        mc.update_from_stats(stats, md)
        # mc.check(params)

        # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
        params.arena_location = stash_arena_location

        # If auto, find the best arena location
        if params.arena_location == "auto":
            params.arena_location = pc.GetArenaLocation(mc.arena_size_k, params.arena_location)

        # Check computed arena size against desired arena location
        arena_size = mc.arena_size_k + params.arena_size_scratch_buffer_padding
        if not pc.CheckArenaSize(arena_size, params.arena_location):
            raise ValueError(
                f"Model size {model_size}KB exceeds available memory for platform {params.platform}"
            )

        # If the arena and model locations are both TCM, make sure they fit, and move the model to MRAM if they don't
        if params.model_location == "TCM" and params.arena_location == "TCM":
            if model_size + mc.arena_size_k > pc.GetDTCMSize():
                print(f"[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                params.model_location = "MRAM"

        create_validation_binary(params, False, mc)
        client = rpc_connect_as_client(params)  # compiling resets EVB, need reconnect
        configModel(params, client, md)

        log.info("Writing MC and MD to Pickle files")
        mc_file = open(mc_name, "wb")
        pickle.dump(mc, mc_file)
        mc_file.close()
        md_file = open(md_name, "wb")
        pickle.dump(md, md_file)
        md_file.close()        

    if params.create_profile:
        print("")
        print(
            f"[NS] *** Stage [{stage}/{total_stages}]: Characterize model performance on EVB"
        )
        stage += 1

        if params.create_binary == False:
            # Read MC and MD from Pickle file
            log.info(f"Reading MC {mc_name} and MD {md_name} from Pickle files")
            mc_file = open(mc_name, "rb")
            mc = pickle.load(mc_file)
            mc_file.close()
            md_file = open(md_name, "rb")
            md = pickle.load(md_file)
            md_file.close()
            print(md)
            print(mc)
            if params.model_location == "TCM" and params.arena_location == "TCM":
                if model_size + mc.arena_size_k > pc.GetDTCMSize():
                    print(f"[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                    params.model_location = "MRAM"
            create_validation_binary(params, False, mc)
            client = rpc_connect_as_client(params)  # compiling resets EVB, need reconnect
            configModel(params, client, md)

        differences = validateModel(params, client, interpreter, md, mc)
        # Get profiling stats
        stats = getModelStats(params, client)
        stats_filename = (
            params.destination_rootdir
            + "/"
            + params.model_name
            + "/"
            + params.model_name
            + "_stats"
        )

        # If full PMU capture is enabled, get the full PMU data by calling getPMUStats
        pmu_csv_header = ""
        overall_pmu_stats = []
        if params.full_pmu_capture:
            events_per_layer = stats[3]
            layers = stats[5]
            print(f"[NS] Full PMU Capture: {events_per_layer} events per layer")
            for layer in range(layers):
                # print(f"[NS] Layer PMU {layer}")
                pmu_csv_header, pmu_stats = getPMUStats(params, client, layer, events_per_layer)
                overall_pmu_stats.append(pmu_stats)
            # print(f"[NS] Full PMU Capture: {len(overall_pmu_stats)} layers captured")
            # print(overall_pmu_stats)

        cycles, macs, time, layers, events_per_layer = printStats(params, mc, stats, stats_filename, pmu_csv_header, overall_pmu_stats)
        results.setProfile(time, macs, cycles, layers)

        # Overwrite MC with new stats
        mc_file = open(mc_name, "wb")
        pickle.dump(mc, mc_file)
        mc_file.close()

        # Write results to pkl
        results_file = open(results_name, "wb")
        pickle.dump(results, results_file)
        results_file.close()

        otIndex = 0
        for d in differences:
            log.info(
                f"Model Output Comparison: Mean difference per output label in tensor({otIndex}): "
                + repr(np.array(d).mean(axis=0))
            )
            otIndex += 1


    if (params.create_profile == False) and (params.create_binary == False):
        # Read MC and MD from Pickle file
        log.info("Reading MC and MD from Pickle files")
        mc_file = open(mc_name, "rb")
        mc = pickle.load(mc_file)
        # print(mc)
        mc_file.close()
        md_file = open(md_name, "rb")
        md = pickle.load(md_file)
        # print(md)
        md_file.close()
        results_file = open(results_name, "rb")
        results = pickle.load(results_file)
        results_file.close()
        # If auto, find the best arena location
        if params.arena_location == "auto":
            params.arena_location = pc.GetArenaLocation(mc.arena_size_k, params.arena_location)
        
        # print (f"[NS] Arena Location: {params.arena_location}")
        # print (f"[NS] Model Location: {params.model_location}")
        if params.model_location == "TCM" and params.arena_location == "TCM":
            # print (f"[NS] Model Size: {model_size} KB")
            # print (f"[NS] Arena Size: {mc.arena_size_k} KB")
            # print (f"[NS] DTCM Size: {pc.GetDTCMSize()} KB")
            if model_size + mc.arena_size_k > pc.GetDTCMSize():
                print(f"[NS] Model plus Arena do not fit in Data TCM. Moving model to MRAM.")
                params.model_location = "MRAM"
    # else:
    #     # Pickle the model details for later use
    #     log.info("Writing MC and MD to Pickle files")
    #     mc_file = open(mc_name, "wb")
    #     pickle.dump(mc, mc_file)
    #     mc_file.close()
    #     md_file = open(md_name, "wb")
    #     pickle.dump(md, md_file)
    #     md_file.close()
    #     results_file = open(results_name, "wb")
    #     pickle.dump(results, results_file)
    #     results_file.close()

    if params.joulescope or params.onboard_perf:
        print("")
        if params.joulescope:
            print(
                f"[NS] *** Stage [{stage}/{total_stages}]: Characterize inference energy consumption on EVB using Joulescope"
            )
        else:
            print(
                f"[NS] *** Stage [{stage}/{total_stages}]: Characterize inference performance on EVB"
            )

        # if cpu_mode is auto, run both LP and HP
        if params.cpu_mode == "auto":
            cpu_modes = ["LP", "HP"]
        else:
            cpu_modes = [params.cpu_mode]

        for cpu_mode in cpu_modes:
            # for cpu_mode in ["NS_MINIMUM_PERF"]:
            generatePowerBinary(params, mc, md, cpu_mode)

            # Only measure power if joulescope is enabled
            if params.joulescope:
                td, i, v, p, c, e = measurePower(params)
                energy = (e["value"] / params.runs_power) * 1000000  # Joules
                t = (td.total_seconds() * 1000) / params.runs_power
                w = (e["value"] / td.total_seconds()) * 1000
                results.setPower(cpu_mode=cpu_mode, mSeconds=t, uJoules=energy, mWatts=w)
                log.info(
                    f"Model Power Measurement in {cpu_mode} mode: {t:.3f} ms and {energy:.3f} uJ per inference (avg {w:.3f} mW))"
                )
            else:
                sleep(10)
                log.info(
                    f"Model Power Measurement in {cpu_mode} mode: Disabled, see SWO output for other data"
                )

    if params.create_library:
        print("")
        print(f"[NS] *** Stage [{stage}/{total_stages}]: Generate minimal static library")
        generateModelLib(params, mc, md, ambiqsuite=False)
        stage += 1

    if params.create_ambiqsuite_example:
        print("")
        print(f"[NS] *** Stage [{stage}/{total_stages}]: Generate AmbiqSuite Example")
        generateModelLib(params, mc, md, ambiqsuite=True)
        stage += 1

    results.print()

if __name__ == "__main__":
    main()