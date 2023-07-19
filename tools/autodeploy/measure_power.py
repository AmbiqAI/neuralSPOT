import datetime
import logging as log
import os
import queue
import shutil
import signal
import time
import traceback

import numpy as np
from joulescope import scan
from ns_utils import createFromTemplate, xxd_c_dump


def generatePowerBinary(params, mc, md, cpu_mode):
    n = params.model_name + "_power"
    d = params.model_path
    adds, addsLen = mc.modelStructureDetails.getAddList()

    rm = {
        "NS_AD_NAME": n,
        "NS_AD_ARENA_SIZE": mc.arena_size_k + params.arena_size_scratch_buffer_padding,
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_POWER_RUNS": params.runs_power,
        "NS_AD_CPU_MODE": cpu_mode,
        "NS_AD_NUM_INPUT_VECTORS": md.numInputs,
        "NS_AD_NUM_OUTPUT_VECTORS": md.numOutputs,
    }
    print(f"Compiling and deploying {cpu_mode} power measurement binary at {d}/{n}")

    # Make destination directory
    os.system(f"mkdir -p {d}/{n}")
    os.system(f"mkdir -p {d}/{n}/src")

    # Generate files from template
    createFromTemplate(
        "autodeploy/templates/template.cc", f"{d}/{n}/src/{n}_model.cc", rm
    )
    createFromTemplate(
        "autodeploy/templates/template.h", f"{d}/{n}/src/{n}_model.h", rm
    )
    createFromTemplate(
        "autodeploy/templates/template_api.h", f"{d}/{n}/src/{n}_api.h", rm
    )
    createFromTemplate(
        "autodeploy/templates/template_power.cc", f"{d}/{n}/src/{n}.cc", rm
    )
    createFromTemplate(
        "autodeploy/templates/template_power.mk", f"{d}/{n}/module.mk", rm
    )

    # Copy needed files
    os.system(f"cp autodeploy/templates/ns_model.h {d}/{n}/src/")

    # Generate model weight file
    xxd_c_dump(
        src_path=params.tflite_filename,
        dst_path=f"{d}/{n}/src/{n}_model_data.h",
        var_name=f"{n}_model",
        chunk_len=12,
        is_header=True,
    )

    # Generate input/output tensor example data
    flatInput = [
        element for sublist in mc.exampleTensors.inputTensors for element in sublist
    ]
    flatOutput = [
        element for sublist in mc.exampleTensors.outputTensors for element in sublist
    ]
    inputs = str(flatInput).replace("[", "{").replace("]", "}")
    outputs = str(flatOutput).replace("[", "{").replace("]", "}")

    typeMap = {"<class 'numpy.float32'>": "float", "<class 'numpy.int8'>": "int8_t"}

    rm["NS_AD_INPUT_TENSORS"] = inputs
    rm["NS_AD_OUTPUT_TENSORS"] = outputs
    rm["NS_AD_INPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    rm["NS_AD_OUTPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    createFromTemplate(
        "autodeploy/templates/template_example_tensors.h",
        f"{d}/{n}/src/{n}_example_tensors.h",
        rm,
    )

    # Generate library and example binary
    if params.verbosity > 3:
        makefile_result = os.system(
            f"cd .. && make -j AUTODEPLOY=1 EXAMPLE={n} && make AUTODEPLOY=1 TARGET={n} EXAMPLE={n} deploy"
        )
    else:
        makefile_result = os.system(
            f"cd .. && make -j AUTODEPLOY=1 EXAMPLE={n} >/dev/null 2>&1 && make AUTODEPLOY=1 EXAMPLE={n} TARGET={n} deploy >/dev/null 2>&1"
        )
    # makefile_result = os.system(
    #         f"cd .. && make -j AUTODEPLOY=1 EXAMPLE={n} && make AUTODEPLOY=1 TARGET={n} EXAMPLE={n} deploy"
    #     )

    if makefile_result != 0:
        log.error("Makefile failed to build minimal example library")
        exit("Makefile failed to build minimal example library")


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


def measurePower():
    global state
    _quit = False
    statistics_queue = queue.Queue()  # resynchronize to main thread
    startTime = 0
    stopTime = 0

    def stop_fn(*args, **kwargs):
        nonlocal _quit
        _quit = True

    signal.signal(signal.SIGINT, stop_fn)  # also quit on CTRL-C
    devices = scan(config="auto")
    try:
        for device in devices:
            cbk = statistics_callback_factory(device, statistics_queue)
            device.statistics_callback_register(cbk, "sensor")
            device.close()
            device.open()
            device.parameter_set("reduction_frequency", "50 Hz")
            device.parameter_set("io_voltage", "3.3V")
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
                        device.parameter_set("gpo0", "0")  # send trigger to EVB
                        state = "getting_ready"
                        log.info("Waiting for trigger to be acknowledged")
                elif state == "getting_ready":
                    if gpi == 1:
                        state = "collecting"
                        device.statistics_accumulators_clear()
                        log.info("Collecting")
                        startTime = datetime.datetime.now()
                elif state == "collecting":
                    if gpi == 2:
                        stopTime = datetime.datetime.now()
                        # print ("Elapsed inference time: %d" % (stopTime - startTime))
                        td = stopTime - startTime
                        state = "reporting"
                        log.info("Done collecting")

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
