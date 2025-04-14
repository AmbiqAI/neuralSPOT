//*****************************************************************************
//
//  lv_ambiq_nema_hal.h
//! @file
//!
//! @brief Support functions for nema_hal.c, these functions
//! are called from nema_hal.c to allocate heap memories for NemaSDK.
//!
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
// This is part of revision release_sdk5p0p0-5f68a8286b of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef LV_AMBIQ_NEMA_HAL_H
#define LV_AMBIQ_NEMA_HAL_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Malloc for NemaSDK
 */
extern void* lv_ambiq_nema_hal_malloc(int pool, uint32_t size_in_byte);

/**
 * Free for NemaSDK
 */
extern void lv_ambiq_nema_hal_free(void* ptr);

#define NEMA_CUSTOM_MALLOC(pool, size) lv_ambiq_nema_hal_malloc(pool, size)
#define NEMA_CUSTOM_FREE(ptr) lv_ambiq_nema_hal_free(ptr)

lv_res_t lv_ambiq_nema_gpu_power_on(void);
lv_res_t lv_ambiq_nema_gpu_check_busy_and_suspend(void);
void lv_ambiq_nema_gpu_isr_cb(int last_cl_id);

extern void nema_build_cl_start(void);
extern void nema_build_cl_end(void);

#define NEMA_BUILDCL_START                          nema_build_cl_start();
#define NEMA_BUILDCL_END                            nema_build_cl_end();

#ifdef __cplusplus
}
#endif

#endif // LV_AMBIQ_NEMA_HAL_H