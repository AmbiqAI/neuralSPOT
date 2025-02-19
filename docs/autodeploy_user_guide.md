# ns_autodeploy.py User Guide

Welcome to the ns_autodeploy.py User Guide. This document explains how to install, configure, and use ns_autodeploy.py for deploying, profiling, and debugging TensorFlow Lite models on Ambiq platforms. It also provides practical examples and tips on using per‐layer profiling results—including using them to debug Helium (MVE) usage.

---

## 1. Introduction

### Overview of ns_autodeploy.py

ns_autodeploy.py is a Python tool that automates the process of building and deploying TFLite models on Ambiq hardware. It can:
- Build and flash binaries from a TFLite file.
- Configure and validate your model on the target device.
- Profile performance and capture power consumption data.
- Generate a minimal static library (and AmbiqSuite example) for further integration.
- Produce detailed per-layer profiling results, which can be exported as CSV files.

### Purpose and Scope

This tool is intended to simplify and accelerate the deployment cycle for embedded ML applications. It is ideal for:
- **Beginners** who want a turnkey solution to see their model running on hardware.
- **Developers and Engineers** who need to tune performance, verify power consumption, or debug model behavior.
- **Researchers** interested in per-layer performance metrics and hardware utilization details.

### Key Features

- **Automated Build and Deployment:** Generates binaries and configures your device with minimal manual intervention.
- **Performance Profiling:** Captures inference times, estimated MAC counts, and additional performance metrics.
- **Power Measurement:** Integrates with tools like Joulescope to record energy consumption.
- **Detailed Per-Layer Profiling:** Outputs CSV files containing per-layer statistics for debugging and optimization.
- **Helium (MVE) Debugging:** Supports analysis of Helium (MVE) usage through per-layer metrics, helping to pinpoint optimization opportunities.

---

## 2. Installation and Setup

### Prerequisites

Ensure you have the following installed on your development system:

- **Python 3.10+** (with pip support)
- **ARM GCC Toolchain:** (e.g., 10.3-2021.10) for compiling ARM code.
- **GNU Make:** Used to build the generated code.
- **SEGGER JLink:** For flashing and debugging on the target hardware.

