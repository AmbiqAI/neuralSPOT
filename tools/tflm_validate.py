#!/usr/bin/python

import argparse
import os
import random
import struct
import sys
import time

from tqdm import tqdm

sys.path.append("../neuralspot/ns-rpc/python/ns-rpc-genericdata/")

from pathlib import Path

import erpc

# import GenericDataOperations_EvbToPc
import GenericDataOperations_PcToEvb
import numpy as np
import pydantic_argparse

# import soundfile as sf
import tensorflow as tf
from pydantic import BaseModel, Field

input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input


class Params(BaseModel):
    seed: int = Field(42, description="Random Seed")
    trained_model_dir: Path = Field(
        "trained_models/", description="Directory where trained models are stored"
    )
    job_dir: Path = Field(
        "artifacts/", description="Directory where artifacts are stored"
    )
    dataset_dir: Path = Field(
        "datasets/", description="Directory where datasets reside"
    )
    dataset: str = Field("dataset.pkl", description="Name of dataset")
    tflite_filename: str = Field("model.tflite", description="Name of tflite model")
    tty: str = Field("/dev/tty.usbmodem1234561", description="Serial device")
    baud: str = Field("115200", description="Baud rate")
    runs: int = Field(1000, description="Number of invokes to run")


# Define the RPC service handlers - one for each EVB-to-PC RPC function


def printDataBlock(block):
    print("Description: %s" % block.description)
    print("Length: %s" % block.length)
    print("cmd: %s" % block.cmd)
    print("dType: %s" % block.dType)
    # print(block.buffer)
    for i in range(len(block.buffer)):
        print("0x%x " % block.buffer[i], end="")
    print("")


def validateModel(params, transport):
    clientManager = erpc.client.ClientManager(transport, erpc.basic_codec.BasicCodec)
    client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
    print("\r\nClient started - press enter send remote procedure calls to EVB")
    input_fn()

    # Load TFLite Model
    interpreter = tf.lite.Interpreter(model_path=params.tflite_filename)
    interpreter.allocate_tensors()

    # Get input/output details, configure EVB
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    sl = interpreter.get_signature_list()
    ts = interpreter.get_tensor_details()

    numberOfInputs = len(input_details)
    if numberOfInputs > 1:
        print(
            "Warning: model has %d inputs but this scripts only supports 1"
            % numberOfInputs
        )

    numberOfOutputs = len(output_details)
    if numberOfOutputs > 1:
        print(
            "Warning: model has %d outputs but this scripts only supports 1"
            % numberOfOutputs
        )

    print(input_details[0]["shape"])
    inputShape = input_details[0]["shape"]
    inputLength = np.prod(inputShape)  # assuming bytes
    print(inputLength)
    outputShape = output_details[0]["shape"]
    outputLength = np.prod(outputShape)  # assuming bytes

    configBytes = struct.pack("<III", 0, inputLength, outputLength)
    configModel = GenericDataOperations_PcToEvb.common.dataBlock(
        description="Model Config",
        dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
        cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
        # buffer=bytearray([0, 10, 20, 30]),
        buffer=configBytes,
        length=9,
    )
    stat = client.ns_rpc_data_sendBlockToEVB(configModel)
    print("Config Model Status = %d" % stat)

    runs = params.runs
    print("Calling invoke %d times." % runs)
    differences = np.zeros((runs, outputLength))
    for i in tqdm(range(runs)):
        # Generate random input
        input_data = np.random.randint(-127, 127, size=tuple(inputShape), dtype=np.int8)
        # print(input_data)

        # Invoke locally and on EVB
        interpreter.set_tensor(input_details[0]["index"], input_data)
        interpreter.invoke()
        output_data = interpreter.get_tensor(output_details[0]["index"])

        # Do it on EVB
        inputTensor = GenericDataOperations_PcToEvb.common.dataBlock(
            description="Input Tensor",
            dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
            cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
            buffer=input_data.flatten().tobytes(),
            length=inputLength,
        )
        outputTensor = erpc.Reference()

        # print("Sending to EVB")
        stat = client.ns_rpc_data_computeOnEVB(inputTensor, outputTensor)
        # print("Status is %d" % stat)

        # input_scale, input_zero_point = input_details[0]["quantization"]
        # output_scale, output_zero_point = output_details[0]["quantization"]

        # Compare outputs
        # print(output_data)
        # printDataBlock(outputTensor.value)
        out_array = struct.unpack(
            "<" + "b" * len(outputTensor.value.buffer), outputTensor.value.buffer
        )
        # print(out_array)
        differences[i] = output_data[0] - out_array
        # time.sleep(.1)
    print(differences)


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

    transport = erpc.transport.SerialTransport(params.tty, int(params.baud))

    validateModel(params, transport)
