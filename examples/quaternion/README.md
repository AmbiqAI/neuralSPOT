# Quaternion Visualization
This example directory contains sample code demonstrating how to use the ns-features library. It includes usage examples for computing quaternion values and extracting features from sensor data.

## Real-time Orientation Display Script
Additionally, the example provides a Python script for displaying the orientation of the sensor in real time. This script utilizes the quaternion values computed by ns-features to visualize the orientation of the sensor graphically.

## Installation and Setup
Before using the examples, ensure you have the necessary Python-based PC-side software installed by following the [instructions here.](../../neuralspot/ns-rpc/README.md).

> **Note for Windows Users:** If you are using Windows, refer to our [Windows eRPC application note](../../docs/Application-Note-neuralSPOT-and-Windows.md) for additional instructions.

Additionally, make sure you have the following Python libraries installed:

```bash
pip install numpy quaternion matplotlib
```

You'll also need to connect the sensor (an MPU6050) to the EVB. Refer to the [instructions here](../../neuralspot/ns-i2c/README.md) for guidance on connecting the MPU6050.

## Visualizing Quaternions
1. Compile and flash the NeuralSPOT quaternion example
2. Connect the second USB cable to your laptop - you'll now have 2 USB connections between the EVB and the laptop
    1. Monitor the EVB SWO printout - you should see "Press Button 0 to begin calibration"
    2. The second connection will mount as a USB TTY device. On a Mac, it'll look something like `/dev/tty.usbmodem1234561`, on PC it'll be `COMx` or similar.
    3. Look for the USB TTY device - if it doesn't pop up, there is a problem. It won't show up until "Press Button" shows up, so make sure you got that far.
3. Start the laptop-side RPC server. It should say "Wait for client to send a eRPC request"
4. Press EVB button 0

After calibration, it'll transmit quaternion values until your stop the server (ctrl-c on laptop, or reset button on EVB). The python script will open a new window in which the orientation of the sensor will be displayed in real time.

**Note:** Due to inherent gyro drift, you may observe slight inaccuracies in orientation estimation.


### Details

#### Building Quaternion

```bash
$> cd .../neuralSPOT
$> make
$> make TARGET=quaternion deploy
$> make view
```

#### Running Laptop-side RPC Server

```bash
$> python -m quaternion_plotter -t /dev/tty.usbmodem1234561 -m server
Server started - waiting for client to send a eRPC request
```