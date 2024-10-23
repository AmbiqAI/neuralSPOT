/**
 * @file model_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "model_api.h"
#include "model_example_tensors.h"
#include "ns_model.h"

/*
Notes:
- This example is compiled along with startup_gcc and linker_script to generate an
  Ambiq EVB image (axf and bin). However, the following code doesn't make any assumptions
  about platform.

- This example is meant to be walked through using a debugger such as GDB. It doesn't
  have I/O code so it doesn't actual input or output anything.

- It includes sample input and output tensors - the examples operate on these without
  regard to datatype (it treats everything as int8 arrays). The tensors are TF tensor
  types, with all dtype, dimensions, etc, preserved as usual.
*/

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

int
main(void) {

    // Initialize the model, get handle if successful
    int status = model_minimal_init(&model); // model init with minimal defaults

    if (status == model_STATUS_FAILURE) {
        while (1)
            example_status = model_STATUS_INIT_FAILED; // hang
    }

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

    // Get data about input and output tensors
    int numInputs = model.numInputTensors;
    int numOutputs = model.numOutputTensors;

    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {
        memcpy(model.model_input[i]->data.int8, ((char *)model_example_input_tensors) + offset,
               model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }

    // Execute the model
    TfLiteStatus invoke_status = model.interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        while (1)
            example_status = model_STATUS_FAILURE; // invoke failed, so hang
    }

    // Compare the bytes of the output tensors against expected values
    offset = 0;
    for (int i = 0; i < numOutputs; i++) {
        if (0 != memcmp(model.model_output[i]->data.int8,
                        ((char *)model_example_output_tensors) + offset,
                        model.model_output[i]->bytes)) {
            while (1)
                example_status = model_STATUS_INVALID_CONFIG; // miscompare, so hang
        }
        offset += model.model_output[i]->bytes;
    }

    while (1) {
        // Success!
        example_status = model_STATUS_SUCCESS;
    }
}
