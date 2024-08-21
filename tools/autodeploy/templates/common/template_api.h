#ifndef __NS_AD_NAME_API_H
#define __NS_AD_NAME_API_H

#include "ns_model.h"

#define NS_AD_NAME_STATUS_SUCCESS 0
#define NS_AD_NAME_STATUS_FAILURE -1
#define NS_AD_NAME_STATUS_INVALID_HANDLE 1
#define NS_AD_NAME_STATUS_INVALID_VERSION 2
#define NS_AD_NAME_STATUS_INVALID_CONFIG 3
#define NS_AD_NAME_STATUS_INIT_FAILED 4

extern int
NS_AD_NAME_init(ns_model_state_t *ms);

extern int
NS_AD_NAME_minimal_init(ns_model_state_t *ms);

#endif
