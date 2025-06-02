import datetime
import logging as log
import os
import queue
import shutil
import signal
import time
import traceback
import pkg_resources
import numpy as np
from joulescope import scan
from neuralspot.tools.ns_utils import createFromTemplate, xxd_c_dump, read_pmu_definitions
import yaml


def generateInputAndOutputTensors(params, mc, md):
    # Generate input and output tensors based on the model
    inExamples = []
    outExamples = []

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
    inExamples.append(input_data.flatten())  # Capture inputs for AutoGen

    # Output tensor is not checked in ns_perf runs, just generate random data
    if md.outputTensors[0].type == np.int8:
        output_data = np.random.randint(
            -127, 127, size=tuple(md.outputTensors[0].shape), dtype=np.int8
        )
    else:
        output_data = (
            np.random.random(size=tuple(md.outputTensors[0].shape)).astype(
                md.outputTensors[0].type
            )
            * 2
            - 1
        )
    outExamples.append(output_data.flatten())  # Capture outputs for AutoGen
    mc.update_from_validation(inExamples, outExamples)


def generatePowerBinary(params, mc, md, cpu_mode):
    # The following 5 lines find the paths relative to the cwd
    model_path = params.destination_rootdir + "/" + params.model_name
    d = os.path.join(params.neuralspot_rootdir, model_path)
    relative_build_path = os.path.relpath(d, params.neuralspot_rootdir)
    n = params.model_name + "_power"
    adds, addsLen = mc.modelStructureDetails.getAddList()
    if params.joulescope or params.onboard_perf:
        generateInputAndOutputTensors(params, mc, md)
    ns_cpu_mode = ""
    if cpu_mode == "LP":
        ns_cpu_mode = "NS_MINIMUM_PERF"
    elif cpu_mode == "HP":
        ns_cpu_mode = "NS_MAXIMUM_PERF"

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
        "NS_AD_NAME": n,
        "NS_AD_ARENA_SIZE": mc.arena_size_k + params.arena_size_scratch_buffer_padding,
        "NS_AD_MODEL_LOCATION": f"NS_AD_{params.model_location}",
        "NS_AD_ARENA_LOCATION": f"NS_AD_{params.arena_location}",
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_POWER_RUNS": params.runs_power,
        "NS_AD_CPU_MODE": ns_cpu_mode,
        "NS_AD_JS_PRESENT": "1" if params.joulescope else "0",
        "NS_AD_NUM_INPUT_VECTORS": md.numInputs,
        "NS_AD_NUM_OUTPUT_VECTORS": md.numOutputs,
        "NS_AD_MAC_ESTIMATE_COUNT": len(mc.modelStructureDetails.macEstimates),
        "NS_AD_MAC_ESTIMATE_LIST": str(mc.modelStructureDetails.macEstimates)
        .replace("[", "")
        .replace("]", ""),
        "NS_AD_PMU_EVENT_0": ev0,
        "NS_AD_PMU_EVENT_1": ev1,
        "NS_AD_PMU_EVENT_2": ev2,
        "NS_AD_PMU_EVENT_3": ev3,
        "NS_AD_PERF_NAME": params.model_name,
        "NS_AD_LAYER_METADATA_CODE": mc.modelStructureDetails.code,
    }
    print(
        f"[NS] Compiling, deploying, and measuring {cpu_mode} power, model location = {params.model_location}, arena location = {params.arena_location}."
    )

    # Make destination directory
    os.makedirs(f"{d}/{n}", exist_ok=True)
    os.makedirs(f"{d}/{n}/src", exist_ok=True)

    # os.system(f"mkdir -p {d}/{n}")
    # os.system(f"mkdir -p {d}/{n}/src")

    template_directory = pkg_resources.resource_filename(__name__, "templates")

    # Generate files from template
    createFromTemplate(
        template_directory + "/common/template_ns_model.cc", f"{d}/{n}/src/{n}_model.cc", rm
    )
    createFromTemplate(
        template_directory + "/common/template_model_metadata.h", f"{d}/{n}/src/{n}_model.h", rm
    )
    createFromTemplate(
        template_directory + "/common/template_api.h", f"{d}/{n}/src/{n}_api.h", rm
    )
    createFromTemplate(
        template_directory + "/perf/template_power.cc", f"{d}/{n}/src/{n}.cc", rm
    )
    createFromTemplate(
        template_directory + "/perf/template_power.mk", f"{d}/{n}/module.mk", rm
    )

    # Copy needed files
    createFromTemplate(
        template_directory + "/common/template_ns_model.h", f"{d}/{n}/src/ns_model.h", rm
    )

    postfix = ""
    if params.model_location == "SRAM":
        loc = "const" # will be compied over to SRAM
        postfix = "_for_sram"
    elif params.model_location == "MRAM":
        loc = "const"
    elif params.model_location == "PSRAM":
        loc = "const" # needs to be copied to PSRAM from MRAM
    else:
        loc = ""

    # Generate model weight file
    xxd_c_dump(
        src_path=params.tflite_filename,
        dst_path=f"{d}/{n}/src/{n}_model_data.h",
        var_name=f"{n}_model{postfix}",
        chunk_len=12,
        is_header=True,
        loc=loc,
    )

    # Generate input/output tensor example data
    flatInput = [
        np.array(element).tolist() for sublist in mc.exampleTensors.inputTensors for element in sublist
    ]
    flatOutput = [
        np.array(element).tolist() for sublist in mc.exampleTensors.outputTensors for element in sublist
    ]
    inputs = str(flatInput).replace("[", "{").replace("]", "}")
    outputs = str(flatOutput).replace("[", "{").replace("]", "}")

    typeMap = {"<class 'numpy.float32'>": "float", "<class 'numpy.int8'>": "int8_t", "<class 'numpy.uint8'>": "uint8_t", "<class 'numpy.int16'>": "int16_t"}

    rm["NS_AD_INPUT_TENSORS"] = inputs
    rm["NS_AD_OUTPUT_TENSORS"] = outputs
    rm["NS_AD_INPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    rm["NS_AD_OUTPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    createFromTemplate(
        template_directory + "/common/template_example_tensors.h",
        f"{d}/{n}/src/{n}_example_tensors.h",
        rm,
    )

    # Remove ../ from path
    d = d.replace("../", "")

    # Windows sucks
    if os.name == "posix":
        ws_null = "/dev/null"
        ws_j = "-j"
        ws_and = "&&"
        ws_p = "-p"
    else:
        ws_null = "NUL"
        ws_j = "-j"
        ws_and = "&&"
        ws_p = ""
        relative_build_path = relative_build_path.replace("\\", "/")

    # Platform Settings
    if params.toolchain == "arm":
        ps = f"PLATFORM={params.platform} TOOLCHAIN={params.toolchain} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"
    else:
        # toolchain is gcc, which is default
        ps = f"PLATFORM={params.platform} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"

    # Generate library and example binary
    if params.onboard_perf:
        mlp = f"MLPROFILE=1 TFLM_VALIDATOR_MAX_EVENTS={mc.modelStructureDetails.layers} TFLM_VALIDATOR=1"
    else:
        mlp = ""

    if params.verbosity > 3:
        print(
            f"cd {params.neuralspot_rootdir} {ws_and} make clean {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} {mlp} EXAMPLE={n} {ws_and} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} TARGET={n} EXAMPLE={n} deploy"
        )
        makefile_result = os.system(
            f"cd {params.neuralspot_rootdir} {ws_and} make clean {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} {mlp} EXAMPLE={n} {ws_and} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} TARGET={n} EXAMPLE={n} deploy"
        )
    else:
        makefile_result = os.system(
            f"cd {params.neuralspot_rootdir} {ws_and} make clean >{ws_null} 2>&1 {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path}  {mlp} EXAMPLE={n} >{ws_null} 2>&1 {ws_and} make {ps} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={n} TARGET={n} deploy >{ws_null} 2>&1"
        )

    if makefile_result != 0:
        log.error("Makefile failed to build power measurement binary")
        exit("Makefile failed to build power measurement binary")
    # else:
    #     print("Makefile successfully built power measurement binary")

    # Do one more reset
    time.sleep(3)
    os.system(f"cd {params.neuralspot_rootdir} {ws_and} make reset {ps}  >{ws_null} 2>&1")
    time.sleep(10)


# Joulescope-specific Code

state = "start"  # waiting, collecting, reporting
i = 0
v = 0
p = 0
c = 0
e = 0


def statistics_callback(serial_number, stats):
    global latest_values
    global state
    global i, v, p, c, e
    """The function called for each statistics.

    :param serial_number: The serial number producing with this update.
    :param stats: The statistics data structure.
    """
    t = stats["time"]["range"]["value"][0]
    i = stats["signals"]["current"]["µ"]
    v = stats["signals"]["voltage"]["µ"]
    p = stats["signals"]["power"]["µ"]
    c = stats["accumulators"]["charge"]
    e = stats["accumulators"]["energy"]

    fmts = ["{x:.9f}", "{x:.3f}", "{x:.9f}", "{x:.9f}", "{x:.9f}"]
    values = []
    for k, fmt in zip([i, v, p, c, e], fmts):
        value = fmt.format(x=k["value"])
        value = f'{value} {k["units"]}'
        values.append(value)
    ", ".join(values)
    # print(f"{serial_number} {t:.1f}: " + ', '.join(values))

    if state == "reporting":
        state = "quit"


def statistics_callback_factory(device, queue):
    def cbk(data, indicator=None):
        serial_number = str(device).split(":")[-1]
        queue.put((serial_number, data))

    return cbk


def handle_queue(q):
    while True:
        try:
            args = q.get(block=False)
            statistics_callback(*args)
        except queue.Empty:
            # print("Queue empty")
            return  # no more data


def measurePower(params):
    global state
    _quit = False
    statistics_queue = queue.Queue()  # resynchronize to main thread
    startTime = 0
    stopTime = 0

    def stop_fn(*args, **kwargs):
        nonlocal _quit
        _quit = True

    # if params.platform in ["apollo3p_evb", "apollo_evb"]:
    #     # AP3 needs 3.3v GPIO
    #     print("Setting GPIO to 3.3V")
    #     gpioVolts = "3.3V"
    # else:
    #     gpioVolts = "1.8V"

    signal.signal(signal.SIGINT, stop_fn)  # also quit on CTRL-C
    devices = scan(config="auto")
    try:
        for device in devices:
            cbk = statistics_callback_factory(device, statistics_queue)
            device.statistics_callback_register(cbk, "sensor")
            device.close()
            device.open()
            device.parameter_set("reduction_frequency", "50 Hz")
            if params.platform in ["apollo3p_evb", "apollo_evb"]:
                device.parameter_set("io_voltage", "3.3V")
            else:
                device.parameter_set("io_voltage", "1.8V")
            device.parameter_set("sensor_power", "on")
            device.parameter_set("i_range", "auto")
            device.parameter_set("v_range", "15V")
            device.statistics_accumulators_clear()
            device.parameter_set("gpo0", "1")  # send trigger to EVB
        while not _quit:
            for device in devices:
                gpi = device.extio_status()["gpi_value"]["value"]
                if state == "start":
                    if gpi == 3:
                        device.parameter_set("gpo0", "0")  # clear trigger to EVB
                        state = "getting_ready"
                        # print("Waiting for trigger to be acknowledged...", end="")
                elif state == "getting_ready":
                    if gpi == 0:
                        state = "collecting"
                        device.statistics_accumulators_clear()
                        # print("Collecting...", end="")
                        startTime = datetime.datetime.now()
                elif state == "collecting":
                    if gpi != 0:
                        stopTime = datetime.datetime.now()
                        # print ("Elapsed inference time: %d" % (stopTime - startTime))
                        td = stopTime - startTime
                        state = "reporting"
                        # print("Done collecting.")

                elif state == "quit":
                    state = "start"
                    stop_fn()

                device.status()
            time.sleep(0.01)
            handle_queue(statistics_queue)

    except:
        log.error("Failed interacting with Joulescope")
        traceback.print_exc()

    finally:
        for device in devices:
            device.stop()
            device.close()
            return td, i, v, p, c, e
