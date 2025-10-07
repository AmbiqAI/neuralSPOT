#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

#if defined(ARM_FLOAT16_SUPPORTED)

#include <arm_fp16.h>

arm_cmsis_nn_status arm_fully_connected_fp16(
    const cmsis_nn_context *ctx,
    const cmsis_nn_fc_params *fc_params,
    const cmsis_nn_dims *input_dims,
    const float16_t *input,
    const cmsis_nn_dims *filter_dims,
    const float16_t *kernel,
    const cmsis_nn_dims *bias_dims,
    const float16_t *bias,
    const cmsis_nn_dims *output_dims,
    float16_t *output,
    const float16_t out_activation_min,
    const float16_t out_activation_max
) {

    int32_t batch_cnt = input_dims->n;

    while (batch_cnt)
    {
        arm_nn_vec_mat_mult_t_fp16(
            input,
            kernel,
            bias,
            output,
            filter_dims->n,
            output_dims->c,
            out_activation_min,
            out_activation_max
        );

        input += filter_dims->n;
        output += output_dims->c;
        batch_cnt--;
    }

    return (ARM_CMSIS_NN_SUCCESS);
}

#endif /* defined(ARM_FLOAT16_SUPPORTED) */
