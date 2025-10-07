import logging as log
import os
import shutil
import struct
import sys
import time
import importlib.resources
import erpc
import numpy as np
import pandas as pd
from typing import Optional
# Set multiple environment variables to ensure XNNPACK is disabled
os.environ.setdefault("TF_LITE_DISABLE_XNNPACK", "1")
os.environ["TF_LITE_DISABLE_XNNPACK"] = "1"
os.environ["TF_LITE_DISABLE_DELEGATES"] = "1"
os.environ["TF_LITE_DISABLE_DEFAULT_DELEGATES"] = "1"
os.environ["TF_LITE_DISABLE_DELEGATE_PLUGINS"] = "1"
os.environ["TF_LITE_USE_XNNPACK"] = "0"
os.environ["TF_LITE_EXPERIMENTAL_USE_XNNPACK"] = "0"
import ai_edge_litert as tflite
from neuralspot.tools.ns_tflite_analyze import analyze_tflite_file
from neuralspot.tools.ns_utils import (
    ModelDetails,
    TensorDetails,
    createFromTemplate,
    get_dataset,
    next_power_of_2,
    printDataBlock,
    reset_dut,
    xxd_c_dump,
    read_pmu_definitions,
    suppress_os_stdio
)
from neuralspot.tools.ns_mac_predictor import (
    compute_counts as _mve_compute_counts,
    FCSpec as _FCSpec,
    Conv2DSpec as _Conv2DSpec,
    DepthwiseConv2DSpec as _DWSpec,
    ElementwiseSpec as _EltSpec,
    ActivationSpec as _ActSpec,
)
from tabulate import tabulate
from tqdm import tqdm
from neuralspot.tools.utils.tflite_helpers import CreateAddFromSnakeOpName
from pathlib import Path
import neuralspot.rpc.GenericDataOperations_PcToEvb as GenericDataOperations_PcToEvb
import yaml

modelConfigPreambleSize = 7  # number of uint32_t words
modelStatPreambleSize = 7+128  # number of uint32_t words

# Max RPC Block Length is roughly 3000 bytes, per testing
maxRpcBlockLength = 3000
max_rpc_buf_size = 4096 # Hardcode this, it isnt really changeable


class ModelStructureDetails:
    def __init__(self, tflite_filename, model_name):
        (
            self.code,
            self.overallOpsNameList,
            self.overallMacEstimates,
            self.macEstimateStrings,
            self.opsetList,
            graph_count,
            self.overall_output_shapes,
            self.overall_dilation_h,
            self.overall_dilation_w,
            self.overall_stride_h,
            self.overall_stride_w,
            self.overall_mac_filter_list,
            self.overall_read_estimate_list,
            self.overall_write_estimate_list,
            self.output_magnitude_list,
            self.input_magnitude_list,
            self.overall_input_dtype_list,
            self.overall_output_dtype_list,
            self.overall_weight_dtype_list,
            self.overall_fused_activation_list,
            self.overall_elem_bits_list,
            self.overall_elem_vec_chunks_list,
            self.overall_elem_predicated_list,
        ) = analyze_tflite_file(tflite_filename, model_name)

        # Resource Variable Count corresponds to number of VAR_HANDLE ops in overallOpsNameList[0]
        self.rv_count = self.overallOpsNameList[0].count("VAR_HANDLE")

        # Handle subgraphs: this code assumes there are a maximum of two graphs, and
        # the base graph (0) calls the subgraph only once (via CALL_ONCE). In order to
        # align this case with EVB execution, we insert the subgraph into where the CALL_ONCE is found
        if graph_count == 1:
            self.macEstimates = self.overallMacEstimates[0]
            self.opsNameList = self.overallOpsNameList[0]
            self.estimateStrings = self.macEstimateStrings[0]
            self.outputShapes = self.overall_output_shapes[0]
            self.dilation_h = self.overall_dilation_h[0]
            self.dilation_w = self.overall_dilation_w[0]
            self.stride_h = self.overall_stride_h[0]
            self.stride_w = self.overall_stride_w[0]
            self.mac_filter_list = self.overall_mac_filter_list[0]
            self.read_estimate_list = self.overall_read_estimate_list[0]
            self.write_estimate_list = self.overall_write_estimate_list[0]
            self.output_magnitude = self.output_magnitude_list[0]
            self.input_magnitude = self.input_magnitude_list[0]
            # Analyzer elementwise hints (flattened)
            self.elem_bits = self.overall_elem_bits_list[0]
            self.elem_vec_chunks = self.overall_elem_vec_chunks_list[0]
            self.elem_predicated = self.overall_elem_predicated_list[0]
            self.fused_act = self.overall_fused_activation_list[0]
        elif graph_count == 2:
            self.macEstimates = []
            self.opsNameList = []
            self.estimateStrings = []
            self.outputShapes = []
            self.dilation_h = []
            self.dilation_w = []
            self.stride_h = []
            self.stride_w = []
            self.mac_filter_list = []
            self.read_estimate_list = []
            self.write_estimate_list = []
            self.output_magnitude = []
            self.input_magnitude = []
            self.elem_bits = []
            self.elem_vec_chunks = []
            self.elem_predicated = []
            self.fused_act = []
            baseGraphIndex = 0
            for op in self.overallOpsNameList[0]:
                self.macEstimates.append(self.overallMacEstimates[0][baseGraphIndex])
                self.estimateStrings.append(self.macEstimateStrings[0][baseGraphIndex])
                self.outputShapes.append(self.overall_output_shapes[0][baseGraphIndex])
                self.dilation_h.append(self.overall_dilation_h[0][baseGraphIndex])
                self.dilation_w.append(self.overall_dilation_w[0][baseGraphIndex])
                self.stride_h.append(self.overall_stride_h[0][baseGraphIndex])
                self.stride_w.append(self.overall_stride_w[0][baseGraphIndex])
                self.mac_filter_list.append(self.overall_mac_filter_list[0][baseGraphIndex])
                self.read_estimate_list.append(self.overall_read_estimate_list[0][baseGraphIndex])
                self.write_estimate_list.append(self.overall_write_estimate_list[0][baseGraphIndex])
                self.output_magnitude.append(self.output_magnitude_list[0][baseGraphIndex])
                self.input_magnitude.append(self.input_magnitude_list[0][baseGraphIndex])
                self.elem_bits.append(self.overall_elem_bits_list[0][baseGraphIndex])
                self.elem_vec_chunks.append(self.overall_elem_vec_chunks_list[0][baseGraphIndex])
                self.elem_predicated.append(self.overall_elem_predicated_list[0][baseGraphIndex])
                self.fused_act.append(self.overall_fused_activation_list[0][baseGraphIndex])
                self.opsNameList.append(op)
                if op == "CALL_ONCE":
                    # insert the subgraph
                    self.macEstimates.extend(self.overallMacEstimates[1])
                    self.opsNameList.extend(self.overallOpsNameList[1])
                    self.estimateStrings.extend(self.macEstimateStrings[1])
                    self.outputShapes.extend(self.overall_output_shapes[1])
                    self.dilation_h.extend(self.overall_dilation_h[1])
                    self.dilation_w.extend(self.overall_dilation_w[1])
                    self.stride_h.extend(self.overall_stride_h[1])
                    self.stride_w.extend(self.overall_stride_w[1])
                    self.mac_filter_list.extend(self.overall_mac_filter_list[1])
                    self.read_estimate_list.extend(self.overall_read_estimate_list[1])
                    self.write_estimate_list.extend(self.overall_write_estimate_list[1])
                    self.output_magnitude.extend(self.output_magnitude_list[1])
                    self.input_magnitude.extend(self.input_magnitude_list[1])
                    self.elem_bits.extend(self.overall_elem_bits_list[1])
                    self.elem_vec_chunks.extend(self.overall_elem_vec_chunks_list[1])
                    self.elem_predicated.extend(self.overall_elem_predicated_list[1])
                    self.fused_act.extend(self.overall_fused_activation_list[1])
                baseGraphIndex += 1

        self.layers = len(self.opsNameList)

    def getAddList(self):
        retval = ""
        for i, opname in self.opsetList[0].items():
            retval += f"resolver.{CreateAddFromSnakeOpName(opname)}();\n"
        return retval, len(self.opsetList[0])


class ExampleTensors:
    def __init__(self, inputTensors, outputTensors):
        self.inputTensors = inputTensors
        self.outputTensors = outputTensors


