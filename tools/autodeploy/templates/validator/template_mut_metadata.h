#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"

// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE NS_AD_ARENA_SIZE
#define TFLM_VALIDATOR_RX_BUFSIZE NS_AD_RPC_BUFSIZE
#define TFLM_VALIDATOR_TX_BUFSIZE NS_AD_RPC_BUFSIZE

#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES NS_AD_RV_COUNT

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT NS_AD_MAC_ESTIMATE_COUNT
uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    NS_AD_MAC_ESTIMATE_LIST};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#endif