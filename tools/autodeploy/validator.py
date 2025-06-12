import logging as log
import os
import shutil
import struct
import sys
import time
import pkg_resources
import erpc
import numpy as np
import pandas as pd
import tensorflow as tf
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
    read_pmu_definitions
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
            self.input_magnitude_list
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
    # print("Config il %d, ol %d" % (inputLength, outputLength))
    configBlock = GenericDataOperations_PcToEvb.common.dataBlock(
        description="Model Config",
        dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
        cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
        buffer=configBytes,
        length=9,
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
            retries -= 1
        else:
            break

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
    for i in range(md.numOutputs):
        differences.append([])

    for i in tqdm(range(runs)):
        inExamples = []
        # Generate or load data
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
            otOffset += ot.bytes
            otIndex += 1

        if i == 0:
            # capture first input and output tensor set
            # Input is from local TF, output is from EVB
            mc.update_from_validation(inExamples, outExamples)

    return differences


def printStats(params, mc, stats, stats_filename, pmu_csv_header, overall_pmu_stats):
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

    Where:
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
    # computed_arena_size = stats[0] # in bytes
    computed_stat_buffer_size = stats[1]  # in bytes
    computed_stat_per_event_size = (
        stats[2] // 4
    )  # stats[2] is in bytes, convert to uint32
    pmu_count = stats[3]  # generally one event per model layer
    pmu_events_per_layer = stats[4]  # generally one event per model layer
    captured_events = stats[5]  # generally one event per model layer
    platform = stats[6]  # AP3, AP4, AP5, etc

    # If full PMU, use PMU header, othwerwise extract from stats array
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
    csv_elements = csv_header.split(",")
    table = [csv_elements]
    # print(csv_elements)
    # print(table)
    totalMacs = 0
    totalTime = 0
    totalCycles = 0

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

    totalMacs += 1
    log.info(tabulate(table, headers="firstrow", tablefmt="simple"))
    if platform == 5:
        log.info(
            f"Model Performance Analysis: Total Inference Time {totalTime} us, total estimated MACs {totalMacs}, layers {captured_events}"
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

    # print("table is ", table)
    # for r in table:
    #     print(r)

    np.savetxt(stats_filename + ".csv", table, delimiter=", ", fmt="% s")

    # Save as an excel file
    df = pd.DataFrame(table[1:], columns=table[0])
    df = df.map(lambda x: x.encode('unicode_escape').decode('utf-8') if isinstance(x, str) else x)
    df.to_excel(stats_filename + ".xlsx", index=False)

    # if params.profile_results_path is not 'none', copy the files to the specified directory with a unique name based on platform, model, tf version, compiler, and timestamp
    if params.profile_results_path != "none":
        # create a unique name for the directory
        import datetime
        unique_name = f"{params.model_name}_{params.platform}_{params.tensorflow_version}_{params.ambiqsuite_version}_{params.toolchain}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"
        # make it safe by removing spaces and . from the name
        unique_name = unique_name.replace(" ", "_").replace(".", "_")
        # create the directory
        os.makedirs(params.profile_results_path, exist_ok=True)
        # print(f"Copying profile results {unique_name} to {params.profile_results_path}")

        # copy the files
        shutil.copy(stats_filename + ".csv", os.path.join(params.profile_results_path, unique_name + ".csv"))
        shutil.copy(stats_filename + ".xlsx", os.path.join(params.profile_results_path, unique_name + ".xlsx"))

    return totalCycles, totalMacs, totalTime, captured_events, pmu_events_per_layer


def compile_and_deploy(params, mc, first_time=False):
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
        ws = "-j"
        ws1 = "&"
        # d = d.replace("/", "\\")
        relative_build_path = relative_build_path.replace("\\", "/")

    # Platform Settings
    ps = f"PLATFORM={params.platform} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"

    if first_time:
        makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make clean >{ws3} 2>&1 ")

    if params.create_profile:

        if params.verbosity > 3:
            print(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} VERBOSE=1 AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE=tflm_validator MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE=tflm_validator TARGET=tflm_validator deploy"
            )

            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} VERBOSE=1 AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE=tflm_validator MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE=tflm_validator TARGET=tflm_validator deploy"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset")
        else:
            # print(f"cd .. {ws1} make {ws} {ps} AUTODEPLOY=1 ADPATH={d} TFLM_VALIDATOR=1 EXAMPLE=tflm_validator MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 ADPATH={d} EXAMPLE=tflm_validator TARGET=tflm_validator deploy >{ws3} 2>&1")

            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} TFLM_VALIDATOR=1 EXAMPLE=tflm_validator MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE=tflm_validator TARGET=tflm_validator deploy >{ws3} 2>&1"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset >{ws3} 2>&1")
    else:
        if params.verbosity > 3:

            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE=tflm_validator {ws1} make ADPATH={relative_build_path} AUTODEPLOY=1 TARGET=tflm_validator deploy"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset")           
        else:
            makefile_result = os.system(
                f"cd {params.neuralspot_rootdir} {ws1} make {ws} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE=tflm_validator >{ws3} 2>&1 {ws1} make AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE=tflm_validator TARGET=tflm_validator deploy >{ws3} 2>&1"
            )
            # time.sleep(3)
            # makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ws1} make {ps} reset >{ws3} 2>&1")

    if makefile_result != 0:
        print("[ERROR] Make failed, return code %d" % makefile_result)
        exit("Make failed, return code %d" % makefile_result)
        return makefile_result

    time.sleep(3)
    # reset_dut()

    return makefile_result



