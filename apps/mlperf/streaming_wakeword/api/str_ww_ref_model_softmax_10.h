#ifndef str_ww_ref_model_softmax_10_h
#define str_ww_ref_model_softmax_10_h

#include <stdint.h>
#include "arm_nnfunctions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the operator
 *
 * @return 0 on SUCCESS
 */
int32_t str_ww_ref_model_softmax_10_init(void);

/**
 * @brief Perform the operation
 *
 * @param[in]   input   Pointer to the input buffer.
 * @param[out]  output  Pointer to the output buffer.
 *
 * @return 0 on SUCCESS
 */
int32_t str_ww_ref_model_softmax_10_run(
    const int8_t* __restrict input,
    int8_t* __restrict output
);

#ifdef __cplusplus
}
#endif

#endif // str_ww_ref_model_softmax_10_h