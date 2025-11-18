//*****************************************************************************
//
//  am_reg_base_addresses.h
//! @file am_reg_base_addresses.h
//!
//! @brief Register defines for all module base addresses
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
#ifndef AM_REG_BASE_ADDRESSES_H
#define AM_REG_BASE_ADDRESSES_H

#include "stdint.h"

//
// The following table is based on maximum memory sizing of Apollo510L,
// specific SKUs may vary.
// ****************************************************************************
// RAM and NVRAM Memory Map:
//  ITCM:                            None
//  NVM (MRAM):                      2MB        0x00400000 - 0x005FFFFF
//  MSPIx External memory:          varies      0x60000000 - 0x8FFFFFFF
//  DTCM:                            256KB      0x20000000 - 0x2003FFFF
//  System SRAM:                   1.768MB      0x20080000 - 0x2023FFFF
// ****************************************************************************

//
// DTCM address space.
//
#define DTCM_BASEADDR                           (0x20000000UL)
#define DTCM_MAX_SIZE                           (256UL * 1024UL)

//
// The Apollo5 maximum SSRAM space is 1.768MB.
//
#define SSRAM_BASEADDR                          (0x20080000)
#define SSRAM_MAX_SIZE                          (1792UL * 1024UL)

// ****************************************************************************
// MRAM Address Space
// ****************************************************************************
#define MRAM_BASEADDR                           (0x00400000UL)
#define MRAM_MAX_SIZE                           (2048UL * 1024UL)

// ****************************************************************************
// INFO base addresses
// ****************************************************************************
#define AM_REG_INFO0_BASEADDR           0x42000000
#define AM_REG_INFO1_BASEADDR           0x42002000
#define AM_REG_OTP_INFO0_BASEADDR       0x42004000
#define AM_REG_OTP_INFO1_BASEADDR       0x42006000
#define AM_REG_OTP_INFOC_BASEADDR       0x400C2000

//
// INFO sizes
//
#define AM_REG_INFO0_SIZE               2048
#define AM_REG_INFO1_SIZE               6144
#define AM_REG_INFOC_SIZE               8192
#define AM_REG_OTP_INFO0_SIZE           256
#define AM_REG_OTP_INFO1_SIZE           2816
#define AM_REG_OTP_INFOC_SIZE           1024


// ****************************************************************************
// MSPI aperture address ranges
// ****************************************************************************
//
// Please note that the END_ADDR as used here is actually the (end address + 1).
//
#define MSPI0_XIP_BASEADDR                      (0x60000000UL)
#define MSPI1_XIP_BASEADDR                      (0x80000000UL)
#define MSPI2_XIP_BASEADDR                      (0x84000000UL)

#define MSPI0_APERTURE_START_ADDR               (0x60000000UL)
#define MSPI0_APERTURE_END_ADDR                 (0x70000000UL)
#define MSPI1_APERTURE_START_ADDR               (0x80000000UL)
#define MSPI1_APERTURE_END_ADDR                 (0x84000000UL)
#define MSPI2_APERTURE_START_ADDR               (0x84000000UL)
#define MSPI2_APERTURE_END_ADDR                 (0x88000000UL)

#endif // AM_REG_BASE_ADDRESSES_H
