#ifndef __MUT_MODEL_METADATA_H
#define __MUT_MODEL_METADATA_H
#include "ns_ambiqsuite_harness.h"

// Default Arena and RPC buffer sizes
#define TFLM_VALIDATOR_ARENA_SIZE 43
#define TFLM_VALIDATOR_RX_BUFSIZE 4096
#define TFLM_VALIDATOR_TX_BUFSIZE 4096

#define TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES 0

#define TFLM_VALIDATOR_MAC_ESTIMATE_COUNT 111
uint32_t tflm_validator_mac_estimates[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT] = {
    50400, 12000, 0,  0,    288,  288, 0, 0, 0,     38400, 8000, 0, 512,  512,  0,    0, 0, 51200,
    0,     8000,  0,  512,  512,  0,   0, 0, 51200, 0,     4800, 0, 0,    512,  512,  0, 0, 0,
    38400, 3600,  0,  1152, 1152, 0,   0, 0, 57600, 0,     3600, 0, 1152, 1152, 0,    0, 0, 57600,
    0,     3600,  0,  0,    576,  576, 0, 0, 0,     39936, 2496, 0, 1024, 1024, 0,    0, 0, 53248,
    0,     2496,  0,  1024, 1024, 0,   0, 0, 53248, 0,     2496, 0, 0,    1024, 1024, 0, 0, 0,
    43008, 2016,  0,  2304, 2304, 0,   0, 0, 64512, 0,     2016, 0, 2304, 2304, 0,    0, 0, 64512,
    0,     0,     288};
const int tflm_validator_number_of_estimates = TFLM_VALIDATOR_MAC_ESTIMATE_COUNT;

#endif
