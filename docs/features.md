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
| AmbiqSuite                           | 4.1.0, 4.2.0, 4.3.0, 4.4.1, R3.3.1 | See [Release Notes](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/release_notes.md) |
| Tensorflow Lite for Microcontrollers | 2.3.1, c046d6e, fecdd5d, d5f819d (latest) |                                                              |
| Embedded RPC                         | 1.9.1 Ambiq Fork | Please use this [fork](https://github.com/AmbiqAI/erpc).     |
| SEGGER RTT                           | 7.70a            |                                                              |
| CMSIS-NN | 5.9.0 | |
| CMSIS-DSP | 1.15.0 | |

### Hardware

| Hardware Support      | Type                              | Notes                                                        |
| --------------------- | --------------------------------- | ------------------------------------------------------------ |
| Apollo4P_evb          | Ambiq Apollo4 Plus EVB             |                                                              |
| Apollo4B_evb        | Ambiq Apollo4 Rev B EVB       | See [Release Notes](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/release_notes.md) |
| Apollo4P_blue_kbr_evb | Ambiq Apollo4P Blue KBR EVB  |                                                   |
| Apollo4P_blue_kxr_evb | Ambiq Apollo4P Blue KXR EVB |                                                   |
| Apollo4 Lite | Ambiq Apollo4 Lite EVB                        |                                                   |
| Apollo4 Blue Lite | Ambiq Apollo4 Lite EVB | |
| Apollo3P Blue | Ambiq  Apollo3P Blue EVB | |
| MPU6050               | Invensense 6D accelerometer       | i2c support                                                  |
| MAX86150              | Invensense ECG single-lead sensor | i2c support                                     |

### Compatibility

| Feature                      | Apollo4B    | Apollo4 Plus      | Apollo4 Blue Plus (KBR/KXR) | Apollo4 Lite      | Apollo4 Blue Lite | Apollo3P Blue |
| ---------------------------- | ----------- | ----------------- | --------------------------- | ----------------- | ----------------- | ------------- |
| USB                          | No          | Yes               | Yes                         | n/a               | n/a               | n/a           |
| BLE                          | n/a         | n/a               | AmbiqSuite 4.3.0+           | n/a               | Yes               | Yes           |
| i2c Devices                  | Yes         | Yes               | Yes                         | Yes               | Yes               | Soon          |
| Analog Audio                 | Yes         | Yes               | Yes                         | n/a               | n/a               | n/a           |
| Digital Audio                | No          | AmbiqSuite 4.3.0+ | AmbiqSuite 4.3.0+           | AmbiqSuite 4.4.1+ | AmbiqSuite 4.4.1+ | Yes           |
| TF Micro Profiler            | TF 0c46d6e+ | TF 0c46d6e+       | TF 0c46d6e+                 | TF 0c46d6e+       | TF 0c46d6e+       | Yes           |
| AutoDeploy Profiling         | No          | Yes               | Yes                         | No                | No                | No            |
| AutoDeploy Power Measurement | Yes         | Yes               | Yes                         | No                | No                | No            |
| AutoDeploy Library Export    | Yes         | Yes               | Yes                         | Yes*              | Yes*              | Yes*          |

> `*` Autodeploy automation relies on USB, so only platforms with USB1 can use automation. Libraries exported by the automation are platform-agnostic.

## Examples

Examples are built entirely on NeuralSPOT's libraries and components, and are meant to showcase NeuralSPOT's features. 

### Examples

| Example                                                      | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [basic_tf_stub](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/basic_tf_stub) | NeuralSPOT's primary example, this audio-based AI model implements KWS to showcase Tensorflow integration, audio sampling and feature extraction, RPC, IPC, and more. It is used as the 'egg' for NeuralSPOT nests. |
| [mpu_data_collection](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/mpu_data_collection) | Shows how to configure and read sensor values from the [MPU6050](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c), and how to use RPC to send those values to a PC. This is useful for creating accelerometer/gyroscope datasets, for example. |
| [rpc_client](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_client) | Shows how to use `ns-rpc` when the EVB is a *client* and the PC is the *server* to collect audio data from the AUDADC and dump it into a WAV file (useful for generating audio datasets). In this configuration, the EVB calls remote procedures and the PC executes those procedures . This example is designed to work with [`generic_data.py`](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/python/ns-rpc-genericdata/generic_data.py) (part of [ns-rpc](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc)), a python script running on the PC that can be run as server or client. |
| [rpc_server](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_server) | Shows how to use `ns-rpc` when the EVB is the *server* and the PC is the *client*. In this configuration, the PC calls remote procedures and the EVB executes those procedures . This example is designed to work with [`generic_data.py`](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/python/ns-rpc-genericdata/generic_data.py) (part of [ns-rpc](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc)), a python script running on the PC that can be run as server or client. |
| [har](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/har) | This example implements a simple Human Activity Recognition model. It collects data from an MPU6050 accelerometer and gyroscopic sensor, and classifies the data into one of 5 activities (sitting, standing, walking, jogging, go up stairs). This example is based on the [Human Activity Recognition Model Zoo repository](https://github.com/AmbiqAI/Human-Activity-Recognition). |
| [tflm_validator](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/tflm_validator) | TF Lite for Microcontrollers Validation target. This isn't, strictly speaking, an example. Rather, it is a harness used by neuralSPOT automatic TFLite deployment, validation, and performance profiling tool. |
| [web_ble](../examples/web_ble)               | Web BLE (bluetooth low energy) example application. This example turns the EVB into a bluetooth peripheral (in BLE terms, a 'server') communicating with a WebBLE dashboard running on the laptop browser. |



# Libraries

| Library                | Location       | Description                                                  | AP3  | AP4  | Usage Guide                                                  |
| ---------------------- | -------------- | ------------------------------------------------------------ | ---- | ---- | ------------------------------------------------------------ |
| ns_audio               | ns-audio       | AUDADC (low power analog audio input) and PDM configuration and audio sampling. | PDM  | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_audio_mfcc          | ns-audio       | Mel Spectrogram Feature Extraction                           | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_malloc              | ns-utils       | malloc/free implementation                                   | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_timer               | ns-utils       | Reading clocks and setting periodic interrupts               | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_power_profile       | ns-utils       | Prints power-relevant register configuration for expert analysis | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_energy_monitor      | ns-utils       | Utility for marking regions of interest for external power monitors | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_cache_profile       | ns-utils       | Tools to capture and analyze cache utilization statistics    | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_perf_profile        | ns-utils       | Tools to capture and analyze CPU perf utilization registers  | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_peripherals_power   | ns-peripherals | Optimized, customizable Apollo4 power configuration for common AI applications. | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_peripherals_button  | ns-peripherals | Utility for reading EVB buttons via ISRs                     | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_i2c_register_driver | ns-i2c         | Generic Apollo4 i2c driver                                   |      | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_mpu6050_i2c_driver  | ns-i2c         | Minimal MPU6050 driver                                       |      | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_max86150_i2c_driver | ns-i2c         | MAX86150 driver                                              |      | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_rpc_generic_data    | ns-rpc         | A generic data RPC implementation offering both client and server modes for sending blocks of data, computing on a block a data and getting a result, and printing remote message. | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc) |
| ns_ambiqsuite_harness  | ns-harness     | Wrappers for common AmbiqSuite operations such as printing, deep-sleep compatible printing, adding delays, etc. | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_micro_profiler      | ns-harness     | Tools for per-layer performance profiling                    | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_debug_log           | ns-harness     | Implementation of printf for Tensorflow debug prints         | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_usb                 | ns-usb         | TinyUSB CDC (TTY) driver for USB port 1.                     | N    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-usb) |
| ns_ble                 | ns-ble         | Simple BLE library for creating BLE peripheral servers.      | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-ble) |
| ns_ipc_ring_buffer     | ns-ipc         | Ring buffer IPC mechanism for getting peripheral data into AI applications | Y    | Y    | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-ipc) |

# Tools

| Tool                | Description                                                  | Location                                                     |
| ------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ns_autodeploy.py    | All-in-one tool for automatically deploying, validating, profiling, and packaging a TFLite-based model | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| ns_mac_estimator.py | Analyzes a TFLite file, estimates MACs per model layer, and exports a C header file for use with ns_micro_profiler | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |
| generic_data.py     | Example PC-side script for interacting with EVB via remote procedure calls. | [Tools](https://github.com/AmbiqAI/neuralSPOT/tree/main/tools) |

