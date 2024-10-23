#ifndef __model_ambiqsuite_API_H
#define __model_ambiqsuite_API_H

#include "ns_model.h"

#define model_ambiqsuite_STATUS_SUCCESS 0
#define model_ambiqsuite_STATUS_FAILURE -1
#define model_ambiqsuite_STATUS_INVALID_HANDLE 1
#define model_ambiqsuite_STATUS_INVALID_VERSION 2
#define model_ambiqsuite_STATUS_INVALID_CONFIG 3
#define model_ambiqsuite_STATUS_INIT_FAILED 4

extern int
model_ambiqsuite_init(ns_model_state_t *ms);

extern int
model_ambiqsuite_minimal_init(ns_model_state_t *ms);

#endif
