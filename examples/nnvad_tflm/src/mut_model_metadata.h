#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"

// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE 52
#define TFLM_VALIDATOR_RX_BUFSIZE 4096
#define TFLM_VALIDATOR_TX_BUFSIZE 4096
#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES 12

#define TFLM_MODEL_LOCATION NS_AD_TCM
#define TFLM_ARENA_LOCATION NS_AD_TCM

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 136
const uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12480, 189440, 87040, 35840, 0, 0, 0, 14336, 0, 0, 0, 2048, 0, 0, 0, 0, 0, 7168, 0, 0, 0, 0, 2048, 0, 0, 0, 2048, 0, 0, 0, 0, 2048, 0, 0, 0, 2048, 0, 0, 0, 0, 2048, 0, 0, 0, 2048, 0, 0, 0, 0, 2048, 0, 0, 0, 2048, 0, 0, 0, 0, 2048, 0, 0, 0, 2048, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_BUS_CYCLES
#endif

#define NS_VALIDATOR_RPC_TRANSPORT NS_AD_RPC_TRANSPORT_USB

#endif