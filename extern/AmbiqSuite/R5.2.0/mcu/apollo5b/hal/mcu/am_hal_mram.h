//*****************************************************************************
//
//! @file am_hal_mram.h
//!
//! @brief BootROM Helper Function Table
//!
//! @addtogroup mram4 MRAM Functionality
//! @ingroup apollo5b_hal
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_MRAM_H
#define AM_HAL_MRAM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//! @name MRAM Program keys.
//! @{
//
//*****************************************************************************
#define AM_HAL_MRAM_PROGRAM_KEY            0x12344321
#define AM_HAL_MRAM_INFO_KEY               0xD894E09E

//
// Region defines for valid_address_range() checks
//
#define AM_HAL_MRAM_MRAM_REGION     0x01
#define AM_HAL_MRAM_ITCM_REGION     0x02
#define AM_HAL_MRAM_DTCM_REGION     0x04
#define AM_HAL_MRAM_SSRAM_REGION    0x08 // Only SSRAM
#define AM_HAL_MRAM_SRAM_REGION     0x10 // All of SRAM (allows spanning if no hole between DTCM/TCM & SSRAM)


//! @}

//*****************************************************************************
//
//! @name Some helpful SRAM values and macros.
//! @{
//
//*****************************************************************************
#define AM_HAL_MRAM_SRAM_ADDR                  SSRAM_BASEADDR
#define AM_HAL_MRAM_SRAM_SIZE                  SSRAM_MAX_SIZE
#define AM_HAL_MRAM_SRAM_LARGEST_VALID_ADDR    (AM_HAL_MRAM_SRAM_ADDR + AM_HAL_MRAM_SRAM_SIZE - 1)
#define AM_HAL_MRAM_DTCM_START                 DTCM_BASEADDR
#define AM_HAL_MRAM_DTCM_SIZE                  DTCM_MAX_SIZE
#define AM_HAL_MRAM_DTCM_END                   (AM_HAL_MRAM_SRAM_ADDR + AM_HAL_MRAM_DTCM_SIZE - 1)
//! @}

//*****************************************************************************
//
//! @name Some helpful MRAM values and macros.
//! @{
//
//*****************************************************************************
#define AM_HAL_MRAM_ADDR                   MRAM_BASEADDR
#define AM_HAL_MRAM_INSTANCE_SIZE          ( 2 * 1024 * 1024 )
#define AM_HAL_MRAM_NUM_INSTANCES          2
#define AM_HAL_MRAM_TOTAL_SIZE             ( AM_HAL_MRAM_INSTANCE_SIZE * AM_HAL_MRAM_NUM_INSTANCES )
#define AM_HAL_MRAM_LARGEST_VALID_ADDR     ( AM_HAL_MRAM_ADDR + AM_HAL_MRAM_TOTAL_SIZE - 1 )
//! @}

#define AM_HAL_INFO0_SIZE_BYTES            (2 * 1024)
#define AM_HAL_INFO1_SIZE_BYTES            (6 * 1024)
#define AM_HAL_INFO1_VISIBLE_OFFSET        (0x1200)

//
// Module specific start for MRAM HAL RC's. Adding aother 0x100 offset so as to
// remove any conflicts in case it's used in conjunction with other modules.
//
#define AM_HAL_MRAM_SUCCESS                 0x0
#define AM_HAL_MRAM_ERROR                   (AM_HAL_STATUS_MODULE_SPECIFIC_START + 0x100)