class ModelConfiguration:
    rv_count = 0  # Resource Variables needed by the model

    def __init__(self, params):
        self.arena_size_k = params.max_arena_size

        # TFLM arena_used_bytes(), which is used to calculate arena size, has a bug
        # where it does not account for scratch buffer padding. This is a workaround.
        self.arena_size_scratch_buffer_padding_k = (
            params.arena_size_scratch_buffer_padding
        )
        self.arena_size = params.max_arena_size * 1024
        self.stat_buffer_size = max_rpc_buf_size  # in bytes
        self.adjusted_stat_buffer_size = max_rpc_buf_size  # in bytes
        # ModelConfiguration.rv_count = params.resource_variable_count
        self.stat_per_event_size = (
            -1
        )  # dontcare, -1 will force an error if used elsewhere
        self.events = -1
        self.modelStructureDetails = ModelStructureDetails(params.tflite_filename, params.model_name)
        self.rv_count = self.modelStructureDetails.rv_count
        self.aot_layers = 0

    def update_from_stats(self, stats, md):
        self.arena_size = stats[0]  # in bytes
        self.arena_size_k = (stats[0] // 1024) + 1
        self.stat_buffer_size = stats[1]  # in bytes
        self.stat_per_event_size = stats[2]  # in bytes
        self.events = stats[3]  # generally one event per model layer
        self.platform = stats[4] # AP3, AP4, AP5, etc
        # next 512 bytes are the CSV header for stats, copy into self
        # for i in range(5, 5 + 512):
        #     self.csv_header += chr(stats[i])
        # print(self.csv_header)
        self.compute_buf_size(md)

    def compute_buf_size(self, md):
        self.adjusted_stat_buf_size = max(
            next_power_of_2(self.stat_buffer_size + 50),
            next_power_of_2(md.totalInputTensorBytes + 50),
            next_power_of_2(md.totalOutputTensorBytes + 50),
            512,  # min
        )

    def update_from_validation(self, inputTensors, outputTensors):
        self.exampleTensors = ExampleTensors(inputTensors, outputTensors)

    def check(self, params):
        if self.arena_size_k > params.max_arena_size:
            print(
                "[ERROR] TF Arena Size is %dk, exceeding limit of %dk."
                % (self.arena_size_k, params.max_arena_size)
            )
            exit("[ERROR] TF Arena Size is too large")
        if self.adjusted_stat_buffer_size > max_rpc_buf_size:
            log.info(
                "Needed RPC buffer size is %d, exceeding limit of %d. Switching to chunk mode."
                % (self.adjusted_stat_buffer_size, max_rpc_buf_size)
            )


def send_model_to_evb(params, client):
    # Load the model as an array of bytes
    with open(params.tflite_filename, "rb") as f:
        model = f.read()

    # Send the model to the EVB in chunks
    for chunk in chunker(model, maxRpcBlockLength):
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
        # else:
            # log.info("Model Send Return Status = %d" % status)
            # print(".", end="")


def configModel(params, client, md):
    if params.create_profile:
        prof_enable = 1  # convert to int just to be explicit for serialization
    else:
        prof_enable = 0
    # print(f"[DEBUG] Configuring model with prof_enable {prof_enable}")
    # Send the model before config
    if params.model_location == "PSRAM":
        log.info("Sending model to EVB over RPC")
        send_model_to_evb(params, client)
        # print("Model sent to EVB")

    inputTensorByteLengths = []
    for tensor in md.inputTensors:
        inputTensorByteLengths.append(tensor.bytes)

    outputTensorByteLengths = []
    for tensor in md.outputTensors:
        outputTensorByteLengths.append(tensor.bytes)

    configBytes = struct.pack(
        "<"
        + "I"
        * (
            modelConfigPreambleSize
            + len(inputTensorByteLengths)
            + len(outputTensorByteLengths)
        ),
        prof_enable,
        md.totalInputTensorBytes,
        md.totalOutputTensorBytes,
        params.profile_warmup - 1,
        md.numInputs,
        md.numOutputs,
        params.full_pmu_capture,
        *inputTensorByteLengths,
        *outputTensorByteLengths,
    )
    # print("Config il %d, ol %d" % (md.totalInputTensorBytes, md.totalOutputTensorBytes))
    configBlock = GenericDataOperations_PcToEvb.common.dataBlock(
        description="Model Config",
        dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
        cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
        buffer=configBytes,
        length=len(configBytes),
    )

    # This is the first RPC call after flashing the firmware, and sometimes the reset doesn't 'take'.
    # If the first RPC call fails, we'll try a reset and then try again.
    retries = 3
    while retries > 0:
        try:
            status = client.ns_rpc_data_sendBlockToEVB(configBlock)
        except:
            log.error("RPC call failed, retrying after reset")
            reset_dut(params)
            time.sleep(10)
            retries -= 1
        else:
            break
    # print(f"[DEBUG] Configuring model with prof_enable {prof_enable} done")
    if status != 0:
        print("[ERROR] Model Configuration Send Status = %d" % status)
        print(
            "[ERROR] This may be caused by allocating too little memory for the tensor arena."
        )
        print(
            "[ERROR] This script uses TFLM's arena_used_bytes() function to determine the arena size,"
        )
        print(
            "[ERROR] which has a bug where it does not account for scratch buffer padding."
        )
        print(
            "[ERROR] To manually add a padding for scratch buffers, use the --arena-size-scratch-buffer-padding option."
        )
        exit("Model Configuration Failed")
    else:
        log.info("Model Configuration Return Status = %d" % status)


def getModelStats(params, client):
    statBlock = erpc.Reference()
    log.info("Fetching model stats")
    status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
    log.info("Fetch stats status %d" % status)
    if status != 0:
        print("[ERROR] Model Stats Fetch Status = %d" % status)
        exit("Model Stats Fetch Failed")

    stat_array = struct.unpack(
        "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
    )

    if statBlock.value.description != "FullStats":
        # Stats are too long to fit in one xfer. Repeated calls to fetchBlock will return chunks
        while statBlock.value.description != "LastStats":
            status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
            log.info(
                "Fetch stats chunk status %d, msg %s"
                % (status, statBlock.value.description)
            )
            stat_array = stat_array + struct.unpack(
                "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
            )

    return stat_array

def getPMUStats(params, client, layer, num_events_per_layer):
    # This function is called after getModelStats has been called. At the point, the 
    # EVB is primed to send PMU stats - it will send the stats for each layer, one layer per call

    # The pmu stat block has this format: 
    # typedef union {
    #     ns_mut_stats_t stats;
    #     char bytes[sizeof(ns_mut_stats_t)];
    #     ns_pmu_stats_t pmu_stats;
    # } ns_outgoing_stats_t; 

    statBlock = erpc.Reference()
    # log.info("Fetching Full PMU stats for layer %d", layer)

    status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
    if status != 0:
        print("[ERROR] Model PMU Stats Fetch Status = %d" % status)
        exit("Model PMU Stats Fetch Failed")

    stat_array = struct.unpack(
        "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
    )

    if statBlock.value.description != "FullPMUStats":
        log.error("Expected FullPMUStats, got %s" % statBlock.value.description)
        exit("Model PMU Stats Fetch Failed")

    # PMU stats for a single layer will always fit in one RPC block
    # Decode the stats
    fetched_layer = stat_array[0]
    if fetched_layer != layer:
        log.error(f"Expected layer {layer}, got layer {fetched_layer}")
        exit("Model PMU Stats Fetch Failed")

    csv_header = stat_array[1:513]
    csv_header = struct.pack("<" + "I" * 512, *csv_header)
    csv_header = csv_header.split(b"\x00")[0]
    csv_header = "".join([chr(c) for c in csv_header])

    # The rest of the stats are the PMU stats for the layer
    pmu_stats = stat_array[513:513+num_events_per_layer]

    # Print the stats
    # log.info(csv_header)
    # log.info(pmu_stats)
    return csv_header, pmu_stats


def chunker(seq, size):
    return (seq[pos : pos + size] for pos in range(0, len(seq), size))


def sendLongInputTensor(client, input_data, chunkLen):
    """
    When a tensor exceeds the RPC size limit, we chunk it
    """
    # ChunkLen is in bytes, convert to word size
    chunkLen = chunkLen // input_data.flatten().itemsize
    # print("Chunking input tensor into %d byte chunks" % chunkLen)
    for chunk in chunker(input_data.flatten(), chunkLen):
        inputChunk = GenericDataOperations_PcToEvb.common.dataBlock(
            description="Input Chunk",
            dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
            cmd=GenericDataOperations_PcToEvb.common.command.write_cmd,
            buffer=chunk.tobytes(),
            length=len(chunk),
        )
        status = client.ns_rpc_data_sendBlockToEVB(inputChunk)


def validateModel(params, client, interpreter, md, mc):
    """
    Generates input vectors (random or from a pkl) and sends them to both
    the local TFLite model and the one running on the EVB. It compares the
    results (small differences are OK due to differing runtimes and CPU numeric
    behavior) and prints a simple report. If profiling is enabled, it leverages
    neuralSPOT microprofiler extension to report on per-layer performance, include
    latency, cache behavior, and more.
    """
    runs = params.runs
    print("[NS] Calling invoke on EVB %d times." % runs)

    if not params.random_data:
        # Load validation data from specified pkl file
        log.info("Loading dataset from %s." % params.dataset)
        data, labels, test_data, test_labels = get_dataset(params)
        input_scale = md.inputTensors[0].scale
        input_zero_point = md.inputTensors[0].zeroPoint
        test_data_int8 = np.asarray(
            test_data / input_scale + input_zero_point, dtype=np.int8
        )
    else:
        log.info("Generating random dataset.")

    differences = (
        []
    )  # accumulator for per-output differences between local and EVB models
    golden_output_tensors = []
    for i in range(md.numOutputs):
        differences.append([])
        golden_output_tensors.append([])

    for i in tqdm(range(runs)):
        inExamples = []
        # Generate or load data
        # Create and store in MC a seed for the random number generator
        np.random.seed(i)
        mc.random_seed = i

        if params.random_data:
            # Generate random input
            if md.inputTensors[0].type == np.int8:
                input_data = np.random.randint(
                    -127, 127, size=tuple(md.inputTensors[0].shape), dtype=np.int8
                )
            else:
                input_data = (
                    np.random.random(size=tuple(md.inputTensors[0].shape)).astype(
                        md.inputTensors[0].type
                    )
                    * 2
                    - 1
                )
        else:
            input_data = np.array([test_data_int8[i]])

        if i == 0:
            inExamples.append(input_data.flatten())  # Capture inputs for AutoGen

        # Invoke locally and on EVB
        interpreter.set_tensor(md.input_details[0]["index"], input_data)

        interpreter.invoke()  # local invoke

        # Prepare input tensors (or pre-send them if chunking is needed) for xmit to EVB
        if md.inputTensors[0].bytes > (maxRpcBlockLength): 
            log.info(
                f"Input tensor exceeds RPC buffer size, chunking from {md.inputTensors[0].bytes} to {maxRpcBlockLength}"
            )
            sendLongInputTensor(client, input_data, (maxRpcBlockLength))
            inputTensor = GenericDataOperations_PcToEvb.common.dataBlock(
                description="Empty Tensor",
                dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
                cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
                buffer=bytearray(),
                length=0,
            )
        else:
            log.info(
                f"Input tensor fits in RPC buffer, sending {md.inputTensors[0].bytes}"
            )
            inputTensor = GenericDataOperations_PcToEvb.common.dataBlock(
                description="Input Tensor",
                dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
                cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
                buffer=input_data.flatten().tobytes(),
                length=md.inputTensors[0].bytes,
            )
        outputTensor = (
            erpc.Reference()
        )  # empty outputTensor, will be filled in by EVB RPC call
        log.info("Calling EVB invoke")
        stat = client.ns_rpc_data_computeOnEVB(inputTensor, outputTensor)  # on EVB
        if stat != 0:
            print(f"[ERROR] EVB invoke returned {stat}")
            exit("EVB invoke failed")

        # print ("Stat is ", stat)
        # If the output tensor is too large to fit in one RPC block, we will receive it in chunks
        # by calling computeOnEVB again with an empty input tensor and a command of 'write'
        accTensor = bytearray()

        if outputTensor.value.description != "FullTensor":
            # print("Output tensor exceeds RPC buffer size, chunking")
            accTensor.extend(outputTensor.value.buffer)
            inputTensor.cmd = GenericDataOperations_PcToEvb.common.command.write_cmd
            while outputTensor.value.description != "LastTensor":
                stat = client.ns_rpc_data_computeOnEVB(
                    inputTensor, outputTensor
                )
                if stat != 0:
                    # print(f"[ERROR] EVB invoke returned {stat}")
                    exit("EVB invoke failed")
                accTensor.extend(outputTensor.value.buffer)
                # print(".", end="")
            outputTensor.value.buffer = accTensor
        # else:
            # print("Output tensor fits in RPC buffer")
            # print(outputTensor.value.description)

        # Resulting output tensors are packed into outputTensor buffer in order (0,1,...)
        # Each has a different length and type; unpack them into an array for later comparison.
        otOffset = 0
        otIndex = 0
        outExamples = []
        for ot in md.outputTensors:
            # print("Output tensor %d, %d bytes" % (otIndex, ot.bytes))
            # print (ot.type)
            if ot.type is np.int8:
                out_array = list(
                    struct.unpack_from(
                        "<" + "b" * ot.bytes, outputTensor.value.buffer, otOffset
                    )
                )
            elif ot.type is np.uint8:
                out_array = list(
                    struct.unpack_from(
                        "<" + "B" * ot.bytes, outputTensor.value.buffer, otOffset
                    )
                )
            elif ot.type is np.int16:
                out_array = list(
                    struct.unpack_from(
                        "<" + "h" * ot.words, outputTensor.value.buffer, otOffset
                    )
                )
            elif ot.type is np.float32:
                out_array = list(
                    struct.unpack_from(
                        "<" + "f" * ot.words, outputTensor.value.buffer, otOffset
                    )
                )

            local_output_data = interpreter.get_tensor(
                md.output_details[otIndex]["index"]
            ).flatten()

            if i == 0:
                outExamples.append(
                    np.array(out_array).flatten()
                )  # Capture outputs for AutoGen

            differences[otIndex].append(local_output_data - out_array)
            golden_output_tensors[otIndex].append(local_output_data)
            otOffset += ot.bytes
            otIndex += 1

        if i == 0:
            # capture first input and output tensor set
            # Input is from local TF, output is from EVB
            mc.update_from_validation(inExamples, outExamples)


    return differences, golden_output_tensors


def prune_for_aot(pmu_csv_header):
    # AOT headers start the same way as TFLM (Event,Tag,uSecond), but then skip all the columns
    # until the first PMU counter column.
    # The amount of columns will vary, so search for the first PMU counter column and return the header from that point on
    # The first PMU counter header is ARM_PMU_SW_INCR, and can appear anywhere as uSeconds
    columns = pmu_csv_header.split(",")
    for i, column in enumerate(columns):
        if column == "ARM_PMU_SW_INCR":
            return ",".join(columns[i:])
    return pmu_csv_header

# ---------------------------------------------------------------------------
# Helpers: analysis-ready Excel emission + dtype derivation
# ---------------------------------------------------------------------------
# def derive_elem_bits_per_layer(mc, md):
def derive_elem_bits_per_layer(mc):
    """
    Best-effort dtype per layer (8/16/32). Replace with analyzer-derived per-op dtype if available.
    Defaults to 8-bit unless float32 or int16 is detected in model tensors.
    """
    import numpy as _np
    # Heuristic: prefer output tensor dtype if any is not int8
    bits = 8
    # try:
        # t = md.outputTensors[0].type
        # if t is _np.float32:
        #     bits = 32
        # elif t is _np.int16:
        #     bits = 16
        # else:
        #     # Keep 8 as default for int8/uint8 paths
        #     bits = 8
    # except Exception:
    #     bits = 8
    return [bits] * len(getattr(mc.modelStructureDetails, "opsNameList", []))

def _emit_analysis_workbook(xlsx_path, df, core_mhz, elem_bits_per_layer):
    """
    Write an analysis-ready XLSX with two tabs:
      - Sheet 'Data'   : raw EVB stats exactly as provided in 'df' (no derived columns)
      - Sheet 'Derived': all predicted and calculated columns (formulas referencing Data!)
      - Sheet 'Config' : named CoreMHz cell
    """
    from contextlib import suppress
    import math

    
    # ---------------------------
    # 1) Data sheet (raw only)
    # ---------------------------
    df_data = df.copy()
    # Ensure key numeric columns are numeric in the raw dataframe (helps cached values)
    _num_cols_data = [
        "uSeconds", "EST_MAC", "ARM_PMU_CPU_CYCLES",
        "ARM_PMU_INST_RETIRED", "ARM_PMU_MVE_INST_RETIRED",
        "ARM_PMU_MVE_INT_MAC_RETIRED", "ARM_PMU_MVE_FP_MAC_RETIRED",
        "ARM_PMU_L1D_CACHE", "ARM_PMU_L1D_CACHE_REFILL",
        "ARM_PMU_L1I_CACHE", "ARM_PMU_L1I_CACHE_REFILL",
        "ARM_PMU_STALL_FRONTEND", "ARM_PMU_STALL_BACKEND", "ARM_PMU_STALL",
    ]

    for _c in _num_cols_data:
        if _c in df_data.columns:
            df_data[_c] = pd.to_numeric(df_data[_c], errors="coerce")

    # ---------------------------
    # 2) Derived sheet skeleton
    # ---------------------------
    nrows = len(df_data.index)
    # ElemBits values (list or default 8)
    if elem_bits_per_layer and len(elem_bits_per_layer) == nrows:
        _elem_bits_series = pd.Series(elem_bits_per_layer, dtype="float64")
    else:
        _elem_bits_series = pd.Series([8]*nrows, dtype="float64")

    # Predictor outputs as values (computed from raw Data rows)
    # (reuses the existing _predict_row helper below)
    def _parse_shape_list(s: str) -> list[int]:
        try:
            toks = [t.strip() for t in str(s).replace("x", "*").split("*") if t.strip()]
            return [int(t) for t in toks]
        except Exception:
            return []
    def _predict_row(row) -> tuple[int, int, int]:
        """Return (pred_int_mac, pred_vreduce_int, pred_pred_cycles) for a single Data row."""
        try:
            tag = str(row.get("Tag", "")).lower()
            elem_bits = int(row.get("ElemBits", 8)) if pd.notna(row.get("ElemBits", np.nan)) else 8
            # Use same width for activations & weights; swap in per-op widths here if available.
            act_bits = elem_bits
            w_bits = elem_bits

            out_shape = _parse_shape_list(row.get("OUTPUT_SHAPE", ""))
            filt_shape = _parse_shape_list(row.get("FILTER_SHAPE", ""))
            # Default stride/dilation to 1 (predictor uses out_shape when present)
            sh = int(row.get("STRIDE_H", 1)) if pd.notna(row.get("STRIDE_H", np.nan)) else 1
            sw = int(row.get("STRIDE_W", 1)) if pd.notna(row.get("STRIDE_W", np.nan)) else 1
            dh = int(row.get("DILATION_H", 1)) if pd.notna(row.get("DILATION_H", np.nan)) else 1
            dw = int(row.get("DILATION_W", 1)) if pd.notna(row.get("DILATION_W", np.nan)) else 1
            # Out type matches act_bits (8, 16, 32)
            if act_bits == 8:
                out_dtype = "int8"
            elif act_bits == 16:
                out_dtype = "int16"
            elif act_bits == 32:
                out_dtype = "int32"
            else:
                out_dtype = "int8"

            # out_dtype = str(row.get("Output_DType", row.get("Output_DTYPE", "")) or "")

            # Bail out with zeros if we lack shapes.
            if not out_shape or not filt_shape:
                return (0, 0, 0)
            
            # ---------------- Fused activation handling (conv/fc/dw) ----------------
            def _act_from_row() -> Optional[_ActSpec]:
                fa = str(row.get("FusedActivation", row.get("FUSED_ACT", "NONE")) or "NONE").upper()
                if fa in ("NONE", "", "0", "NA"):
                    return None
                # Map knowns; clamp bounds not carried in table (leave None)
                if fa in ("RELU", "RELU6", "RELU_N1_TO_1", "CLAMP"):
                    return _ActSpec(type=fa.lower(), bits=None)
                # Tanh/Logistic appear as separate ops, not a fused flag, but if they show up here, ignore (nonlinear, not modeled as fused in conv).
                return _ActSpec(type="none")
            
            # Dispatch by op kind
            if "depthwise" in tag:
                # FILTER_SHAPE: Kh * Kw * Cin * DepthMultiplier
                if len(filt_shape) < 4 or len(out_shape) < 4:
                    return (0, 0, 0)
                kh, kw, cin, dm = filt_shape[0], filt_shape[1], filt_shape[2], filt_shape[3]
                n, ho, wo, co = out_shape[0], out_shape[1], out_shape[2], out_shape[3]
                spec = _DWSpec(
                    n=n, h=ho, w=wo, cin=cin, kh=kh, kw=kw,
                    depth_multiplier=dm, stride=(sh, sw), dilation=(dh, dw),
                    out_shape=(n, ho, wo, co),
                )
                d = _mve_compute_counts("depthwise_conv2d", act_bits, w_bits, dw=spec, activation=_act_from_row(), out_dtype=out_dtype)
            elif "conv_2d" in tag or "conv2d" in tag:
                # FILTER_SHAPE: Cout * Cin * Kh * Kw
                if len(filt_shape) < 4 or len(out_shape) < 4:
                    return (0, 0, 0)
                cout, cin, kh, kw = filt_shape[0], filt_shape[1], filt_shape[2], filt_shape[3]
                n, ho, wo, co = out_shape[0], out_shape[1], out_shape[2], out_shape[3]
                # co may or may not equal cout (it should) – keep out_shape as truth
                spec = _Conv2DSpec(
                    n=n, h=ho, w=wo, cin=cin, kh=kh, kw=kw, cout=co,
                    stride=(sh, sw), dilation=(dh, dw),
                    out_shape=(n, ho, wo, co),
                )
                d = _mve_compute_counts("conv2d", act_bits, w_bits, conv2d=spec, activation=_act_from_row(), out_dtype=out_dtype)
            elif "fully_connected" in tag or "fullyconnected" in tag or "fully" in tag:
                # FILTER_SHAPE: Cout * Cin * 1 * 1  (common layout)
                if len(filt_shape) < 2:
                    return (0, 0, 0)
                cout, cin = filt_shape[0], filt_shape[1]
                n = out_shape[0] if len(out_shape) >= 1 else 1
                spec = _FCSpec(batch=n, cin=cin, cout=cout)
                d = _mve_compute_counts("fully_connected", act_bits, w_bits, fc=spec, activation=_act_from_row(), out_dtype=out_dtype)
            else:
                # -------- Elementwise / Pool / Mean path --------
                # Recognize common eltwise ops in the tag
                _elt_ops = [
                    "add","sub","mul","div","maximum","minimum","abs","neg",
                    "sqrt","rsqrt","exp","log","sin","cos","tanh","logistic",
                    "relu","relu6","relu_n1_to_1","leaky_relu","hard_swish",
                    "square","power","floor","ceil","round","clamp","pad","softmax"
                ]
                if any(op in tag for op in _elt_ops):
                    # OUT_SHAPE for softmax bounds; otherwise used only to compute elems
                    out_shape = _parse_shape_list(row.get("OUTPUT_SHAPE", ""))
                    # elems: prefer product of OUTPUT_SHAPE; fallback to Output_Magnitude
                    elems = 1
                    if out_shape:
                        for d in out_shape: elems *= d
                    else:
                        # tolerant to a couple name variants
                        elems = int(row.get("Output_Magnitude", row.get("OUTPUT_MAGNITUDE", 0)) or 0)
                    # Analyzer hints (optional)
                    vec_chunks_hint = int(row.get("ElemVecChunks", 0) or 0)
                    predicated_hint = int(row.get("ElemPredicatedInstrs", 0) or 0)
                    out_dtype = str(row.get("Output_DType", row.get("Output_DTYPE","")) or "")
                    # op name (upper) – strip extra tokens from Tag (e.g., "CONCATENATION" won’t match; returns default)
                    op_name = tag.upper()
                    # Build spec and call predictor
                    elt_spec = _EltSpec(
                        elems=elems,
                        op_name=op_name,
                        out_shape=tuple(out_shape) if len(out_shape)==4 else None,
                        vec_chunks_hint=vec_chunks_hint if vec_chunks_hint>0 else None,
                        predicated_hint=predicated_hint if predicated_hint>=0 else None,
                        out_dtype=out_dtype,
                        # clamp bounds not carried here; defaults are fine
                    )
                    d = _mve_compute_counts("elementwise", act_bits, w_bits, elementwise=elt_spec)
                    return (
                        int(d.get("MVE_INT_MAC_RETIRED", 0)),
                        int(d.get("MVE_VREDUCE_INT_RETIRED", 0)),
                        int(d.get("MVE_PRED", 0)),
                    )
                # MAX_POOL_2D
                if "max_pool" in tag:
                    out_shape = _parse_shape_list(row.get("OUTPUT_SHAPE", ""))
                    elems = 1
                    if out_shape:
                        for d in out_shape: elems *= d
                    else:
                        elems = int(row.get("Output_Magnitude", row.get("OUTPUT_MAGNITUDE", 0)) or 0)
                    vec_chunks_hint = int(row.get("ElemVecChunks", 0) or 0)
                    predicated_hint = int(row.get("ElemPredicatedInstrs", 0) or 0)
                    d = _mve_compute_counts("max_pool_2d", act_bits, w_bits,
                                            pool_output_elems=elems,
                                            pool_vec_chunks_hint=vec_chunks_hint if vec_chunks_hint>0 else None,
                                            pool_predicated_hint=predicated_hint if predicated_hint>=0 else None)
                    return (
                        int(d.get("MVE_INT_MAC_RETIRED", 0)),
                        int(d.get("MVE_VREDUCE_INT_RETIRED", 0)),
                        int(d.get("MVE_PRED", 0)),
                    )
                # MEAN / REDUCE_MEAN / AVERAGE_POOL_2D
                if "reduce_mean" in tag or "mean" in tag or "average_pool" in tag:
                    out_shape = _parse_shape_list(row.get("OUTPUT_SHAPE", ""))
                    elems = 1
                    if out_shape:
                        for d_ in out_shape: elems *= d_
                    else:
                        elems = int(row.get("Output_Magnitude", row.get("OUTPUT_MAGNITUDE", 0)) or 0)
                    vec_chunks_hint = int(row.get("ElemVecChunks", 0) or 0)
                    predicated_hint = int(row.get("ElemPredicatedInstrs", 0) or 0)
                    # If reducer length known (future analyzer enhancement), pass via FILTER_SHAPE or a new column.
                    # For now, try to parse a plausible K from FILTER_SHAPE if present (Kh*Kw*... or Co).
                    filt = _parse_shape_list(row.get("FILTER_SHAPE", ""))
                    reduce_len = None
                    if filt:
                        # crude heuristic: if filter has at least 2 dims, K≈Kh*Kw; else if 1 dim, use it
                        if len(filt) >= 2:
                            reduce_len = max(1, int(filt[0]*filt[1]))
                        elif len(filt) == 1:
                            reduce_len = max(1, int(filt[0]))
                    d = _mve_compute_counts("mean", act_bits, w_bits,
                                            pool_output_elems=elems,
                                            pool_vec_chunks_hint=vec_chunks_hint if vec_chunks_hint>0 else None,
                                            pool_predicated_hint=predicated_hint if predicated_hint>=0 else None,
                                            mean_reduce_axis_len=reduce_len,
                                            mean_out_shape=tuple(out_shape) if len(out_shape)==4 else None)
                    return (
                        int(d.get("MVE_INT_MAC_RETIRED", 0)),
                        int(d.get("MVE_VREDUCE_INT_RETIRED", 0)),
                        int(d.get("MVE_PRED", 0)),
                    )

                # Unknown op → no prediction
                return (0, 0, 0)

            return (
                int(d.get("MVE_INT_MAC_RETIRED", 0)),
                int(d.get("MVE_VREDUCE_INT_RETIRED", 0)),
                int(d.get("MVE_PRED", 0)),
            )
        except Exception as e:
            # print the exception error
            print(f"foo exception: {e}")
            print(f"foo exception: {tag}")
            return (0, 0, 0)

    # Reuse _predict_row defined below (no change)
    preds = [ _predict_row(r) for _, r in df_data.iterrows() ]
    if preds:
        a, b, c = zip(*preds)
        pred_intmac   = np.array(a, dtype="int64")
        pred_vreduce  = np.array(b, dtype="int64")
        pred_pred     = np.array(c, dtype="int64")
    else:
        pred_intmac = pred_vreduce = pred_pred = np.array([np.nan]*nrows)

    # Create Derived dataframe (values-only columns here; formulas will be written with xlsxwriter)
    df_derived = pd.DataFrame({
        # For context/lookup
        "Event": df_data["Event"] if "Event" in df_data.columns else pd.Series(range(nrows)),
        "Tag":   df_data["Tag"]   if "Tag"   in df_data.columns else pd.Series([""]*nrows),
        # Predictor (values)
        "PRED_MVE_INT_MAC_RETIRED": pred_intmac,
        "PRED_MVE_VREDUCE_INT_RETIRED": pred_vreduce,
        "PRED_MVE_PRED": pred_pred,
        # Derived helpers (values)
        "ElemBits": _elem_bits_series,
        "ElemBytes": _elem_bits_series/8.0,
        # Pre-create empty formula columns so headers exist
        "Cycles": "",
        "MACs_per_cycle": "",
        "MACs_per_second": "",
        "Util_@8": "", "Util_@16": "", "Util_@32": "",
        "PeakMACsPerCycle": "", "Util_%": "",
        "IPC": "", "%Vectorized": "", "MVE_MAC_ratio": "",
        "L1D_Hit": "", "L1I_Hit": "",
        "FE_Stall%": "", "BE_Stall%": "", "Stall%": "",
        "Read_Bytes": "", "Write_Bytes": "", "BW_Bytes_s": "", "MACs_per_Byte": "",
        "OpKind": "", "Time_Share%": "", "INT_MAC_achv%": "", "VREDUCE_INT_achv%": "", "PRED_achv%": "",
        "Flag": "",
    })

    def _parse_shape_list(s: str) -> list[int]:
        try:
            toks = [t.strip() for t in str(s).replace("x", "*").split("*") if t.strip() != ""]
            return [int(t) for t in toks]
        except Exception:
            return []


    # Guardrail: detect duplicate headers early (Excel may "repair" by dropping the table)
    hdrs = df_data.columns.tolist()
    dups = [h for h in set(hdrs) if hdrs.count(h) > 1]
    if dups:
        print(f"[NS][XLSX] WARNING: duplicate header names detected: {dups}. Excel may repair the file.")

    # Choose engine locally
    try:
        import xlsxwriter  # noqa: F401
        _engine = "xlsxwriter"
    except Exception:
        _engine = "openpyxl"


    with pd.ExcelWriter(xlsx_path, engine=_engine) as ew:
        # ---------------------------
        # Write Data sheet (raw)
        # ---------------------------
        df_data.to_excel(ew, sheet_name="Data", index=False)
        wb = ew.book
        ws_data = ew.sheets["Data"]
        # Force Excel to fully recalculate on open so formula cells don't show 0s
        try:
            wb.set_calc_mode("automatic")
            wb.set_calc_on_load()
        except Exception:
            pass        
        
        # Config sheet + named CoreMHz
        cfg = wb.add_worksheet("Config")
        cfg.write("A1", "CoreMHz"); cfg.write_number("B1", float(core_mhz or 0.0))
        wb.define_name("CoreMHz", "=Config!$B$1")

        # Create Data table over full range
        nrows, ncols = df_data.shape
        first_row, first_col = 0, 0
        last_row, last_col = nrows, ncols - 1
        columns = [{"header": c} for c in df_data.columns]
        ws_data.add_table(first_row, first_col, last_row, last_col,
                          {"name": "DataTbl", "columns": columns, "style": "Table Style Medium 9"})
        headers_data = df_data.columns.tolist()

        def cidx(name):
            with suppress(ValueError):
                return headers_data.index(name)
            return None

        # A1 helpers -----------------------------------------------------
        def col_letter(ci0):
            # ci0 is 0-based index -> Excel column letters
            ci = ci0 + 1
            s = ""
            while ci:
                ci, r = divmod(ci-1, 26)
                s = chr(65+r) + s
            return s
        def a1(ci0, r0):
            # ci0: 0-based col, r0: 0-based row in worksheet
            return f"{col_letter(ci0)}{r0+1}"

        def a1d(col_name, r0):
            """A1 on Data sheet for given column name/row index (1-based row)"""
            ci = cidx(col_name)
            return f"Data!{a1(ci, r0)}" if ci is not None else None
        
        # Precompute useful A1 ranges on Data
        u_c = cidx("uSeconds")
        u_rng = f"Data!{col_letter(u_c)}2:Data!{col_letter(u_c)}{nrows+1}" if u_c is not None else None

        # ---------------------------
        # Write Derived sheet
        # ---------------------------
        df_derived.to_excel(ew, sheet_name="Derived", index=False)
        ws_der = ew.sheets["Derived"]
        headers_der = df_derived.columns.tolist()
        nrows_der, ncols_der = df_derived.shape
        ws_der.add_table(0, 0, nrows_der, ncols_der - 1,
                         {"name": "DerivedTbl",
                          "columns": [{"header": c} for c in headers_der],
                          "style": "Table Style Medium 9"})
        pct_fmt = wb.add_format({"num_format": "0.00%"})
        for name in headers_der:
            if "%" in name:
                ci = headers_der.index(name)
                ws_der.set_column(ci, ci, None, pct_fmt)

        def cidx_der(name):
            with suppress(ValueError):
                return headers_der.index(name)
            return None
        def a1der(ci0, r0):
            return a1(ci0, r0)  # on Derived sheet
        def a1nder(col_name, r0):
            ci = cidx_der(col_name)
            return a1(ci, r0) if ci is not None else None


        # ---------- Precomputed cached values for derived columns ----------
        _mhz = float(core_mhz or 0.0)
        _us = df_data["uSeconds"] if "uSeconds" in df_data.columns else pd.Series([np.nan]*nrows)
        _cycles_src = df_data["ARM_PMU_CPU_CYCLES"] if "ARM_PMU_CPU_CYCLES" in df_data.columns else None
        _cycles_series = (_cycles_src if _cycles_src is not None else (_us * _mhz)).astype("float64")
        _macs = df_data["EST_MAC"].astype("float64") if "EST_MAC" in df_data.columns else pd.Series([np.nan]*nrows)
        _elem_bits = df_derived["ElemBits"].astype("float64")
        _peak = _elem_bits.map({8:32, 16:16, 32:8}).fillna(np.nan)
        _inst = df_data["ARM_PMU_INST_RETIRED"].astype("float64") if "ARM_PMU_INST_RETIRED" in df_data.columns else pd.Series([np.nan]*nrows)
        _mve = df_data["ARM_PMU_MVE_INST_RETIRED"].astype("float64") if "ARM_PMU_MVE_INST_RETIRED" in df_data.columns else pd.Series([np.nan]*nrows)
        _mve_int_mac = df_data["ARM_PMU_MVE_INT_MAC_RETIRED"].astype("float64") if "ARM_PMU_MVE_INT_MAC_RETIRED" in df_data.columns else pd.Series([np.nan]*nrows)
        _mve_fp_mac  = df_data["ARM_PMU_MVE_FP_MAC_RETIRED"].astype("float64")  if "ARM_PMU_MVE_FP_MAC_RETIRED"  in df_data.columns else pd.Series([np.nan]*nrows)
        _l1d        = df_data["ARM_PMU_L1D_CACHE"].astype("float64")            if "ARM_PMU_L1D_CACHE"            in df_data.columns else pd.Series([np.nan]*nrows)
        _l1d_refill = df_data["ARM_PMU_L1D_CACHE_REFILL"].astype("float64")     if "ARM_PMU_L1D_CACHE_REFILL"     in df_data.columns else pd.Series([np.nan]*nrows)
        _l1i        = df_data["ARM_PMU_L1I_CACHE"].astype("float64")            if "ARM_PMU_L1I_CACHE"            in df_data.columns else pd.Series([np.nan]*nrows)
        _l1i_refill = df_data["ARM_PMU_L1I_CACHE_REFILL"].astype("float64")     if "ARM_PMU_L1I_CACHE_REFILL"     in df_data.columns else pd.Series([np.nan]*nrows)
        _fe_stall   = df_data["ARM_PMU_STALL_FRONTEND"].astype("float64")       if "ARM_PMU_STALL_FRONTEND"       in df_data.columns else pd.Series([np.nan]*nrows)
        _be_stall   = df_data["ARM_PMU_STALL_BACKEND"].astype("float64")        if "ARM_PMU_STALL_BACKEND"        in df_data.columns else pd.Series([np.nan]*nrows)
        _stall      = df_data["ARM_PMU_STALL"].astype("float64")                if "ARM_PMU_STALL"                in df_data.columns else pd.Series([np.nan]*nrows)
        _us_total = float(_us.sum()) if "uSeconds" in df_data and pd.notna(_us).any() else np.nan
        # For % achieved (actuals on Data, predictions are values on Derived)
        _mve_vreduce = df_data["ARM_PMU_MVE_VREDUCE_INT_RETIRED"].astype("float64") if "ARM_PMU_MVE_VREDUCE_INT_RETIRED" in df_data.columns else pd.Series([np.nan]*nrows)
        _pred_cycles = df_data["ARM_PMU_MVE_PRED"].astype("float64") if "ARM_PMU_MVE_PRED" in df_data.columns else pd.Series([np.nan]*nrows)
        
        # Fill formulas into Derived (with cached results)
        # def write_cache(ws, row_idx, col_name, formula, cached):
        #     ci = cidx_der(col_name)
        #     if ci is not None:
        #         ws.write_formula(row_idx, ci, formula, None, cached)

        def _write_formula_cached(ws, row_idx, col_name, formula, cached):
            """
            Write a formula and (optionally) a cached result.
            IMPORTANT: Do NOT pass None/NaN/Inf as a cached value – XlsxWriter will
            serialize the literal string 'None' (or invalid numeric), which causes Excel
            to 'repair' the sheet on open. If cached isn't a real finite number, omit it.
            """
            ci = cidx_der(col_name)
            if ci is None:
                return
            try:
                is_bad = False
                if cached is None:
                    is_bad = True
                elif isinstance(cached, float):
                    import math
                    if math.isnan(cached) or math.isinf(cached):
                        is_bad = True
                if is_bad:
                    ws.write_formula(row_idx, ci, formula)
                else:
                    ws.write_formula(row_idx, ci, formula, None, cached)
            except Exception as e:
                print(f"Error writing formula {formula} for {col_name} at row {row_idx}: {e}")
                raise e

        # Build A1 formulas (no structured refs) --------------------------
        HAS_CYC = "ARM_PMU_CPU_CYCLES" in headers_data
        # Base (Cycles)
        if HAS_CYC:
            for r in range(1, nrows + 1):
                _write_formula_cached(ws_der, r, "Cycles",
                            f"={a1d('ARM_PMU_CPU_CYCLES', r)}",
                            df_data.at[r-1, "ARM_PMU_CPU_CYCLES"])
        else:
            for r in range(1, nrows + 1):
                _write_formula_cached(ws_der, r, "Cycles",
                            f"={a1d('uSeconds', r)}*CoreMHz",
                            (float(df_data.at[r-1, "uSeconds"])*_mhz) if ("uSeconds" in df_data.columns and pd.notna(df_data.at[r-1, "uSeconds"])) else None)

        # Helpers per row
        def f(col): return cidx_der(col)

        for r in range(1, nrows + 1):
            # A1 cell refs, this row
            mac  = a1d("EST_MAC", r)
            cyc  = a1nder("Cycles", r)
            usec = a1d("uSeconds", r)
            ebits = a1nder("ElemBits", r)
            inst = a1d("ARM_PMU_INST_RETIRED", r)
            mvei = a1d("ARM_PMU_MVE_INST_RETIRED", r)
            mve_int_mac = a1d("ARM_PMU_MVE_INT_MAC_RETIRED", r)
            mve_fp_mac  = a1d("ARM_PMU_MVE_FP_MAC_RETIRED", r)
            mve_vreduce_int = a1d("ARM_PMU_MVE_VREDUCE_INT_RETIRED", r) if cidx("ARM_PMU_MVE_VREDUCE_INT_RETIRED") is not None else None
            pred_cycles_a1  = a1d("ARM_PMU_MVE_PRED", r) if cidx("ARM_PMU_MVE_PRED") is not None else None
            l1d = a1d("ARM_PMU_L1D_CACHE", r)
            l1d_refill = a1d("ARM_PMU_L1D_CACHE_REFILL", r)
            l1i = a1d("ARM_PMU_L1I_CACHE", r)
            l1i_refill = a1d("ARM_PMU_L1I_CACHE_REFILL", r)
            fe = a1d("ARM_PMU_STALL_FRONTEND", r)
            be = a1d("ARM_PMU_STALL_BACKEND", r)
            st = a1d("ARM_PMU_STALL", r)
            pred_intmac_a1   = a1nder("PRED_MVE_INT_MAC_RETIRED", r)
            pred_vreduce_a1  = a1nder("PRED_MVE_VREDUCE_INT_RETIRED", r)
            pred_pred_a1     = a1nder("PRED_MVE_PRED", r)

            # MACs/cycle, MACs/sec


            _write_formula_cached(ws_der, r, "MACs_per_cycle",
                f'=IF(AND({mac}>0,{cyc}>0),{mac}/{cyc},"")',
                (_macs[r-1]/_cycles_series[r-1]) if (_macs[r-1]>0 and _cycles_series[r-1]>0) else None)
            _write_formula_cached(ws_der, r, "MACs_per_second",
                f'=IF({usec}>0,{mac}/({usec}/1E6),"")',
                (_macs[r-1]/(_us[r-1]/1e6)) if (_macs[r-1]>0 and _us[r-1]>0) else None)


            # Utilization + Peak
            _write_formula_cached(ws_der, r, "Util_@8",  f'=IF({mac}>0,{mac}/{cyc}/8,"")',
                (_macs[r-1]/_cycles_series[r-1]/8.0)  if (_macs[r-1]>0 and _cycles_series[r-1]>0) else None)
            _write_formula_cached(ws_der, r, "Util_@16", f'=IF({mac}>0,{mac}/{cyc}/16,"")',
                (_macs[r-1]/_cycles_series[r-1]/16.0) if (_macs[r-1]>0 and _cycles_series[r-1]>0) else None)
            _write_formula_cached(ws_der, r, "Util_@32", f'=IF({mac}>0,{mac}/{cyc}/32,"")',
                (_macs[r-1]/_cycles_series[r-1]/32.0) if (_macs[r-1]>0 and _cycles_series[r-1]>0) else None)

            _write_formula_cached(ws_der, r, "PeakMACsPerCycle",
                f'=IF({ebits}=8,8,IF({ebits}=16,4,IF({ebits}=32,2,"")))',
                _peak[r-1] if pd.notna(_peak[r-1]) else None)
            
            _write_formula_cached(ws_der, r, "Util_%",
                f'=IF(AND({mac}>0,{cyc}>0,{a1nder("PeakMACsPerCycle", r)}<>""),{mac}/{cyc}/{a1nder("PeakMACsPerCycle", r)},"")',
                (_macs[r-1]/_cycles_series[r-1]/_peak[r-1]) if (_macs[r-1]>0 and _cycles_series[r-1]>0 and pd.notna(_peak[r-1]) and _peak[r-1]>0) else None)
            
            # IPC, Vectorization, MVE MAC ratio
            if inst:
                _write_formula_cached(ws_der, r, "IPC",
                                 f'=IF(AND({inst}>0,{cyc}>0),{inst}/{cyc},"")',
                                 (_inst[r-1]/_cycles_series[r-1]) if (_cycles_series[r-1]>0 and _inst[r-1]>0) else None)
            if inst and mvei:
                _write_formula_cached(ws_der, r, "%Vectorized",
                                 f'=IF({inst}>0,{mvei}/{inst},"")',
                                 (_mve[r-1]/_inst[r-1]) if (_inst[r-1] and _inst[r-1]>0) else None)
            if mvei and mve_int_mac and mve_fp_mac:
                _write_formula_cached(ws_der, r, "MVE_MAC_ratio",
                                 f'=IF({mvei}>0,({mve_int_mac}+{mve_fp_mac})/{mvei},"")',
                                 ((_mve_int_mac[r-1]+_mve_fp_mac[r-1])/_mve[r-1]) if (_mve[r-1] and _mve[r-1]>0) else None)
            # Cache hit-rates
            if l1d and l1d_refill:
                _write_formula_cached(ws_der, r, "L1D_Hit",
                                 f'=IF({l1d}>0,1-{l1d_refill}/{l1d},"")',
                                 (1.0-(_l1d_refill[r-1]/_l1d[r-1])) if (_l1d[r-1] and _l1d[r-1]>0) else None)
            if l1i and l1i_refill:
                _write_formula_cached(ws_der, r, "L1I_Hit",
                                 f'=IF({l1i}>0,1-{l1i_refill}/{l1i},"")',
                                 (1.0-(_l1i_refill[r-1]/_l1i[r-1])) if (_l1i[r-1] and _l1i[r-1]>0) else None)
            # Stalls
            if fe:
                _write_formula_cached(ws_der, r, "FE_Stall%",
                                 f'=IF({cyc}>0,{fe}/{cyc},"")',
                                 (_fe_stall[r-1]/_cycles_series[r-1]) if (_cycles_series[r-1]>0 and _fe_stall[r-1] >= 0) else None)
            if be:
                _write_formula_cached(ws_der, r, "BE_Stall%",
                                 f'=IF({cyc}>0,{be}/{cyc},"")',
                                 (_be_stall[r-1]/_cycles_series[r-1]) if (_cycles_series[r-1]>0 and _be_stall[r-1] >= 0) else None)
            if st:
                _write_formula_cached(ws_der, r, "Stall%",
                                 f'=IF({cyc}>0,{st}/{cyc},"")',
                                 (_stall[r-1]/_cycles_series[r-1]) if (_cycles_series[r-1]>0 and _stall[r-1] >= 0) else None)
            # Bytes/intensity
            if "read_estimate" in headers_data and "write_estimate" in headers_data:
                re = a1d("read_estimate", r)
                we = a1d("write_estimate", r)
                eb = a1nder("ElemBytes", r)
                rb = a1nder("Read_Bytes", r)    
                wb_ = a1nder("Write_Bytes", r)
                _write_formula_cached(ws_der, r, "Read_Bytes",  f'={re}*{eb}', re*eb)
                _write_formula_cached(ws_der, r, "Write_Bytes", f'={we}*{eb}', we*eb)
                _write_formula_cached(ws_der, r, "BW_Bytes_s",  f'=IF({usec}>0,({rb}+{wb_})/({usec}/1E6),"")', (rb+wb_)/(_us[r-1]/1e6))
                if "EST_MAC" in headers_data:
                    _write_formula_cached(ws_der, r, "MACs_per_Byte", f'=IF(({rb}+{wb_})>0,{mac}/({rb}+{wb_}),"")', mac/(rb+wb_))

            else:
                # put zeros in the columns
                _write_formula_cached(ws_der, r, "Read_Bytes",  f'0', 0)
                _write_formula_cached(ws_der, r, "Write_Bytes", f'0', 0)
                _write_formula_cached(ws_der, r, "BW_Bytes_s",  f'0', 0)
                _write_formula_cached(ws_der, r, "MACs_per_Byte", f'0', 0)
                    
            # OpKind
            if "Tag" in headers_data:
                tag = a1d("Tag", r)
                _write_formula_cached(ws_der, r, "OpKind",
                    f'=IFERROR(IF(ISNUMBER(SEARCH("DEPTHWISE",{tag})),"DWConv",'
                    f'IF(ISNUMBER(SEARCH("CONV_2D",{tag})),"Conv",'
                    f'IF(ISNUMBER(SEARCH("FULLY_CONNECTED",{tag})),"FC",'
                    f'IF(ISNUMBER(SEARCH("POOL",{tag})),"Pool",'
                    f'IF(OR(ISNUMBER(SEARCH("ADD",{tag})),ISNUMBER(SEARCH("MUL",{tag})),ISNUMBER(SEARCH("SUB",{tag})),ISNUMBER(SEARCH("RELU",{tag}))),"Elementwise","Other"))))),"Other")',
                    f'IFERROR(IF(ISNUMBER(SEARCH("DEPTHWISE",{tag})),"DWConv",'
                    f'IF(ISNUMBER(SEARCH("CONV_2D",{tag})),"Conv",'
                    f'IF(ISNUMBER(SEARCH("FULLY_CONNECTED",{tag})),"FC",'
                    f'IF(ISNUMBER(SEARCH("POOL",{tag})),"Pool",'
                    f'IF(OR(ISNUMBER(SEARCH("ADD",{tag})),ISNUMBER(SEARCH("MUL",{tag})),ISNUMBER(SEARCH("SUB",{tag})),ISNUMBER(SEARCH("RELU",{tag}))),"Elementwise","Other"))))),"Other")')
            # Time share
            if u_rng:
                _write_formula_cached(ws_der, r, "Time_Share%",
                    f'=IFERROR({usec}/SUM({u_rng}),"")',
                    (_us[r-1]/_us.sum()) if (_us.sum()>0 and _us[r-1]>=0) else None)

            # -------------------- % Achieved vs Predicted (A1) --------------------
            # INT MAC %
            if cidx_der("INT_MAC_achv%") is not None and mve_int_mac and pred_intmac_a1:
                _write_formula_cached(ws_der, r, "INT_MAC_achv%",
                    f'=IF({pred_intmac_a1}>0,{mve_int_mac}/{pred_intmac_a1},"")',
                    (_mve_int_mac[r-1]/df_derived.at[r-1,"PRED_MVE_INT_MAC_RETIRED"])
                        if (pd.notna(_mve_int_mac[r-1]) and df_derived.at[r-1,"PRED_MVE_INT_MAC_RETIRED"]>0) else None)

            # VREDUCE INT %
            if cidx_der("VREDUCE_INT_achv%") is not None and mve_vreduce_int and pred_vreduce_a1:
                _write_formula_cached(ws_der, r, "VREDUCE_INT_achv%",
                    f'=IF({pred_vreduce_a1}>0,{mve_vreduce_int}/{pred_vreduce_a1},"")',
                    (_mve_vreduce[r-1]/df_derived.at[r-1,"PRED_MVE_VREDUCE_INT_RETIRED"])
                        if (pd.notna(_mve_vreduce[r-1]) and df_derived.at[r-1,"PRED_MVE_VREDUCE_INT_RETIRED"]>0) else None)

            # PRED cycles %
            if cidx_der("PRED_achv%") is not None and pred_cycles_a1 and pred_pred_a1:
                _write_formula_cached(ws_der, r, "PRED_achv%",
                    f'=IF({pred_pred_a1}>0,{pred_cycles_a1}/{pred_pred_a1},"")',
                    (_pred_cycles[r-1]/df_derived.at[r-1,"PRED_MVE_PRED"])
                        if (pd.notna(_pred_cycles[r-1]) and df_derived.at[r-1,"PRED_MVE_PRED"]>0) else None)

            # Flag
            required_flag_cols = ("Util_%","MACs_per_Byte","L1D_Hit","FE_Stall%","L1I_Hit","BE_Stall%","OpKind","Time_Share%","Flag")
            if all(h in headers_der for h in required_flag_cols):
                U   = cidx_der("Util_%");        MB  = cidx_der("MACs_per_Byte"); L1DH = cidx_der("L1D_Hit")
                FE  = cidx_der("FE_Stall%");     LIH = cidx_der("L1I_Hit");       BEP  = cidx_der("BE_Stall%")
                OP  = cidx_der("OpKind");        TS  = cidx_der("Time_Share%")
                for r in range(1, nrows + 1):
                    ws_der.write_formula(
                        r, cidx_der("Flag"),
                        ('=IF({u}<0.25,"Compute under-utilized",'
                         'IF(AND(NOT(ISBLANK({mb})),{mb}<0.5,NOT(ISBLANK({l1})),{l1}<0.85),"Memory-bound",'
                         'IF(OR(IFERROR({fe},0)>0.20,IFERROR({lih},1)<0.85),"Frontend stalls",'
                         'IF(IFERROR({be},0)>0.25,"LSU stalls",'
                         'IF(AND(IFERROR({op},"")="Elementwise",IFERROR({ts},0)>0.10),"Elementwise hotspot","")))))')
                        .format(u=a1der(U,r), mb=a1der(MB,r), l1=a1der(L1DH,r),
                                fe=a1der(FE,r), lih=a1der(LIH,r), be=a1der(BEP,r),
                                op=a1der(OP,r), ts=a1der(TS,r)))


        # Conditional formatting on Derived sheet
        def rng_der(col_name):
            ci = cidx_der(col_name)
            return (1, ci, nrows, ci) if ci is not None else None
        for h in ("Time_Share%","MACs_per_second", "Cycles"):
            r = rng_der(h)
            if r: ws_der.conditional_format(*r, {"type": "data_bar"})
        for h in ("Util_%","L1D_Hit","L1I_Hit","INT_MAC_achv%","VREDUCE_INT_achv%","PRED_achv%", "IPC", "%Vectorized", "MVE_MAC_ratio", "Util_@", "MACs_per_cycle", "MACs_per_second"):
            r = rng_der(h)
            if r: ws_der.conditional_format(*r, {"type": "3_color_scale"})
        for h in ("FE_Stall%","BE_Stall%","Stall%"):
            r = rng_der(h)
            if r: ws_der.conditional_format(*r, {"type": "3_color_scale",
                'min_color': "#63BE7B",  # Green for lowest values
                'mid_color': "#FFEB84",  # Yellow for middle values
                'max_color': "#F8696B"   # Red for highest values
            })
        # ---------------------------------------------------------------
        # 4) "Pivots" sheet: native pivots if available, else SUMIF/AVERAGEIF summaries
        # ---------------------------------------------------------------
        # Helper: formula-based “pivots” (works everywhere)
        def _build_formula_pivots():
            piv = wb.add_worksheet("Pivots")
            try:
                import xlsxwriter as _xw
                _xw_ver = getattr(_xw, "__version__", "unknown")
            except Exception:
                _xw_ver = "unknown"
            piv.write('A1', f'Pivot tables not available (engine={_engine}, xlsxwriter={_xw_ver}). Using formula-based summaries.')
            pct = wb.add_format({"num_format": "0.00%"})
            # Unique labels (computed here so we don't rely on Excel dynamic arrays)
            opkinds = sorted([v for v in pd.unique(df_derived["OpKind"]) if isinstance(v, str) and v])
            tags    = sorted([v for v in pd.unique(df_derived["Tag"])    if isinstance(v, str) and v])

            # ---- 1) Time% by OpKind ----
            piv.write_row('A3', ['OpKind','Sum Time_Share%','Avg Util_%'])
            start = 3  # A4 is first data row (1-based)
            for i, ok in enumerate(opkinds):
                r = start + i
                piv.write(r-1, 0, ok)  # 0-based row index
                piv.write_formula(r-1, 1, f'=IFERROR(SUMIF(DerivedTbl[OpKind], $A{r}, DerivedTbl[Time_Share%]), "")')
                piv.write_formula(r-1, 2, f'=IFERROR(AVERAGEIF(DerivedTbl[OpKind], $A{r}, DerivedTbl[Util_%]), "")')
            piv.set_column(1, 2, None, pct)
            piv.conditional_format(start-1, 1, start-2+len(opkinds), 1, {"type": "data_bar"})

            # ---- 2) Top Offenders (by Tag) ----
            piv.write_row('H3', ['Tag','Sum Time_Share%','Avg Util_%'])
            startH = 3
            for i, tg in enumerate(tags):
                r = startH + i
                piv.write(r-1, 7, tg)
                piv.write_formula(r-1, 8, f'=IFERROR(SUMIF(DerivedTbl[Tag], $H{r}, DerivedTbl[Time_Share%]), "")')
                piv.write_formula(r-1, 9, f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $H{r}, DerivedTbl[Util_%]), "")')
            piv.set_column(8, 9, None, pct)
            piv.conditional_format(startH-1, 8, startH-2+len(tags), 8, {"type": "data_bar"})

            # Compute a base row to avoid overlap with block lengths
            base = max(start - 1 + len(opkinds), startH - 1 + len(tags)) + 3

            # ---- 3) Stall Hotspots ----
            piv.write(base-1, 0, 'Stall Hotspots')
            piv.write_row(base, 0, ['Tag','Avg FE_Stall%','Avg BE_Stall%','Avg Stall%','Sum Time_Share%'])
            for i, tg in enumerate(tags):
                r = base + 1 + i
                piv.write(r, 0, tg)
                piv.write_formula(r, 1, f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $A{r+1}, DerivedTbl[FE_Stall%]), "")')
                piv.write_formula(r, 2, f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $A{r+1}, DerivedTbl[BE_Stall%]), "")')
                piv.write_formula(r, 3, f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $A{r+1}, DerivedTbl[Stall%]), "")')
                piv.write_formula(r, 4, f'=IFERROR(SUMIF(DerivedTbl[Tag], $A{r+1}, DerivedTbl[Time_Share%]), "")')
            piv.set_column(1, 3, None, pct); piv.set_column(4, 4, None, pct)
            piv.conditional_format(base+1, 1, base+len(tags), 3, {"type": "3_color_scale"})

            # ---- 4) Memory-bound Candidates ----
            base2 = base + len(tags) + 4
            piv.write(base2-1, 7, 'Memory-bound Candidates')
            piv.write_row(base2, 7, ['Tag','Avg MACs_per_Byte','Avg L1D_Hit','Avg L1I_Hit','Sum Time_Share%'])
            for i, tg in enumerate(tags):
                r = base2 + 1 + i
                piv.write(r, 7, tg)
                piv.write_formula(r, 8,  f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $H{r+1}, DerivedTbl[MACs_per_Byte]), "")')
                piv.write_formula(r, 9,  f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $H{r+1}, DerivedTbl[L1D_Hit]), "")')
                piv.write_formula(r, 10, f'=IFERROR(AVERAGEIF(DerivedTbl[Tag], $H{r+1}, DerivedTbl[L1I_Hit]), "")')
                piv.write_formula(r, 11, f'=IFERROR(SUMIF(DerivedTbl[Tag], $H{r+1}, DerivedTbl[Time_Share%]), "")')
            piv.set_column(9, 10, None, pct); piv.set_column(11, 11, None, pct)
            piv.conditional_format(base2+1, 8, base2+len(tags), 8, {"type": "3_color_scale"})

        # Try native pivots when the writer supports them; otherwise fall back.
        try:
            if (_engine == "xlsxwriter") and hasattr(wb, "add_pivot_table"):
                piv = wb.add_worksheet("Pivots")
                data_ref = f"Derived!A1:{col_letter(ncols_der-1)}{nrows_der+1}"
                # Titles
                piv.write('A1', 'Time% by OpKind')
                piv.write('H1', 'Top Offenders (by Tag)')
                piv.write('A20', 'Stall Hotspots')
                piv.write('H20', 'Memory-bound Candidates')
                # Pivot 1
                wb.add_pivot_table({
                    'name': 'pvt_TimeByOpKind',
                    'data': data_ref,
                    'location': 'Pivots!A3',
                    'row_fields':    [{'name': 'OpKind'}],
                    'data_fields':   [
                        {'name': 'Time_Share%', 'function': 'sum', 'num_format': '0.00%'},
                        {'name': 'Util_%',      'function': 'average', 'num_format': '0.00%'},
                    ],
                })
                # Pivot 2
                wb.add_pivot_table({
                    'name': 'pvt_TopOffenders',
                    'data': data_ref,
                    'location': 'Pivots!H3',
                    'row_fields':    [{'name': 'Tag'}],
                    'data_fields':   [
                        {'name': 'Time_Share%', 'function': 'sum', 'num_format': '0.00%'},
                        {'name': 'Util_%',      'function': 'average', 'num_format': '0.00%'},
                    ],
                })
                # Pivot 3
                wb.add_pivot_table({
                    'name': 'pvt_Stalls',
                    'data': data_ref,
                    'location': 'Pivots!A22',
                    'row_fields':    [{'name': 'Tag'}],
                    'data_fields':   [
                        {'name': 'FE_Stall%',   'function': 'average', 'num_format': '0.00%'},
                        {'name': 'BE_Stall%',   'function': 'average', 'num_format': '0.00%'},
                        {'name': 'Stall%',      'function': 'average', 'num_format': '0.00%'},
                        {'name': 'Time_Share%', 'function': 'sum',     'num_format': '0.00%'},
                    ],
                })
                # Pivot 4
                if all(h in headers_der for h in ("MACs_per_Byte","L1D_Hit","L1I_Hit")):
                    wb.add_pivot_table({
                        'name': 'pvt_Memory',
                        'data': data_ref,
                        'location': 'Pivots!H22',
                        'row_fields':    [{'name': 'Tag'}],
                        'data_fields':   [
                            {'name': 'MACs_per_Byte', 'function': 'average'},
                            {'name': 'L1D_Hit',        'function': 'average', 'num_format': '0.00%'},
                            {'name': 'L1I_Hit',        'function': 'average', 'num_format': '0.00%'},
                            {'name': 'Time_Share%',    'function': 'sum',     'num_format': '0.00%'},
                        ],
                    })
            else:
                _build_formula_pivots()
        except Exception:
            _build_formula_pivots()


