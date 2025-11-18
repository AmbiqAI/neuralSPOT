//*****************************************************************************
//
//! @file ios_burst.h
//!
//! @brief Example used for demonstrating the ios burst mode.
//!
//! @addtogroup interface_examples Interface Examples
//!
//! @defgroup ios_burst IOM Burst Host Example
//! @ingroup interfaces_examples
//! @{
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

#define IOS_MODE_CNTLR_WRITE    1
#define IOS_MODE_CNTLR_READ     2

#define AM_HAL_IOS_INT_ERR      (AM_HAL_IOS_INT_FOVFL | AM_HAL_IOS_INT_FUNDFL | AM_HAL_IOS_INT_FRDERR)
#define AM_HAL_IOS_XCMP_INT     (AM_HAL_IOS_INT_XCMPWR | AM_HAL_IOS_INT_XCMPWF | AM_HAL_IOS_INT_XCMPRR | AM_HAL_IOS_INT_XCMPRF)

#define SENSOR0_DATA_SIZE       896

#define SENSOR0_FREQ            120 // 12 times a second

#define IOM_IOS_CHECKSUM        1

#define IOS_WRAP_START_ADDRESS          0x00

#define IOS_DBG_MSG                     0

#define IOS_REGACC_THRESHOLD_01         AM_HAL_IOS_ACCESS_INT_08    // 0x8 at wrap mode
#define IOS_REGACC_THRESHOLD_02         AM_HAL_IOS_ACCESS_INT_1F    // 0x1F at wrap mode
#define IOS_WRAP_HALF_ADDRESS           0x20

//
// Apollo5 LRAM is defined as device memory which should be access aligned
// so we re-define here to replace that in lib
//
void ios_memcpy(void* dst, const void* src, size_t size);
inline void ios_memcpy(void* dst, const void* src, size_t size)
{
    for ( uint16_t cpy_index = 0; cpy_index < (size); cpy_index++ )
    {
        *((uint8_t*)dst + cpy_index) = *((uint8_t*)src + cpy_index);
    }
}

void ios_memset(void * dst, uint8_t val, size_t size);
inline void ios_memset(void * dst, uint8_t val, size_t size)
{
    for ( uint16_t cpy_index = 0; cpy_index < (size); cpy_index++ )
    {
        *((uint8_t*)dst + cpy_index) = val;
    }
}