#define AM_HAL_MRAM_INVLD_KEY               ( AM_HAL_MRAM_ERROR + 0x01 )            //  1  Invalid key
#define AM_HAL_MRAM_INVLD_START_MRAM_ADDR   ( AM_HAL_MRAM_ERROR + 0x02 )            //  2  pui32Dst is invalid
#define AM_HAL_MRAM_INVLD_END_MRAM_ADDR     ( AM_HAL_MRAM_ERROR + 0x03 )            //  3  NRAM addressing range would be exceeded
#define AM_HAL_MRAM_INVLD_SRC_START_ADDR    ( AM_HAL_MRAM_ERROR + 0x04 )            //  4  pui32Src invalid start address
#define AM_HAL_MRAM_INVLD_SRC_END_ADDR      ( AM_HAL_MRAM_ERROR + 0x05 )            //  5  pui32Src invalid end address
#define AM_HAL_MRAM_HW_FAIL                 ( AM_HAL_MRAM_ERROR + 0x06 )            //  6  Hardware error
#define AM_HAL_MRAM_INTERNAL7               ( AM_HAL_MRAM_ERROR + 0x07 )            //  7  Internal error
#define AM_HAL_MRAM_INTERNAL8               ( AM_HAL_MRAM_ERROR + 0x08 )            //  8  Internal error
#define AM_HAL_MRAM_INTERNAL9               ( AM_HAL_MRAM_ERROR + 0x09 )            //  9  Internal error
#define AM_HAL_MRAM_PWR_OFF                 ( AM_HAL_MRAM_ERROR + 0x0A )            // 10  Memory not powered
#define AM_HAL_MRAM_WRITE_PROT              ( AM_HAL_MRAM_ERROR + 0x0B )            // 11  NVRAM is write protected
#define AM_HAL_MRAM_READ_PROT               ( AM_HAL_MRAM_ERROR + 0x0C )            // 12  NVRAM is read protected
#define AM_HAL_MRAM_INTERNALD               ( AM_HAL_MRAM_ERROR + 0x0D )            // 13  Internal error
#define AM_HAL_MRAM_INVLD_ADDR_ALIGNMENT    ( AM_HAL_MRAM_ERROR + 0x40 )            // 64  pui32Dst not properly aligned for requested operation


//*****************************************************************************
//
//! @brief This programs up to N words of the Main MRAM
//!
//! @param ui32ProgramKey - The programming key, AM_HAL_MRAM_PROGRAM_KEY.
//! @param pui32Src - Pointer to word aligned array of data to program into
//! the MRAM.
//! @param pui32Dst - Pointer to the 4 words (16 byte) aligned MRAM location where
//! programming of the MRAM is to begin.
//! @param ui32NumWords - The number of words to be programmed. This MUST be
//! a multiple of 4 (16 byte multiple)
//!
//! This function will program multiple 4 words (16 byte) tuples in main MRAM.
//!
//! @note THIS FUNCTION ONLY OPERATES ON 16 BYTE BLOCKS OF MAIN MRAM. The pDst
//! MUST be 4 words (16 byte) aligned, and ui32NumWords MUST be multiple of 4.
//!
//! @return 0 for success, non-zero for failure.
//!     Return codes indicate:
//!     AM_HAL_MRAM_SUCCESS                     Operation was successful.
//!     AM_HAL_MRAM_INVLD_KEY                   ui32InfoKey is invalid.
//!     AM_HAL_MRAM_INVLD_START_MRAM_ADDR       ui32Offset is invalid.
//!     AM_HAL_MRAM_INVLD_END_MRAM_ADDR         Destination addressing range would be exceeded.
//!     AM_HAL_MRAM_INVLD_SRC_START_ADDR        pui32Src is invalid.
//!     AM_HAL_MRAM_INVLD_SRC_END_ADDR          Source addressing range would be exceeded.
//!     AM_HAL_MRAM_HW_FAIL                     Hardware error.
//!     AM_HAL_MRAM_INTERNAL7                   Internal error.
//!     AM_HAL_MRAM_INTERNAL8                   Internal error.
//!     AM_HAL_MRAM_INTERNAL9                   Internal error.
//!     AM_HAL_MRAM_PWR_OFF                     Memory is not powered.
//!     AM_HAL_MRAM_WRITE_PROT                  NVRAM is write protected.
//!     AM_HAL_MRAM_READ_PROT                   NVRAM is read protected.
//!     AM_HAL_MRAM_INTERNALD                   Internal error.
//!     AM_HAL_MRAM_INVLD_ADDR_ALIGNMENT        puiDst not properly aligned for requested operation
//



