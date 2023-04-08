/**
 * @file tflm_validator.cc
 * @author Carlos Morales
 * @brief Shell application for instantiating a TFLM model and allowing a RPC
 * client to set input tensors, invoke() it, and collect output tensors
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_example_tensors.h"
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

// TFLM Config and arena
static ns_model_state_t model;

int
main(void) {

    // Initialize the model, get handle if successful
    int status = NS_AD_NAME_minimal_init(&model); // model init with minimal defaults

    if (status == NS_AD_NAME_STATUS_FAILURE) {
        while (1)
            ; // hang
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
        memcpy(model.model_input[i]->data.int8, ((char *)NS_AD_NAME_example_input_tensors) + offset,
               model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }

    // Execute the model
    TfLiteStatus invoke_status = model.interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        while (1)
            ; // invoke failed, so hang
    }

    // Compare the bytes of the output tensors against expected values
    offset = 0;
    for (int i = 0; i < numOutputs; i++) {
        if (0 != memcmp(model.model_output[i]->data.int8,
                        ((char *)NS_AD_NAME_example_output_tensors) + offset,
                        model.model_output[i]->bytes)) {
            while (1)
                ; // miscompare, so hang
        }
        offset += model.model_output[i]->bytes;
    }

    while (1) {
        // Success!
    }
}
