#ifndef __arrhythmia_model_power_API_H
#define __arrhythmia_model_power_API_H

#include "ns_model.h"

#define arrhythmia_model_power_STATUS_SUCCESS 0
#define arrhythmia_model_power_STATUS_FAILURE -1
#define arrhythmia_model_power_STATUS_INVALID_HANDLE 1
#define arrhythmia_model_power_STATUS_INVALID_VERSION 2
#define arrhythmia_model_power_STATUS_INVALID_CONFIG 3
#define arrhythmia_model_power_STATUS_INIT_FAILED 4

extern int
arrhythmia_model_power_init(ns_model_state_t *ms);

extern int
arrhythmia_model_power_minimal_init(ns_model_state_t *ms);

#endif
