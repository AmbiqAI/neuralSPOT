#!/usr/bin/env python3
import argparse
import subprocess
import yaml
import sys
import os

def load_yaml_models(yaml_path):
    if not os.path.exists(yaml_path):
        print(f"Error: YAML file '{yaml_path}' does not exist.")
        sys.exit(1)
    with open(yaml_path, "r") as f:
        try:
            data = yaml.safe_load(f)
        except yaml.YAMLError as e:
            print("Error parsing YAML:", e)
            sys.exit(1)
    if not data or "models" not in data:
        print("Error: YAML file must contain a top-level 'models' key.")
        sys.exit(1)
    return data["models"]

def build_command(model, args):
    """
    Build the ns_autodeploy command based on global arguments and per-model settings.
    """
    cmd = ["ns_autodeploy"]

    # Global parameters.
    cmd.extend(["--platform", args.platform])
    cmd.extend(["--toolchain", args.toolchain])
    cmd.extend(["--resultlog-file", args.resultlog_file])
    cmd.extend(["--profile-results-path", args.profile_results_path])

    if args.tflm_version != "auto":
        cmd.extend(["--tensorflow-version", args.tflm_version])

    # Enable joulescope if specified.
    if args.joulescope:
        cmd.append("--joulescope")

    # If the platform belongs to the apollo5 family, enable full PMU capture.
    if "apollo5" in args.platform.lower():
        cmd.append("--full-pmu-capture")

    # Model-specific: tflite filename is required.
    if "tflite_filename" not in model:
        print("Error: Each model must specify a 'tflite_filename' in the YAML file.")
        sys.exit(1)
    # build model path
    model_path = os.path.join(args.model_directory, model["tflite_filename"])
    if not os.path.exists(model_path):
        print(f"Error: Model file '{model_path}' does not exist.")
        sys.exit(1)
    cmd.extend(["--tflite", model_path])
    # cmd.extend(["--tflite-filename", model["tflite_filename"]])

    # Optionally set model name.
    if "model_name" in model:
        cmd.extend(["--model-name", model["model_name"]])
    
    # Add arena_size_scratch_buffer_padding if provided.
    if "arena_size_scratch_buffer_padding" in model:
        cmd.extend(["--arena-size-scratch-buffer-padding", str(model["arena_size_scratch_buffer_padding"])])
    
    if "runs_power" in model:
        cmd.extend(["--runs-power", str(model["runs_power"])])

    if "arena_location" in model:
        cmd.extend(["--arena-location", model["arena_location"]])

    if "model_location" in model:
        cmd.extend(["--model-location", model["model_location"]])

    if "id" in model:
        cmd.extend(["--run-log-id", str(model["id"])])

    return cmd

def main():
    parser = argparse.ArgumentParser(
        description="Invoke ns_autodeploy for multiple models defined in a YAML file."
    )
    parser.add_argument(
        "--model-directory",
        type=str,
        required=False,
        default="../model_perf_tests/models/",
        help="Directory where the models are located."
    )
    parser.add_argument(
        "yaml_file",
        type=str,
        help="Path to the YAML file containing models configuration."
    )
    parser.add_argument(
        "--platform",
        type=str,
        required=True,
        help="Target platform (e.g. apollo4p_evb, apollo510_evb, etc.)."
    )
    parser.add_argument(
        "--toolchain",
        type=str,
        required=True,
        help="Target toolchain (e.g. gcc, arm)."
    )
    parser.add_argument(
        "--tflm-version",
        type=str,
        required=False,
        default="auto",
        help="Target neuralSPOT TFLM version."
    )
    parser.add_argument(
        "--resultlog-file",
        type=str,
        required=True,
        help="Path and filename to store the running log results."
    )
    parser.add_argument(
        "--profile-results-path",
        type=str,
        required=True,
        help="Path to store per-model profile results."
    )
    parser.add_argument(
        "--joulescope",
        action="store_true",
        required=False,
        default=False,
        help="Enable joulescope power consumption measurement."
    )
    args = parser.parse_args()

    models = load_yaml_models(args.yaml_file)

    # Timeout: 15 minutes (900 seconds).
    TIMEOUT_SECONDS = 900

    failed_models = []

    for model in models:
        model_id = model.get("model_name", model.get("tflite_filename", "Unknown model"))
        
        # Check unsupported_platforms, if specified.
        if "unsupported_platforms" in model:
            if args.platform in model["unsupported_platforms"]:
                print(f"Skipping model '{model_id}': target platform '{args.platform}' is in unsupported_platforms {model['unsupported_platforms']}")
                continue

        cmd = build_command(model, args)
        print(f"\nRunning ns_autodeploy for model: {model_id}")
        print("Command:", " ".join(cmd))
        
        success = False
        for attempt in range(2):
            try:
                print(f"Attempt {attempt + 1} for model: {model_id}")
                subprocess.run(cmd, check=True, timeout=TIMEOUT_SECONDS)
                print(f"Completed model: {model_id}")
                success = True
                break  # Exit the retry loop on success.
            except subprocess.TimeoutExpired:
                if attempt == 0:
                    print(f"Timeout expired after {TIMEOUT_SECONDS} seconds for model '{model_id}'. Retrying once...")
                else:
                    print(f"Timeout expired again for model '{model_id}'. Moving to next model.")
            except subprocess.CalledProcessError as e:
                print(f"ns_autodeploy failed for model '{model_id}' with error: {e}")
                break  # Do not retry on other errors.
        
        if not success:
            failed_models.append(model)

    if failed_models:
        print("\nThe following models failed:")
        for model in failed_models:
            print(f" - {model.get('model_name', model.get('tflite_filename', 'Unknown model'))}")
        # Generate a YAML file for failed models.
        print(failed_models)
        failed_yaml = {"models": failed_models}
        output_filename = "failed_models.yaml"
        with open(output_filename, "w") as f:
            yaml.dump(failed_yaml, f, default_flow_style=False)
        print(f"\nGenerated '{output_filename}' to re-execute the failed models.")
    else:
        print("\nAll models completed successfully.")

if __name__ == "__main__":
    main()
