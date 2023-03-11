# NeuralSPOT Tools
This directory contains assorted scripts designed to work with neuralSPOT.

1. generic_data.py: example RPC script
2. tflite_profile.py: Analyzes a TFLite file and estimates macs per layer
3. schema_py_generated.py: Cloned from TLFM repo, used by tflite_profile.py
4. tflm_validate.py: Convert TFlite to binary, fine tune, and characterize.


## TFLM Validate Theory of Operations
The tflm_validate.py script is a all-in-one tool for automatically testing TFLite files on Ambiq EVBs. Briefly, the script will:
1. Load the specified TFLite model file
1. Convert it to a C file
1. Wrap it in a baseline neuralSPOT application
1. Flash it to an EVB
1. Perform an initial characterization over USB using RPC
1. Use the results to fine-tune the application's memory allocation
1. Flash the tuned application to the EVB
1. Run invoke() both locally and on the EVB, feeding the same data to both
1. Compare the resulting output tensors and summarize the differences
1. Profile the execution of the model on a per-layer basis, exporting the results as a CSV file.

Example usage:
```bash
python -m tflm_validate --tflite-filename=../trained_models/model.tflite --random-data --create-binary --profile-enable --runs 100 --profile-warmup 3
```

### Caveats
This script is experimental, with known limitations (we're working on addressing these):
- Only one input and output tensor are supported, and only int8
- Only 1 subgraph is supported
- Tensors must be less than 4k bytes
