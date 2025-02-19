#ifndef __NNSE_MODEL_H__
#define __NNSE_MODEL_H__
#define LOCAL_MODEL_NAME nnaedrelunopad
// Calculated Arena size
#define NNSE_COMPUTED_ARENA_SIZE 800

#define nnaedrelunopad_MODEL_LOCATION NS_AD_TCM
#define nnaedrelunopad_ARENA_LOCATION NS_AD_TCM

#define nnaedrelunopad_STATUS_SUCCESS 0
#define nnaedrelunopad_STATUS_FAILURE -1
#define nnaedrelunopad_STATUS_INVALID_HANDLE 1
#define nnaedrelunopad_STATUS_INVALID_VERSION 2
#define nnaedrelunopad_STATUS_INVALID_CONFIG 3
#define nnaedrelunopad_STATUS_INIT_FAILED 4

#include "ns_model.h"
int
nnse_init(ns_model_state_t *ms);

int
nnse_minimal_init(ns_model_state_t *ms);

#endif // __NNSE_MODEL_H__

