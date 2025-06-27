# Human Activity Recognition Example
This example implements a simple HAR model. It collects data from an IMU accelerometer and gyroscopic sensor, and classifies the data into one of 5 activities (sitting, standing, walking, jogging, go up stairs). This example is based on the [Human Activity Recognition Model Zoo repository](https://github.com/AmbiqAI/Human-Activity-Recognition).

The model classifies the last 10 seconds of movement every 2s.

To use this model, you'll need to connect the sensor (an MPU6050) to the EVB ([instructions here](../../../neuralspot/ns-i2c/README.md)).

## Required Hardware and Configuration
This example is requires the following hardware:
  * Apollo510 EVB
  * ICM-45605 Mikroe Board (MIKRO-6040) installed in EVB's Clickboard slot
  * J13 jumper selecting 3.3v

# Building and Running the Example
This is the primary neuralSPOT example, so the makefile defaults many of its options to it.

```bash
$> cd .../neuralSPOT
$> make clean
$> make -j EXAMPLE=ai/har deploy
$> make view # connects to EVB SWO via SEGGER Jlink
```
## Additional Example Code
There are 2 additional files (*.dnc, which stands for 'do not compile') with alternate implementations:
1. har.cc.ap4: uses an i2c-based IMU instead of the SPI ICM IMU
2. har.cc.blockmode: collects 10s blocks when a button is pressed instead of using a continuous window