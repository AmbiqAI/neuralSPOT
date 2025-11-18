//*****************************************************************************
//
//! @file ambiq_core_config.h
//!
//! @brief Configuration options for running coremark on Ambiq devices.
//!
//! By default, Coremark is optimized for performance.
//! By default, CoremarkLP is optimized for power.
//!
//! The biggest differences between the two versions are that CoremarkLP uses
//! -O0 and -mlong-calls optimization options, while Coremark uses -o3 and
//! -funroll-loops.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AMBIQ_CORE_CONFIG_H
#define AMBIQ_CORE_CONFIG_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Ambiq configuration
//
//*****************************************************************************
//
// Specify the number of iterations.
// For Apollo5, the iterations value had to be increased from 2000 to 2250
// in order to get the benchmark to run at least 10s.
//
#define ITERATIONS                          2250

#define CRYPTO_CLK_GATE_EN

//
// AM_PRINTF_RESULTS will print the normal Coremark results to the console upon
//  completion. The printing is done to a RAM buffer during execution, and
//  afterwards, the ITM is turned on and the buffer is dumped to the console.
//

//
//  Set ALL_RETAIN macro to 1 when running code from ITCM for both Min Memory and All Memory Case
//
#define ALL_RETAIN 0 // 0 for min TCM retain (default), 1 for all retain

#define AM_PRINT_RESULTS                    1

#define ENABLE_BURST_MODE                   0

#define COREMARK_GPIO                       0

#define ELP_ON                              0 // 0 for ELP ret, 1 for ELP on
#define HP_MODE                             0 // 0 for LP mode, 1 for HP mode
//
//  Enable Register Dumps i.e. CLKCTRL, MCUCTRL, PWRCTRL
//
#define REGDUMP_ENABLE                      0

//
// Clear 64K of iCache before starting Coremark.
//
#define ICACHE_PREFILL                      1

//*****************************************************************************
//
// Coremark configuration
// These parameters generally need not be modified.
//
//*****************************************************************************
#define AMBIQ_SPECIFIC                      1
#define HAS_TIME_H                          0
#define HAS_STDIO                           0
#define HAS_PRINTF                          1
#define HAS_FLOAT                           1           // Indicates that printf %f is available
#define MAIN_HAS_NOARGC                     1
#define MEM_METHOD                          MEM_STACK   // MEM_STACK is default.  See core_portme.h

//*****************************************************************************
//
// Specify print output.
//
//*****************************************************************************

#define COMPILER_FLAGS                      "flags: -oMax"

#if MEM_METHOD == MEM_STACK
#define MEM_LOCATION                        "STACK"
#elif MEM_METHOD == MEM_STATIC
#define MEM_LOCATION                        "STATIC"
#elif MEM_METHOD == MEM_MALLOC
#define MEM_LOCATION                        "MALLOC"
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
int am_sprintf(char *pcFmt, ...);

#endif // AMBIQ_CORE_CONFIG_H
