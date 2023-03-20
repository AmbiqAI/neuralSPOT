#!/usr/bin/python

import argparse
import gzip
import math
import os
import random
import struct
import sys
import time

import GenericDataOperations_PcToEvb
from autodeploy.gen_library import generateModelLib
from autodeploy.validator import (
    checks,
    compile_and_deploy,
    configModel,
    create_mut_metadata,
    decodeStatsHead,
    getModelStats,
    printStats,
    validateModel,
)
from ns_utils import getDetails, reset_dut, xxd_c_dump
from tabulate import tabulate
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

# input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input


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
        print("[INFO] Compiling and deploy baseline image (large arena and buffers)")
        compile_and_deploy(params, first_time=True)
    else:
        reset_dut()

    # Configure the model on the EVB
    try:
        transport = erpc.transport.SerialTransport(params.tty, int(params.baud))
        clientManager = erpc.client.ClientManager(
            transport, erpc.basic_codec.BasicCodec
        )
        client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
    except:
        print("Couldn't establish RPC connection EVB USB device %s" % params.tty)

    # tf.lite.experimental.Analyzer.analyze(model_path=params.tflite_filename)

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

    configModel(params, client, inputLength, outputLength)
    stats = getModelStats(params, client)
    checks(params, stats, inputLength, outputLength)

    # We now know RPC buffer sizes and Arena size, create new metadata file and recompile
    if params.create_binary:
        create_mut_metadata(params, tflm_dir, stats, inputLength, outputLength)
        print("[INFO] Compiling and deploy tuned image (detected arena and buffers)")
        compile_and_deploy(params, first_time=False)
        try:
            transport = erpc.transport.SerialTransport(params.tty, int(params.baud))
            clientManager = erpc.client.ClientManager(
                transport, erpc.basic_codec.BasicCodec
            )
            client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
        except:
            print("Couldn't establish RPC connection EVB USB device %s" % params.tty)
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
        generateModelLib(params, stats)
