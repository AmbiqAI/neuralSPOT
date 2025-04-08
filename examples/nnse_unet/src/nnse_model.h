#ifndef __NNSE_MODEL_H__
#define __NNSE_MODEL_H__
#define LOCAL_MODEL_NAME nnaedrelunopad
// Calculated Arena size
#define NNSE_COMPUTED_ARENA_SIZE 200

#define nnaedrelunopad_MODEL_LOCATION NS_AD_TCM
#define nnaedrelunopad_ARENA_LOCATION NS_AD_TCM

#define nnaedrelunopad_STATUS_SUCCESS 0
#define nnaedrelunopad_STATUS_FAILURE -1
#define nnaedrelunopad_STATUS_INVALID_HANDLE 1
#define nnaedrelunopad_STATUS_INVALID_VERSION 2
#define nnaedrelunopad_STATUS_INVALID_CONFIG 3
#define nnaedrelunopad_STATUS_INIT_FAILED 4
// #include "third_party/cmsis_nn/Include/arm_nnsupportfunctions.h"
// #include "third_party/cmsis_nn/Include/Internal/arm_nn_compiler.h"
#include "ns_model.h"
int
nnse_init(ns_model_state_t *ms);

int
nnse_minimal_init(ns_model_state_t *ms);

// arm_cmsis_nn_status arm_nn_vec_mat_mult_t_s16_test(
//     const int16_t *lhs,
//     const int8_t *rhs,
//     const int64_t *bias,
//     int32_t *dst,
//     const int32_t rhs_cols,
//     const int32_t rhs_rows);
#endif // __NNSE_MODEL_H__

