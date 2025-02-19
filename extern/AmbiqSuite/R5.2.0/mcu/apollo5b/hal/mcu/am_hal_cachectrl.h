// ****************************************************************************
//
//! @file am_hal_cachectrl.h
//!
//! @brief Functions for interfacing with the CACHE controller.
//!
//! @addtogroup cachectrl4 CACHE - Cache Control
//! @ingroup apollo5b_hal
//! @{
//
// ****************************************************************************

// ****************************************************************************
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
// ****************************************************************************
#ifndef AM_HAL_CACHECTRL_H
#define AM_HAL_CACHECTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

//! Cache invalidate range.
typedef struct
{
    uint32_t    ui32StartAddr;      // Starting address of block to invalidate.
    uint32_t    ui32Size;           // Size of block to invalidate in bytes.
} am_hal_cachectrl_range_t;

//! Prefetch configuration.
typedef struct
{
    uint8_t ui8MaxOutTransactions;  // Maximum Outstanding Transactions on AXI Bus.
    uint8_t ui8MaxLookAhead;        // Maximum Look Ahead.
    uint8_t ui8MinLookAhead;        // Minimum Look Ahead.
} am_hal_cachectrl_prefetch_t;

//
//! CACHECTRL Request
//
typedef enum
{
  //! Used to change the the CM55 pre-fetch configuration.
  //! Pass am_hal_cachectrl_prefetch_t* as pArgs
  AM_HAL_CACHECTRL_REQ_PREFETCH_CFG = 0,
  AM_HAL_CACHECTRL_REQ_MAX
} am_hal_cachectrl_req_e;

// ****************************************************************************
//
// Function prototypes
//
// ****************************************************************************
// ****************************************************************************
//
//! @brief Enable the Instruction cache for operation.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_icache_enable(void);

// ****************************************************************************
//
//! @brief Disable the Instruction cache.
//!
//! Use this function to disable icache.  Other configuration settings are not
//! not required.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_icache_disable(void);

// ****************************************************************************
//
//! @brief Enable the Data cache for operation.
//!
//! @param bClean - TRUE: Clean the dcache in addition to invalidating it.
//!                 FALSE: Simply invalidate the cache.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_dcache_enable(bool bClean);

// ****************************************************************************
//
//! @brief Disable the Data cache.
//!
//! Use this function to disable dcache.  Other configuration settings are not
//! not required.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_dcache_disable(void);

// ****************************************************************************
//
//! @brief Invalidate the Insruction cache.
//!
//! @param pRange - pointer to an address range to invalidate.  If NULL, then
//!                 invalidate the entire cache.
//!
//! Use this function to invalidate the icache.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_icache_invalidate(am_hal_cachectrl_range_t *pRange);

// ****************************************************************************
//
//! @brief Invalidate the Data cache.
//!
//! @param pRange - pointer to an address range to invalidate.  If NULL, then
//!                 invalidate the entire cache.
//! @param bClean - TRUE: Clean the dcache in addition to invalidating it.
//!                 FALSE: Simply invalidate the cache.
//!
//! Use this function to invalidate the dcache.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_dcache_invalidate(am_hal_cachectrl_range_t *pRange,
                                                   bool bClean);

//*****************************************************************************
//
//! @brief CACHECTRL control function
//!
//! @param eReq         - device specific special request code.
//! @param pArgs        - Pointer to arguments for Control Switch Case
//!
//! This function allows advanced settings
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_cachectrl_control(am_hal_cachectrl_req_e eReq, void *pArgs);

// ****************************************************************************
//
//! @brief Clean the Data cache.
//!
//! @param pRange - pointer to an address range to clean. If NULL, then
//!                 clean the entire cache.
//!
//! Use this function to clean the dcache.
//!
//! @return Status.
//
// ****************************************************************************
extern uint32_t am_hal_cachectrl_dcache_clean(am_hal_cachectrl_range_t *pRange);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CACHECTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