def printStats(params, mc, stats, stats_filename, pmu_csv_header, overall_pmu_stats, aot=False):
    """
    Stats are from these structs in EVB-land:
    typedef struct {
        uint32_t computed_arena_size;
        uint32_t computed_stat_buffer_size;
        uint32_t captured_events;
        uint32_t pmu_events_per_layer;
        uint32_t platform; // dictates how to interpret the stats
        char csv_header[512]; // CSV header for the stats
        ns_profiler_event_stats_t stat_buffer[NS_PROFILER_RPC_EVENTS_MAX];
    } ns_mut_stats_t;

    typedef struct {
        #ifdef AM_PART_APOLLO5B
        ns_pmu_counters_t pmu_delta;
        #else
        ns_cache_dump_t cache_delta;
        #endif
        ns_perf_counters_t perf_delta;
        uint32_t estimated_macs;
        uint32_t elapsed_us;
        char tag[NS_PROFILER_TAG_SIZE]; ///< Tag from TFLM microprofiler
    } ns_profiler_event_stats_t;

    And:

    typedef struct {
        uint32_t counterValue[8]; /// Value read from the PMU
    } ns_pmu_counters_t;

    typedef struct {
        uint32_t daccess;
        uint32_t dtaglookup;
        uint32_t dhitslookup;
        uint32_t dhitsline;
        uint32_t iaccess;
        uint32_t itaglookup;
        uint32_t ihitslookup;
        uint32_t ihitsline;
    } ns_cache_dump_t;

    typedef struct {
        uint32_t cyccnt;
        uint32_t cpicnt;
        uint32_t exccnt;
        uint32_t sleepcnt;
        uint32_t lsucnt;
        uint32_t foldcnt;
    } ns_perf_counters_t;

    """
    computed_stat_buffer_size = stats[1]  # in bytes

    # NOTE: ns_mut_stats_t preamble layout (uint32 words):
    # [0] computed_arena_size
    # [1] computed_stat_buffer_size
    # [2] computed_stat_per_event_size (bytes)
    # [3] pmu_count
    # [4] pmu_events_per_layer
    # [5] captured_events
    # [6] platform
    computed_stat_per_event_size = stats[2] // 4  # convert bytes to uint32 words
    pmu_count = stats[3]
    pmu_events_per_layer = stats[4]
    captured_events = stats[5]
    platform = stats[6]

    # Build the CSV header string
    if aot:
        # AOT FullStats carry only per-layer time (us) in stat_buffer with header "layer,us".
        # When full PMU is requested, we augment the header to include timing + PMU counters.
        if params.full_pmu_capture:
            csv_header = "Event,Tag,uSeconds," + prune_for_aot(pmu_csv_header)
        else:
            csv_header = "Event,Tag,uSeconds"
    else:
        # TFLM path (unchanged): decode CSV header from stats blob
        if params.full_pmu_capture:
            log.info("Including Full PMU Capture")
            csv_header = pmu_csv_header
        else:
            csv_header = stats[7:7 + 128]  # 512 bytes as 128 words
            csv_header = struct.pack("<" + "I" * 128, *csv_header)
            csv_header = csv_header.split(b"\x00")[0]
            csv_header = "".join([chr(c) for c in csv_header])
    
    # print(stats)
    log.info(
        "Decoding statistics. Number of events = %d, buff_size = %d size = %d platform %d arraylen %d"
        % (
            captured_events,
            computed_stat_buffer_size,
            computed_stat_per_event_size,
            platform,
            len(stats),
        )
    )
    # print("CSV Header is: " + csv_header)
    # for each captured event, decode stat_buffer into cach, perf, macs, and time
    offset = modelStatPreambleSize  # size of stats preamble

    # Array may not contain entire event log (limited by ns_ambiq_harness' NS_PROFILER_RPC_EVENTS_MAX), truncate to whatever
    # we got back
    if ((len(stats) - offset) // computed_stat_per_event_size) < captured_events:
        captured_events = (len(stats) - offset) // computed_stat_per_event_size
        log.warning(
            "Number of events greater than allowed for by RPC buffer size (suggestion: increase NS_PROFILER_RPC_EVENTS_MAX). Statistics will be truncated to %d events"
            % captured_events
        )

    # convert csv header to table with elements separated by commas
    # csv_elements = csv_header.split(",")
    csv_elements = [tok.strip() for tok in csv_header.split(",")]

    table = [csv_elements]
    # print(csv_elements)
    # print(table)
    totalMacs = 0
    totalTime = 0
    totalCycles = 0

    # If AOT, pre-extract per-layer times now (each event = 1 uint32 us)
    if aot:
        # Each event is exactly one uint32 = elapsed microseconds
        available_events = (len(stats) - offset) // max(1, computed_stat_per_event_size)
        if captured_events > available_events:
            captured_events = available_events
            log.warning(
                "AOT: Truncating events to %d based on payload size", captured_events
            )

        # Pull per-layer us timings
        aot_us = [stats[offset + i] for i in range(captured_events)]
        totalTime = int(sum(aot_us))
        # Build table rows. If Full PMU was requested, append PMU counters.
        if params.full_pmu_capture:
            # Header already includes PMU names via pmu_csv_header
            for i in range(len(mc.aot_layer_identifiers)):
                id_int = int(mc.aot_layer_identifiers[i])
                row = [mc.aot_layer_identifiers[i], mc.aot_layer_names[i], aot_us[id_int]]
                # overall_pmu_stats[i] is a flat list of counters for layer i
                row.extend(list(overall_pmu_stats[id_int]))
                table.append(row)
        else:
            for i in range(len(mc.aot_layer_identifiers)):
                # print(f"AOT Layer {i}: {mc.aot_layer_identifiers[i]}")
                # print(f"{mc.aot_layer_names[i]}")
                # print(f"{mc.aot_layer_identifiers[i]}")
                id_int = int(mc.aot_layer_identifiers[i])
                # print(f"{aot_us[id_int]}")
                table.append([mc.aot_layer_identifiers[i], mc.aot_layer_names[i], aot_us[id_int]])

        # Print/Log the table
        if params.joulescope or params.onboard_perf:
            log.info(tabulate(table, headers="firstrow", tablefmt="simple"))
        else:
            print(tabulate(table, headers="firstrow", tablefmt="simple"))

        # Summary for AOT: only total time (MACs unknown / not applicable)
        msg = f"[NS] Model Performance Analysis (AOT): Total Inference Time {totalTime} us, layers {captured_events}"
        if params.joulescope or params.onboard_perf:
            log.info(msg)
        else:
            print(msg)

        log.info(
            "Model Performance Analysis: Per-layer performance statistics saved to: %s",
            stats_filename,
        )
        np.savetxt(stats_filename + ".csv", table, delimiter=", ", fmt="% s")
        df = pd.DataFrame(table[1:], columns=table[0])
        df = df.map(lambda x: x.encode('unicode_escape').decode('utf-8') if isinstance(x, str) else x)
        # Use the same two-tab workbook so calculated columns sit on Derived
        _elem_bits = derive_elem_bits_per_layer(mc)
        _core_mhz = 250 # getattr(params, "core_mhz", 0)
        _emit_analysis_workbook(stats_filename + ".xlsx", df, _core_mhz, _elem_bits)

        if params.profile_results_path != "none":
            import datetime, os, shutil
            unique_name = f"{params.model_name}_{params.platform}_AOT_{params.ambiqsuite_version}_{params.toolchain}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"
            unique_name = unique_name.replace(" ", "_").replace(".", "_")
            os.makedirs(params.profile_results_path, exist_ok=True)
            shutil.copy(stats_filename + ".csv", os.path.join(params.profile_results_path, unique_name + ".csv"))
            shutil.copy(stats_filename + ".xlsx", os.path.join(params.profile_results_path, unique_name + ".xlsx"))

        return totalCycles, totalMacs, totalTime, captured_events, pmu_events_per_layer
 

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
        totalMacs += macs
        totalTime += time
        if platform == 5:
            totalCycles = 0
        else:
            totalCycles += stats[offset + 8]
        # print(f"Layer {i} {tag} time {time} MACs {macs} cycles {stats[offset + 8]}")
        # print(platform)
        if platform == 5:
            # AP5, so only PMU counters - either 4 (normal mode) or all of them (full mode)
            
            if params.full_pmu_capture:
                row.append(mc.modelStructureDetails.estimateStrings[i])
                row.append(mc.modelStructureDetails.output_magnitude[i])
                row.append(mc.modelStructureDetails.outputShapes[i])
                row.append(mc.modelStructureDetails.mac_filter_list[i])
                row.append(mc.modelStructureDetails.stride_h[i])
                row.append(mc.modelStructureDetails.stride_w[i])
                row.append(mc.modelStructureDetails.dilation_h[i])
                row.append(mc.modelStructureDetails.dilation_w[i])
                log.info(f"Full PMU Capture, {len(overall_pmu_stats)} layers")
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

    if params.joulescope or params.onboard_perf:
        log.info(tabulate(table, headers="firstrow", tablefmt="simple"))
    else:
        print(tabulate(table, headers="firstrow", tablefmt="simple"))

    if platform == 5:
        if params.joulescope or params.onboard_perf:
            log.info(
                f"Model Performance Analysis: Total Inference Time {totalTime} us, total estimated MACs {totalMacs}, layers {captured_events}"
            )
            log.info(
                f"Model Performance Analysis: MAC/second {(totalMacs*1000000/totalTime):.2f}"
            )
        else:
            print(
                f"[NS] Model Performance Analysis: Total Inference Time {totalTime} us, total estimated MACs {totalMacs}, layers {captured_events}"
            )
            print(
                f"[NS] Model Performance Analysis: MAC/second {(totalMacs*1000000/totalTime):.2f}"
            )        
    else:
        log.info(
            f"Model Performance Analysis: Total Inference Time {totalTime} us, total estimated MACs {totalMacs}, total cycles {totalCycles}, layers {captured_events}"
        )
        log.info(
            f"Model Performance Analysis: MAC/second {(totalMacs*1000000/totalTime):.2f}, cycles/MAC {(totalCycles/totalMacs):.2f}"
        )

    log.info(
        "Model Performance Analysis: Per-layer performance statistics saved to: %s"
        % stats_filename
    )

    np.savetxt(stats_filename + ".csv", table, delimiter=", ", fmt="% s")

    # Save as an excel file
    df = pd.DataFrame(table[1:], columns=table[0])
    df = df.map(lambda x: x.encode('unicode_escape').decode('utf-8') if isinstance(x, str) else x)

    # Save as an analysis-ready Excel workbook
    df = pd.DataFrame(table[1:], columns=table[0])
    df = df.map(lambda x: x.encode('unicode_escape').decode('utf-8') if isinstance(x, str) else x)
    _elem_bits = derive_elem_bits_per_layer(mc)
    _core_mhz = 250 #getattr(params, "core_mhz", 0)  # fallback, PMU cycles preferred
    _emit_analysis_workbook(stats_filename + ".xlsx", df, _core_mhz, _elem_bits)

    # df.to_excel(stats_filename + ".xlsx", index=False)

    # if params.profile_results_path is not 'none', copy the files to the specified directory with a unique name based on platform, model, tf version, compiler, and timestamp
    if params.profile_results_path != "none":
        # create a unique name for the directory
        import datetime, os, shutil
        if aot:
            unique_name = f"{params.model_name}_{params.platform}_AOT_{params.ambiqsuite_version}_{params.toolchain}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"
        else:
            unique_name = f"{params.model_name}_{params.platform}_{params.tensorflow_version}_{params.ambiqsuite_version}_{params.toolchain}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"
        # make it safe by removing spaces and . from the name
        unique_name = unique_name.replace(" ", "_").replace(".", "_")
        os.makedirs(params.profile_results_path, exist_ok=True)

        # copy the files
        shutil.copy(stats_filename + ".csv", os.path.join(params.profile_results_path, unique_name + ".csv"))
        shutil.copy(stats_filename + ".xlsx", os.path.join(params.profile_results_path, unique_name + ".xlsx"))

    return totalCycles, totalMacs, totalTime, captured_events, pmu_events_per_layer


def compile_and_deploy(params, mc, first_time=False, aot=False):
   # The following lines find the paths relative to the cwd
    model_path = params.destination_rootdir + "/" + params.model_name
    d = os.path.join(params.neuralspot_rootdir, model_path)
    relative_build_path = os.path.relpath(d, start=params.neuralspot_rootdir) # this line is used so that the Makefile doens't put the absolute path in the build directory

    # Windows sucks
    if os.name == "posix":
        ws3 = "/dev/null"
        ws = "-j"
        ws1 = "&&"
    else:
        ws3 = "NUL"
        ws = "-j4"
        ws1 = "&"
        # d = d.replace("/", "\\")
        relative_build_path = relative_build_path.replace("\\", "/")

    # Platform Settings
    if params.toolchain == "arm":
        ps = f"PLATFORM={params.platform} TOOLCHAIN={params.toolchain} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"
    else:
        # toolchain is gcc, which is default
        ps = f"PLATFORM={params.platform} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"
    example = "tflm_validator"
    if first_time:
        makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make clean >{ws3} 2>&1 ")
    else:
        # The first time (create-binary) we only create the TFLM binary, otherwise we honor the aot flag
        if aot:
            example = "aot_validator"

    if (params.tflm_location == "ITCM"):
        itcm = "TFLM_IN_ITCM=1"
    else:
        itcm = ""

    # If full PMU capture is enabled and the platform is AP5, we need to increase the stack size
    if params.full_pmu_capture and ((params.platform == "apollo510_evb") or (params.platform == "apollo510L_eb") or (params.platform == "apollo510b_evb")):
        itcm = itcm + " STACK_SIZE_IN_32B_WORDS=5120"

    if (params.create_profile) or (params.create_binary):

        if params.verbosity > 3:
            print(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE={example} MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE={example} deploy"
            )

            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE={example} MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE={example} deploy"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset")
        else:
            # print(f"cd .. {ws1} make {ws} {ps} AUTODEPLOY=1 ADPATH={d} TFLM_VALIDATOR=1 EXAMPLE=tflm_validator MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 ADPATH={d} EXAMPLE=tflm_validator TARGET=tflm_validator deploy >{ws3} 2>&1")

            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE={example} MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE={example} deploy >{ws3} 2>&1"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset >{ws3} 2>&1")
    else:
        if params.verbosity > 3:
            print (
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={example} {ws1} make ADPATH={relative_build_path} AUTODEPLOY=1 EXAMPLE={example} deploy")
            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={example} {ws1} make ADPATH={relative_build_path} AUTODEPLOY=1 EXAMPLE={example} deploy"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset")           
        else:
            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} {itcm} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={example} >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={example} deploy >{ws3} 2>&1"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset >{ws3} 2>&1")

    if makefile_result != 0:
        print("[ERROR] Make failed, return code %d" % makefile_result)
        exit("Make failed, return code %d" % makefile_result)
        return makefile_result
    time.sleep(3)
    # os.system(f"cd {params.neuralspot_rootdir} {ws1} make reset {ps}  >{ws3} 2>&1")
    # print("[NS] Resetting after flashing the firmware")
    # reset_dut(params)
    # time.sleep(6)
    # reset_dut()
    # wait for key press
    # input("Press Enter to continue...")
    return makefile_result



