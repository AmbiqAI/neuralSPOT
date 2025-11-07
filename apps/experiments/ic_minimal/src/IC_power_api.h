#ifndef __IC_power_API_H
#define __IC_power_API_H

#if 0 == 0
#include "ns_model.h"
extern int
IC_power_init(ns_model_state_t *ms);

extern int
IC_power_minimal_init(ns_model_state_t *ms);
#endif

#define IC_power_STATUS_SUCCESS 0
#define IC_power_STATUS_FAILURE -1
#define IC_power_STATUS_INVALID_HANDLE 1
#define IC_power_STATUS_INVALID_VERSION 2
#define IC_power_STATUS_INVALID_CONFIG 3
#define IC_power_STATUS_INIT_FAILED 4

#endif
