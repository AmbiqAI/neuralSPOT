#!/usr/bin/python

import argparse
import gzip
import math
import os
import random
import struct
import sys
import time

from tqdm import tqdm

sys.path.append("../neuralspot/ns-rpc/python/ns-rpc-genericdata/")

import pickle
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
    dataset: str = Field("dataset.pkl", description="Name of dataset")
    tflite_filename: str = Field("model.tflite", description="Name of tflite model")
    tflm_filename: str = Field("mut_model_data.h", description="Name of TFLM CC file")
    tflm_src_path: str = Field(
        "../examples/tflm_validator/src",
        description="Path to Validator example src directory",
    )
    tty: str = Field("/dev/tty.usbmodem1234561", description="Serial device")
    baud: str = Field("115200", description="Baud rate")
    runs: int = Field(10, description="Number of invokes to run")
    random_data: bool = Field(True, description="Use random data")
    create_binary: bool = Field(
        True, description="Create a neuralSPOT binary based on TFlite file"
    )


def xxd_c_dump(
    src_path: str,
    dst_path: str,
    var_name: str = "mut_model",
    chunk_len: int = 12,
    is_header: bool = True,
):
    """Generate C like char array of hex values from binary source. Equivalent to `xxd -i src_path > dst_path`
        but with added features to provide # columns and variable name.
    Args:
        src_path (str): Binary file source path
        dst_path (str): C file destination path
        var_name (str, optional): C variable name. Defaults to 'g_model'.
        chunk_len (int, optional): # of elements per row. Defaults to 12.
    """
    var_len = 0
    with open(src_path, "rb", encoding=None) as rfp, open(
        dst_path, "w", encoding="UTF-8"
    ) as wfp:
        if is_header:
            wfp.write(f"#ifndef __{var_name.upper()}_H{os.linesep}")
            wfp.write(f"#define __{var_name.upper()}_H{os.linesep}")

        wfp.write(f"const unsigned char {var_name}[] = {{{os.linesep}")
        for chunk in iter(lambda: rfp.read(chunk_len), b""):
            wfp.write(
                "  " + ", ".join((f"0x{c:02x}" for c in chunk)) + f", {os.linesep}"
            )
            var_len += len(chunk)
        wfp.write(f"}};{os.linesep}")
        wfp.write(f"const unsigned int {var_name}_len = {var_len};{os.linesep}")
        if is_header:
            wfp.write(f"#endif // __{var_name.upper()}_H{os.linesep}")


# Define the RPC service handlers - one for each EVB-to-PC RPC function
def load_pkl(file: str, compress: bool = True):
    """Load pickled file.

    Args:
        file (str): File path (.pkl)
        compress (bool, optional): If file is compressed. Defaults to True.

    Returns:
        Any: Contents of pickle
    """
    if compress:
        with gzip.open(file, "rb") as fh:
            return pickle.load(fh)
    else:
        with open(file, "rb") as fh:
            return pickle.load(fh)


def get_dataset(params):
    fn = params.dataset
    if fn and os.path.isfile(fn):
        # print("Loading augmented dataset from " + fn)
        ds = load_pkl(fn)
        return ds["X"], ds["y"], ds["XT"], ds["yt"]


def printDataBlock(block):
    print("Description: %s" % block.description)
    print("Length: %s" % block.length)
    print("cmd: %s" % block.cmd)
    print("dType: %s" % block.dType)
    # print(block.buffer)
    for i in range(len(block.buffer)):
        print("0x%x " % block.buffer[i], end="")
    print("")


def getDetails(interpreter):
    # Get input/output details, configure EVB
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
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

    inputShape = input_details[0]["shape"]
    inputLength = np.prod(inputShape)  # assuming bytes
    print("[INFO] input tensor total size: %d" % inputLength)
    print(input_details[0]["shape"])
    print("[INFO] input tensor shape " + repr(input_details[0]["shape"]))
    outputShape = output_details[0]["shape"]
    outputLength = np.prod(outputShape)  # assuming bytes
    return (
        numberOfInputs,
        numberOfOutputs,
        inputShape,
        outputShape,
        inputLength,
        outputLength,
    )


def configModel(params, client):
    configBytes = struct.pack("<III", 0, inputLength, outputLength)
    configModel = GenericDataOperations_PcToEvb.common.dataBlock(
        description="Model Config",
        dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
        cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
        buffer=configBytes,
        length=9,
    )
    status = client.ns_rpc_data_sendBlockToEVB(configModel)
    print("[INFO] Model Configuration Return Status = %d" % status)


def getModelStats(params, client):
    statBlock = erpc.Reference()
    status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
    print("[INFO] Fetch stats status %d" % status)
    # print(statBlock)
    # print(len(statBlock.value.buffer))
    stat_array = struct.unpack(
        "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
    )
    return stat_array


