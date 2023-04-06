import os
import struct
import sys
import time

import erpc
import numpy as np
import tensorflow as tf
from ns_tflite_analyze import analyze_tflite_file
from ns_utils import (
    ModelDetails,
    TensorDetails,
    createFromTemplate,
    get_dataset,
    next_power_of_2,
    printDataBlock,
    reset_dut,
    xxd_c_dump,
)
from tabulate import tabulate
from tqdm import tqdm

sys.path.append("../neuralspot/ns-rpc/python/ns-rpc-genericdata/")
import GenericDataOperations_PcToEvb

modelConfigPreambleSize = 6  # number of uint32_t words
modelStatPreambleSize = 4  # number of uint32_t words


class ModelStructureDetails:
    def __init__(self, tflite_filename):
        (
            self.code,
            self.overallOpsNameList,
            self.overallMacEstimates,
            self.opsetList,
            graph_count,
        ) = analyze_tflite_file(tflite_filename)

        # Handle subgraphs: this code assumes there are a maximum of two graphs, and
        # the base graph (0) calls the subgraph only once (via CALL_ONCE). In order to
        # align this case with EVB execution, we insert the subgraph into where the CALL_ONCE is found
        if graph_count == 1:
            self.macEstimates = self.overallMacEstimates[0]
            self.opsNameList = self.overallOpsNameList[0]
        elif graph_count == 2:
            self.macEstimates = []
            self.opsNameList = []
            baseGraphIndex = 0
            for op in self.overallOpsNameList[0]:
                self.macEstimates.append(self.overallMacEstimates[0][baseGraphIndex])
                self.opsNameList.append(op)
                if op == "CALL_ONCE":
                    # insert the subgraph
                    self.macEstimates.extend(self.overallMacEstimates[1])
                    self.opsNameList.extend(self.overallOpsNameList[1])
                baseGraphIndex += 1


