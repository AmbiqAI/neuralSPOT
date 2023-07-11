import logging as log

import numpy as np
import pydantic_argparse
from autodeploy.gen_library import generateModelLib
from autodeploy.validator import (
    ModelConfiguration,
    configModel,
    create_validation_binary,
    get_interpreter,
    getModelStats,
    printStats,
    validateModel,
)
from ns_utils import ModelDetails, reset_dut, rpc_connect_as_client
from pydantic import BaseModel, Field


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
    tflite_filename: str = Field("model.tflite", description="Name of tflite model")

    # Create Binary Parameters
    tflm_filename: str = Field("mut_model_data.h", description="Name of TFLM C file")
    tflm_src_path: str = Field(
        # "../examples/tflm_validator/src",
        "../projects/models/tflm_validator/src",
        description="Path to Validator example src directory",
    )
    max_arena_size: int = Field(
        120, description="Maximum KB to be allocated for TF arena"
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
    runs: int = Field(100, description="Number of inferences to run")

    # Library Parameters
    model_name: str = Field(
        "model", description="Name of model to be used in generated library"
    )
    model_path: str = Field(
        "../projects/models",
        description="Directory where generated library will be placed",
    )

    # Profile Parameters
    profile_warmup: int = Field(1, description="How many inferences to profile")
    stats_filename: str = Field(
        "stats.csv", description="Name of exported profile statistics CSV file"
    )

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


if __name__ == "__main__":
    # parse cmd parameters
    parser = create_parser()
    params = parser.parse_typed_args()
    print("")  # put a blank line between obnoxious TF output and our output

    # set logging level
    log.basicConfig(
        level=log.DEBUG
        if params.verbosity > 2
        else log.INFO
        if params.verbosity > 1
        else log.WARNING,
        format="%(levelname)s: %(message)s",
    )

    interpreter = get_interpreter(params)

    print("*** Phase 1: Create and fine-tune EVB image")
    mc = ModelConfiguration(params)
    md = ModelDetails(interpreter)

    if params.create_binary:
        create_validation_binary(params, True, mc)
    else:
        reset_dut()

    # Configure the model on the EVB
    client = rpc_connect_as_client(params)

    configModel(params, client, md)
    stats = getModelStats(params, client)
    mc.update_from_stats(stats, md)
    mc.check(params)

    # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
    if params.create_binary:
        create_validation_binary(params, False, mc)
        client = rpc_connect_as_client(params)  # compiling resets EVB, need reconnect
        configModel(params, client, md)

    print("*** Phase 2: Characterize model performance on EVB")

    differences = validateModel(params, client, interpreter, md, mc)
    if params.create_profile:
        # Get profiling stats
        stats = getModelStats(params, client)
        printStats(stats, params.stats_filename)

    otIndex = 0
    for d in differences:
        print(
            f"Model Output Comparison: Mean difference per output label in tensor({otIndex}): "
            + repr(np.array(d).mean(axis=0))
        )
        otIndex += 1

    if params.create_library:
        print("*** Phase 3: Generate minimal static library")
        generateModelLib(params, mc, md)
