import datetime
import logging as log
import os
import queue
import shutil
import signal
import time
import traceback
import warnings
import importlib.resources
import numpy as np
from joulescope import scan

import dataclasses
from typing import Optional, Dict, Any, List, Tuple
import threading
from neuralspot.tools.ns_utils import createFromTemplate, xxd_c_dump, read_pmu_definitions
import yaml

# Suppress Joulescope packet index warnings


# Suppress all warnings from joulescope-related modules
for logger_name in ["joulescope", "pyjoulescope_driver", "jsdrv"]:
    log.getLogger(logger_name).setLevel(log.ERROR)

# Also suppress warnings at the system level for joulescope modules
# warnings.filterwarnings("ignore", module="joulescope")
# warnings.filterwarnings("ignore", module="pyjoulescope_driver")
# warnings.filterwarnings("ignore", module="jsdrv")

# Suppress Joulescope atexit cleanup errors
import sys
import atexit

def suppress_joulescope_atexit_errors():
    """Suppress Joulescope atexit cleanup errors that occur during program shutdown"""
    import warnings
    warnings.filterwarnings("ignore", message=".*jsdrv_unsubscribe failed.*")
    warnings.filterwarnings("ignore", message=".*jsdrv_publish timed out.*")
    warnings.filterwarnings("ignore", message=".*NOT_FOUND.*")

# Register the suppression function to run at exit
atexit.register(suppress_joulescope_atexit_errors)

# Monkey patch Joulescope device close to handle cleanup errors gracefully
def safe_device_close(original_close):
    """Wrapper around device.close() that suppresses cleanup errors"""
    def wrapper(self, *args, **kwargs):
        try:
            return original_close(self, *args, **kwargs)
        except (TimeoutError, RuntimeError) as exc:
            if "jsdrv_unsubscribe failed" in str(exc) or "jsdrv_publish timed out" in str(exc):
                # Silently ignore these cleanup errors
                pass
            else:
                # Re-raise other errors
                raise
    return wrapper

# Apply the monkey patch if joulescope is available
try:
    from joulescope.v1.device import Device
    Device.close = safe_device_close(Device.close)
except ImportError:
    pass


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


