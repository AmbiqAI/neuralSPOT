//*****************************************************************************
//
//  lv_ambiq_misc.h
//! @file
//!
//! @brief APIs called by ambiq gpu acceleration lib.
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


#ifndef LV_AMBIQ_MISC_H
#define LV_AMBIQ_MISC_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern const char* lv_ambiq_gpu_error_interpret(uint32_t error_code);
extern lv_color_t lv_ambiq_gpu_get_chroma_key(void);
extern void lv_ambiq_cache_invalid_draw_buffer(lv_disp_t * disp);
extern void lv_ambiq_cache_clean_draw_buffer(lv_disp_t * disp);
extern void lv_ambiq_cache_invalid_and_clean_draw_buffer(lv_disp_t * disp);
extern void lv_ambiq_cache_clean_mask_buffer(const lv_opa_t* mask, const lv_area_t* area);
extern void lv_ambiq_cache_clean_map_buffer(const lv_color_t* map, const lv_area_t* area);
#ifdef __cplusplus
}
#endif

#endif // LV_AMBIQ_MISC_H


