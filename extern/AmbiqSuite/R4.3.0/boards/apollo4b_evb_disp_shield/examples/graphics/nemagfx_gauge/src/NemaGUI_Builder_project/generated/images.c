//*****************************************************************************
//
//! @file images.c
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

#include "nema_core.h"

#define EXTERN
#include "images.h"
#undef EXTERN

img_obj_t _speedo_needle_a8;
img_obj_t _rpm_scaled_rgba5551;
img_obj_t _fuel2_a8;
#include "assets/images/speedo_needle_a8.h"
#include "assets/images/rpm_scaled_rgba5551.h"
#include "assets/images/fuel2_a8.h"

void load_image_assets(void)
{
    _speedo_needle_a8.bo = NG_LOAD_ARRAY(speedo_needle_a8);
    _speedo_needle_a8.w = 60;
    _speedo_needle_a8.h = 20;
    _speedo_needle_a8.format = NEMA_A8;
    _speedo_needle_a8.stride = -1;
    _speedo_needle_a8.sampling_mode = NEMA_FILTER_BL;
    _speedo_needle_a8.color = 0xff0000ff;

    _rpm_scaled_rgba5551.bo = NG_LOAD_ARRAY(rpm_scaled_rgba5551);
    _rpm_scaled_rgba5551.w = 240;
    _rpm_scaled_rgba5551.h = 240;
    _rpm_scaled_rgba5551.format = NEMA_RGBA5551;
    _rpm_scaled_rgba5551.stride = -1;
    _rpm_scaled_rgba5551.sampling_mode = NEMA_FILTER_BL;
    _rpm_scaled_rgba5551.color = 0xff000000;

    _fuel2_a8.bo = NG_LOAD_ARRAY(fuel2_a8);
    _fuel2_a8.w = 79;
    _fuel2_a8.h = 74;
    _fuel2_a8.format = NEMA_A8;
    _fuel2_a8.stride = -1;
    _fuel2_a8.sampling_mode = NEMA_FILTER_BL;
    _fuel2_a8.color = 0xffffffff;

}
