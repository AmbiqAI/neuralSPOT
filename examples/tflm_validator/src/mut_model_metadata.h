#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"

// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE 75
#define TFLM_VALIDATOR_RX_BUFSIZE 4096
#define TFLM_VALIDATOR_TX_BUFSIZE 4096

#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES 14

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 122
uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    0, 0, 0,      0, 0, 0, 0,     0,      0, 0, 0,     0,      0,  0, 0, 0, 0,      0,
    0, 0, 0,      0, 0, 0, 0,     0,      0, 0, 0,     0,      0,  0, 0, 0, 0,      0,
    0, 0, 0,      0, 0, 0, 0,     0,      0, 0, 0,     0,      0,  0, 0, 0, 0,      0,
    0, 0, 0,      0, 0, 0, 0,     221184, 0, 0, 0,     221184, 0,  0, 0, 0, 110592, 0,
    0, 0, 110592, 0, 0, 0, 0,     55296,  0, 0, 0,     55296,  0,  0, 0, 0, 55296,  0,
    0, 0, 110592, 0, 0, 0, 0,     55296,  0, 0, 0,     55296,  0,  0, 0, 0, 27648,  0,
    0, 0, 27648,  0, 0, 0, 27648, 0,      0, 0, 27648, 0,      96, 0};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#endif
