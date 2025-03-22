#ifndef __NNSE_MODEL_H__
#define __NNSE_MODEL_H__

#define LOCAL_MODEL_NAME NNSE
// Calculated Arena size
#define NNSE_COMPUTED_ARENA_SIZE 20

#define NNSE_MODEL_LOCATION NS_AD_TCM
#define NNSE_ARENA_LOCATION NS_AD_TCM

#define NNSE_STATUS_SUCCESS 0
#define NNSE_STATUS_FAILURE -1
#define NNSE_STATUS_INVALID_HANDLE 1
#define NNSE_STATUS_INVALID_VERSION 2
#define NNSE_STATUS_INVALID_CONFIG 3
#define NNSE_STATUS_INIT_FAILED 4
#include "ns_model.h"
#include "arm_mve.h"
int
nnse_init(ns_model_state_t *ms);

int
nnse_minimal_init(ns_model_state_t *ms);


#endif // __NNSE_MODEL_H__

