#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"

// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE 54
#define TFLM_VALIDATOR_RX_BUFSIZE 4096
#define TFLM_VALIDATOR_TX_BUFSIZE 4096

#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES 0

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 17
uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    0, 57024, 0, 0, 304128, 0, 0, 608256, 0, 0, 2433024, 0, 0, 0, 0, 63360, 0};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#endif