class ModelConfiguration:
    rv_count = 0  # Resource Variables needed by the model

    def __init__(self, params):
        self.arena_size_k = params.max_arena_size
        self.arena_size = params.max_arena_size * 1024
        self.stat_buffer_size = params.max_rpc_buf_size  # in bytes
        self.adjusted_stat_buffer_size = params.max_rpc_buf_size  # in bytes
        ModelConfiguration.rv_count = params.resource_variable_count
        self.stat_per_event_size = (
            -1
        )  # dontcare, -1 will force an error if used elsewhere
        self.events = -1
        self.modelStructureDetails = ModelStructureDetails(params.tflite_filename)

    def update_from_stats(self, stats, md):
        self.arena_size = stats[0]  # in bytes
        self.arena_size_k = (stats[0] // 1024) + 1
        self.stat_buffer_size = stats[1]  # in bytes
        self.stat_per_event_size = stats[2]  # in bytes
        self.events = stats[3]  # generally one event per model layer
        self.compute_buf_size(md)

    def compute_buf_size(self, md):
        self.adjusted_stat_buf_size = max(
            next_power_of_2(self.stat_buffer_size + 50),
            next_power_of_2(md.totalInputTensorBytes + 50),
            next_power_of_2(md.totalOutputTensorBytes + 50),
            512,  # min
        )

    def check(self, params):
        if self.arena_size_k > params.max_arena_size:
            print(
                "[ERROR] TF Arena Size is %dk, exceeding limit of %dk."
                % (self.arena_size_k, params.max_arena_size)
            )
        if self.adjusted_stat_buffer_size > params.max_rpc_buf_size:
            print(
                "[INFO] Needed RPC buffer size is %d, exceeding limit of %d. Switching to chunk mode."
                % (self.adjusted_stat_buffer_size, params.max_rpc_buf_size)
            )


def configModel(params, client, md):
    if params.create_profile:
        prof_enable = 1  # convert to int just to be explicit for serialization
    else:
        prof_enable = 0

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
    status = client.ns_rpc_data_sendBlockToEVB(configBlock)
    print("[INFO] Model Configuration Return Status = %d" % status)


def getModelStats(params, client):
    statBlock = erpc.Reference()
    status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
    print("[INFO] Fetch stats status %d" % status)

    stat_array = struct.unpack(
        "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
    )

    if statBlock.value.description != "FullStats":
        # Stats are too long to fit in one xfer. Repeated calls to fetchBlock will return chunks
        while statBlock.value.description != "LastStats":
            status = client.ns_rpc_data_fetchBlockFromEVB(statBlock)
            print(
                "[INFO] Fetch stats chunk status %d, msg %s"
                % (status, statBlock.value.description)
            )
            stat_array = stat_array + struct.unpack(
                "<" + "I" * (len(statBlock.value.buffer) // 4), statBlock.value.buffer
            )

    return stat_array


def chunker(seq, size):
    return (seq[pos : pos + size] for pos in range(0, len(seq), size))


def sendLongInputTensor(client, input_data, chunkLen):
    """
    When a tensor exceeds the RPC size limit, we chunk it
    """
    # ChunkLen is in bytes, convert to word size
    chunkLen = chunkLen // input_data.flatten().itemsize

    for chunk in chunker(input_data.flatten(), chunkLen):
        inputChunk = GenericDataOperations_PcToEvb.common.dataBlock(
            description="Input Chunk",
            dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
            cmd=GenericDataOperations_PcToEvb.common.command.write_cmd,
            buffer=chunk.tobytes(),
            length=len(chunk),
        )
        status = client.ns_rpc_data_sendBlockToEVB(inputChunk)


def validateModel(params, client, interpreter, md):
    """
    Generates input vectors (random or from a pkl) and sends them to both
    the local TFLite model and the one running on the EVB. It compares the
    results (small differences are OK due to differing runtimes and CPU numeric
    behavior) and prints a simple report. If profiling is enabled, it leverages
    neuralSPOT microprofiler extension to report on per-layer performance, include
    latency, cache behavior, and more.
    """
    runs = params.runs
    print("[INFO] Calling invoke %d times." % runs)

    if not params.random_data:
        # Load validation data from specified pkl file
        print("[INFO] Load dataset from %s." % params.dataset)
        data, labels, test_data, test_labels = get_dataset(params)
        input_scale = md.inputTensors[0].scale
        input_zero_point = md.inputTensors[0].zeroPoint
        test_data_int8 = np.asarray(
            test_data / input_scale + input_zero_point, dtype=np.int8
        )
    else:
        print("[INFO] Generate random dataset.")

    differences = (
        []
    )  # accumulator for per-output differences between local and EVB models
    for i in range(md.numOutputs):
        differences.append([])

    for i in tqdm(range(runs)):

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

        # Invoke locally and on EVB
        interpreter.set_tensor(md.input_details[0]["index"], input_data)
        interpreter.invoke()  # local invoke

        # Prepare input tensors (or pre-send them if chunking is needed) for xmit to EVB
        if md.inputTensors[0].bytes > (
            params.max_rpc_buf_size - 600
        ):  # TODO: calculate the 600
            sendLongInputTensor(client, input_data, (params.max_rpc_buf_size - 600))
            inputTensor = GenericDataOperations_PcToEvb.common.dataBlock(
                description="Empty Tensor",
                dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
                cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
                buffer=bytearray(),
                length=0,
            )
        else:
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

        stat = client.ns_rpc_data_computeOnEVB(inputTensor, outputTensor)  # on EVB
        if stat != 0:
            print(f"[ERROR] EVB invoke returned {stat}")

        # Resulting output tensors are packed into outputTensor buffer in order (0,1,...)
        # Each has a different length and type; unpack them into an array for later comparison.
        otOffset = 0
        otIndex = 0
        for ot in md.outputTensors:
            if ot.type is np.int8:
                out_array = list(
                    struct.unpack_from(
                        "<" + "b" * ot.bytes, outputTensor.value.buffer, otOffset
                    )
                )
            elif md.outputTensors[0].type is np.float32:
                out_array = list(
                    struct.unpack_from(
                        "<" + "f" * ot.words, outputTensor.value.buffer, otOffset
                    )
                )

            local_output_data = interpreter.get_tensor(
                md.output_details[otIndex]["index"]
            ).flatten()

            differences[otIndex].append(local_output_data - out_array)
            otOffset += ot.bytes
            otIndex += 1
    return differences


def printStats(stats, stats_filename):
    """
    Stats are from these structs in EVB-land:
    typedef struct {
        uint32_t computed_arena_size;
        uint32_t computed_stat_buffer_size;
        uint32_t captured_events;
        ns_profiler_event_stats_t stat_buffer[NS_PROFILER_RPC_EVENTS_MAX];
    } ns_mut_stats_t;

    Where:
    typedef struct {
        ns_cache_dump_t cache_delta;
        ns_perf_counters_t perf_delta;
        uint32_t estimated_macs;
        uint32_t elapsed_us;
        char tag[12]; ///< Tag from TFLM microprofiler
    } ns_profiler_event_stats_t;

    And:
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
    captured_events = stats[3]  # generally one event per model layer

    print(
        "[INFO] Decoding statistics. Number of events = %d, buff_size = %d size = %d arraylen %d"
        % (
            captured_events,
            computed_stat_buffer_size,
            computed_stat_per_event_size,
            len(stats),
        )
    )
    # for each captured event, decode stat_buffer into cach, perf, macs, and time
    offset = modelStatPreambleSize  # size of stats preamble

    # Array may not contain entire event log (limited by ns_ambiq_harness' NS_PROFILER_RPC_EVENTS_MAX), truncate to whatever
    # we got back
    if ((len(stats) - offset) // computed_stat_per_event_size) < captured_events:
        captured_events = (len(stats) - offset) // computed_stat_per_event_size
        print(
            "[WARNING] Number of events greater than allowed for by RPC buffer size (suggestion: increase NS_PROFILER_RPC_EVENTS_MAX). Statistics will be truncated to %d events"
            % captured_events
        )

    table = [
        [
            "Event",
            "Tag",
            "uSeconds",
            "Est MACs",
            "cycles",
            "cpi",
            "exc",
            "sleep",
            "lsu",
            "fold",
            "daccess",
            "dtaglookup",
            "dhitslookup",
            "dhitsline",
            "iaccess",
            "itaglookup",
            "ihitslookup",
            "ihitsline",
        ]
    ]

    totalMacs = 0
    totalTime = 0
    totalCycles = 0

    for i in range(captured_events):
        row = []
        time = stats[offset + 15]
        macs = stats[offset + 14]
        tag = str(
            struct.unpack(
                "<12s",
                struct.pack(
                    "<III", stats[offset + 16], stats[offset + 17], stats[offset + 18]
                ),
            )[0],
            "utf-8",
        )

        row.append(i)
        row.append(tag)
        row.append(time)
        row.append(macs)
        totalMacs += macs
        totalTime += time
        totalCycles += stats[offset + 8]
        for j in range(6):
            row.append(stats[offset + 8 + j])
        for j in range(8):
            row.append(stats[offset + j])
        table.append(row)
        offset = offset + computed_stat_per_event_size
    print(tabulate(table, headers="firstrow", tablefmt="fancy_grid"))
    print(
        f"[INFO] Model Performance Analysis: Total Inference Time {totalTime}uS, total estimated MACs {totalMacs}, total cycles {totalCycles}, layers {captured_events}"
    )
    print(
        f"[INFO] MAC/second {totalMacs*1000000/totalTime}, cycles/MAC {totalCycles/totalMacs}"
    )
    np.savetxt(stats_filename, table, delimiter=", ", fmt="% s")


def compile_and_deploy(params, first_time=False):

    if first_time:
        makefile_result = os.system("cd .. && make clean >/dev/null 2>&1")

    if params.create_profile:
        makefile_result = os.system(
            f"cd .. && make -j TFLM_VALIDATOR=1 MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={params.max_profile_events}>/dev/null 2>&1 && make TARGET=tflm_validator deploy >/dev/null 2>&1"
        )
    else:
        makefile_result = os.system(
            "cd .. && make -j >/dev/null 2>&1 && make TARGET=tflm_validator deploy >/dev/null 2>&1"
        )

    if makefile_result != 0:
        print("[ERROR] Make failed, return code %d" % makefile_result)
        return makefile_result

    time.sleep(2)
    reset_dut()
    return makefile_result


def create_mut_metadata(tflm_dir, mc):
    """
    Create mut_model_metadata.h, a config header for examples/tflm_validator. Can be
     used to create the default (large buffer) configuration or a version tuned on
     stats discovered by running the default
    """

    rm = {
        "NS_AD_RPC_BUFSIZE": mc.adjusted_stat_buffer_size,
        "NS_AD_ARENA_SIZE": mc.arena_size_k,
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_MAC_ESTIMATE_COUNT": len(mc.modelStructureDetails.macEstimates),
        "NS_AD_MAC_ESTIMATE_LIST": str(mc.modelStructureDetails.macEstimates)
        .replace("[", "")
        .replace("]", ""),
    }
    print(
        "[INFO] Create metadata file with %dk arena size, RPC RX/TX buffer %d, RV Count %d"
        % (mc.arena_size_k, mc.adjusted_stat_buffer_size, mc.rv_count)
    )
    createFromTemplate(
        "autodeploy/templates/template_mut_metadata.h",
        f"{tflm_dir}/mut_model_metadata.h",
        rm,
    )


def create_validation_binary(params, baseline, mc):
    tflm_dir = params.tflm_src_path

    if baseline:
        xxd_c_dump(
            src_path=params.tflite_filename,
            dst_path=tflm_dir + "/" + params.tflm_filename,
            var_name="mut_model",
            chunk_len=12,
            is_header=True,
        )
    print(
        f"[INFO] Compiling and deploying Validation image - baseline {baseline}, arena {mc.arena_size_k}k, RPC buffers {mc.adjusted_stat_buffer_size}, Resource Variables {mc.rv_count}"
    )
    create_mut_metadata(tflm_dir, mc)
    compile_and_deploy(params, first_time=baseline)


def get_interpreter(params):
    # tf.lite.experimental.Analyzer.analyze(model_path=params.tflite_filename)
    interpreter = tf.lite.Interpreter(model_path=params.tflite_filename)
    interpreter.allocate_tensors()
    return interpreter