def create_mut_metadata(params, tflm_dir, mc, aot):
    """
    Create mut_model_metadata.h, a config header for examples/tflm_validator. Can be
     used to create the default (large buffer) configuration or a version tuned on
     stats discovered by running the default
    """
    # Put arena and model in location defined by params
    if (params.model_location == "flash"):
        ns_ad_large_arena = 0

    # TODO adjust 120 for AP5's TCM size
    if (mc.arena_size_k + mc.arena_size_scratch_buffer_padding_k) > 120:
        ns_ad_large_arena = 1
    else:
        ns_ad_large_arena = 0

    # TODO pass in large model setting
    if (params.model_location == "PSRAM"):
        ns_ad_large_model = 1
    else:
        ns_ad_large_model = 0

    # If Apollo5, load PMU definitions
    if params.platform in ["apollo510_eb", "apollo510_evb", "apollo510L_eb", "apollo510b_evb"] :
        pmu_defs = read_pmu_definitions(params)
        ev0 = pmu_defs["PMU_EVENT0"]["name"]
        ev1 = pmu_defs["PMU_EVENT1"]["name"]
        ev2 = pmu_defs["PMU_EVENT2"]["name"]
        ev3 = pmu_defs["PMU_EVENT3"]["name"]
    else:
        pmu_defs = None
        ev0 = "PMU_EVENT0_NA"
        ev1 = "PMU_EVENT1_NA"
        ev2 = "PMU_EVENT2_NA"
        ev3 = "PMU_EVENT3_NA"

    rm = {
        "NS_AD_LARGE_MODEL": ns_ad_large_model,
        "NS_AD_MODEL_LOCATION": f"NS_AD_{params.model_location}",
        "NS_AD_ARENA_LOCATION": f"NS_AD_{params.arena_location}",
        "NS_AD_RPC_BUFSIZE": mc.adjusted_stat_buffer_size,
        "NS_AD_ARENA_SIZE": mc.arena_size_k + mc.arena_size_scratch_buffer_padding_k,
        "NS_AD_LARGE_ARENA": ns_ad_large_arena,
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_MAC_ESTIMATE_COUNT": len(mc.modelStructureDetails.macEstimates),
        "NS_AD_MAC_ESTIMATE_LIST": str(mc.modelStructureDetails.macEstimates)        
        .replace("[", "")
        .replace("]", ""),
        "NS_AD_PMU_EVENT_0": ev0,
        "NS_AD_PMU_EVENT_1": ev1,
        "NS_AD_PMU_EVENT_2": ev2,
        "NS_AD_PMU_EVENT_3": ev3,
        "NS_AD_TRANSPORT": f"NS_AD_RPC_TRANSPORT_{params.transport}",
        "NS_AD_AOT_VALUE": 1 if aot else 0,
        "NS_AD_AOT_NUM_LAYERS": mc.aot_layers if aot else 0,
        "NS_AD_AOT_LAST_IDENTIFIER": mc.aot_layer_last_identifier if aot else 0,
    }
    log.info(
        "Create metadata file with %dk arena size, %dk padding, RPC RX/TX buffer %d, RV Count %d"
        % (
            mc.arena_size_k,
            mc.arena_size_scratch_buffer_padding_k,
            mc.adjusted_stat_buffer_size,
            mc.rv_count,
        )
    )
    template_directory = str(importlib.resources.files(__name__) / "templates")

    createFromTemplate(
        template_directory + "/validator/template_mut_metadata.h",
        f"{tflm_dir}/src/mut_model_metadata.h",
        rm,
    )


