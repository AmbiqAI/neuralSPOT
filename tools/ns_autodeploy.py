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
    printStats,
    validateModel,
)
from neuralspot.tools.ns_utils import ModelDetails, reset_dut, rpc_connect_as_client
from pydantic import BaseModel, Field
from pathlib import Path


class Params(BaseModel):
    # General Parameters
    seed: int = Field(42, description="Random Seed")
    create_binary: bool = Field(
        True,
        description="Create a neuralSPOT Validation EVB image based on TFlite file",
    )
    create_profile: bool = Field(
        True, description="Profile the performance of the model on the EVB"
    )
    create_library: bool = Field(
        True, description="Create minimal static library based on TFlite file"
    )
    create_ambiqsuite_example: bool = Field(
        True, description="Create AmbiqSuite example based on TFlite file"
    )
    joulescope: bool = Field(
        False,
        description="Measure power consumption of the model on the EVB using Joulescope",
    )
    onboard_perf: bool = Field(
        False, 
        description="Capture and print performance measurements on EVB"
    )
    model_location: str = Field(
        "TCM", description="Where the model is stored on the EVB (TCM, SRAM, or MRAM)"
    )

    arena_location: str = Field(
        "TCM", description="Where the arena is stored on the EVB (TCM or SRAM)"
    )
    tflite_filename: str = Field(
        "model.tflite", description="Name of tflite model to be analyzed"
    )

    configfile: str = Field(
        "", description="Optional configuration file for parameters"
    )

    # Create Binary Parameters
    tflm_filename: str = Field(
        "mut_model_data.h", description="Name of TFLM C file for Characterization phase"
    )
    # tflm_src_path: str = Field(
    #     "../projects/models/tflm_validator/src",
    #     description="Root directory for ns_autodeploy to place generated files",
    # )
    max_arena_size: int = Field(
        100, description="Maximum KB to be allocated for TF arena"
    )
    arena_size_scratch_buffer_padding: int = Field(
        0,
        description="(TFLM Workaround) Padding to be added to arena size to account for scratch buffer (in KB)",
    )
    max_rpc_buf_size: int = Field(
        4096, description="Maximum bytes to be allocated for RPC RX and TX buffers"
    )
    resource_variable_count: int = Field(
        0,
        description="Maximum ResourceVariables needed by model (typically used by RNNs)",
    )
    max_profile_events: int = Field(
        40,
        description="Maximum number of events (layers) captured during model profiling",
    )

    # Validation Parameters
    dataset: str = Field("dataset.pkl", description="Name of dataset")
    random_data: bool = Field(True, description="Use random data")
    runs: int = Field(
        10, description="Number of inferences to run for characterization"
    )
    runs_power: int = Field(
        100, description="Number of inferences to run for power measurement"
    )
    cpu_mode: str = Field("NS_MAXIMUM_PERF", description="CPU mode to use for performance measurements")

    # Library Parameters
    model_name: str = Field(
        "model", description="Name of model to be used in generated library"
    )
    destination_rootdir: str = Field(
        str(Path.cwd()),
        description="Directory where generated library will be placed",
    )

    neuralspot_rootdir: str = Field(
        str(Path.cwd()),
        description="Directory where neuralSPOT makefile is located (not the Makefile itself)"
    )

    # Profile Parameters
    profile_warmup: int = Field(1, description="How many inferences to profile")
    # stats_filename: str = Field(
    #     "stats.csv", description="Name of exported profile statistics CSV file"
    # )

    # Logging Parameters
    verbosity: int = Field(1, description="Verbosity level (0-4)")

    # RPC Parameters
    tty: str = Field("/dev/tty.usbmodem1234561", description="Serial device")
    baud: str = Field("115200", description="Baud rate")


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

    def print(self):
        print("")
        print((f"Characterization Report for {self.model_name}:"))
        print(
            f"[Profile] Per-Layer Statistics file:         {self.model_name}_stats.csv"
        )
        # print(
        #     f"[Profile] Max Perf Inference Time (ms):      {(self.profileTotalInferenceTime/1000):0.3f}"
        # )
        print(
            f"[Profile] Total Estimated MACs:              {self.profileTotalEstimatedMacs}"
        )
        print(f"[Profile] Total CPU Cycles:                  {self.profileTotalCycles}")
        print(f"[Profile] Total Model Layers:                {self.profileTotalLayers}")
        # print(
        #     f"[Profile] MACs per second:                   {((self.profileTotalEstimatedMacs*1000000)/self.profileTotalInferenceTime):0.3f}"
        # )
        print(
            f"[Profile] Cycles per MAC:                    {(self.profileTotalCycles/self.profileTotalEstimatedMacs):0.3f}"
        )
        print(
            f"[Power]   Max Perf Inference Time (ms):      {self.powerMaxPerfInferenceTime:0.3f}"
        )
        print(
            f"[Power]   Max Perf Inference Energy (uJ):    {self.powerMaxPerfJoules:0.3f}"
        )
        print(
            f"[Power]   Max Perf Inference Avg Power (mW): {self.powerMaxPerfWatts:0.3f}"
        )
        print(
            f"[Power]   Min Perf Inference Time (ms):      {self.powerMinPerfInferenceTime:0.3f}"
        )
        print(
            f"[Power]   Min Perf Inference Energy (uJ):    {self.powerMinPerfJoules:0.3f}"
        )
        print(
            f"[Power]   Min Perf Inference Avg Power (mW): {self.powerMinPerfWatts:0.3f}"
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
            f"{self.model_name},96,{self.profileTotalEstimatedMacs},{self.profileTotalCycles},{self.powerMinPerfInferenceTime},{self.powerMinPerfJoules},{self.powerMinPerfWatts}"
        )
        log.info(
            f"{self.model_name},192,{self.profileTotalEstimatedMacs},{self.profileTotalCycles},{self.powerMaxPerfInferenceTime},{self.powerMaxPerfJoules},{self.powerMaxPerfWatts}"
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
        if cpu_mode == "NS_MINIMUM_PERF":
            self.powerMinPerfInferenceTime = mSeconds
            self.powerMinPerfJoules = uJoules
            self.powerMinPerfWatts = mWatts
        else:
            self.powerMaxPerfInferenceTime = mSeconds
            self.powerMaxPerfJoules = uJoules
            self.powerMaxPerfWatts = mWatts

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

    # check if params.destination_rootdir is relative path. If it is, make it absolute so that measure_power.py can use it
    if not os.path.isabs(params.destination_rootdir):
        params.destination_rootdir = os.path.abspath(params.destination_rootdir)
    
    # set logging level
    log.basicConfig(
        level=log.DEBUG
        if params.verbosity > 2
        else log.INFO
        if params.verbosity > 1
        else log.WARNING,
        format="%(levelname)s: %(message)s",
    )    

    # check if destination_rootdir is within neuralSPOT 
    destination_path = Path(params.destination_rootdir).resolve()
    is_subdir = any(part.name == "neuralSPOT" for part in destination_path.parents) or destination_path.name == "neuralSPOT"
    if not is_subdir:
        log.error("Destination rootdir cannot be outside of neuralSPOT")
        exit("Autodeploy failed")

    # check if tflite-filename was specified
    if not os.path.exists(params.tflite_filename):
        log.error("TFLite file not found. Please specify a valid path.")
        exit("Autodeploy failed")
    if not params.tflite_filename.endswith('.tflite'):
        log.error("The specified file is not a TFLite file. Please provide a file with a .tflite extension.")
        exit("ns_perf failed")

    # check if neuralspot-rootdir is valid
    if not os.path.exists(os.path.abspath(params.neuralspot_rootdir)):
        log.error("NeuralSPOT directory not found. Please specify a valid path.")
        exit("ns_autodeploy failed")

    results = adResults(params)

    print("")  # put a blank line between obnoxious TF output and our output
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

    interpreter = get_interpreter(params)

    # Pickle the model details for later use
    # mc_file = open("model_config.pkl", "wb")
    # pickle.dump(mc, mc_file)
    # mc_file.close()

    # if params.create_binary or params.create_profile:
    #     # Configure the model on the EVB
    #     client = rpc_connect_as_client(params)


    pkl_dir = params.destination_rootdir + "/" + params.model_name + "/"
    mc_name = pkl_dir + f"{params.model_name}_mc.pkl"
    md_name = pkl_dir + f"{params.model_name}_md.pkl"
    results_name = pkl_dir + f"{params.model_name}_results.pkl"
    if (params.create_profile == False) or (params.create_binary == False):
        if not os.path.exists(mc_name) or not os.path.exists(md_name) or not os.path.exists(results_name):
            log.error(
                "Cannot skip create_profile and create_binary phases without having run them at least once. Please run the phases in order."
            )
            exit("Autodeploy failed")

        # Read MC and MD from Pickle file
        log.info("Reading MC and MD from Pickle files")
        mc_file = open(mc_name, "rb")
        mc = pickle.load(mc_file)
        mc_file.close()
        md_file = open(md_name, "rb")
        md = pickle.load(md_file)
        md_file.close()
        results_file = open(results_name, "rb")
        results = pickle.load(results_file)
        results_file.close()
        if params.create_profile == True or params.create_binary == True:
            total_stages -= 1
    else:
        mc = ModelConfiguration(params)
        md = ModelDetails(interpreter)
        print(
            f"*** Stage [{stage}/{total_stages}]: Create and fine-tune EVB model characterization image"
        )
        stage += 1
        create_validation_binary(params, True, mc)
        client = rpc_connect_as_client(params)

        configModel(params, client, md)

        stats = getModelStats(params, client)
        mc.update_from_stats(stats, md)
        # check common params mistakes
        mc.check(params)

        # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
        create_validation_binary(params, False, mc)
        client = rpc_connect_as_client(params)  # compiling resets EVB, need reconnect
        configModel(params, client, md)

        #Create profile
        print("")
        print(
            f"*** Stage [{stage}/{total_stages}]: Characterize model performance on EVB"
        )
        stage += 1
        differences = validateModel(params, client, interpreter, md, mc)
        # Get profiling stats
        stats = getModelStats(params, client)
        stats_filename = (
            params.destination_rootdir
            + "/"
            + params.model_name
            + "/"
            + params.model_name
            + "_stats.csv"
        )
        cycles, macs, time, layers = printStats(stats, stats_filename)
        results.setProfile(time, macs, cycles, layers)

        otIndex = 0
        for d in differences:
            log.info(
                f"Model Output Comparison: Mean difference per output label in tensor({otIndex}): "
                + repr(np.array(d).mean(axis=0))
            )
            otIndex += 1

        # Pickle the model details for later use
        log.info("Writing MC and MD to Pickle files")
        mc_file = open(mc_name, "wb")
        pickle.dump(mc, mc_file)
        mc_file.close()
        md_file = open(md_name, "wb")
        pickle.dump(md, md_file)
        md_file.close()
        results_file = open(results_name, "wb")
        pickle.dump(results, results_file)
        results_file.close()

    if params.onboard_perf:
        print("")
        print(
            f"*** Stage [{stage}/{total_stages}]: Characterize inference energy consumption on EVB onboard measurements"
        )
        generatePowerBinary(params, mc, md, params.cpu_mode)
        print(
            f"{params.cpu_mode} Performance code flashed to EVB - connect to SWO and press reset to see results."
        )
        stage += 1

    elif params.joulescope:
        print("")
        print(
            f"*** Stage [{stage}/{total_stages}]: Characterize inference energy consumption on EVB using Joulescope"
        )

        for cpu_mode in ["NS_MINIMUM_PERF", "NS_MAXIMUM_PERF"]:
            # for cpu_mode in ["NS_MINIMUM_PERF"]:
            generatePowerBinary(params, mc, md, cpu_mode)
            td, i, v, p, c, e = measurePower()
            energy = (e["value"] / params.runs_power) * 1000000  # Joules
            t = (td.total_seconds() * 1000) / params.runs_power
            w = (e["value"] / td.total_seconds()) * 1000
            results.setPower(cpu_mode=cpu_mode, mSeconds=t, uJoules=energy, mWatts=w)
            log.info(
                f"Model Power Measurement in {cpu_mode} mode: {t:.3f} ms and {energy:.3f} uJ per inference (avg {w:.3f} mW))"
            )
        stage += 1

    if params.create_library:
        print("")
        print(f"*** Stage [{stage}/{total_stages}]: Generate minimal static library")
        generateModelLib(params, mc, md, ambiqsuite=False)
        stage += 1

    if params.create_ambiqsuite_example:
        print("")
        print(f"*** Stage [{stage}/{total_stages}]: Generate AmbiqSuite Example")
        generateModelLib(params, mc, md, ambiqsuite=True)
        stage += 1

    results.print()

if __name__ == "__main__":
    main()