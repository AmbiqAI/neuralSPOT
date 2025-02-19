# ns_autodeploy Multi-Model Runner

This Python script automates the deployment of multiple TFLite models using the `ns_autodeploy` tool. The models and their configurations are defined in a YAML file. The script passes global parameters (e.g. target platform, toolchain, result log file, and profile results path) as well as per-model settings to `ns_autodeploy`.

## Features

- **Multiple Models**: Read and deploy a series of models specified in a YAML file.
- **Platform Filtering**: Each model may define an `unsupported_platforms` list. Models will be skipped if the target platform is found in that list.
- **Custom Arguments**: Supports per-model `arena_size_scratch_buffer_padding` to customize the `--arena-size-scratch-buffer-padding` argument.
- **Timeout & Retry**: Each `ns_autodeploy` invocation is given a 15-minute timeout. If the timeout is exceeded, the process is killed and retried once before moving on.
- **Additional Parameters**: Global options such as `--platform`, `--toolchain`, `--resultlog-file`, `--profile-results-path`, and the option to enable `--joulescope` are supported.

## Requirements

- Python 3
- `pyyaml` module (install via `pip install pyyaml` if needed)
- `ns_autodeploy` must be available in your system's PATH

## YAML File Format

The YAML file should have a top-level key `models` that contains a list of model definitions. Each model can include the following keys:

- `tflite_filename` (required): Path or name of the TFLite model file.
- `model_name` (optional): Name to be used for the model.
- `unsupported_platforms` (optional): A list of platform strings on which the model should **not** be deployed.
- `arena_size_scratch_buffer_padding` (optional): A numeric value to pass to the `--arena-size-scratch-buffer-padding` parameter.

### Example `models.yaml`

```yaml
models:
  - tflite_filename: "model1.tflite"
    model_name: "model1"
    unsupported_platforms:
      - apollo4p_evb
    arena_size_scratch_buffer_padding: 5

  - tflite_filename: "model2.tflite"
    unsupported_platforms:
      - apollo510_evb
```

## Usage

Run the script by providing the YAML file and the required command-line arguments:

```bash
python3 deploy_models.py models.yaml \
  --platform apollo510_evb \
  --toolchain gcc \
  --resultlog-file /path/to/result.log \
  --profile-results-path /path/to/profiles \
  --joulescope
```

### Command-Line Arguments

- **Positional:**
  - `yaml_file`: Path to the YAML file containing the models configuration.

- **Global Options:**
  - `--platform`: Target platform (e.g. `apollo4p_evb`, `apollo510_evb`, etc.).
  - `--toolchain`: Target toolchain (e.g. `gcc`, `arm`).
  - `--resultlog-file`: Path and filename where the deployment log will be stored.
  - `--profile-results-path`: Directory where per-model profile results will be stored.
  - `--joulescope`: Enable power consumption measurement using Joulescope.

## Behavior

1. **Model Filtering**:  
   If a model's YAML configuration includes an `unsupported_platforms` list and the target platform (passed via `--platform`) is found in that list, the model is skipped.

2. **Command Construction**:  
   The script builds the `ns_autodeploy` command by combining global parameters with per-model parameters:
   - Adds `--arena-size-scratch-buffer-padding` if specified.
   - If the platform string contains "apollo5", the script automatically adds the `--full-pmu-capture` flag.

3. **Timeout and Retry**:  
   Each model deployment is given 15 minutes (900 seconds) to complete. If `ns_autodeploy` does not complete in this time, it is terminated and retried once. If the second attempt also times out, the script moves on to the next model.