def create_mut_modelinit(tflm_dir, mc):
    # print(f"[DEBUG] Creating mut_model_init.cc")
    adds, addsLen = mc.modelStructureDetails.getAddList()
    rm = {
        "NS_AD_NAME": "tflm_validator",
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_LAYER_METADATA_CODE": mc.modelStructureDetails.code,
    }
    template_directory = str(importlib.resources.files(__name__) / "templates")
    createFromTemplate(
        template_directory + "/validator/template_tflm_model.cc",
        f"{tflm_dir}/src/mut_model_init.cc",
        rm,
    )


def create_mut_main(params, tflm_dir, mc, md, aot):
    """
    New generator that installs the refactored validator sources:
      - Common runtime/RPC/memory/chunk headers & sources
      - TFLM main + runtime glue   → src/refactor/
      - AOT  main + runtime glue   → src/refactor/aot/
    Also installs:
      - tflm_validator.h (structures shared with RPC)
      - tflm_ns_model.h  (ns model helpers used by TFLM runtime)
      - module.mk (already globs src/refactor/*)
    """
    # Base dirs
    src_dir        = os.path.join(tflm_dir, "src")
    refactor_dir   = src_dir #os.path.join(src_dir, "refactor")
    refactor_aot   = os.path.join(refactor_dir, "aot")
    os.makedirs(refactor_dir, exist_ok=True)
    if aot:
        os.makedirs(refactor_aot, exist_ok=True)

    # Paths to our template tree
    tmpl_root   = importlib.resources.files(__name__)
    tmpl_common = str(tmpl_root / "templates/validator")
    tmpl_tflm   = os.path.join(tmpl_common, "tflm")
    tmpl_aot    = os.path.join(tmpl_common, "aot")

    # --- Common refactor files (headers + sources) ---
    for fname in [
        "validator_chunk.c",
        "validator_chunk.h",
        "validator_mem.c",
        "validator_mem.h",
        "validator_rpc.c",
        "validator_runtime_iface.h",
    ]:
        shutil.copyfile(os.path.join(tmpl_common, fname),
                        os.path.join(refactor_dir, fname))

    # --- Runtime-specific files ---
    if aot:
        # AOT main + glue live under aot/
        typeMap = {"<class 'numpy.float32'>": "float", "<class 'numpy.int8'>": "int8_t", "<class 'numpy.uint8'>": "uint8_t", "<class 'numpy.int16'>": "int16_t"}
        flatInput = [
            np.array(element).tolist() for sublist in mc.exampleTensors.inputTensors for element in sublist
        ]
        flatOutput = [
            np.array(element).tolist() for sublist in mc.exampleTensors.outputTensors for element in sublist
        ]
        rm = {
            "NS_AD_NAME": params.model_name,
            "NS_AD_INPUT_TENSOR_TYPE": typeMap[str(md.inputTensors[0].type)],
            "NS_AD_OUTPUT_TENSOR_TYPE": typeMap[str(md.outputTensors[0].type)],
            "NS_AD_INPUT_TENSOR_LEN": len(flatInput),
            "NS_AD_OUTPUT_TENSOR_LEN": len(flatOutput),
        }
        createFromTemplate(os.path.join(tmpl_aot, "aot_validator_main.c"),
                           os.path.join(refactor_dir, "aot_validator_main.c"),
                           rm)
        createFromTemplate(os.path.join(tmpl_aot, "validator_runtime_aot.c"),
                           os.path.join(refactor_dir, "validator_runtime_aot.c"),
                           rm)
    else:
        # TFLM main + glue live directly in refactor/
        # Apply template to tflm_validator_main.c
        rm = {
            "NS_AD_NAME": params.model_name,
            "NS_AD_LAYER_METADATA_CODE": mc.modelStructureDetails.code,
            "NS_AD_MAC_ESTIMATE_LIST": str(mc.modelStructureDetails.macEstimates)        
            .replace("[", "")
            .replace("]", ""),
        }
        createFromTemplate(os.path.join(tmpl_tflm, "tflm_validator_main.c"),
                           os.path.join(refactor_dir, "tflm_validator_main.c"),
                           rm)
        # shutil.copyfile(os.path.join(tmpl_tflm, "tflm_validator_main.c"),
        #                 os.path.join(refactor_dir, "tflm_validator_main.c"))
        shutil.copyfile(os.path.join(tmpl_tflm, "validator_runtime_tflm.cc"),
                        os.path.join(refactor_dir, "validator_runtime_tflm.cc"))

    # --- Headers used by the new sources ---
    # tflm_validator.h (structs used by both mains and RPC)
    shutil.copyfile(str(tmpl_root / "templates/validator/template_tflm_validator.h"),
                    os.path.join(src_dir, "tflm_validator.h"))
    # ns model helper for TFLM runtime glue
    shutil.copyfile(str(tmpl_root / "templates/common/template_ns_model.h"),
                    os.path.join(src_dir, "tflm_ns_model.h"))

    # --- Makefile (already includes src/refactor/* in both variants) ---
    if aot:
        mk_template = str(tmpl_root / "templates/validator/template_aot_validator.mk")
        rm = {"NS_AD_NAME_AOT": params.model_name}
        createFromTemplate(mk_template, f"{tflm_dir}/module.mk", rm)
    else:
        mk_template = str(tmpl_root / "templates/validator/template_tflm_validator.mk")
        shutil.copyfile(mk_template, f"{tflm_dir}/module.mk")

