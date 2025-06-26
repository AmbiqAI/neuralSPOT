import sys
import time

import pydantic_argparse
from neuralspot.tools.ns_utils import ModelDetails, reset_dut, rpc_connect_as_client
from pydantic import BaseModel, Field

import neuralspot.rpc.GenericDataOperations_PcToEvb as GenericDataOperations_PcToEvb


class Params(BaseModel):
    tflite_filename: str = Field(
        "model.tflite", description="Name of tflite model to be sent to EVB"
    )
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


def chunker(seq, size):
    return (seq[pos : pos + size] for pos in range(0, len(seq), size))


def send_model_to_evb(params, client):
    # Load the model as an array of bytes
    with open(params.tflite_filename, "rb") as f:
        model = f.read()

    # Send the model to the EVB in chunks
    for chunk in chunker(model, 2000):
        print("Sending chunk of size %d" % len(chunk))
        modelBlock = GenericDataOperations_PcToEvb.common.dataBlock(
            description="Model Chunk",
            dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
            cmd=GenericDataOperations_PcToEvb.common.command.read,  # re-use the read opcode
            buffer=chunk,
            # buffer=chunk.tobytes(),
            length=len(chunk),
        )
        status = client.ns_rpc_data_sendBlockToEVB(modelBlock)
        if status != 0:
            print("[ERROR] Model Send Status = %d" % status)
            exit("Model Send Failed")
        else:
            print("Model Send Return Status = %d" % status)


if __name__ == "__main__":
    # parse cmd parameters
    parser = create_parser()
    params = parser.parse_typed_args()
    client = rpc_connect_as_client(params)
    # time.sleep(2)
    send_model_to_evb(params, client)
    print("Model sent to EVB")