def validateModel(params, client):
    # Get input/output details
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    runs = params.runs
    print("[INFO] Calling invoke %d times." % runs)

    if not params.random_data:
        # Load validation data from specified pkl file
        print("[INFO] Load dataset from %s." % params.dataset)
        data, labels, test_data, test_labels = get_dataset(params)
        input_scale, input_zero_point = input_details[0]["quantization"]
        test_data_int8 = np.asarray(
            test_data / input_scale + input_zero_point, dtype=np.int8
        )
    else:
        print("[INFO] Generate random dataset.")

    differences = np.zeros((runs, outputLength))
    for i in tqdm(range(runs)):
        # Generate random input
        if params.random_data:
            input_data = np.random.randint(
                -127, 127, size=tuple(inputShape), dtype=np.int8
            )
        else:
            input_data = np.array([test_data_int8[i]])
        # print("Input data")
        # print(input_data)
        # print("first 5 bytes")
        # for j in range(5):
        #     print ("0x%s " % hex(input_data.flatten()[j]), end="")
        # print ("")
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

        stat = client.ns_rpc_data_computeOnEVB(inputTensor, outputTensor)
        out_array = struct.unpack(
            "<" + "b" * len(outputTensor.value.buffer), outputTensor.value.buffer
        )
        differences[i] = output_data[0] - out_array
        # print(output_data[0])
        # print(out_array)
    # print(differences.mean(axis=0))
    return differences


def next_power_of_2(x):
    return 1 if x == 0 else 2 ** math.ceil(math.log2(x))


def create_mut_metadata(tflm_dir, stats, inputLength, outputLength):
    # Print out tuned metadata file
    # print("hello")
    code = """
// Model Under Test (MUT) Metadata.
// This file is automatically generated by neuralspot's tflm_validate tool
#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H

// Calculated Arena and RPC buffer sizes
"""
    code = code + "#define TFLM_VALIDATOR_ARENA_SIZE " + repr(stats[0] // 1024) + "\n"
    code = (
        code
        + "#define TFLM_VALIDATOR_RX_BUFSIZE "
        + repr(max(next_power_of_2(inputLength), 512))
        + "\n"
    )
    code = (
        code
        + "#define TFLM_VALIDATOR_TX_BUFSIZE "
        + repr(max(next_power_of_2(outputLength), 512))
        + "\n"
    )
    code = code + "#endif // __MUT_MODEL_METADATA_H"
    with open(tflm_dir + "/" + "mut_model_metadata.h", "w") as f:
        f.write(code)
    # print(code)


def compile_and_deploy():
    # Compile & Deploy
    makefile_result = os.system(
        "cd .. && make -j >/dev/null 2>&1 && make TARGET=tflm_validator deploy >/dev/null 2>&1"
    )
    if makefile_result != 0:
        print("[ERROR] Make failed, return code %d", makefile_result)
        return makefile_result

    time.sleep(2)
    makefile_result = os.system("cd .. && make reset >/dev/null 2>&1")
    time.sleep(2)  # give jlink a chance to settle
    return makefile_result


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

    tflm_dir = params.tflm_src_path

    if params.create_binary:
        xxd_c_dump(
            src_path=params.tflite_filename,
            dst_path=tflm_dir + "/" + params.tflm_filename,
            var_name="mut_model",
            chunk_len=12,
            is_header=True,
        )
        # Copy default metadata to metadata header to start from vanilla configuration
        os.system(
            "cp %s/mut_model_metadata_default.h %s/mut_model_metadata.h >/dev/null 2>&1"
            % (tflm_dir, tflm_dir)
        )
        compile_and_deploy()

    # Configure the model on the EVB
    try:
        transport = erpc.transport.SerialTransport(params.tty, int(params.baud))
        clientManager = erpc.client.ClientManager(
            transport, erpc.basic_codec.BasicCodec
        )
        client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
    except:
        print("Couldn't establish RPC connection EVB USB device %s" % params.tty)

    interpreter = tf.lite.Interpreter(model_path=params.tflite_filename)
    interpreter.allocate_tensors()

    (
        numberOfInputs,
        numberOfOutputs,
        inputShape,
        outputShape,
        inputLength,
        outputLength,
    ) = getDetails(interpreter)

    configModel(params, client)
    stats = getModelStats(params, client)

    # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
    if params.create_binary:
        create_mut_metadata(tflm_dir, stats, inputLength, outputLength)
        compile_and_deploy()
        try:
            transport = erpc.transport.SerialTransport(params.tty, int(params.baud))
            clientManager = erpc.client.ClientManager(
                transport, erpc.basic_codec.BasicCodec
            )
            client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
        except:
            print("Couldn't establish RPC connection EVB USB device %s" % params.tty)
        configModel(params, client)

    # if params.characterize
    # flash model with large arena, run it once to figure out arena_size
    # update code, compile and flash again with Profiler enabled

    differences = validateModel(params, client)
    print(differences)
    print(differences.mean(axis=0))
