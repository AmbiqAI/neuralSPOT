# Human Activity Recognition Example
This example implements a simple HAR model. It collects data from an MPU6050 accelerometer and gyroscopic sensor, and classifies the data into one of 5 activities (sitting, standing, walking, jogging, go up stairs). This example is based on the [Human Activity Recognition Model Zoo repository](https://github.com/AmbiqAI/Human-Activity-Recognition).

To use this model, you'll need to connect the sensor (an MPU6050) to the EVB ([instructions here](../../neuralspot/ns-i2c/README.md)).

# Building and Running the Example
This is the primary neuralSPOT example, so the makefile defaults many of its options to it.

```bash
$> cd .../neuralSPOT
$> make clean
$> make -j # makes everything
$> make TARGET=har deploy # flashes HAR to the target EVB
$> make view # connects to EVB SWO via SEGGER Jlink
```
