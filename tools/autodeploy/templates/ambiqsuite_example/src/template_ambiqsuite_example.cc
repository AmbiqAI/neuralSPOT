/**
 * @file NS_AD_NAME_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_example_tensors.h"
#include "ns_model.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

/*
Notes:
- This example is compiled along with startup_gcc and linker_script to generate an
  Ambiq EVB image (axf and bin). However, the following code doesn't make any assumptions
  about platform.

- It includes sample input and output tensors - the examples operate on these without
  regard to datatype (it treats everything as int8 arrays). The tensors are TF tensor
  types, with all dtype, dimensions, etc, preserved as usual.
*/

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

int main(void) {

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

    // Initialize the printf interface for ITM output
    if (am_bsp_debug_printf_enable()) {
        // Cannot print - so no point proceeding
        while (1)
            ;
    }

    // Initialize the model, get handle if successful
    am_util_stdio_printf("Initializing NS_AD_NAME model...");
    int status = NS_AD_NAME_minimal_init(&model); // model init with minimal defaults

    if (status == NS_AD_NAME_STATUS_FAILURE) {
        am_util_stdio_printf(" failed. Status = %d\n", status);
        while (1)
            ;
    } else {
        am_util_stdio_printf(" success!\n");
    }

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

    // Get data about input and output tensors
    int numInputs = model.numInputTensors;
    int numOutputs = model.numOutputTensors;

    am_util_stdio_printf("Initializing %d input tensor(s)\n", numInputs);
    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {
        am_util_debug_printf(
            "Initializing input tensor %d (%d bytes)\n", i, model.model_input[i]->bytes);
        memcpy(
            model.model_input[i]->data.int8, ((char *)NS_AD_NAME_example_input_tensors) + offset,
            model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }

    // Execute the model
    am_util_stdio_printf("Invoking model...");
    TfLiteStatus invoke_status = model.interpreter->Invoke();
    am_util_stdio_printf(" success!\n");

    if (invoke_status != kTfLiteOk) {
        while (1)
            example_status = NS_AD_NAME_STATUS_FAILURE; // invoke failed, so hang
    }

    // Compare the bytes of the output tensors against expected values
    offset = 0;
    am_util_stdio_printf("Checking %d output tensors\n", numOutputs);
    for (int i = 0; i < numOutputs; i++) {
        am_util_stdio_printf(
            "Checking output tensor %d (%d bytes)\n", i, model.model_output[i]->bytes);
        if (0 != memcmp(
                     model.model_output[i]->data.int8,
                     ((char *)NS_AD_NAME_example_output_tensors) + offset,
                     model.model_output[i]->bytes)) {
            // Miscompare!
            am_util_stdio_printf("Miscompare in output tensor %d\n", i);

            while (1)
                example_status = NS_AD_NAME_STATUS_INVALID_CONFIG; // miscompare, so hang
        }
        offset += model.model_output[i]->bytes;
    }
    am_util_stdio_printf("All output tensors matched expected results.\n");

    while (1) {
        // Success!
        example_status = NS_AD_NAME_STATUS_SUCCESS;
    }
}
