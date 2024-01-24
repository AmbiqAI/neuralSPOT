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
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
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
//
#define ITERATIONS                          2000

//
// AM_PRINTF_RESULTS will print the normal Coremark results to the console upon
//  completion. The printing is done to a RAM buffer during execution, and
//  afterwards, the ITM is turned on and the buffer is dumped to the console.
//
#define AM_PRINT_RESULTS                    1

#define COREMARK_GPIO                       0

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
#define AM_CORECLK_HZ                       AM_HAL_CLKGEN_FREQ_MAX_HZ
#define AM_CORECLK_MHZ                      AM_HAL_CLKGEN_FREQ_MAX_MHZ
#define AM_CORECLK_96MHZ                   (AM_HAL_CLKGEN_FREQ_MAX_MHZ * 2)


//
// Since the stringize operator itself does not first expand macros, two levels
//  of indirection are required in order to fully resolve the pre-defined
//  compiler (integer) macros.  The 1st level expands the macro, and the 2nd
//  level actually stringizes it.
// This method will also work even if the argument is not a macro. However, if
//  the argument is already a string, the string will end up with inserted quote
//   marks.
//
#define STRINGIZE_VAL(n)                    STRINGIZE_VAL2(n)
#define STRINGIZE_VAL2(n)                   #n

#define COMPILER_FLAGS                      "flags: -o3"

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
int am_sprintf(char*, ...);

#endif // AMBIQ_CORE_CONFIG_H
