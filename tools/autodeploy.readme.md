## NS_AutoDeploy Theory of Operations

The ns_autodeploy script is a all-in-one tool for automatically deploy, testing, profiling, and package TFLite files on Ambiq EVBs.

![image-20230331153515132](../docs/images/autodeploy-flow.png)

*NOTE*: For a detailed description of how AutoDeploy can be used to characterize a TFLite model on Apollo EVBs, see [the application note](../docs/From%20TF%20to%20EVB%20-%20testing,%20profiling,%20and%20deploying%20AI%20models.md).


Briefly, the script will:

1. Load the specified TFLite model file and compute a few things (input and output tensors, number of layers, etc.)
1. Convert the TFlite into a C file, wrap it in a baseline neuralSPOT application, and flash it to an EVB
1. Perform an initial characterization over USB using RPC and use the results to fine-tune the application's memory allocation, then flash the fine-tuned version.
1. Run invoke() both locally and on the EVB, feeding the same data to both, optionally profiling the performance of the first invoke() on the EVB
1. Compare the resulting output tensors and summarize the differences, and report the performance (and store it in a CSV)
1. If a joulescope is present and power profiling is enable, run inferences in both LP and HP modes to measure power, energy, and time per inference.
1. Create a static library (with headers) containing the model, TFLM, and a minimal wrapper to ease integration into applications.
1. Create a simple AmbiqSuite example suitable for copying into AmbiqSuites example directory.

Example usage:
```bash
$> ns_autodeploy --model-name mymodel --tflite-filename mymodel.tflite
```

This will produce output similar to:

```bash
*** Stage [1/4]: Create and fine-tune EVB model characterization image
Compiling and deploying Baseline image: arena size = 120k, RPC buffer size = 4096, Resource Variables count = 0
Compiling and deploying Tuned image:    arena size = 99k, RPC buffer size = 4096, Resource Variables count = 0

*** Stage [2/4]: Characterize model performance on EVB
Calling invoke on EVB 3 times.
100%|███████████████| 3/3 [00:03<00:00,  1.05s/it]

*** Stage [3/4]: Generate minimal static library
Generating minimal library at neuralSPOT/mymodel/mymodel

*** Stage [4/4]: Generate AmbiqSuite Example
Generating AmbiqSuite example at ../projects/autodeploy/vww/vww_ambiqsuite
AmbiqSuite example generated successfully at neuralSPOT/mymodel/mymodel_ambiqsuite

Characterization Report for vww:
[Profile] Per-Layer Statistics file:         vww_stats.csv
[Profile] Max Perf Inference Time (ms):      930.264
[Profile] Total Estimated MACs:              7489664
[Profile] Total CPU Cycles:                  178609526
[Profile] Total Model Layers:                31
[Profile] MACs per second:                   8051116.672
[Profile] Cycles per MAC:                    23.847
[Power]   Max Perf Inference Time (ms):      0.000
[Power]   Max Perf Inference Energy (uJ):    0.000
[Power]   Max Perf Inference Avg Power (mW): 0.000
[Power]   Min Perf Inference Time (ms):      0.000
[Power]   Min Perf Inference Energy (uJ):    0.000
[Power]   Min Perf Inference Avg Power (mW): 0.000

Notes:
        - Statistics marked with [Profile] are collected from the first inference, whereas [Power] statistics
          are collected from the average of the 100 inferences. This will lead to slight
          differences due to cache warmup, etc.
        - CPU cycles are captured via Arm ETM traces
        - MACs are estimated based on the number of operations in the model, not via instrumented code
```

