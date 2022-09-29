//*****************************************************************************
//
//! @file path_data.h
//!
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

#define RESX 452
#define RESY 452

#define COLOR_STOPS       5

nema_img_obj_t fb = {{0}, RESX, RESY, RESX*4, 0, NEMA_RGBA8888, 0};

nema_img_obj_t mask_a8 = {{0},  296,  154,  296    , 0, NEMA_A8, 0};
nema_img_obj_t mask_a4 = {{0},  296,  154,  296 / 2  , 0, NEMA_A4, 0};
nema_img_obj_t mask_a2 = {{0},  296,  154,  296 / 4  , 0, NEMA_A2, 0};
nema_img_obj_t mask_a1 = {{0},  296,  154,  296 / 8  , 0, NEMA_A1, 0};

nema_img_obj_t *A_masks[4];

uint8_t cmds_rectangle[] = {NEMA_VG_PRIM_MOVE, NEMA_VG_PRIM_LINE, NEMA_VG_PRIM_LINE, NEMA_VG_PRIM_LINE, NEMA_VG_PRIM_CLOSE};

float  coords_rectangle[] = {0.0f, 0.0f, 0.0f, 150.0f, 80.0f, 150.0f, 80.0f, 0.0f};

uint8_t fill_rules[] = {NEMA_VG_FILL_DRAW, NEMA_VG_FILL_EVEN_ODD, NEMA_VG_FILL_NON_ZERO};

float stops[COLOR_STOPS] = {0.0f, 0.25f, 0.50f, 0.75f, 1.0f};

color_var_t colors[COLOR_STOPS] = {{0  , 0  , 0  , 255}, //black
                                   {255, 0  , 0  , 255}, //red
                                   {0  , 255, 0  , 255}, //green
                                   {0  , 0  , 255, 255}, //blue
                                   {255, 255, 255, 255}}; //white
