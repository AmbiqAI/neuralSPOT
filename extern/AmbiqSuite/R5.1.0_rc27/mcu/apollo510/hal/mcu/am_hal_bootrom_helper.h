//*****************************************************************************
//
//! @file am_hal_bootrom_helper.h
//!
//! @brief BootROM Helper Function Table
//!
//! @addtogroup bootrom4_ap510 Bootrom Functionality
//! @ingroup apollo510_hal
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
// This is part of revision release_sdk5p1p0-acc60980d8 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_BOOTROM_HELPER_H
#define AM_HAL_BOOTROM_HELPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//  BootROM Helper Function calling documentation.
//
// ****************************************************************************

//*****************************************************************************
//
//! Structure of pointers to helper functions invoking flash operations.
//
//! The functions we are pointing to here are in the Apollo 4
//! integrated BOOTROM.
//
//*****************************************************************************
typedef struct am_hal_bootrom_helper_struct
{
    //
    //! MRAM Infospace programming function.
    //
    int  (*nv_program_info_area)(uint32_t, uint32_t *, uint32_t, uint32_t);

    //
    //! Helpful utilities.
    //
    int  (*nv_program_main2)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

    uint32_t (*bootrom_util_read_word)( uint32_t *);
    void (*bootrom_util_write_word)( uint32_t *, uint32_t);

    //
    //! Valid address range.
    //
    bool (*valid_address_range)(void *pStart, uint32_t size, uint32_t regionMask);
    //
    //! Cycle accurate delay function.
    //
    void (*bootrom_delay_cycles)(uint32_t ui32Cycles);

    //
    //! OTP Infospace programming function.
    //
    int  (*otp_program_info_area)(uint32_t, uint32_t*, uint32_t, uint32_t);

    //
    //! BootROM version info.
    //
    uint32_t (*bootrom_version_info)(void);

} am_hal_bootrom_helper_t;

//*****************************************************************************
//
//  Global data
//
//*****************************************************************************
extern const am_hal_bootrom_helper_t g_am_hal_bootrom_helper;

//*****************************************************************************
//
//! @brief Program INFO0 (customer) block NVRAM.
//!
//! @param value - Customer info key.
//! @param pSrc - Pointer to word-aligned array of data to program.
//! @param Offset - Word offset into info block (0x01 means second 32-bit word).
//! @param NumberOfWords - Number of words to program.
//!
//! @return       0 for success, non-zero for failure<br>
//!               1 Invalid Key<br>
//!               2 Invalid destination start address<br>
//!               3 Invalid destination end address<br>
//!               4 Invalid source buffer start address<br>
//!               5 Invalid source buffer end address<br>
//!               6 Hardware error<br>
//!               7 Reserved<br>
//!               8 Hardware error<br>
//!               9 OTP interrupts detected<br>
//!              10 Memory not powered on<br>
//!              11 Area is write protected<br>
//!              12 Area is copy protected<br>
//!              13 Internal error<br>
//
//*****************************************************************************
extern int nv_program_info_area(uint32_t  value,
                     uint32_t *pSrc,
                     uint32_t  Offset,
                     uint32_t  NumberOfWords);

//*****************************************************************************
//
//! @brief Program main NVRAM area.
//!
//! @param value - Programming value.
//! @param Program_nWipe - Program or wipe flag.
//! @param Addr_WipeData - Address for wipe data.
//! @param WordOffset - Word offset in the area.
//! @param NumberOfWords - Number of words to program.
//!
//! @return       0 for success, non-zero for failure<br>
//!               1 Invalid Key<br>
//!               2 Invalid destination start address<br>
//!               3 Invalid destination end address<br>
//!               4 Invalid source buffer start address<br>
//!               5 Invalid source buffer end address<br>
//!               6 Hardware error<br>
//!               7 Reserved<br>
//!               8 Hardware error<br>
//!               9 OTP interrupts detected<br>
//!              10 Memory not powered on<br>
//!              11 Area is write protected<br>
//!              12 Area is copy protected<br>
//!              13 Internal error<br>
//
//*****************************************************************************
extern int nv_program_main2(uint32_t  value,
                 uint32_t  Program_nWipe,
                 uint32_t  Addr_WipeData,
                 uint32_t  WordOffset,
                 uint32_t  NumberOfWords);


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_BOOTROM_HELPER_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

