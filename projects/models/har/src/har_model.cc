/**
 * @file har_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "har_model.h"
#include "har_model_data.h"
#include "ns_model.h"

static constexpr int har_tensor_arena_size = 1024 * har_COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t har_tensor_arena[har_tensor_arena_size];

int
har_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = har_model;
    ms->arena = har_tensor_arena;
    ms->arena_size = har_tensor_arena_size;
    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = ns_model_init(ms);
    return status;
}
