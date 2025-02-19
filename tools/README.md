# Tools Directory

Below is an overview of the scripts available in this directory and a brief description of their purpose:

| **Script**               | **Description**                                                                                                     |
|--------------------------|---------------------------------------------------------------------------------------------------------------------|
| `ns_autodeploy.py`       | Deploys TFLite models to an EVB including binary creation, profiling, power measurement, and library generation.      |
| `ns_tflite_analyze.py`   | Analyzes TFLite models to estimate MAC counts, memory reads/writes, and layer statistics; outputs reports in CSV/Excel.|
| `generic_data.py`        | eRPC server/client for handling generic data transfer (audio, sensor data, etc.) between EVB and PC.                |
| `ns_ad_batch.py`         | Batch deployment of multiple models using YAML configuration files.                                                 |
| `ns_test.py`             | Automated testing framework for NeuralSPOT using configuration files and command-line arguments.                      |
| `measurer.py`            | Measures power consumption (using a Joulescope or similar) and sends data via ZMQ for visualization or logging.       |
| `opus_receive.py`        | Receives and decodes Opus-encoded audio data from the EVB and writes decoded PCM data to a WAV file.                  |
| `plotter.py`             | Dash-based web application that displays live performance and power metrics graphs using Plotly.                      |

---

## Overview

This directory contains a collection of utility scripts that support various tasks for the NeuralSPOT project. These tools facilitate data communication between an EVB (Evaluation Board) and a PC, model analysis and deployment, automated testing, power measurement, and live data visualization. Below is an overview of each script and instructions on how to use them.

---

## Contents
- **`ns_autodeploy.py`**  
  The primary tool for deploying TFLite models to an EVB board. This script supports:
  - Creating validation binaries.
  - Profiling model performance.
  - Power measurement (via Joulescope or onboard performance capture).
  - Generating minimal static libraries and AmbiqSuite examples.
  See the autodeploy [Theory of Operations](./autodeploy.readme.md) for more details.

- **`ns_tflite_analyze.py`**  
  Analyzes TensorFlow Lite (TFLite) model files find optimization targets, estimate MAC counts, memory reads/writes, and other layer statistics. It generates human-readable output in table format, as well as CSV and Excel reports.

- **`ns_test.py`**  
  Runs automated tests for the NeuralSPOT project. It uses a configuration file (INI format) along with command-line parameters parsed via `pydantic_argparse` to generate and run tests.

- **`generic_data.py`**  
  Implements an eRPC server and client for handling generic data operations between the EVB and PC. This tool supports:
  - Audio capture (32-bit stereo or 16-bit mono PCM) and writing to WAV files.
  - MPU6050 sensor data capture and logging to CSV.
  - Remote procedure calls for data fetching, computation (e.g., MFCC calculation), and printing.
  
- **`ns_ad_batch.py`**  
  Batch deploys multiple neural network models defined in a YAML configuration file. For each model, it builds the appropriate `ns_autodeploy` command and executes it with support for retries and logging.
  See the ad_batch [Usage Guide](./ns_ad_batch.readme.md) for more details
   
- **`measurer.py`**  
  Measures power consumption (using a Joulescope or similar power measurement tool) during model inference. The script packs the measurement data using MessagePack and sends it over a ZMQ PUB socket (port 5556) for further processing or visualization.
  
- **`opus_receive.py`**  
  Receives Opus-encoded audio data sent over a serial connection via eRPC, decodes the Opus frames into PCM, and writes the output to a WAV file.

- **`plotter.py`**  
  A Dash-based web application that subscribes to power and performance data (via ZMQ) and displays live graphs (e.g., latency and energy consumption). It uses Plotly for real-time data visualization.

---

## Prerequisites

ns_autodeploy can be installed using pip:
```bash
cd ..
pip install .
```

To run the other tools, ensure that you have Python 3.6 or later installed along with the following packages:

- **Core Packages:**  
  `numpy`, `pyyaml`, `pydantic`, `pydantic_argparse`

- **Audio & Signal Processing:**  
  `soundfile`, `pyogg`

- **Communication & RPC:**  
  `erpc`

- **Data Packing & Messaging:**  
  `msgpack`, `msgpack_numpy`

- **Visualization:**  
  `dash`, `plotly`

You can install the required packages using pip. For example:

```bash
pip install numpy pyyaml pydantic pydantic_argparse soundfile pyogg erpc msgpack msgpack_numpy dash plotly
