//*****************************************************************************
//
//  lv_ambiq_font_align.h
//! @file
//!
//! @brief Ambiq font realignment.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************


#ifndef LV_AMBIQ_FONT_ALIGN_H
#define LV_AMBIQ_FONT_ALIGN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @brief Get the font bitmap length after alignment.
//!
//! @param font - Font to be align.
//!
//! @return the bitmap length after alignment.
//
//*****************************************************************************
extern uint32_t lv_ambiq_bitmap_length_get(const lv_font_fmt_txt_dsc_t* font);

//*****************************************************************************
//
//! @brief Align the A4 format font bitmap
//!
//! @param font - Font to be align.
//! @param new_bitmap_addr - a buffer to hold the aligned bitmap, 
//!                         call lv_ambiq_bitmap_length_get to get the its length .
//!
//! @note This function will modify the glyph_bitmap pointer to the input new_bitmap_addr.
//!
//! @return None.
//
//*****************************************************************************
extern void lv_ambiq_A4_font_align(lv_font_fmt_txt_dsc_t* font, uint8_t* new_bitmap_addr);

#ifdef __cplusplus
}
#endif

#endif // LV_AMBIQ_FONT_ALIGN_H