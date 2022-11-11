# NeuralSPOT Features
This is a detailed list of the features spread out over neuralSPOT. If you'd like to see a new feature, please file an [issue on the repo](https://github.com/AmbiqAI/neuralSPOT/issues).

_Contents_

- [Supported Hardware and External Software Components](#supported-hardware-and-software)
- [Examples and Projects](#examples-and-projects)
- [Library Features](#libraries)

# Supported Hardware and Software

Generally, neuralSPOT will maintain support for the last 2 versions of AmbiqSuite, and will stay within 3 months of TFLM's latest releases.

### Software
| External Component                   | Versions         | Notes                                                        |
| ------------------------------------ | ---------------- | ------------------------------------------------------------ |
| AmbiqSuite                           | 4.1.0, 4.3.0     | See [Release Notes](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/release_notes.md) |
| Tensorflow Lite for Microcontrollers | 2.3.1, c046d6e   |                                                              |
| Embedded RPC                         | 1.9.1 Ambiq Fork | Please use this [fork](https://github.com/AmbiqAI/erpc).     |
| SEGGER RTT                           | 7.70a            |                                                              |

### Hardware

| Hardware Support      | Type                              | Notes                                                        |
| --------------------- | --------------------------------- | ------------------------------------------------------------ |
| Apollo4P_evb          | Ambiq EVB                         |                                                              |
| Apollo4B_evb          | Ambiq EVB                         | See [Release Notes](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/release_notes.md) |
| Apollo4P_blue_kbr_evb | Ambiq EVB                         | Coming soon                                                  |
| Apollo4P_blue_kxr_evb | Ambiq EVB                         | Coming soon                                                  |
| MPU6050               | Invensense 6D accelerometer       | i2c support                                                  |
| MAX86150              | Invensense ECG single-lead sensor | i2c support                                     |

# Examples and Projects

NeuralSPOT includes _[examples](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples)_ and _[projects](https://github.com/AmbiqAI/neuralSPOT/tree/main/projects)_. Examples are built entirely on NeuralSPOT's libraries and components, and are meant to showcase NeuralSPOT's features. Projects, on the other hand, are meant as demonstrations of how to integrate externally generated AI components  (from EdgeImpulse, for example) that are built on top of modified NeuralSPOT "[nests](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/Developing_with_NeuralSPOT.md)".

## Examples

| Example                                                      | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [basic_tf_stub](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/basic_tf_stub) | NeuralSPOT's primary example, this audio-based AI model showcases Tensorflow integration, audio sampling and feature extraction, RPC, IPC, and more. It is used as the 'egg' for NeuralSPOT nests. |
| [mpu_data_collection](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/mpu_data_collection) | Shows how to configure and read sensor values from the [MPU6050](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c), and how to use RPC to send those values to a PC. This is useful for creating accelerometer/gyroscope datasets, for example. |
| [rpc_client](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_client_example) | Shows how to use `ns-rpc` when the EVB is a *client* and the PC is the *server* to collect audio data from the AUDADC and dump it into a WAV file (useful for generating audio datasets). In this configuration, the EVB calls remote procedures and the PC executes those procedures . This example is designed to work with [`generic_data.py`](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/python/ns-rpc-genericdata/generic_data.py) (part of [ns-rpc](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc)), a python script running on the PC that can be run as server or client. |
| [rpc_server](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_server_example) | Shows how to use `ns-rpc` when the EVB is the *server* and the PC is the *client*. In this configuration, the PC calls remote procedures and the EVB executes those procedures . This example is designed to work with [`generic_data.py`](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/python/ns-rpc-genericdata/generic_data.py) (part of [ns-rpc](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc)), a python script running on the PC that can be run as server or client. |



## Projects

| Project                                                      | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [ei_basic_example](https://github.com/AmbiqAI/neuralSPOT/tree/main/projects/edgeimpulse/ei_basic_example) | Shows how to integrate an [EdgeImpulse model](https://studio.edgeimpulse.com/public/145105/latest) [exported as generic C++](https://docs.edgeimpulse.com/docs/deployment/running-your-impulse-locally/deploy-your-model-as-a-c-library)  to NeuralSPOT |
| [ei_anomaly_detection](https://github.com/AmbiqAI/neuralSPOT/tree/main/projects/edgeimpulse/ei_anomaly_detection) | Shows a practical [fan speed anomaly detector](https://studio.edgeimpulse.com/public/145382/latest) built in EdgeImpulse and using MPU6050 data collected using our [MPU data collector](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/mpu_data_collection). |
| [ei_yes_no](https://github.com/AmbiqAI/neuralSPOT/tree/main/projects/edgeimpulse/ei_yes_no) | Shows a [continuous inference](https://docs.edgeimpulse.com/docs/tutorials/continuous-audio-sampling) [audio model detecting whether 'yes' or 'no'](https://studio.edgeimpulse.com/public/146769/latest) was spoken, built in EdgeImpulse and using NeuralSPOT's libraries including low power[AUDADC audio sampling.](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| [hello_ambiq](https://github.com/AmbiqAI/neuralSPOT/tree/main/projects/edgeimpulse/hello_ambiq_WIP_doesntwork) (WIP) | Shows a continuous inference audio [model detecting whether 'hello Ambiq' was spoken](https://studio.edgeimpulse.com/studio/149224/versions), built in EdgeImpulse using transfer learning and using NeuralSPOT's libraries including AUDADC audio sampling. |



# Libraries

| Library                | Location       | Description                                                  | Usage Guide                                                  |
| ---------------------- | -------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ns_audio               | ns-audio       | AUDADC (low power analog audio input) configuration and audio sampling. | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_audio_mfcc          | ns-audio       | Mel Spectrogram Feature Extraction                           | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-audio) |
| ns_malloc              | ns-utils       | malloc/free implementation                                   | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_timer               | ns-utils       | Reading Apollo4 clocks and setting periodic interrupts       | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_power_profile       | ns-utils       | Prints power-relevant register configuration for expert analysis | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_energy_monitor      | ns-utils       | Utility for marking regions of interest for external power monitors | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_cache_profile       | ns-utils       | Prints cache utilization statistics                          | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-utils) |
| ns_peripherals_power   | ns-peripherals | Optimized, customizable Apollo4 power configuration for common AI applications. | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_peripherals_button  | ns-peripherals | Utility for reading EVB buttons via ISRs                     | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_peripherals_led     | ns-peripherals | Utility for controlling EVB LEDs                             | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-peripherals) |
| ns_i2c_register_driver | ns-i2c         | Generic Apollo4 i2c driver                                   | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_mpu6050_i2c_driver  | ns-i2c         | Minimal MPU6050 driver                                       | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_max86150_i2c_driver | ns-i2c         | MAX86150 driver                                              | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-i2c) |
| ns_rpc_generic_data    | ns-rpc         | A generic data RPC implementation offering both client and server modes for sending blocks of data, computing on a block a data and getting a result, and printing remote message. | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc) |
| ns_ambiqsuite_harness  | ns-harness     | Wrappers for common AmbiqSuite operations such as printing, deep-sleep compatible printing, adding delays, etc. | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_debug_log           | ns-harness     | Implementation of printf for Tensorflow debug prints         | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-harness) |
| ns_usb                 | ns-usb         | TinyUSB CDC (TTY) driver for USB port 1.                     | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-usb) |
| ns_ipc_ring_buffer     | ns-ipc         | Ring buffer IPC mechanism for getting peripheral data into AI applications | [Here](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-ipc) |
