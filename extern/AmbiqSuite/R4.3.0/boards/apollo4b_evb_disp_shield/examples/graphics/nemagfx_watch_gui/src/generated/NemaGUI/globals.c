//*****************************************************************************
//
//! @file globals.c
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

//Params for window-swipe / temporary animations
#define TEMP_ANIMATIONS_COUNT 10 //up to 10 temporary animations simultaneously
int dx_git = 0;
int dy_git = 0;
int dx_win[TEMP_ANIMATIONS_COUNT] = {0};
int dy_win[TEMP_ANIMATIONS_COUNT] = {0};
event_t animation_temp[TEMP_ANIMATIONS_COUNT] = {{0}};

gitem_t *popup_gitem                    = 0;
tree_node_t *popup_node                 = 0;
int popup_off_x                         = 0;
int popup_off_y                         = 0;

uint8_t animation_timer_running         = 0;
int force_display                       = 0;
int animation_timer;
int global_off_x                        = 0;
int global_off_y                        = 0;

//animation params
int doing_transition                    = 0;
int do_popup                            = 0;
float step                              = 0.f;
uint8_t show_screen                     = 0;
nema_transition_t show_screen_effect    = NEMA_TRANS_LINEAR_H;
int going_right                         = 0;

nema_cmdlist_t cl, cl_screen;
uintptr_t cur_fb_base_phys;

#define FRAME_BUFFERS 5
bool TEX_BUFFER0_OCCUPIED = 0;
bool TEX_BUFFER1_OCCUPIED = 0;
img_obj_t fb[FRAME_BUFFERS];
nemadc_layer_t layer[FRAME_BUFFERS] = {{0}};
#define ANIMATION_DURATION 500
#define ANIMATION_STEP_0_1 ((float)TIMER_ANIM_PERIOD / ANIMATION_DURATION)

#define left(scr)  (((scr) - 1 + NUM_SCREENS) % NUM_SCREENS)
#define right(scr) (((scr) + 1) % NUM_SCREENS)

typedef struct _quad_t
{
    float x0, y0;
    float x1, y1;
    float x2, y2;
    float x3, y3;
} quad_t;
