# Tools Directory

Below is an overview of the scripts available in this directory and a brief description of their purpose:

| **Script**               | **Description**                                                                                                     |
|--------------------------|---------------------------------------------------------------------------------------------------------------------|
| `ns_autodeploy.py`       | Deploys TFLite models to an EVB including binary creation, profiling, power measurement, and library generation.      |
| `ns_tflite_analyze.py`   | Analyzes TFLite models to estimate MAC counts, memory reads/writes, and layer statistics; outputs reports in CSV/Excel.|
| `ns_ad_batch.py`         | Batch deployment of multiple models using YAML configuration files.                                                 |
| `ns_test.py`             | Automated testing framework for NeuralSPOT using configuration files and command-line arguments.                      |

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

- **`ns_ad_batch.py`**  
  Batch deploys multiple neural network models defined in a YAML configuration file. For each model, it builds the appropriate `ns_autodeploy` command and executes it with support for retries and logging.
  See the ad_batch [Usage Guide](./ns_ad_batch.readme.md) for more details
   

---

## Prerequisites

ns_autodeploy can be installed using pip:
```bash
cd .../neuralSPOT
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
