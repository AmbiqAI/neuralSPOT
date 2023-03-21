import pydantic_argparse
from autodeploy.gen_library import generateModelLib
from autodeploy.validator import (
    checks,
    configModel,
    create_validation_binary,
    decodeStatsHead,
    get_interpreter,
    getModelStats,
    printStats,
    validateModel,
)
from ns_utils import getDetails, reset_dut, rpc_connect_as_client
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
    tflm_filename: str = Field("mut_model_data.h", description="Name of TFLM CC file")
    tflm_src_path: str = Field(
        "../examples/tflm_validator/src",
        description="Path to Validator example src directory",
    )
    max_arena_size: int = Field(
        120, description="Maximum KB to be allocated for TF arena"
    )
    max_rpc_buf_size: int = Field(
        4096, description="Maximum bytes to be allocated for RPC RX and TX buffers"
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

    if params.create_binary:
        create_validation_binary(params, True, 0, 0, 0)
    else:
        reset_dut()

    # Configure the model on the EVB
    client = rpc_connect_as_client(params)

    interpreter = get_interpreter(params)

    (
        numberOfInputs,
        numberOfOutputs,
        inputShape,
        outputShape,
        inputLength,
        outputLength,
    ) = getDetails(interpreter)

    configModel(params, client, inputLength, outputLength)
    stats = getModelStats(params, client)
    checks(params, stats, inputLength, outputLength)

    # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
    if params.create_binary:
        create_validation_binary(params, False, stats, inputLength, outputLength)
        client = rpc_connect_as_client(params)
        configModel(params, client, inputLength, outputLength)

    differences = validateModel(params, client, interpreter)
    if params.create_profile:
        # Get profiling stats
        stats = getModelStats(params, client)
        printStats(stats, params.stats_filename)

    # print(differences)
    print("Mean difference per output label: " + repr(differences.mean(axis=0)))

    if params.create_library:
        arena_size, _, _, _ = decodeStatsHead(stats)
        generateModelLib(params, arena_size)
