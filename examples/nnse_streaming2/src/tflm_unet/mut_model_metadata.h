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

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 164
const uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 204, 1428, 0, 0, 0, 0, 0, 0, 4032, 28224, 0, 0, 0, 0, 0, 0, 1764, 12348, 0, 0, 0, 0, 0, 0, 756, 5292, 0, 0, 0, 0, 0, 0, 63504, 63504, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3528, 24696, 0, 0, 0, 0, 0, 0, 0, 0, 7560, 52920, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16632, 116424, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34776, 5796, 0, 17733, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_BUS_CYCLES
#endif

#define NS_VALIDATOR_RPC_TRANSPORT NS_AD_RPC_TRANSPORT_USB

#endif