def create_mut_metadata(params, tflm_dir, mc):
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
    if params.platform in ["apollo510_eb", "apollo510_evb"] :
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
    template_directory = pkg_resources.resource_filename(__name__, "templates")

    createFromTemplate(
        template_directory + "/validator/template_mut_metadata.h",
        f"{tflm_dir}/src/mut_model_metadata.h",
        rm,
    )


def create_mut_modelinit(tflm_dir, mc):
    adds, addsLen = mc.modelStructureDetails.getAddList()
    rm = {
        "NS_AD_NAME": "tflm_validator",
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_LAYER_METADATA_CODE": mc.modelStructureDetails.code,
    }
    template_directory = pkg_resources.resource_filename(__name__, "templates")
    createFromTemplate(
        template_directory + "/validator/template_tflm_model.cc",
        f"{tflm_dir}/src/mut_model_init.cc",
        rm,
    )


def create_mut_main(params, tflm_dir, mc):
    # make directory for tflm_validator
    os.makedirs(tflm_dir + "/src/", exist_ok=True)

    # Copy template main.cc to tflm_dir
    template_file = pkg_resources.resource_filename(__name__, "templates/validator/template_tflm_validator.cc")
    rm = {
        "NS_AD_NAME": params.model_name,
        "NS_AD_LAYER_METADATA_CODE": mc.modelStructureDetails.code,
    }
    createFromTemplate(template_file, f"{tflm_dir}/src/tflm_validator.cc", rm)
    # shutil.copyfile(template_file, f"{tflm_dir}/src/tflm_validator.cc")

    template_file = pkg_resources.resource_filename(__name__, "templates/validator/template_tflm_validator.h")
    shutil.copyfile(template_file, f"{tflm_dir}/src/tflm_validator.h")

    template_file = pkg_resources.resource_filename(__name__, "templates/validator/template_tflm_validator.mk")
    shutil.copyfile(template_file, f"{tflm_dir}/module.mk")

    template_file = pkg_resources.resource_filename(__name__, "templates/common/template_ns_model.h")
    shutil.copyfile(template_file, f"{tflm_dir}/src/tflm_ns_model.h")


def create_validation_binary(params, baseline, mc):
    # tflm_dir = params.tflm_src_path
    tflm_dir = params.destination_rootdir + "/" + params.model_name + "/tflm_validator"
    create_mut_main(params, tflm_dir, mc)

    # map model location parameter to linker locations
    if params.model_location == "SRAM":
        loc = "sram"
    elif params.model_location == "MRAM":
        loc = "const"
    else:
        loc = ""

    if not params.model_location == "PSRAM":
        if baseline:
            xxd_c_dump(
                src_path=params.tflite_filename,
                dst_path=tflm_dir + "/src/mut_model_data.h",
                var_name="mut_model",
                chunk_len=12,
                is_header=True,
                loc=loc,
            )

    if baseline:
        print(
            f"[NS] Compiling and deploying Baseline image: arena size = {mc.arena_size_k}k, arena location = {params.arena_location} model_location = {params.model_location}, Resource Variables count = {mc.rv_count}"
        )
    else:
        print(
            f"[NS] Compiling and deploying Tuned image:    arena size = {mc.arena_size_k}k, arena location = {params.arena_location} model_location = {params.model_location}, Resource Variables count = {mc.rv_count}"
        )

    create_mut_metadata(params, tflm_dir, mc)
    create_mut_modelinit(tflm_dir, mc)
    compile_and_deploy(params, mc, first_time=baseline)
    time.sleep(3)


def get_interpreter(params):
    # tf.lite.experimental.Analyzer.analyze(model_path=params.tflite_filename)
    interpreter = tf.lite.Interpreter(model_path=params.tflite_filename)
    interpreter.allocate_tensors()
    return interpreter