Readers will note that the `[Power]` values are all zeros - this is because power characterization requires a [https://www.joulescope.com](https://www.joulescope.com) and is disabled by default. For more information on how to measure power using Autodeploy, see see [the application note](../docs/From%20TF%20to%20EVB%20-%20testing,%20profiling,%20and%20deploying%20AI%20models.md).

Autodeploy is capable of showing more information via the `verbosity` command line option. For example, for a detailed per-layer performance analysis, use `--verbosity=1`, though this same information is always saved as a CSV.


![image-20230331154338838](../docs/images/image-20230331154338838.png)

### Automatic Behaviors
Autodeploy uses automation to simplify it's usability. It knows the target platform capabilities and the TFlite model's characteristics, and uses that to determine:
- What transport to use (UART or USB, depending on apollo EVB), and what TTY to connect to (depending on host operating system)
- Whether the model fits on the platform, and if so, where to best place it.

Autodeploy uses the specified `--platform` to automatically determine many of the command line parameters. Note that these parameters can be overridden using the related CLI parameter.

1. model_location: chosen based on computed model size 
1. arena_location: chosen based on arena size (max for first iteration, computed for rest)
1. max_arena_size: chosen based on platform's largest SRAM. If arena exceeds SRAM size (resulting in compile error) please set it manually to 8000 and set arena_location manually to PSRAM
1. ambiqsuite version: latest for platform
1. tensorflow version: latest compiled for platform

Knowing the platform also allows platform-specific error checking, such as
  1. Checks in memories (arena and model size) exceed a platform's capabilities
  1. Ensures that perf and example stages are not run unless the model fits in MRAM (larger models require USB, and perf/examples do not include USB)

Platform capabilities are specified in a separate yaml file (tools/ns_platform.yaml) for easy updating.

### Very Large Model Support
Autodeploy allows users to specify where the model will be stored (MRAM, TCM, SRAM, PSRAM) and where the tensor arena (e.g. activations) will be stored (TCM, SRAM, PSRAM) via the `model-location` and `arena-location` command line parameters. This allows profiling and comparison of different memory combinations.

For models that do not fit into on-board memories (aka Very Large Models aka VLMs), autodeploy supports locating those models in PSRAM and loading of the weights via USB. This capability is only supported by the profiling (i.e. create-profile) functionality since the performance binaries do not include USB enablement in order to minimize power. 

To run a VLM, invoke ns_autodeploy like so:
```bash
ns_autodeploy --apollo510_evb --model-name efficient-psram-hex --tflite-filename ../../../ourmodels/efficientnet-lite0-int8.tflite  --tty /dev/tty.usbmodem1234571   --no-create-library --no-create-ambiqsuite-example --runs 1 --model-location PSRAM --arena-location PSRAM
  ```
Note the arena size (when using PSRAM, it can be as high as 10MB, or 10000), and the number of runs being '1' (large models may take a long time to infer). The script will upload the model via USB, which may take some time, invoke it, and return the resulting tensors. Models as large as 20MB weights and 10MB arena are supported. For statistics capture, up to 1600 layers are supported.

The model used as an example above (efficientnet-lite0-int8) needs 1.6MBs for the arena, which happens to fit in SRAM. To place it there instead of PSRAM, use the following command:
```bash
ns_autodeploy --platform apollo510_evb --model-name efficient-psram-hex --tflite-filename ../../../ourmodels/efficientnet-lite0-int8.tflite  --tty /dev/tty.usbmodem1234571   --no-create-library --no-create-ambiqsuite-example --runs 1 --model-location PSRAM --arena-location SRAM
  ```


### Autodeploy Command Line Options

```bash
python -m ns_autodeploy --help
optional arguments:
  --seed SEED           Random Seed (default: 42)
  --platform PLATFORM   Platform to deploy model on (e.g. apollo4p_evb, apollo510_evb...) (default: apollo510_evb)
  --no-create-binary    Create a neuralSPOT Validation EVB image based on TFlite file (default: True)
  --no-create-profile   Profile the performance of the model on the EVB (default: True)
  --no-create-library   Create minimal static library based on TFlite file (default: True)
  --create-ambiqsuite-example
                        Create AmbiqSuite example based on TFlite file (default: False)
  --joulescope          Measure power consumption of the model on the EVB using Joulescope (default: False)
  --onboard-perf        Capture and print performance measurements on EVB (default: False)
  --model-location MODEL_LOCATION
                        Where the model is stored on the EVB (Auto, TCM, SRAM, MRAM, or PSRAM) (default: auto)
  --arena-location ARENA_LOCATION
                        Where the arena is stored on the EVB (auto, TCM, SRAM, or PSRAM) (default: auto)
  --tflite-filename TFLITE_FILENAME
                        Name of tflite model to be analyzed (default: undefined)
  --max-arena-size MAX_ARENA_SIZE
                        Maximum KB to be allocated for TF arena, 0 for auto (default: 0)
  --arena-size-scratch-buffer-padding ARENA_SIZE_SCRATCH_BUFFER_PADDING
                        (TFLM Workaround) Padding to be added to arena size to account for scratch buffer (in KB) (default: 0)
  --resource-variable-count RESOURCE_VARIABLE_COUNT
                        Maximum ResourceVariables needed by model (typically used by RNNs) (default: 0)
  --no-random-data      Use random input tensor data (default: True)
  --dataset DATASET     Name of dataset if --random_data is not set (default: dataset.pkl)
  --runs RUNS           Number of inferences to run for characterization (default: 10)
  --runs-power RUNS_POWER
                        Number of inferences to run for power measurement (default: 100)
  --cpu-mode CPU_MODE   Validation CPU Mode - can be LP (low power) or HP (high performance) (default: HP)
  --model-name MODEL_NAME
                        Name of model to be used in generated library (default: model)
  --working-directory WORKING_DIRECTORY
                        Directory where generated library will be placed (default: ../projects/autodeploy)
  --ambiqsuite-version AMBIQSUITE_VERSION
                        AmbiqSuite version used to generate minimal example, 'auto' for latest (default: auto)
  --tensorflow-version TENSORFLOW_VERSION
                        Tensorflow version used to generate minimal example, 'auto' for latest (default: auto)
  --profile-warmup PROFILE_WARMUP
                        How many inferences to profile (default: 1)
  --verbosity VERBOSITY
                        Verbosity level (0-4) (default: 1)
```

### Caveats
There is a known TFLM bug wherein the arena size estimator fails to account for temporary scratch buffers. If this occurs, the defaul ns_autodeploy parameters will cause a configuration failure, and the script will exit with the following error:
```bash
Model Configuration Failed
```
When this occurs, padding for scratch buffers must be manually added via the `--arena_size_scratch_buffer_padding` option. The value, in kilobytes, must be chosen via experimentation (in other words, pick a number and go up or down from there).

