#!/usr/bin/env python
import logging as log
import pickle
import yaml
import struct
import os
import numpy as np
import pandas as pd
import pydantic_argparse
from tabulate import tabulate
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
import erpc


def collectProfilerStats(params, client):
    modelStatPreambleSize = 16  # number of uint32_t words
    statBlock = erpc.Reference()
    log.info("Fetching model stats")
    status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
    log.info("Fetch stats status %d" % status)
    if status != 0:
        print("[ERROR] Model Stats Fetch Status = %d" % status)
        exit("Model Stats Fetch Failed")

    def parse_csv_header(byte_data):
        # Find the position of the first null byte
        null_pos = byte_data.find(b'\x00')
        if null_pos == -1:
            raise ValueError("No null byte found in the data")
        
        # Extract the valid portion of the string
        valid_str = byte_data[:null_pos].decode('ascii')
        
        # Split by comma and strip whitespace
        valid_str = valid_str.replace(' ', '')
        parsed_list = [item.strip() for item in valid_str.split(',')]
        if parsed_list[-1] == '':
            parsed_list = parsed_list[:-1]
        
        return parsed_list
    stat_array = struct.unpack(
            "<" + "I" * (modelStatPreambleSize//4), statBlock.value.buffer[: modelStatPreambleSize])

    pmu_data = statBlock.value.buffer[modelStatPreambleSize:]
    pmu_count, platform, captured_events, computed_stats_buffer_size = stat_array

    pmu_array = struct.unpack("<" + "I" * (pmu_count), pmu_data[:pmu_count*4])

    str_data = pmu_data[(pmu_count+1)*4:]

    csv_header = parse_csv_header(str_data)
    pmu_array = [pmu_array[:len(csv_header)]]
    df = pd.DataFrame(columns = csv_header, data = pmu_array) 
    return df 

def getProfilerStats(params, stats, stats_filename, pmu_csv_header, overall_pmu_stats):
    computed_stat_per_event_size = (
        stats[2] // 4
    )  # stats[2] is in bytes, convert to uint32
    pmu_count = stats[3]  # generally one event per model layer
    pmu_events_per_layer = stats[4]  # generally one event per model layer
    captured_events = stats[5]  # generally one event per model layer
    platform = stats[6]  # AP3, AP4, AP5, etc
    if params.full_pmu_capture:
        log.info("Including Full PMU Capture")
        csv_header = pmu_csv_header
    else:
        csv_header = stats[7:7 + 128] # 512 bytes
        # convert csv words to bytes
        csv_header = struct.pack("<" + "I" * 128, *csv_header)
        # find the null terminator
        csv_header = csv_header.split(b"\x00")[0]
        # convert null-terminated array to string
        csv_header = "".join([chr(c) for c in csv_header])
        # print(csv_header)
    offset = modelStatPreambleSize  # size of stats preamble
    if computed_stat_per_event_size != 0:
        if ((len(stats) - offset) // computed_stat_per_event_size) < captured_events:
            captured_events = (len(stats) - offset) // computed_stat_per_event_size
            log.warning(
                "Number of events greater than allowed for by RPC buffer size (suggestion: increase NS_PROFILER_RPC_EVENTS_MAX). Statistics will be truncated to %d events"
                % captured_events
            )

    # convert csv header to table with elements separated by commas
    csv_elements = csv_header.split(",")
    table = [csv_elements]
    for i in range(captured_events):
        row = []
        time = stats[offset + 15]
        macs = stats[offset + 14]
        tag = str(
            struct.unpack(
                "<20s",
                struct.pack(
                    "<IIIII", stats[offset + 16], stats[offset + 17], stats[offset + 18], stats[offset + 19], stats[offset + 20]
                ),
            )[0],
            "utf-8",
        )

        # Get rid of \0x00 characters
        tag = tag.replace("\x00", "")

        row.append(i)
        row.append(tag)
        row.append(time)
        row.append(macs)
        if platform == 5:
            totalCycles = 0
        else:
            totalCycles += stats[offset + 8]
        # print(f"Layer {i} {tag} time {time} MACs {macs} cycles {stats[offset + 8]}")
        # print(platform)
        if platform == 5:
            # AP5, so only PMU counters - either 4 (normal mode) or all of them (full mode)
            
            if params.full_pmu_capture:
                row.append(0)
                row.append(0)
                row.append(0)
                row.append(0)
                row.append(0)
                row.append(0)
                row.append(0)
                row.append(0)
                # log.info(f"Full PMU Capture, {len(overall_pmu_stats)} layers")
                for event in overall_pmu_stats[i]:
                    row.append(event)
            else:
                for j in range(4):
                    row.append(stats[offset + j])
        else:
            # print("offset is ", offset)
            for j in range(6):
                row.append(stats[offset + 8 + j])
            for j in range(8):

                row.append(stats[offset + j])
        table.append(row)
        offset = offset + computed_stat_per_event_size

    log.info(tabulate(table, headers="firstrow", tablefmt="simple"))
    totalMacs = totalTime = totalCycles = 1 
    log.info(
        "Model Performance Analysis: Per-layer performance statistics saved to: %s"
        % stats_filename
    )

    # print("table is ", table)
    # for r in table:
    #     print(r)

    np.savetxt(stats_filename + ".csv", table, delimiter=", ", fmt="% s")


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
        False,
        description="create binary",
    )
    create_profile: bool = Field(
        True, description="Profile the performance of the model on the EVB"
    )
    onboard_perf: bool = Field(
        False, 
        description="Capture and print performance measurements on EVB"
    )
    full_pmu_capture: bool = Field(
        True,
        description="Capture full PMU data during performance measurements on EVB",
    )

    configfile: str = Field(
        "", description="Optional configuration file for parameters"
    )
    pmu_config_file: str = Field(
        "default", description="M55 PMU configuration override file for peformance profiling"
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
        self.powerIterations = 0 
        # self.stats_filename = p.stats_filename
        self.model_name = "rpc_profiler" 
        self.p = p
        self.model_size = 0

    def print(self):
        print((f"Characterization Report"))
        print(
            f"[Profile] Per-Layer Statistics file:         {self.model_name}_stats.csv"
        )
        print(f"[Profile] Total Model Layers:                {self.profileTotalLayers}")
    
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


    params.neuralspot_rootdir = str(params.neuralspot_rootdir)

    if not os.path.isabs(params.neuralspot_rootdir):
        params.neuralspot_rootdir = os.path.abspath(params.neuralspot_rootdir)

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

    # Override any 'auto' params with platform defaults (except arena location because 
    # true size isn't known yet)
    if params.ambiqsuite_version == "auto":
        params.ambiqsuite_version = pc.platform_config["as_version"]
        print(f"[NS] Using AmbiqSuite Version: {params.ambiqsuite_version}")
    
    if params.tensorflow_version == "auto":
        print(f"[NS] Using TensorFlow Version: {params.tensorflow_version}")

    if params.transport == "auto":
        # Set it to USB if available, otherwise UART
        params.transport = "USB" if pc.GetSupportsUsb() else "UART"

    log.info(f"[NS] Using transport: {params.transport}")



    if params.create_binary:
        raise Exception("no support for binary creation") 
        #create binary 

    if params.create_profile:
        print("")
        # Get profiling stats
        client = rpc_connect_as_client(params)  # compiling resets EVB, need reconnect

        #stats = collectProfilerStats(params, client)

        # Run the function 100 times and collect results.
        dfs = [collectProfilerStats(params, client) for _ in range(100)]

        # Average the dataframes (assuming they have the same index/structure).
        df_avg = pd.concat(dfs).groupby(level=0).mean()

        # Add 'version' as the first column.
        stats = df_avg

        cols_to_front = [
            "ARM_PMU_BUS_CYCLES",
            "ARM_PMU_MEM_ACCESS",
            "ARM_PMU_MVE_INST_RETIRED",
            "ARM_PMU_MVE_LDST_RETIRED",
            "ARM_PMU_LD_RETIRED",
            "ARM_PMU_ST_RETIRED",
            "ARM_PMU_INST_RETIRED",
            "ARM_PMU_CPU_CYCLES",
            "ARM_PMU_MVE_STALL",
            "ARM_PMU_DTCM_ACCESS",
            "ARM_PMU_ITCM_ACCESS"
        ]

        remaining_cols = [col for col in stats.columns if col not in cols_to_front]
        stats = stats[cols_to_front + remaining_cols]

        stats.insert(0, 'version', params.tensorflow_version)
        
        file_path=  "rpc_profiler_stats.csv"

        if os.path.exists(file_path):
            # Append row without headers.
            stats.to_csv(file_path, mode='a', header=False, index=False)
        else:
            # Write new file with headers.
            stats.to_csv(file_path, mode='w', header=True, index=False)
        # If full PMU capture is enabled, get the full PMU data by calling getPMUStats


if __name__ == "__main__":
    main()