//*****************************************************************************
extern uint32_t am_hal_mram_main_program(uint32_t ui32ProgramKey, uint32_t *pui32Src,
                                         uint32_t *pui32Dst, uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief This Fills up to N words of the Main MRAM
//!
//! @param ui32ProgramKey - The programming key, AM_HAL_MRAM_PROGRAM_KEY.
//! @param ui32Value - 32-bit data value to fill into the MRAM
//! @param pui32Dst - Pointer to the 4 words (16 byte) aligned MRAM location where
//! programming of the MRAM is to begin.
//! @param ui32NumWords - The number of words to be programmed. This MUST be
//! a multiple of 4 (16 byte multiple)
//!
//! This function will fill multiple 16 byte tuples in main MRAM with specified pattern.
//!
//! @note THIS FUNCTION ONLY OPERATES ON 16 BYTE BLOCKS OF MAIN MRAM. The pDst
//! MUST be 4 words (16 byte) aligned, and ui32NumWords MUST be multiple of 4.
//!
//! @return 0 for success, non-zero for failure.
//!     Return codes indicate:
//!     AM_HAL_MRAM_SUCCESS                     Operation was successful.
//!     AM_HAL_MRAM_INVLD_KEY                   ui32InfoKey is invalid.
//!     AM_HAL_MRAM_INVLD_START_MRAM_ADDR       ui32Offset is invalid.
//!     AM_HAL_MRAM_INVLD_END_MRAM_ADDR         Destination addressing range would be exceeded.
//!     AM_HAL_MRAM_INVLD_SRC_START_ADDR        pui32Src is invalid.
//!     AM_HAL_MRAM_INVLD_SRC_END_ADDR          Source addressing range would be exceeded.
//!     AM_HAL_MRAM_HW_FAIL                     Hardware error.
//!     AM_HAL_MRAM_INTERNAL7                   Internal error.
//!     AM_HAL_MRAM_INTERNAL8                   Internal error.
//!     AM_HAL_MRAM_INTERNAL9                   Internal error.
//!     AM_HAL_MRAM_PWR_OFF                     Memory is not powered.
//!     AM_HAL_MRAM_WRITE_PROT                  NVRAM is write protected.
//!     AM_HAL_MRAM_READ_PROT                   NVRAM is read protected.
//!     AM_HAL_MRAM_INTERNALD                   Internal error.
//!     AM_HAL_MRAM_INVLD_ADDR_ALIGNMENT        puiDst not properly aligned for requested operation
//
//*****************************************************************************
extern uint32_t am_hal_mram_main_fill(uint32_t ui32ProgramKey, uint32_t ui32Value,
                                      uint32_t *pui32Dst, uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief This programs up to N words of the Main MRAM
//!
//! @param ui32ProgramKey - The programming key, AM_HAL_MRAM_PROGRAM_KEY.
//! @param pui32Src - Pointer to word aligned array of data to program into
//! the MRAM.
//! @param pui32Dst - Pointer to the words (4 byte) aligned MRAM location where
//! programming of the MRAM is to begin.
//! @param ui32NumWords - The number of words to be programmed.
//!
//! This function will program multiple words (4 byte) tuples in main MRAM.
//!
//! @note This function is provided only for convenience. It is most efficient
//! to operate on MRAM in groups of 4 words (16 byte) aligned multiples. Doing word
//! access is programming inefficient and should be avoided.
//!
//! @return 0 for success, non-zero for failure.
//!     Return codes indicate:
//!     AM_HAL_MRAM_SUCCESS                     Operation was successful.
//!     AM_HAL_MRAM_INVLD_KEY                   ui32InfoKey is invalid.
//!     AM_HAL_MRAM_INVLD_START_MRAM_ADDR       ui32Offset is invalid.
//!     AM_HAL_MRAM_INVLD_END_MRAM_ADDR         Destination addressing range would be exceeded.
//!     AM_HAL_MRAM_INVLD_SRC_START_ADDR        pui32Src is invalid.
//!     AM_HAL_MRAM_INVLD_SRC_END_ADDR          Source addressing range would be exceeded.
//!     AM_HAL_MRAM_HW_FAIL                     Hardware error.
//!     AM_HAL_MRAM_INTERNAL7                   Internal error.
//!     AM_HAL_MRAM_INTERNAL8                   Internal error.
//!     AM_HAL_MRAM_INTERNAL9                   Internal error.
//!     AM_HAL_MRAM_PWR_OFF                     Memory is not powered.
//!     AM_HAL_MRAM_WRITE_PROT                  NVRAM is write protected.
//!     AM_HAL_MRAM_READ_PROT                   NVRAM is read protected.
//!     AM_HAL_MRAM_INTERNALD                   Internal error.
//!     AM_HAL_MRAM_INVLD_ADDR_ALIGNMENT        puiDst not properly aligned for requested operation
//
//*****************************************************************************
extern uint32_t am_hal_mram_main_words_program(uint32_t ui32ProgramKey, uint32_t *pui32Src,
                                               uint32_t *pui32Dst, uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief Initialize MRAM for DeepSleep.
//!
//! This function implements a workaround required for Apollo4 B0 parts in
//! order to fix the MRAM DeepSleep config params.
//!
//! @return 0 for success, non-zero for failure.
//
//*****************************************************************************
extern uint32_t am_hal_mram_ds_init(void);


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_MRAM_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

