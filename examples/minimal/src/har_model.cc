/**
 * @file XXX_model.c
 * @author Template code for
 * @brief
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "har_model.h"
#include "har_model_data.h"
#include "ns_model.h"

#define lmn(x, p) local_##x##p
#define LOCAL_NAME(x, postfix) lmn(x, postfix)

#define ns_lm_state LOCAL_NAME(LOCAL_MODEL_NAME, _tflm)
#define ns_lm_init LOCAL_NAME(LOCAL_MODEL_NAME, _init)
#define ns_lm_tensor_arena LOCAL_NAME(LOCAL_MODEL_NAME, _tensor_arena)
#define ns_lm_tensor_arena_size LOCAL_NAME(LOCAL_MODEL_NAME, _tensor_arena_size)
#define ns_lm_tensor_arena_size LOCAL_NAME(LOCAL_MODEL_NAME, _tensor_arena_size)
#define ns_lm_model LOCAL_NAME(LOCAL_MODEL_NAME, _model)

static constexpr int ns_lm_tensor_arena_size = 1024 * LOCAL_NAME(LOCAL_MODEL_NAME, _ARENA_SIZE);
alignas(16) static uint8_t ns_lm_tensor_arena[ns_lm_tensor_arena_size];

int
ns_lm_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = ns_lm_model;
    ms->arena = ns_lm_tensor_arena;
    ms->arena_size = ns_lm_tensor_arena_size;
    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = ns_model_init(ms);
    return status;
}
