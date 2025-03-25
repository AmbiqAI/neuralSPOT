#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"
// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE 100
#define TFLM_VALIDATOR_RX_BUFSIZE 4096
#define TFLM_VALIDATOR_TX_BUFSIZE 4096
#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES 14

#define TFLM_MODEL_LOCATION NS_AD_TCM
#define TFLM_ARENA_LOCATION NS_AD_TCM

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 161
const uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 768, 1024, 0, 0, 0, 0, 0, 0, 3024, 4032, 0, 0, 0, 0, 0, 0, 1488, 1984, 0, 0, 0, 0, 0, 0, 720, 960, 0, 0, 0, 0, 0, 0, 57600, 57600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2976, 3968, 0, 0, 0, 0, 0, 0, 0, 0, 6048, 8064, 0, 0, 0, 0, 0, 0, 0, 0, 12192, 16256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24672, 4112, 0, 66049, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_BUS_CYCLES
#endif

#define NS_VALIDATOR_RPC_TRANSPORT NS_AD_RPC_TRANSPORT_USB

#endif