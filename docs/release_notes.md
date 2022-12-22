# neuralSPOT Errata and Release Notes



## Errata

| ID                    | Version | Fixed In | Description                                                  |
| --------------------- | ------- | -------- | ------------------------------------------------------------ |
| 4B_audadc_support     | 0.0.1   | n/a      | AUDADC is not supported on Apollo4B devices running AmbiqSuite4.1.0 |
| hello_world_hardfault | 0.0.1   |          | Hello World example EdgeImpulse project causes HardFault due to misaligned memory access |
|                       |         |          |                                                              |



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
