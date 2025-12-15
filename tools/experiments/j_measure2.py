#!/usr/bin/env python3
import argparse
import dataclasses
import logging
import queue
import sys
import threading
import time
from typing import Optional, Dict, Any

import numpy as np
from joulescope import scan


log = logging.getLogger("j_measure")
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s: %(message)s")


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
    collect_timeout=60.0,
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


def main(argv=None):
    parser = argparse.ArgumentParser(description="Time an operation and measure energy with Joulescope.")
    parser.add_argument("--reduction-frequency", default="50 Hz",
                        help="Statistics update rate, e.g. '50 Hz' or '2 Hz' (default: 50 Hz)")
    parser.add_argument("--io-voltage", default="1.8V", help="EXTIO voltage (default: 1.8V)")
    parser.add_argument("--start-ack-timeout", type=float, default=5.0,
                        help="Seconds to wait for initial acknowledge level (default: 5)")
    parser.add_argument("--ready-timeout", type=float, default=5.0,
                        help="Seconds to wait for the ready level before collecting (default: 5)")
    parser.add_argument("--collect-timeout", type=float, default=60.0,
                        help="Maximum seconds to collect before timing out (default: 60)")
    parser.add_argument("--ack-level", type=int, default=3,
                        help="Numeric GPI level that indicates 'ack' (default: 3)")
    parser.add_argument("--ready-level", type=int, default=0,
                        help="Numeric GPI level that indicates 'ready/collect' (default: 0)")
    args = parser.parse_args(argv)

    try:
        snap = measure_once(
            reduction_frequency=args.reduction_frequency,
            io_voltage=args.io_voltage,
            ack_level=args.ack_level,
            ready_level=args.ready_level,
            start_ack_timeout=args.start_ack_timeout,
            ready_timeout=args.ready_timeout,
            collect_timeout=args.collect_timeout,
        )
    except KeyboardInterrupt:
        print("Interrupted.", file=sys.stderr)
        return 130
    except Exception as exc:
        log.error("Measurement failed: %s", exc)
        return 1

    duration_s = snap.t1 - snap.t0
    # Pretty print results
    def fmt(v, unit):
        if v is None or (isinstance(v, float) and not np.isfinite(v)):
            return "n/a"
        if abs(v) >= 1e3 or (abs(v) > 0 and abs(v) < 1e-3):
            return f"{v:.6e} {unit}"
        return f"{v:.6f} {unit}"

    print("----- Joulescope Measurement -----")
    print(f"Elapsed inference time: {duration_s:.6f} s")
    print(f"Mean current: {fmt(snap.i_mean, 'A')}")
    print(f"Mean voltage: {fmt(snap.v_mean, 'V')}")
    print(f"Mean power:   {fmt(snap.p_mean, 'W')}")
    print(f"Charge:       {fmt(snap.charge_c, 'C')}")
    print(f"Energy:       {fmt(snap.energy_j, 'J')}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

