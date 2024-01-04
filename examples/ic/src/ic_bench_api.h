#ifndef __ic_bench_API_H
#define __ic_bench_API_H

#include "ns_model.h"

#define ic_bench_STATUS_SUCCESS 0
#define ic_bench_STATUS_FAILURE -1
#define ic_bench_STATUS_INVALID_HANDLE 1
#define ic_bench_STATUS_INVALID_VERSION 2
#define ic_bench_STATUS_INVALID_CONFIG 3
#define ic_bench_STATUS_INIT_FAILED 4

extern int ic_bench_init(ns_model_state_t *ms);

extern int ic_bench_minimal_init(ns_model_state_t *ms);

#endif
