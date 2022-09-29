//*****************************************************************************
//
//! @file load_image_assets.c
//!
//! @brief NemaGFX example.
//!
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

img_obj_t _DigitalClockBackground_l8;
img_obj_t _DigitalClockBackground4_l8;
img_obj_t _poi_second_white_rgba;
img_obj_t _poi_min_white_rgba;
img_obj_t _poi_hour_white_rgba;
img_obj_t _clock_sporty2_bg_03_a8;

img_obj_t *image_assets[6];

#include "assets/images/DigitalClockBackground_l8.h"
#include "assets/images/DigitalClockBackground4_l8.h"
#include "assets/images/poi_second_white_rgba.h"
#include "assets/images/poi_min_white_rgba.h"
#include "assets/images/poi_hour_white_rgba.h"
#include "assets/images/clock_sporty2_bg_03_a8.h"

void load_image_assets(void)
{
    _DigitalClockBackground_l8.bo = LOAD_ARRAY(DigitalClockBackground_l8);
    _DigitalClockBackground_l8.w = 320;
    _DigitalClockBackground_l8.h = 320;
    _DigitalClockBackground_l8.format = NEMA_L8;
    _DigitalClockBackground_l8.stride = -1;
    _DigitalClockBackground_l8.sampling_mode = 1;
    _DigitalClockBackground_l8.color = 0xff333333;

    image_assets[0] = &_DigitalClockBackground_l8;

    _DigitalClockBackground4_l8.bo = LOAD_ARRAY(DigitalClockBackground4_l8);
    _DigitalClockBackground4_l8.w = 320;
    _DigitalClockBackground4_l8.h = 320;
    _DigitalClockBackground4_l8.format = NEMA_L8;
    _DigitalClockBackground4_l8.stride = -1;
    _DigitalClockBackground4_l8.sampling_mode = 1;
    _DigitalClockBackground4_l8.color = 0xff000000;

    image_assets[1] = &_DigitalClockBackground4_l8;

    _poi_second_white_rgba.bo = LOAD_ARRAY(poi_second_white_rgba);
    _poi_second_white_rgba.w = 30;
    _poi_second_white_rgba.h = 258;
    _poi_second_white_rgba.format = NEMA_RGBA8888;
    _poi_second_white_rgba.stride = -1;
    _poi_second_white_rgba.sampling_mode = 1;
    _poi_second_white_rgba.color = 0xff000000;

    image_assets[2] = &_poi_second_white_rgba;

    _poi_min_white_rgba.bo = LOAD_ARRAY(poi_min_white_rgba);
    _poi_min_white_rgba.w = 20;
    _poi_min_white_rgba.h = 240;
    _poi_min_white_rgba.format = NEMA_RGBA8888;
    _poi_min_white_rgba.stride = -1;
    _poi_min_white_rgba.sampling_mode = 1;
    _poi_min_white_rgba.color = 0xff000000;

    image_assets[3] = &_poi_min_white_rgba;

    _poi_hour_white_rgba.bo = LOAD_ARRAY(poi_hour_white_rgba);
    _poi_hour_white_rgba.w = 20;
    _poi_hour_white_rgba.h = 240;
    _poi_hour_white_rgba.format = NEMA_RGBA8888;
    _poi_hour_white_rgba.stride = -1;
    _poi_hour_white_rgba.sampling_mode = 1;
    _poi_hour_white_rgba.color = 0xff000000;

    image_assets[4] = &_poi_hour_white_rgba;

    _clock_sporty2_bg_03_a8.bo = LOAD_ARRAY(clock_sporty2_bg_03_a8);
    _clock_sporty2_bg_03_a8.w = 320;
    _clock_sporty2_bg_03_a8.h = 320;
    _clock_sporty2_bg_03_a8.format = NEMA_A8;
    _clock_sporty2_bg_03_a8.stride = -1;
    _clock_sporty2_bg_03_a8.sampling_mode = 1;
    _clock_sporty2_bg_03_a8.color = 0xffffffff;

    image_assets[5] = &_clock_sporty2_bg_03_a8;

}

