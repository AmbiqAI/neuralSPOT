# Quaternion Visualization
This example directory contains sample code demonstrating how to use the ns-features library. It includes usage examples for computing quaternion values and extracting features from sensor data.

## Real-time Orientation Display Script
Additionally, the example provides a Python script for displaying the orientation of the sensor in real time. This script utilizes the quaternion values computed by ns-features to visualize the orientation of the sensor graphically.

## Installation and Setup
Before using the examples, ensure you have the necessary Python-based PC-side software installed by following the [instructions here.](../../../neuralspot/ns-rpc/README.md).

> **Note for Windows Users:** If you are using Windows, refer to our [Windows eRPC application note](../../../docs/Application-Note-neuralSPOT-and-Windows.md) for additional instructions.

Additionally, make sure you have the following Python libraries installed:

```bash
pip install numpy numpy-quaternion matplotlib
```

## Required Hardware and Configuration
This example is requires the following hardware:
  * Apollo510 EVB
  * ICM-45605 Mikroe Board (MIKRO-6040) installed in EVB's Clickboard slot
  * J13 jumper selecting 3.3v
  * USB connection between EVB USB_AP5 port (J18) and laptop

## Running the Example
Once the above is installed, the following steps are needed to run the example:
1. Compile and flash the example (optionally, start the SWO viewer)
2. Start the visualization script
3. Press Button 0 on EVB

### Compile, Deploy, and View SWO
```bash
$> cd .../neuralSPOT
$> make -j EXAMPLE=examples/quaternion deploy
$> make view
```

### Start the Visualization Script
```bash
$> cd .../neuralSPOT/examples/quaternion/source
$> python quaternion_plotter.py
```

At this point, the script will ask you to press the EVB button and wait for calibration, whereupon it will launch the visualizer.