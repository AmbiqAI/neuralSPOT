/**
 * @brief
 *
 */
#ifndef __TFLM_VALIDATOR_H
#define __TFLM_VALIDATOR_H
#include "ns_ambiqsuite_harness.h"
#include <cstdlib>
#include <cstring>

typedef struct {
    uint32_t profile_mut;
    uint32_t input_length;  // in bytes
    uint32_t output_length; // in bytes
} ns_mut_config_t;

typedef union {
    ns_mut_config_t config;
    char bytes[sizeof(ns_mut_config_t)];
} ns_incoming_config_t;

typedef struct {
    uint32_t computed_arena_size;
} ns_mut_stats_t;

typedef union {
    ns_mut_stats_t stats;
    char bytes[sizeof(ns_mut_stats_t)];
} ns_outgoing_stats_t;

extern ns_incoming_config_t mut_cfg;
extern ns_outgoing_stats_t mut_stats;
#endif