def create_validation_binary(params, mc, md, baseline, aot):
    if aot:
        subdir = "aot_validator"
    else:
        subdir = "tflm_validator"
    validator_dir = params.destination_rootdir + "/" + params.model_name + "/" + subdir
    create_mut_main(params, validator_dir, mc, md, aot)

    # map model location parameter to linker locations
    if params.model_location == "SRAM":
        loc = "sram"
    elif params.model_location == "MRAM":
        loc = "const"
    else:
        loc = ""

    if not params.model_location == "PSRAM" and not aot:
        # if baseline:
        xxd_c_dump(
            src_path=params.tflite_filename,
            dst_path=validator_dir + "/src/mut_model_data.h",
            var_name="mut_model",
            chunk_len=12,
            is_header=True,
            loc=loc,
            align_language="c"
        )

    if baseline:
        print(
            f"[NS] Compiling and deploying Baseline image: arena size = {mc.arena_size_k}k, arena location = {params.arena_location} model_location = {params.model_location}, Resource Variables count = {mc.rv_count}"
        )
    elif aot:
        print(
            f"[NS] Compiling and deploying AOT image: arena location = {params.arena_location} model_location = {params.model_location}, Resource Variables count = {mc.rv_count}"
        )
    else:
        print(
            f"[NS] Compiling and deploying Tuned image:    arena size = {mc.arena_size_k}k, arena location = {params.arena_location} model_location = {params.model_location}, Resource Variables count = {mc.rv_count}"
        )

    create_mut_metadata(params, validator_dir, mc, aot)
    create_mut_modelinit(validator_dir, mc)
    compile_and_deploy(params, mc, first_time=baseline, aot=aot)
    time.sleep(3)


