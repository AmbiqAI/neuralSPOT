/**
 * @file <NS_AD_NAME>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "<NS_AD_NAME>_model.h"
#include "<NS_AD_NAME>_model_data.h"
#include "ns_model.h"

static constexpr int<NS_AD_NAME> _tensor_arena_size = 1024 * <NS_AD_NAME> _COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t<NS_AD_NAME> _tensor_arena[<NS_AD_NAME> _tensor_arena_size];

int<NS_AD_NAME>
_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = <NS_AD_NAME> _model;
    ms->arena = <NS_AD_NAME> _tensor_arena;
    ms->arena_size = <NS_AD_NAME> _tensor_arena_size;
    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = ns_model_init(ms);
    return status;
}