### Installing ns_autodeploy and Dependencies

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/AmbiqAI/neuralSPOT.git
   cd neuralSPOT
   ```

2. **Install Autodeploy:**
   ```bash
   pip install .
   ```
   This command installs ns_autodeploy. We recommend doing so within a Python virtual environment (venv).

### Environment Configuration

Make sure your PATH includes the directories for:
- ARM GCC and/or Armclang toolchain executables
- GNU Make
- Python and pip

---

## 3. Getting Started

### Basic Usage

To deploy your model, run ns_autodeploy.py from your terminal:
```bash
ns_autodeploy --tflite-filename your_model.tflite --platform apollo510_evb --toolchain gcc
```
Adjust the parameter values to suit your target platform and TFLite model.

### Command-Line Interface Overview

Some important CLI options include:

- `--tflite-filename`  
  The full path to your TFLite model file.

- `--platform`  
  The target Ambiq platform (e.g., `apollo510_evb`, `apollo4p_evb`). See the [Platform Definitions File](../tools/ns_platform.yaml) for the list of supported platforms.

- `--toolchain`  
  Choose your compiler, such as `gcc` or `arm`.

- Stage Selection Flags:
  - `--no-create-binary`: Build a binary image. Enabled by default, use `--no-create-binary` to skip.
  - `--no-create-profile`: Run per-layer profile. Enabled by default, use `--no-create-profile` to skip.
  - `--onboard-perf`: Runs a standalone performance test, results printed on SWO.
  - `--joulescope`: Runs a power performance test.
  - `--create_library`: Generate a static library.
  - `--create_ambiqsuite_example`: Generate an AmbiqSuite example.

- Profiling & Power Options:
  - `--profile-warmup`: Number of warmup runs before profiling.
  - `--runs`: Number of inferences for performance profiling.
  - `--runs-power`: Number of inferences for power measurements.
  - `--cpu-mode`: Set CPU mode (`LP` for low power, `HP` for high performance).

- RPC Options:
  - `--transport`: Specify the communication transport (auto, USB, or UART). If `auto` will autodetect.
  - `--tty` and `--baud`: Manually set the TTY port and baud rate if `auto` doesn't produce good results.

### Example Commands

- **Build Binary and Profile Performance:**
*IMPORTANT*: This step must be run at least once per model as it is used to detect model configuration parameters for other steps.

  ```bash
  ns_autodeploy --tflite-filename model.tflite --platform apollo510_evb
  ```

- **Generate a Static Library:**
  ```bash
  ns_autodeploy --tflite-filename model.tflite --platform apollo4p_evb --create-library
  ```

- **Measure Power with Joulescope:**
  ```bash
  ns_autodeploy --tflite-filename model.tflite --platform apollo3p_evb --joulescope
  ```

---

## 4. Configuration Options

### General Parameters

- **Platform:**  
  Set your target hardware with `--platform`. For example, `apollo510_evb`.

- **Toolchain:**  
  Use `--toolchain` to choose between compilers like `gcc` or `arm`.

- **Model Name:**  
  Either specify a custom name using `--model_name` or let the tool use the TFLite file’s base name.

### Stage Selection Options

Control the build and deployment process using flags:
  - `--no-create-binary`: Build a binary image. Enabled by default, use `--no-create-binary` to skip.
  - `--no-create-profile`: Run per-layer profile. Enabled by default, use `--no-create-profile` to skip.
  - `--onboard-perf`: Runs a standalone performance test, results printed on SWO.
  - `--joulescope`: Runs a power performance test.
  - `--create_library`: Generate a static library.
  - `--create_ambiqsuite_example`: Generate an AmbiqSuite example.

### Model and File Settings

- **TFLite Filename:**  
  Use the `--tflite-filename` option to provide the path to your model.

- **YAML Configuration:**  
  Optionally, prepare a YAML file with many configuration settings and pass it via `--configfile`.

### Performance and Profiling Options

- **Inference Runs:**  
  Use `--runs` (for profiling) and `--runs_power` (for power measurement) to define how many times to run your model.

- **CPU Mode:**  
  Specify the CPU mode (e.g., `LP` for low-power or `HP` for high-performance) using `--cpu_mode`.

### RPC and Communication Settings

- **Transport Mode:**  
  The tool can automatically select USB or UART. Override with `--transport` if needed.

- **Serial Settings:**  
  Set the TTY device using `--tty` and the baud rate using `--baud`.

---

## 5. Workflow and Process

### Overview of Deployment Stages

ns_autodeploy.py typically goes through these stages:

1. **Binary Creation:**  
   The tool compiles your model’s code into a binary image for your target device.

2. **Model Configuration and Validation:**  
   After flashing, the tool configures the model on the device and validates that the model’s memory allocation and input/output tensor settings are correct.

3. **Performance Profiling:**  
   The tool runs your model multiple times to capture per-layer statistics. These include inference time, estimated MAC counts, cycle counts, and (if enabled) PMU counters.

4. **Power Measurement:**  
   With options such as `--joulescope`, the tool measures energy consumption during model inference.

5. **Library and Example Generation:**  
   Finally, you can create a static library or an AmbiqSuite example to integrate the model into your own projects.

### How ns_autodeploy.py Handles Model Data

- **Pickling Model Details:**  
  Model configuration and analysis data are saved to pickle files. These are reused in later stages to avoid re-analysis.

- **Generating Example Tensors:**  
  The tool creates sample input and output tensors (or uses a dataset) to test that the deployed model produces correct results.

---

## 6. Profiling and Debugging with CSV Results

### Example Profiling CSV Output

When performance profiling is enabled, ns_autodeploy.py collects per-layer metrics and writes them to a CSV file. A typical CSV header might look like this:

```
Layer,Tag,Time_us,Estimated_MACs,Cycles,PMU_Event0,PMU_Event1,PMU_Event2,PMU_Event3
```

#### Sample Row Example

Consider a row in the CSV file:
```
3,CONV_2D,450,120000,1800,230,150,80,40
```

This row indicates:
- **Layer:** 3  
- **Tag:** "CONV_2D" (the type of operation)
- **Time_us:** 450 microseconds for this layer’s execution.
- **Estimated_MACs:** 120,000 multiply–accumulate operations estimated.
- **Cycles:** 1,800 CPU cycles were used.
- **PMU_Event0 to PMU_Event3:** Hardware counters (such as instruction counts or cache accesses) with values 230, 150, 80, and 40 respectively.

These values help you understand both the computational cost (MACs and cycles) and the hardware activity (via PMU events).

### Debugging Helium (MVE) Usage with Per-Layer Profiling

Helium is ARM’s technology for M-Profile Vector Extension (MVE). In many Ambiq devices (such as Apollo5B), MVE can accelerate neural network inference. However, issues such as suboptimal vectorization or misconfiguration can result in lower-than-expected performance.

#### Using Per-Layer Results to Debug MVE (Helium)

1. **Review the Operation Tags:**  
   In the CSV file, each row’s “Tag” shows the operation type (e.g., CONV_2D, DEPTHWISE_CONV_2D). For operations that can benefit from MVE, you expect to see high estimated MAC counts and corresponding cycle counts.

2. **Compare Estimated MACs and Cycles:**  
   If an operation (such as a convolution) shows a much lower cycle count relative to the estimated MACs, it may indicate that the MVE instructions are not being fully utilized. For example, if you see a “CONV_2D” layer with 120,000 estimated MACs but only 1,000 cycles, compare that to similar layers on a known good configuration.

3. **Analyze PMU Counters:**  
   The PMU columns (PMU_Event0 through PMU_Event3) often include counters for vector operations. A lower-than-expected PMU count in these columns might signal that the Helium (MVE) unit is not active. In contrast, if non-vectorized operations are taking place, you might see a different pattern in these counters.

4. **Cross-Layer Comparison:**  
   Use the CSV to compare layers that are supposed to run with MVE acceleration. Look for inconsistencies: if most convolution layers have a similar ratio of cycles/MACs and one layer is an outlier, this layer may be the target of debugging.

5. **Configuration Checks:**  
   Ensure that your build and toolchain settings (for example, enabling MVE-specific compiler flags) are correctly set. The profiling data can help confirm that the expected hardware events occur, thus verifying that the Helium unit is in use.

#### Practical Debugging Example

Imagine your CSV profiling results show:
- Layer 5 (CONV_2D): Time 500us, Estimated_MACs 150,000, Cycles 2,000, PMU_Event0 = 300.
- Layer 6 (DEPTHWISE_CONV_2D): Time 480us, Estimated_MACs 140,000, Cycles 2,100, PMU_Event0 = 310.
- Layer 7 (CONV_2D): Time 600us, Estimated_MACs 150,000, Cycles 4,500, PMU_Event0 = 120.

Here, Layer 7 has a higher cycle count and a noticeably lower PMU counter than Layers 5 and 6. This suggests that Layer 7 might not be taking advantage of Helium (MVE) acceleration. You can then investigate whether the input dimensions, filter shapes, or compiler settings for Layer 7 differ from the other layers. Adjusting the code or the build configuration might help align its performance with expectations.

---

## 7. Developer Guide

### Code Structure Overview

The ns_autodeploy repository is organized as follows:
- **tools/** – Contains ns_autodeploy.py and other Python modules.
- **templates/** – Contains C/C++ code templates for generating model binaries and libraries.
- **profiles/** – Contains YAML files with PMU and other configuration settings.
- **ns_platform.yaml:** Contains platform-specific memory and configuration details.

### Key Modules and Functions

- **ns_autodeploy.py:**  
  The main script that orchestrates building, profiling, and deployment.

- **measure_power.py:**  
  Handles power measurement, integrating with devices like Joulescope.

- **validator.py:**  
  Validates the model by running inference on the device and comparing outputs.

- **gen_library.py:**  
  Generates static libraries and examples for integration into other projects.

- **ns_utils.py:**  
  Utility functions for template processing, file conversions, RPC connectivity, etc.

### Extending and Customizing ns_autodeploy.py

To customize ns_autodeploy.py:
- Edit the YAML configuration files to add new platforms or modify memory settings.
- Adjust template files in the `templates/` directory if you want to change the generated code.
- Use the provided Python modules as examples for adding new functionality, such as additional profiling metrics.

---

## 8. Appendix

### Glossary of Terms

- **TFLite:** TensorFlow Lite, a lightweight version of TensorFlow for embedded devices.
- **RPC:** Remote Procedure Call, used to communicate between the PC and target device.
- **PMU:** Performance Monitoring Unit, used to capture low-level hardware statistics.
- **Arena:** A memory buffer allocated for TFLite operations.
- **MAC:** Multiply–Accumulate operation, a key measure of computational work.
- **Helium (MVE):** ARM’s M-Profile Vector Extension used to accelerate vector operations.

### Frequently Asked Questions (FAQ)

**Q:** What do the PMU counter values mean?  
**A:** They represent hardware-level events (like vector operations or cache activity). Check the CSV header definitions to map these values to specific hardware events.

**Q:** How do I debug if my Helium (MVE) isn’t used?  
**A:** Compare per-layer metrics. Look for unexpectedly high cycle counts or low PMU event counts in layers that should be accelerated by Helium. Also verify that your compiler flags enable MVE.

### References and Resources

- [neuralSPOT GitHub Repository](https://github.com/AmbiqAI/neuralSPOT)
- [TensorFlow Lite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers)
- [AmbiqSuite SDK](https://www.ambiq.com/ambiqsuite)
- [Helium (MVE) Overview](https://developer.arm.com/architectures/learn-more/arm-architecture/helium)

### Change Log

Keep an eye on the repository’s release notes for updates to ns_autodeploy.py and related tools.

---

This comprehensive guide should help you install, configure, use, and debug ns_autodeploy.py. Whether you’re simply deploying a model or digging into per-layer performance to optimize Helium (MVE) usage, the information here provides clear steps and examples to get you started.