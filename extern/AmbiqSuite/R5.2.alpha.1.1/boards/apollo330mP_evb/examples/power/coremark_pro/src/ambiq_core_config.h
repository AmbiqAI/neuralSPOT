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

#if ENABLE_CPU_HP_250
    #define MY_CORECLK_HZ                   ( AM_HAL_CLKGEN_FREQ_HP250_HZ )
#else
    #define MY_CORECLK_HZ                   ( AM_CORECLK_HZ )
#endif
#define MY_CORECLK_MHZ                      ( MY_CORECLK_HZ / 1000000 )

//
// Specify the number of iterations.
//
#if ENABLE_CPU_HP_250
    #define ITERATIONS                      3000
#else
    #define ITERATIONS                      2000
#endif

//*****************************************************************************
//
// Specify print output.
//
//*****************************************************************************
#define AM_CORECLK_HZ                       AM_HAL_CLKGEN_FREQ_MAX_HZ
#define AM_CORECLK_MHZ                      (AM_CORECLK_HZ/1000000)

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

#endif // AMBIQ_CORE_CONFIG_H