def generatePowerBinary(params, mc, md, cpu_mode, aot):
    # The following 5 lines find the paths relative to the cwd
    model_path = params.destination_rootdir + "/" + params.model_name
    d = os.path.join(params.neuralspot_rootdir, model_path)
    relative_build_path = os.path.relpath(d, params.neuralspot_rootdir)
    # n_module and n_aot are module names, n is the filename base
    n_module = params.model_name + "_power"
    n_aot = params.model_name
    n = params.model_name + "_power"
    if aot:
        n = n + "_aot"

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
        "NS_AD_NAME_AOT": n_aot, # only used when aot is True
        "NS_AD_NAME": n_module,
        "NS_AD_AOT_LAYERS": mc.aot_layers,
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
        "NS_AD_AOT": "1" if aot else "0",
    }
    if aot:
        print(f"[NS] Compiling, deploying, and measuring AOT {cpu_mode} power, model location = {params.model_location}, arena location = {params.arena_location}.")
    else:  
        print(
            f"[NS] Compiling, deploying, and measuring {cpu_mode} power, model location = {params.model_location}, arena location = {params.arena_location}."
        )

    # Make destination directory
    os.makedirs(f"{d}/{n}", exist_ok=True)
    os.makedirs(f"{d}/{n}/src", exist_ok=True)

    template_directory = str(importlib.resources.files(__name__) / "templates")

    # Generate files from template
    createFromTemplate(
        template_directory + "/perf/template_power.cc", f"{d}/{n}/src/{n}.cc", rm
    )

    createFromTemplate(
        template_directory + "/common/template_api.h", f"{d}/{n}/src/{n}_api.h", rm
    )

    # The following files are for TFLM, not AOT
    if not aot:
        createFromTemplate(
            template_directory + "/common/template_ns_model.cc", f"{d}/{n}/src/{n}_model.cc", rm
        )
        createFromTemplate(
            template_directory + "/common/template_model_metadata.h", f"{d}/{n}/src/{n}_model.h", rm
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
        elif params.model_location == "PSRAM" or params.model_location == "NVM":
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
    else:
        # AOT files were already generated by HeliosAOT, point to them
        createFromTemplate(
            template_directory + "/perf/template_power_aot.mk", f"{d}/{n}/module.mk", rm
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
    # length of output tensor array
    rm["NS_AD_OUTPUT_TENSOR_LEN"] = len(flatOutput)
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
            f"cd {params.neuralspot_rootdir} {ws_and} make clean {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} {mlp} EXAMPLE={n} {ws_and} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE={n} deploy"
        )
        makefile_result = os.system(
            f"cd {params.neuralspot_rootdir} {ws_and} make clean {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path} {mlp} EXAMPLE={n} {ws_and} make AUTODEPLOY=1 {ps} ADPATH={relative_build_path} EXAMPLE={n} deploy"
        )
    else:
        makefile_result = os.system(
            f"cd {params.neuralspot_rootdir} {ws_and} make clean >{ws_null} 2>&1 {ws_and} make {ws_j} {ps} AUTODEPLOY=1 ADPATH={relative_build_path}  {mlp} EXAMPLE={n} >{ws_null} 2>&1 {ws_and} make {ps} AUTODEPLOY=1 ADPATH={relative_build_path} EXAMPLE={n} deploy >{ws_null} 2>&1"
        )

    if makefile_result != 0:
        log.error("Makefile failed to build power measurement binary")
        exit("Makefile failed to build power measurement binary")
    # else:
    #     print("Makefile successfully built power measurement binary")


    # Do one more reset
    time.sleep(6)
    # reset if measuring power
    if params.joulescope:
        os.system(f"cd {params.neuralspot_rootdir} {ws_and} make reset {ps}  >{ws_null} 2>&1")
    else:
        time.sleep(6)
    if (params.model_location == "PSRAM" or params.arena_location == "PSRAM"):
        time.sleep(10) # wait for PSRAM to be ready
    else:
        if aot:
            time.sleep(12)
        else:
            time.sleep(10)


# Joulescope-specific Code


def joulescope_power_on_old():
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
            try:
                device.close()
            except (TimeoutError, RuntimeError) as exc:
                log.warning(f"Failed to close Joulescope device during initialization: {exc}")
            except Exception as exc:
                log.warning(f"Failed to close Joulescope device during initialization: {exc}")
        return True
    except Exception as exc:
        log.error(f"Failed to initialize Joulescope (likely collision with Joulescope app): {exc}")
        return False
    



def ensure_sensor_power_on(dev, retries: int = 3, delay_s: float = 0.1) -> bool:
    """
    Safely power on the Joulescope sensor.
    - No-ops if it's already on.
    - Verifies state after setting and retries a few times.

    :return: True if sensor is on (or confirmed on), False otherwise.
    """
    def _is_on(val) -> bool:
        # parameter_get may return 'on'/'off' or numeric 1/0
        if isinstance(val, str):
            return val.lower() == "on"
        try:
            return int(val) != 0
        except Exception as exc:
            log.debug("sensor_power get failed (%s); will attempt to enable.", exc)
            return False

    try:
        cur = dev.parameter_get("sensor_power")
        if _is_on(cur):
            log.debug("Sensor power already on.")
            return True
    except Exception as exc:
        log.debug("sensor_power get failed (%s); will attempt to enable.", exc)

    for attempt in range(1, retries + 1):
        try:
            dev.parameter_set("sensor_power", "on")
        except Exception as exc:
            log.debug("sensor_power set attempt %d failed: %s", attempt, exc)
        time.sleep(delay_s)
        try:
            cur = dev.parameter_get("sensor_power")
            if _is_on(cur):
                return True
        except Exception as exc:
            log.debug("sensor_power get attempt %d failed: %s", attempt, exc)
            pass
    log.error(f"Failed to initialize Joulescope (likely collision with Joulescope app)")
    return False


@dataclasses.dataclass
class StatsSnapshot:
    t0: float = 0.0           # measurement window start (monotonic)
    t1: float = 0.0           # measurement window end (monotonic)
    i_mean: Optional[float] = None
    v_mean: Optional[float] = None
    p_mean: Optional[float] = None
    charge_c: Optional[float] = None
    energy_j: Optional[float] = None
    last_raw: Optional[Dict[str, Any]] = None  # last full stats payload (for debugging)


class SafeStats:
    """Thread-safe holder for the last stats received."""
    def __init__(self):
        self._lock = threading.Lock()
        self._last: Optional[Dict[str, Any]] = None

    def update(self, stats: Dict[str, Any]):
        with self._lock:
            self._last = stats

    def get(self) -> Optional[Dict[str, Any]]:
        with self._lock:
            return self._last


def _extract_mean(sig: Dict[str, Any]) -> Optional[float]:
    # Support both keys: 'µ' (micro sign) and 'mean' to be defensive across API variants
    if sig is None:
        return None
    if "µ" in sig and isinstance(sig["µ"], dict):
        return sig["µ"].get("value")
    if "mean" in sig and isinstance(sig["mean"], dict):
        return sig["mean"].get("value")
    return None


def _extract_accum(stats: Dict[str, Any], key: str) -> Optional[float]:
    acc = stats.get("accumulators", {})
    if not isinstance(acc, dict):
        return None
    k = acc.get(key)
    if isinstance(k, dict):
        return k.get("value")
    return None


def _stats_callback_factory(device, sink: SafeStats):
    """Keep callback tiny: just stash data for the main thread."""
    def cbk(data, indicator=None):
        # device may be a proxy; avoid heavy ops here
        sink.update(data)
    return cbk


def _wait_for_gpi(device, predicate, timeout_s: float, poll_s: float = 0.01) -> bool:
    """Poll GPI until predicate(extio_value) is True or timeout."""
    t_end = time.monotonic() + timeout_s
    while time.monotonic() < t_end:
        try:
            gpi_val = device.extio_status()["gpi_value"]["value"]
        except Exception as exc:
            log.warning("extio_status failed: %s", exc)
            gpi_val = None
        if gpi_val is not None and predicate(gpi_val):
            return True
        time.sleep(poll_s)
    return False


def measure_once(
    reduction_frequency="50 Hz",
    io_voltage="1.8V",
    ack_level=3,
    ready_level=0,
    start_ack_timeout=5.0,
    ready_timeout=5.0,
    collect_timeout=300.0,
) -> StatsSnapshot:
    """
    Perform one measurement:
      1) Pulse GPO0 high to request start
      2) Wait for GPI == ack_level (acknowledge)
      3) Drop GPO0 low
      4) Wait for GPI == ready_level (ready-to-start window)
      5) Clear accumulators and mark t0
      6) Collect until GPI != ready_level (done) or timeout
    """
    devices = scan(config="auto")
    if not devices:
        raise RuntimeError("No Joulescope devices found.")

    if len(devices) > 1:
        log.info("Multiple devices detected; using the first: %s", devices[0])

    dev = devices[0]
    sink = SafeStats()
    cbk = _stats_callback_factory(dev, sink)

    opened = False
    cbk_registered = False
    try:
        dev.open()
        opened = True

        # Minimal, safe configuration
        dev.parameter_set("sensor_power", "on")
        dev.parameter_set("i_range", "auto")
        dev.parameter_set("v_range", "15V")
        dev.parameter_set("reduction_frequency", reduction_frequency)
        dev.parameter_set("io_voltage", io_voltage)

        # Register stats callback AFTER device is configured, BEFORE we start pulses
        dev.statistics_callback_register(cbk, "sensor")
        cbk_registered = True

        # Ensure known GPO state
        dev.parameter_set("gpo0", "0")
        time.sleep(0.005)

        # Pulse start: set high
        dev.parameter_set("gpo0", "1")

        # 1) Wait for acknowledge (both inputs high? caller passes ack_level=3)
        if not _wait_for_gpi(dev, lambda g: g == ack_level, start_ack_timeout):
            raise TimeoutError(f"Start acknowledge not observed on GPI within {start_ack_timeout}s")

        # 2) Drop start line
        dev.parameter_set("gpo0", "0")

        # 3) Wait for ready window (usually both low)
        if not _wait_for_gpi(dev, lambda g: g == ready_level, ready_timeout):
            raise TimeoutError(f"Ready level {ready_level} not observed on GPI within {ready_timeout}s")

        # 4) Clear accumulators & start timing
        dev.statistics_accumulators_clear()
        t0 = time.monotonic()

        # 5) Collect until GPI != ready_level (operation done) or timeout
        t_end = time.monotonic() + collect_timeout
        while time.monotonic() < t_end:
            gpi_val = dev.extio_status()["gpi_value"]["value"]
            if gpi_val != ready_level:
                break
            # keep the loop light; stats arrive via callback
            time.sleep(0.01)
        else:
            raise TimeoutError(f"Collection exceeded timeout of {collect_timeout}s")

        t1 = time.monotonic()

        # Drain a tiny moment to ensure we have a recent stats snapshot
        time.sleep(0.05)
        last = sink.get()
        if last is None:
            # Stats can be delayed at very low reduction rates; try once more
            time.sleep(0.2)
            last = sink.get()
        if last is None:
            raise RuntimeError("No statistics were received from the device.")

        snap = StatsSnapshot(
            t0=t0,
            t1=t1,
            i_mean=_extract_mean((last.get("signals") or {}).get("current")),
            v_mean=_extract_mean((last.get("signals") or {}).get("voltage")),
            p_mean=_extract_mean((last.get("signals") or {}).get("power")),
            charge_c=_extract_accum(last, "charge"),
            energy_j=_extract_accum(last, "energy"),
            last_raw=last,
        )
        return snap

    finally:
        # Always leave the device and driver in a clean state
        try:
            if cbk_registered:
                dev.statistics_callback_unregister(cbk)
        except Exception:
            pass
        try:
            if opened:
                # Best effort to leave outputs low
                try:
                    dev.parameter_set("gpo0", "0")
                except Exception:
                    pass
                dev.stop()
                dev.close()
        except Exception:
            pass

def ensure_sensor_power_on_standalone(
    all_devices: bool = False,
    retries: int = 3,
    delay_s: float = 0.1,
) -> List[Tuple[str, bool]]:
    """
    Standalone power-on: scan, open device(s), ensure sensor power ON (noop if already on),
    then close. Returns a list of (device_str, success) results.
    """
    devices = scan(config="auto")
    if not devices:
        raise RuntimeError("No Joulescope devices found.")

    targets = devices if all_devices else [devices[0]]
    results: List[Tuple[str, bool]] = []

    for dev in targets:
        opened = False
        name = str(dev)
        try:
            dev.open()
            opened = True
            ok = ensure_sensor_power_on(dev, retries=retries, delay_s=delay_s)
            results.append((name, ok))
        except Exception as exc:
            log.error("Failed power-on for %s: %s", name, exc)
            results.append((name, False))
        finally:
            try:
                if opened:
                    dev.close()
            except Exception:
                pass

    return results

def joulescope_power_on():
    result = ensure_sensor_power_on_standalone()
    if not result[0][1]:
        return False
    return True


def measurePower(params):
    # Call measure_once and return the results as previously done
    if params.platform in ["apollo3p_evb", "apollo_evb"]:
        io_voltage = "3.3V"
    else:
        io_voltage = "1.8V"
    snap = measure_once(io_voltage=io_voltage) 
    # return td, i, v, p, c, e

    total_seconds = snap.t1 - snap.t0
    return total_seconds, snap.i_mean, snap.v_mean, snap.p_mean, snap.charge_c, snap.energy_j