# def get_interpreter(params):
#     # tf.lite.experimental.Analyzer.analyze(model_path=params.tflite_filename)
#     with suppress_os_stdio():
#         from ai_edge_litert.interpreter import Interpreter
#         interp = Interpreter(model_path=params.tflite_filename)  # first try (may enable XNNPACK)

#         try:
#             interp.allocate_tensors()
#             return interp
#         except Exception as e:
#             print("XNNPACK failed during allocate_tensors (%s). Retrying without XNNPACK.", e)
#             os.environ["TF_LITE_DISABLE_XNNPACK"] = "1"  # disable delegate for this process
#             # Recreate a clean interpreter (avoid any cached delegate state)
#             interp = Interpreter(model_path=params.tflite_filename)  # now XNNPACK is disabled
#             interp.allocate_tensors()
#             return interp


# def get_interpreter(params):
#     """
#     Create an AI Edge LiteRT interpreter, with a robust fallback path that
#     explicitly disables XNNPACK (some builds ignore TF_LITE_DISABLE_XNNPACK).
#     """
#     # Import outside of the stdout/stderr suppression so TypeErrors bubble up cleanly
#     from ai_edge_litert.interpreter import Interpreter

#     def _make_and_alloc(**kwargs):
#         # Suppress noisy C++ logs during creation/allocation only
#         with suppress_os_stdio():
#             interp = Interpreter(model_path=params.tflite_filename, **kwargs)
#             interp.allocate_tensors()
#             return interp

