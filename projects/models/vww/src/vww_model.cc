/**
 * @file <vww>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "vww_model.h"
#include "vww_model_data.h"
#include "ns_model.h"

static constexpr int vww_tensor_arena_size = 1024 * vww_COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t vww_tensor_arena[vww_tensor_arena_size];

int
vww_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = vww_model;
    ms->arena = vww_tensor_arena;
    ms->arena_size = vww_tensor_arena_size;
    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = ns_model_init(ms);
    return status;
}
