#ifndef __model_API_H
#define __model_API_H

#include "ns_model.h"

#define model_STATUS_SUCCESS 0
#define model_STATUS_FAILURE -1
#define model_STATUS_INVALID_HANDLE 1
#define model_STATUS_INVALID_VERSION 2
#define model_STATUS_INVALID_CONFIG 3
#define model_STATUS_INIT_FAILED 4

extern int
model_init(ns_model_state_t *ms);

extern int
model_minimal_init(ns_model_state_t *ms);

#endif