#     # 1) First try: whatever the runtime wants (env may already disable XNNPACK)
#     try:
#         return _make_and_alloc()
#     except Exception as e1:
#         # 2) Hard-disable delegates and retry. Some runtimes use "experimental_delegates",
#         # others "custom_delegates". We also pin threads to 1 to avoid any implicit XNNPACK path.
#         os.environ["TF_LITE_DISABLE_XNNPACK"] = "1"  # guard future creations in this process

#         last_err = e1

#         for delegates_kw in ("experimental_delegates", "custom_delegates"):
#             try:
#                 return _make_and_alloc(**{delegates_kw: [], "num_threads": 1})
#             except TypeError:
#                 # This kw doesn't exist in this build — try the next spelling.
#                 pass
#             except Exception as e2:
#                 last_err = e2
#                 # If we got here, the kw existed but creation/allocation still failed.
#                 # We’ll still try a final minimal retry below.

#         # 3) Final fallback: just pin threads (some builds honor this to avoid XNNPACK)
#         try:
#             return _make_and_alloc(num_threads=1)
#         except Exception as e3:
#             raise RuntimeError(
#                 "Failed to create LiteRT interpreter even after disabling XNNPACK "
#                 f"(first error: {e1!r}; last error: {e3!r})"
#             ) from e3

def get_interpreter(params):
    """
    Create an AI Edge LiteRT interpreter with XNNPACK and other auto-delegates
    aggressively disabled. We try several API surfaces because different builds
    expose different flags (kw-args vs. an options object with setters).
    """
    import os
    from ai_edge_litert import interpreter as _tfl

    # Hard-disable via env *before* creating any interpreter objects.
    # Set multiple environment variables to ensure XNNPACK is disabled
    os.environ["TF_LITE_DISABLE_XNNPACK"] = "1"
    os.environ["TF_LITE_DISABLE_DELEGATES"] = "1"
    os.environ["TF_LITE_DISABLE_DEFAULT_DELEGATES"] = "1"
    os.environ["TF_LITE_DISABLE_DELEGATE_PLUGINS"] = "1"
    # Additional environment variables that might help
    os.environ["TF_LITE_USE_XNNPACK"] = "0"
    os.environ["TF_LITE_EXPERIMENTAL_USE_XNNPACK"] = "0"

    def _try_with_options():
        """Prefer an options object when available; disable all delegate plugins."""
        if not hasattr(_tfl, "InterpreterOptions"):
            raise TypeError("InterpreterOptions not available")
        opts = _tfl.InterpreterOptions()

        # Try both attribute-style and setter-style controls (varies by build).
        def _set(opt, name, value):
            try:
                setattr(opt, name, value)
                return True
            except Exception:
                pass
            try:
                setter = getattr(opt, f"set_{name}", None)
                if callable(setter):
                    setter(value)
                    return True
            except Exception:
                pass
            return False

        # Threads
        _set(opts, "num_threads", 1)

        # Nuke all delegate lists we can find
        for field in ("experimental_delegates", "custom_delegates", "delegates"):
            _set(opts, field, [])

        # Kill auto delegate plugins (XNNPACK is one of them in many builds)
        for flag in ("disable_delegate_plugins", "disable_default_delegates"):
            _set(opts, flag, True)

        # Explicit XNNPACK off (name varies by build)
        for flag in ("experimental_use_xnnpack", "use_xnnpack", "enable_xnnpack"):
            _set(opts, flag, False)

        # Construct + allocate
        with suppress_os_stdio():
            interp = _tfl.Interpreter(model_path=params.tflite_filename, options=opts)
            interp.allocate_tensors()
            return interp

    def _try_with_kwargs():
        """Older builds accept kw-args on the Interpreter ctor."""
        kwargs_tried = []

        # Most aggressive first: disable plugins, no delegates, 1 thread, xnnpack off (if known)
        candidate_kw_sets = [
            {"experimental_delegates": [], "num_threads": 1,
             "disable_delegate_plugins": True, "experimental_use_xnnpack": False},
            {"experimental_delegates": [], "num_threads": 1,
             "disable_default_delegates": True, "use_xnnpack": False},
            {"custom_delegates": [], "num_threads": 1, "experimental_use_xnnpack": False},
            {"experimental_delegates": [], "num_threads": 1},
            {"num_threads": 1},
            # Try with explicit empty delegate lists
            {"delegates": [], "num_threads": 1},
            {"experimental_delegates": [], "custom_delegates": [], "delegates": [], "num_threads": 1},
        ]
        for kws in candidate_kw_sets:
            try:
                with suppress_os_stdio():
                    interp = _tfl.Interpreter(model_path=params.tflite_filename, **kws)
                    interp.allocate_tensors()
                    return interp
            except TypeError:
                # This build doesn’t support some kw – try the next combination.
                kwargs_tried.append(kws)
                continue
        # If we got here, kw route failed.
        raise RuntimeError(f"Interpreter kw-arg route failed; tried: {kwargs_tried!r}")

    # Try options() path first (newer / plugin-aware builds), then kw-args.
    first_err = None
    try:
        return _try_with_options()
    except Exception as e:
        first_err = e
    try:
        return _try_with_kwargs()
    except Exception as last_err:
        # Final fallback: try with absolute minimal configuration
        try:
            log.warning("All standard XNNPACK disabling methods failed. Trying minimal configuration...")
            with suppress_os_stdio():
                # Try with the most basic interpreter creation possible
                interp = _tfl.Interpreter(model_path=params.tflite_filename)
                interp.allocate_tensors()
                return interp
        except Exception as final_err:
            # Last resort: try to create interpreter with explicit delegate exclusion
            try:
                log.warning("Minimal configuration failed. Trying with explicit delegate exclusion...")
                with suppress_os_stdio():
                    # Try to create interpreter and manually remove XNNPACK delegates
                    interp = _tfl.Interpreter(model_path=params.tflite_filename)
                    # Get the interpreter's delegate list and remove XNNPACK
                    try:
                        # Some builds expose delegate information
                        if hasattr(interp, '_interpreter') and hasattr(interp._interpreter, 'GetDelegates'):
                            delegates = interp._interpreter.GetDelegates()
                            # Remove any XNNPACK delegates
                            filtered_delegates = [d for d in delegates if 'XNNPACK' not in str(type(d))]
                            if len(filtered_delegates) < len(delegates):
                                log.info(f"Removed {len(delegates) - len(filtered_delegates)} XNNPACK delegates")
                    except Exception:
                        pass  # Ignore if delegate manipulation isn't available
                    interp.allocate_tensors()
                    return interp
            except Exception as delegate_err:
                # If all else fails, provide a more helpful error message
                log.error(f"All interpreter creation methods failed. This model appears to be incompatible with the current TensorFlow Lite runtime.")
                log.error(f"Model: {params.tflite_filename}")
                log.error(f"TensorFlow Lite version: {getattr(_tfl, '__version__', 'unknown')}")
                
                # Provide specific guidance based on the error
                if "XNNPACK" in str(delegate_err):
                    log.error("The model is failing due to XNNPACK delegate issues. This suggests:")
                    log.error("1. The model may have been optimized for XNNPACK but is incompatible with the current runtime")
                    log.error("2. The model may contain operations not supported by XNNPACK")
                    log.error("3. There may be a version mismatch between the model and runtime")
                    log.error("Consider:")
                    log.error("- Using a different model that doesn't rely on XNNPACK")
                    log.error("- Re-converting the model without XNNPACK optimizations")
                    log.error("- Updating the TensorFlow Lite runtime")
                
                raise RuntimeError(
                    f"Failed to create LiteRT interpreter for model '{params.tflite_filename}'. "
                    f"This model appears to be incompatible with the current TensorFlow Lite runtime (version {getattr(_tfl, '__version__', 'unknown')}). "
                    f"Options error: {first_err!r}; kwargs error: {last_err!r}; "
                    f"minimal config error: {final_err!r}; delegate exclusion error: {delegate_err!r}. "
                    "Consider trying a different model or updating the TensorFlow Lite runtime."
                ) from delegate_err
