/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. All wrapped libc
methods from th_libc.h and all testharness methods from th_lib.h are here.
==============================================================================*/
/// \file
/// \brief Submitter-implemented methods required to perform inference.
/// \detail All methods with names starting with th_ are to be implemented by
/// the submitter. All basic I/O, inference and timer APIs must be implemented
/// in order for the benchmark to output useful results, but some auxiliary
/// methods default to an empty implementation. These methods are provided to
/// enable submitter optimizations, and are not required for submission.

#ifndef MLPERF_TINY_V0_1_API_SUBMITTER_IMPLEMENTED_MODEL_H_
#define MLPERF_TINY_V0_1_API_SUBMITTER_IMPLEMENTED_MODEL_H_

/// \brief These defines set logging prefixes for test harness integration.
/// \detail This API is designed for performance evaluation only. In order to
/// gather energy measurments we recommend using the EEMBC test suite.

#define MAX_DB_INPUT_SIZE (32 * 32 * 3)
#define TH_MODEL_VERSION EE_MODEL_VERSION_IC01


#endif  // MLPERF_TINY_V0_1_API_SUBMITTER_IMPLEMENTED_MODEL_H_
