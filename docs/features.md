# NeuralSPOT Features
This is a detailed list of the features spread out over neuralSPOT. If you'd like to see a new feature, please file an [issue on the repo](https://github.com/AmbiqAI/neuralSPOT/issues).

_Contents_

- [Supported Hardware and External Software Components](#supported-hardware-and-software)
- [Examples and Projects](#examples-and-projects)
- [Library Features](#libraries)

## Supported Hardware and Software

Generally, neuralSPOT will maintain support for the last 2 versions of AmbiqSuite, and will stay within 3 months of TFLM's latest releases.

### Software
| External Component                   | Versions         | Notes                                                        |
| ------------------------------------ | ---------------- | ------------------------------------------------------------ |
| AmbiqSuite                           | 3.1.1, 4.4.1, 4.5.0, 5.2.0, R5.3.0 | See [Release Notes](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/release_notes.md) |
| Tensorflow Lite for Microcontrollers | Oct_08_24, ns-tflm-v1.0.0 |                                                              |
| Embedded RPC                         | 1.9.1 (Ambiq Fork) | Please use this [fork](https://github.com/AmbiqAI/erpc).     |
| CMSIS-NN | 5.9.0 | |
| CMSIS-DSP | 1.16.0 | |

### Hardware

| Hardware Support      | Type                              | Notes                                                        |
| --------------------- | --------------------------------- | ------------------------------------------------------------ |
| Apollo510_evb         | Ambiq Apollo510 EVB | |
| Apollo4P_evb          | Ambiq Apollo4 Plus EVB             |                                                              |
| Apollo4P_blue_kbr_evb | Ambiq Apollo4P Blue KBR EVB  |                                                   |
| Apollo4P_blue_kxr_evb | Ambiq Apollo4P Blue KXR EVB |                                                   |
| Apollo4 Lite | Ambiq Apollo4 Lite EVB                        |                                                   |
| Apollo4 Blue Lite | Ambiq Apollo4 Lite EVB | |
| Apollo3P Blue | Ambiq  Apollo3P Blue EVB | |
| MPU6050               | Invensense 6D accelerometer       | i2c support                                                  |
| MAX86150              | Invensense ECG single-lead sensor | i2c support                                     |
| Arducam SPI 5MP       | SPI-based camera                  | SPI support |

### Compatibility

| Feature                      | Apollo510   | Apollo4 Plus      | Apollo4 Blue Plus (KBR/KXR) | Apollo4 Lite      | Apollo4 Blue Lite | Apollo3P Blue |
| ---------------------------- | ----------- | ----------------- | --------------------------- | ----------------- | ----------------- | ------------- |
| USB                          | Yes          | Yes               | Yes                         | n/a               | n/a               | n/a           |
| BLE                          | n/a         | n/a               | AmbiqSuite 4.3.0+           | n/a               | Yes               | Yes           |
| i2c Devices                  | Yes         | Yes               | Yes                         | Yes               | Yes               | Soon          |
| Analog Audio                 | n/a         | Yes               | Yes                         | n/a               | n/a               | n/a           |
| Digital Audio                | Yes          | AmbiqSuite 4.3.0+ | AmbiqSuite 4.3.0+           | AmbiqSuite 4.4.1+ | AmbiqSuite 4.4.1+ | Yes           |
| TF Micro Profiler            | Yes         | Yes      | Yes         | Yes       | Yes       | Yes           |
| AutoDeploy Profiling         | Yes          | Yes               | Yes                         | Yes                | Yes                | Yes            |
| AutoDeploy Power Measurement | Yes         | Yes               | Yes                         | Yes                | Yes                | Yes            |
| AutoDeploy Library Export    | Yes         | Yes               | Yes                         | Yes              | Yes             | Yes         |


## Examples

Examples are built entirely on NeuralSPOT's libraries and components, and are meant to showcase NeuralSPOT's features. 

[See here](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples) for the latest list of examples

# Libraries

| Library                | Location       | Description                                                  | AP3  | AP4  | AP5  | Usage Guide                                                  |
| ---------------------- | -------------- | ------------------------------------------------------------ | ---- | ---- | ---- | ------------------------------------------------------------ |
| ns_audio               | ns-audio       | AUDADC (low power analog audio input) and PDM configuration and audio sampling. | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_audio_mfcc          | ns-audio       | Mel Spectrogram Feature Extraction                           | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_malloc              | ns-utils       | malloc/free implementation                                   | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_timer               | ns-utils       | Reading clocks and setting periodic interrupts               | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_power_profile       | ns-utils       | Prints power-relevant register configuration for expert analysis | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_energy_monitor      | ns-utils       | Utility for marking regions of interest for external power monitors | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_cache_profile       | ns-utils       | Tools to capture and analyze cache utilization statistics    | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_perf_profile        | ns-utils       | Tools to capture and analyze CPU perf utilization registers  | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_peripherals_power   | ns-peripherals | Optimized, customizable Apollo4 power configuration for common AI applications. | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_peripherals_button  | ns-peripherals | Utility for reading EVB buttons via ISRs                     | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_i2c_register_driver | ns-i2c         | Generic Apollo4 i2c driver                                   | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_mpu6050_i2c_driver  | ns-i2c         | Minimal MPU6050 driver                                       | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_max86150_i2c_driver | ns-i2c         | MAX86150 driver                                              | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_camera              | ns_camera      | Arducam driver and JPG utilities                             | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-camera) |
| ns_rpc_generic_data    | ns-rpc         | A generic data RPC implementation offering both client and server modes for sending blocks of data, computing on a block a data and getting a result, and printing remote message. | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc) |
| ns_ambiqsuite_harness  | ns-harness     | Wrappers for common AmbiqSuite operations such as printing, deep-sleep compatible printing, adding delays, etc. | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_micro_profiler      | ns-harness     | Tools for per-layer performance profiling                    | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_debug_log           | ns-harness     | Implementation of printf for Tensorflow debug prints         | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_usb                 | ns-usb         | TinyUSB CDC (TTY) driver for USB port 1.                     | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-usb) |
| ns_ble                 | ns-ble         | Simple BLE library for creating BLE peripheral servers.      | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-ble) |
| ns_spi                 | ns_spi         | Easy to use SPI library                                      | N    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-spi) |
| ns_pmu                 | ns-utils       | Utilities for using M55 PMU event counters                   | N    | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_uart                | ns_uart        | Easy to use UART library                                     | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-uart) |
| ns_nnsp                | ns_nnsp        | Neural Network Speech Processing: a collection of neural network and feature extraction functions | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-nnsp) |
| ns_ipc_ring_buffer     | ns-ipc         | Ring buffer IPC mechanism for getting peripheral data into AI applications | Y    | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-ipc) |

# Tools

| Tool                  | Description                                                  | Location                                                     |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ns_autodeploy.py      | All-in-one tool for automatically deploying, validating, profiling, and packaging a TFLite-based model | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| ns_tflite_analyzer.py | Analyzes a TFLite file, finds potential performance problems, estimates MACs per model layer, and exports a C header file with model information. | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| ns_test.py            | Tool for running unit tests                                  | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| ns_ad_batch.py        | Profiles a batch of models using autodeploy - useful for regression testing optimizations, etc. | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| generic_data.py       | Example PC-side script for interacting with EVB via remote procedure calls. | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |

