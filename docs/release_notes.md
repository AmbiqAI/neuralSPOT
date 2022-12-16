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
- NeuralSPOT Release 1.0.0
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