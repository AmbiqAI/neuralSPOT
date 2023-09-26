# neuralSPOT Errata and Release Notes



## Errata

| ID                    | Version | Fixed In | Description                                                  |
| --------------------- | ------- | -------- | ------------------------------------------------------------ |
| 4B_audadc_support     | 0.0.1   | n/a      | AUDADC is not supported on Apollo4B devices running AmbiqSuite4.1.0 |
| hello_world_hardfault | 0.0.1   |          | Hello World example EdgeImpulse project causes HardFault due to misaligned memory access |
| TFLM Profiler         | 1.0.2   | n/a      | TFLM profiling not supported for TFLM version 2.3.1          |
| ns-ble_support        | 0.0.1   | n/a      | BLE is only supported for AmbiqSuite 4.3.0 and beyond.       |



## Releases

- NeuralSPOT Release 0.0.1
  - Initial Release
- NeuralSPOT Release 1.0.0-alpha
  - Adds version checking to APIs
  - Adds too many new features to list (see checkin history), including
    - CMSIS
    - New and Upgraded feature extraction libraries
    - New USB SW interface
    - Low power debug harness and helper functions
    - MAX86150 ECG i2c driver
    - Better Windows support
    - Power optimizations
    - Better CI/CD support
    - Easier control of stack and heap sizes
    - Better performance profiling tools
    - Better audio capture and quality
    - Adds CMSIS 5.9.0 and Ambiq 4.2.0 external components
    - Numerous bug fixes, tweaks, and documentation updates
    - *Note* future updates will follow semantic versioning and will slow to a bi-weekly cadence
- NeuralSPOT Release 1.0.1-alpha (minor release) *NOTE* This is a running collection of checked-in changes in anticipation of release 1.0.1-alpha, which hasn't been tagged yet.
  - EdgeImpulse Updates: move neuralSPOT version used by projects/edgeimpulse examples to v1.0.0-alpha + changes describe below
  - I2c API and MPU Calibration update: fixes and re-enabled MPU calibration, adds MPU init API
  - Windows Makefile: fixes problem when making nests in Windows
  - Add API to select CPU clock, uses it in basic_tf_stub.cc
  - Added a ‘no print’ path to ns_lp_printf
  - Add no-op path to ns_energy_monitor
  - Add API to clear Timer, adds API checks to timer read functions
  - Fixed compilation for BLE versions of Apollo4P EVB boards
  - Added new TFLM version (github hash fecdd5d, date 1/09/2022
    - *NOTE* The TFLM team refactored micro_error_reporter a bit, requiring application code changes (see basic_tf_stub's basic_model.h)
- NeuralSPOT Release 1.0.2-alpha (minor release) *NOTE* This is a running collection of checked-in changes in anticipation of release 1.0.2-alpha, which hasn't been tagged yet.
  - [TFLM Ambiq-aware Profiler](../neuralspot/ns-harness/README.md) (new feature) - profiles per-layer TFLM model execution (time, perf counters, cache behavior).
  - Assorted minor fixes
- NeuralSPOT Release 1.0.3-alpha *NOTE* This is a running collection of checked-in changes in anticipation of release 1.0.3-alpha, which hasn't been tagged yet.
  - RPC improvements
    - Adds interrupt-based server polling, reduces number of USB pings
    - Moves RPC RX/TX buffer allocation to application code
    - Improves USB RX error handling
  - Makefile improvements
    - Adds 'make reset': reset the EVB via jlink
    - Fixes dependencies for example header files
  - Automatic TFLM validation (new feature, WIP)
    - Automatically converts a TFLite model file to a binary that can be run on the EVB, tuning TF arena size and RPC RX/TX buffers
    - Deploys the binary
    - Characterizes the binary over RPC, comparing EVB inference results to PC-side python inference results
  - Human Activity Recognition example application, based on [HAR modelzoo](https://github.com/AmbiqAI/Human-Activity-Recognition).

- ns-audio 2.0.0 Release Notes

  - Version 2.0.0 adds features and fixes minor bugs. Taking advantage of these changes requires an API change, but backwards compatibility has been preserved via the API version feature. New features include:

    1. **PDM support**: PDM is now supported as an audio input source. Note that the EVB does not include digital microphones. PDM support has been tested with the VoS kit digital microphones.

    1. **Dual-channel support**: 2 channel audio can now be captured for both AUDADC and PDM sources

- ns-ble 0.0.1 Release Notes
  - The alpha release of ns-ble adds BLE server support for Bluetooth-enabled EVBs.
    - **Simplified API**: ns-ble offers a simplified BLE API, which supports
      - BLE service creation
      - BLE characteristic creation (read, write, and notify characteristics supported)
      - Simplified event handling (read, write, and notify)

    - **web_ble example**: this new example application implements a mocked-up WebBLE service designed to serve sensor data to WebBLE dashboard.

- NeuralSPOT 1.1.2-alpha pre-release
  - Adds support for Arm's armclang toolchain
  - Adds support for recent tensorflow lite for microcontrollers (git hash d5f819d)
  - Adds support for CMSIS-DSP version 1.15.0
  - Fixes various usb race conditions, rpc_server example hang
  - Adds 16-byte alignment to model definitions (needed by armclang)
  - removes unneeded files
