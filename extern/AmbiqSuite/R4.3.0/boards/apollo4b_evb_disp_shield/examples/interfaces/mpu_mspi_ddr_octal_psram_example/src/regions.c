//*****************************************************************************
//
//! @file regions.c
//!
//! @brief Provides MPU region structures for the mpu_sniff_test example.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "am_mcu_apollo.h"
#include "regions.h"

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
tMPURegion sMRAM =
{
    1,                          // Region number: 1
    0x00000000,                 // Base address: 0x00000000
    20,                         // Size: 2MB (2^(20+1))
    PRIV_RW_PUB_RW,             // Full Access
    false,                      // Don't prevent execution
    0x0                         // Don't disable any subregions.
};

tMPURegion sTCM =
{
    2,                          // Region number: 2
    0x10000000,                 // Base address: 0x10000000
    18,                         // Size: 512KB (2^(18+1)) TCM size = 384KB
    PRIV_RW_PUB_RW,             // Full Access
    false,                      // Don't prevent execution
    0xC0                        // Disable subregions number 7,8
};

tMPURegion sSSRAM_A =
{
    3,                          // Region number: 3
    0x10000000,                 // Base address: 0x10060000 Protection from 0x10060000
    19,                         // Size: 1MB (2^(19+1)) Protection Size = 1MB
    PRIV_RO_PUB_RO,             // Read Only
    false,                      // Don't prevent execution
    0x7                         // Disable subregions number 1,2,3
};

tMPURegion sSSRAM_B =
{
    4,                          // Region number: 6
    0x10100000,                 // Base address: 0x10060000 Protection from 0x10060000
    18,                         // Size: 512KB (2^(18+1)) Protection Size = 512KB
    PRIV_RO_PUB_RO,             // Read Only
    false,                      // Don't prevent execution
    0xC0                        // Disable subregions number 7,8
};

tMPURegion sDSPRAM_A =
{
    5,                          // Region number: 5
    0x10100000,                 // Base address: 0x10160000
    18,                         // Size: 512KB (2^(18+1)) Protection Size = 512KB
    PRIV_RO_PUB_RO,             // Read only
    false,                      // Don't prevent execution
    0x3F                         // Don't disable any subregions.
};

tMPURegion sDSPRAM_B =
{
    6,                          // Region number: 5
    0x10180000,                 // Base address: 0x10160000
    18,                         // Size: 512KB (2^(18+1)) Protection Size = 512KB
    PRIV_RO_PUB_RO,             // Read only
    false,                      // Don't prevent execution
    0x0                         // Don't disable any subregions.
};

tMPURegion sPSRAM =
{
    7,                          // Region number: 7
    0x14000000,                 // Base address: 0x14000000 for MSPI0
    25,                         // Size: 64MB (2^(25+1))
    PRIV_RO_PUB_RO,             // Read only
    false,                      // Don't prevent execution
    0x0                         // Don't disable any subregions.
};

