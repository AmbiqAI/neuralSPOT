#ifndef TENSORFLOW_LITE_MICRO_KERNELS_AMBIQ_AMBIQ_COMMON_H_
#define TENSORFLOW_LITE_MICRO_KERNELS_AMBIQ_AMBIQ_COMMON_H_

arm_cmsis_nn_status arm_fully_connected_wrapper_s16(
    const cmsis_nn_context *ctx,
    const cmsis_nn_fc_params *fc_params,
    const cmsis_nn_quant_params *quant_params,
    const cmsis_nn_dims *input_dims,
    const int16_t *input_data,
    const cmsis_nn_dims *filter_dims,
    const int8_t *filter_data,
    const cmsis_nn_dims *bias_dims,
    const int64_t *bias_data,
    const cmsis_nn_dims *output_dims,
    int16_t *output_data
);

arm_cmsis_nn_status arm_fully_connected_per_channel_s16(
    const cmsis_nn_context *ctx,
    const cmsis_nn_fc_params *fc_params,
    const cmsis_nn_per_channel_quant_params *quant_params,
    const cmsis_nn_dims *input_dims,
    const int16_t *input_data,
    const cmsis_nn_dims *filter_dims,
    const int8_t *kernel,
    const cmsis_nn_dims *bias_dims,
    const int64_t *bias_data,
    const cmsis_nn_dims *output_dims,
    int16_t *output_data
);

arm_cmsis_nn_status arm_nn_vec_mat_mult_t_per_ch_s16(
    const int16_t *lhs,
    const int8_t *rhs,
    const int64_t *bias,
    int16_t *dst,
    const int32_t *dst_multiplier,
    const int32_t *dst_shift,
    const int32_t rhs_cols,
    const int32_t rhs_rows,
    const int32_t activation_min,
    const int32_t activation_max
);

#endif  // TENSORFLOW_LITE_MICRO_KERNELS_AMBIQ_AMBIQ_COMMON_H_
