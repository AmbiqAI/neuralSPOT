# neuralSPOT Harness Library

ns-harness is a collection of "harness" functions that wrap commonly used AmbiqSuite and TFLM operations.



## AmbiqSuite Harness

The AmbiqSuite harness includes AI- and power-optimized wrappers for common operations:

| Function              | Description                                                  | Comments                               |
| --------------------- | ------------------------------------------------------------ | -------------------------------------- |
| ns_lp_printf          | Power-aware printf() - enables only the minimum peripherals needed to printf, and only when needed. |                                        |
| ns_itm_printf_enable  | Power-aware ITM enable - turns on crypto block only as long as actually needed. | Must call ns_core_init() before using. |
| ns_uart_printf_enable | Power-aware UART print enable.                               |                                        |
| ns_itm_printf_disable | Power-aware ITM disable - turns off blocks turned on by ns_itm_printf_enable. |                                        |

## TFLM Debug Harness

This includes support for TFLM debug prints and logs, and for an Ambiq-specific version of the TFLM MicroProfiler.

neuralSPOT includes 3 variations of each TFLM version's static library:

| Variation         | Description                                                  | make flag                                        |
| ----------------- | ------------------------------------------------------------ | ------------------------------------------------ |
| Release           | TFLM compiled _with no prints_ and with all optimization flags set | (no flag, this is the default)                   |
| Release-with-logs | TFLM compiled _with prints_ and with all optimization flags set. Can be used for MicroProfiler. | `make MLPROFILE=1`                               |
| Debug             | TFLM compiled _with prints_, with no optimization, and with symbols included. Can be used for MicroProfiler. | `make MLDEBUG=1` or `make MLDEBUG=1 MLPROFILE=1` |

Generally, MLPROFILE=1 is used to identify bottlenecks when optimizing TFLM model architectures, and MLDEBUG=1 is used when 'stepping into' TFLM execution using GDB or similar.

### Using the MicroProfiler

To use the MicroProfiler, the binary must be compiled using a print-enabled TFLM static library

```bash
make MLPROFILE=1
```

To use the MicroProfiler, it must be instantiated, initialized, and invoked:

```c
#include "ns_debug_log.h"
#include "tensorflow/lite/micro/micro_profiler.h"

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
#endif

main() {
#ifdef NS_MLPROFILE
    static tflite::MicroProfiler micro_profiler; // instantiate
    profiler = &micro_profiler;
    ns_perf_mac_count_t basic_mac = { // from static analysis, see below
        .number_of_layers = kws_ref_model_number_of_estimates,
        .mac_count_map = kws_ref_model_mac_estimates
    };
    ns_TFDebugLogInit(&basic_tickTimer, &basic_mac);
#endif

            TfLiteStatus invoke_status = interpreter->Invoke(); // this is being profiled
#ifdef NS_MLPROFILE
            profiler->LogCsv(); // print results in CSV format
#endif
}
```

The neuralSPOT extended MicroProfiler captures the following data per NN layer:

- Execution time
- (optionally) Estimated MAC count
- Cycles, CPI, Exception, Sleep, LSU, and Fold counts
- Cache access, tag lookups, hits lookups, line hits for both icache and cache

> *NOTE* MAC counts are estimated based on static analysis of a tflite model using
> tools/tflite_profile.py, which creates an include file with per-layer MAC count
> estimates. It is optional - if this analysis in not available, pass NULL as the
> second parameter to ns_TFDebugLogInit()

### MicroProfiler Caveats

When enabled, the MicroProfiler:

- When enabled, the MicroProfiler increases power utilization.
- When enabled, the MicroProfiler enables and uses the TimerTick clock.
- When enabled, the MicroProfiler allocates ~62KB of heap for logging.
- Some of the HW counters (CPI, Exception, Sleep, LSU, and Fold) are 8 bits. Only rudimentary wrap handling is implemented.
- MAC only supports Conv2D, DWConv2D, and FC
