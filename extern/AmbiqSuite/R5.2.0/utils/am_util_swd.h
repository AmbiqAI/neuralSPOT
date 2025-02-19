//*****************************************************************************
//
//! @file am_util_swd.h
//!
//! @brief Bit-bang SWD utility.
//!
//! This file provides the interface for bit-bang SWD functions.
//!
//! @addtogroup swd SWD Utility
//! @ingroup utils
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

#ifndef AM_UTIL_SWD_H
#define AM_UTIL_SWD_H

#if defined(AM_PART_APOLLO3_API) || defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
#else
#error "Error: am_util_swd.h only supports Apollo3 and Apollo4, Apollo5."
#endif

//*****************************************************************************
//
//! SWD utitlity configuration options.
//
//*****************************************************************************
#define APOLLO4_COOPER_INTERNAL             1

//*****************************************************************************
//
//! @name Pin Configuration.
//! @{
//
//*****************************************************************************
#if defined(AM_PART_APOLLO3_API)
#define AM_UTIL_SWD_SWDIO_PIN               6
#define AM_UTIL_SWD_SWCLK_PIN               5
#elif defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
#if APOLLO4_COOPER_INTERNAL
#define AM_UTIL_SWD_SWDIO_PIN               97
#define AM_UTIL_SWD_SWCLK_PIN               98
#else
#define AM_UTIL_SWD_SWDIO_PIN               71
#define AM_UTIL_SWD_SWCLK_PIN               70
#endif // APOLLO4_COOPER_INTERNAL
#endif // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API

#if defined(AM_PART_APOLLO3_API)
#define AM_UTIL_SWD_OUTPUT_CFG              g_AM_HAL_GPIO_OUTPUT
#define AM_UTIL_SWD_INPUT_CFG               g_AM_HAL_GPIO_INPUT
#elif defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
#define AM_UTIL_SWD_OUTPUT_CFG              AM_HAL_GPIO_PINCFG_OUTPUT
#define AM_UTIL_SWD_INPUT_CFG               AM_HAL_GPIO_PINCFG_INPUT
#else
#error
#endif
//! @}

//*****************************************************************************
//
//! @name Memory Pools
//! @{
//
//*****************************************************************************
#define AM_UTIL_SWD_VARIABLE_POOL_SIZE      512
#define AM_UTIL_SWD_FIXED_BUFFER_SIZE       20
#define AM_UTIL_SWD_FIXED_BUFFER_COUNT      1024
//! @}

//*****************************************************************************
//
//! @name LibSWD configuration
//! @{
//
//*****************************************************************************
#define AM_UTIL_SWD_WRITE_BLOCK_SIZE        128
#define AM_UTIL_SWD_READ_BLOCK_SIZE         32
//! @}

//*****************************************************************************
//
// Debug features.
//
//*****************************************************************************

//*****************************************************************************
//
//! Enable while (1) on internal errors.
//
//*****************************************************************************
#define AM_UTIL_SWD_HALT_ON_ERROR           1

//
// Enable trace pins.
//

//*****************************************************************************
//
//! Core registers
//
//*****************************************************************************
typedef enum
{
    AM_UTIL_SWD_CORE_R0   = 0,
    AM_UTIL_SWD_CORE_R1   = 1,
    AM_UTIL_SWD_CORE_R2   = 2,
    AM_UTIL_SWD_CORE_R3   = 3,
    AM_UTIL_SWD_CORE_R4   = 4,
    AM_UTIL_SWD_CORE_R5   = 5,
    AM_UTIL_SWD_CORE_R6   = 6,
    AM_UTIL_SWD_CORE_R7   = 7,
    AM_UTIL_SWD_CORE_R8   = 8,
    AM_UTIL_SWD_CORE_R9   = 9,
    AM_UTIL_SWD_CORE_R10  = 10,
    AM_UTIL_SWD_CORE_R11  = 11,
    AM_UTIL_SWD_CORE_R12  = 12,
    AM_UTIL_SWD_CORE_R13  = 13,
    AM_UTIL_SWD_CORE_R14  = 14,
    AM_UTIL_SWD_CORE_R15  = 15,
    AM_UTIL_SWD_CORE_XPSR = 16,
    AM_UTIL_SWD_CORE_MSP  = 17,
    AM_UTIL_SWD_CORE_PSP  = 18,
}
am_util_swd_core_reg_t;

//*****************************************************************************
//
//! @name Core register Macros
//! @{
//
//*****************************************************************************
#define AM_UTIL_SWD_CORE_SP     AM_UTIL_SWD_CORE_R13
#define AM_UTIL_SWD_CORE_LR     AM_UTIL_SWD_CORE_R14
#define AM_UTIL_SWD_CORE_PC     AM_UTIL_SWD_CORE_R15
//! @}

//*****************************************************************************
//
//! @brief Initialize the SWD interface.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_initialize(void);

//*****************************************************************************
//
//! @brief Deinitialize the SWD interface.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_deinitialize(void);

//*****************************************************************************
//
//! @brief Deinitialize the SWD interface.
//!
//! @return ui32ErrorCode from libswd_debug_init
//
//*****************************************************************************
extern uint32_t am_util_swd_connect(void);

//*****************************************************************************
//
//! @brief Halt the SWD interface.
//!
//! @return ui32ErrorCode from libswd_debug_halt
//
//*****************************************************************************
extern uint32_t am_util_swd_halt(void);

//*****************************************************************************
//
//! @brief Check if the SWD interface is halted
//!
//! @return bHalted from libswd_debug_is_halted
//
//*****************************************************************************
extern bool am_util_swd_halt_check(void);

//*****************************************************************************
//
//! @brief Let the processor resume running
//!
//! @return ui32ErrorCode from libswd_debug_run
//
//*****************************************************************************
extern uint32_t am_util_swd_run(void);

//*****************************************************************************
//
//! @brief Initialize the SWD interface.
//!
//! @param ui32Address
//! @param *pui8Data
//! @param ui32NumWords
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_mem_write(uint32_t ui32Address, uint8_t *pui8Data,
                                      uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief Read from memory with 32-bit accesses
//!
//! @param ui32Address
//! @param *pui8Data
//! @param ui32NumWords
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_mem_read(uint32_t ui32Address, uint8_t *pui8Data,
                                     uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief Write to memory with 32-bit accesses
//!
//! @param ui32Address
//! @param *pui8Data
//! @param ui32NumWords
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_mem_write_words(uint32_t ui32Address,
                                            uint32_t *pui32Data,
                                            uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief Read from memory with 32-bit accesses
//!
//! @param ui32Address
//! @param *pui8Data
//! @param ui32NumWords
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_swd_mem_read_words(uint32_t ui32Address,
                                           uint32_t *pui32Data,
                                           uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief Write a core register
//!
//! @param eCoreReg register to read
//! @param ui32Value value to be used by libswd_memap_write_int_32
//!
//! @return iResults from libswd_memap_write_int_csw or libswd_memap_write_int
//
//*****************************************************************************
extern uint32_t am_util_swd_reg_write(am_util_swd_core_reg_t eCoreReg,
                                      uint32_t ui32Value);

//*****************************************************************************
//
//! @brief Read a core register
//!
//! @param eCoreReg register to read
//! @param *ui32Value pointer to value retunred from libswd_memap_read_int_32
//!
//! @return iResult from libswd_memap_write_int_32 or libswd_memap_read_int_32
//
//*****************************************************************************
extern uint32_t am_util_swd_reg_read(am_util_swd_core_reg_t eCoreReg,
                                     uint32_t *ui32Value);

extern void am_util_swd_print_malloc_stats(void);

#endif // AM_UTIL_SWD_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

