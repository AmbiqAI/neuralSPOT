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
        print(f"{serial_number} {t:.1f}: " + ',' .join(values))


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

def joulescope_power_on():
    # Sometimes autodeploy is invoked with Joulescope powered off.
    # Ensure we can talk to joulescope and turn on the DUT power.
    devices = scan(config="auto")
    if not devices:
        log.error("No Joulescope devices found. Ensure Joulescope is connected and powered on.")
        return False
    try:
        for device in devices:
            device.open()
            device.parameter_set("sensor_power", "on")
            device.parameter_set("i_range", "auto")
            device.parameter_set("v_range", "15V")
            device.parameter_set("reduction_frequency", "50 Hz")
            # if params.platform in ["apollo3p_evb", "apollo_evb"]:
            #     device.parameter_set("io_voltage", "3.3V")
            # else:
            device.parameter_set("io_voltage", "1.8V")
            device.close()
        return True
    except Exception as exc:
        log.error(f"Failed to initialize Joulescope: {exc}")
        return False
    
def measurePower():
    global state
    global i, v, p, c, e
    i, v, p, c, e = 0, 0, 0, 0, 0    
    _quit = False
    statistics_queue = queue.Queue()  # resynchronize to main thread
    startTime = 0
    stopTime = 0
    td = 0  # time delta


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
            try:
                device.open()
            except Exception as exc:
                log.error(f"Failed to open device {device} (ensure Joulescope App is not running): {exc}")
                return 0, 0, 0, 0, 0, 0
            device.parameter_set("sensor_power", "on")
            device.parameter_set("i_range", "auto")            
            device.parameter_set("reduction_frequency", "50 Hz")
            # if params.platform in ["apollo3p_evb", "apollo_evb"]:
            #     device.parameter_set("io_voltage", "3.3V")
            # else:
            device.parameter_set("io_voltage", "1.8V")

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
                        print ("Elapsed inference time:", stopTime - startTime)
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
        return 0, 0, 0, 0, 0, 0

    finally:
        for device in devices:
            device.stop()
            device.close()
        return td, i, v, p, c, e


if __name__ == "__main__":
    try:
        td, i, v, p, c, e = measurePower()
    except:
        log.error("Failed to measure power")
        traceback.print_exc()
        exit(1)
