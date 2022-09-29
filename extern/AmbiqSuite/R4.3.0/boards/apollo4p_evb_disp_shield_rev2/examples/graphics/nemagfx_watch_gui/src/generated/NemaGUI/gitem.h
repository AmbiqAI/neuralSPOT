//*****************************************************************************
//
//! @file gitem.h
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

#ifndef __GITEM_H__
#define __GITEM_H__

#define GITEMF_ALWAYS     (1 << 0)
#define GITEMF_PRESS      (1 << 1)
#define GITEMF_RELEASE    (1 << 2)
#define GITEMF_HOLD       (1 << 3)
#define GITEMF_DRAG       (1 << 4)

#define GITEMF_STOP_RECUR   (1 << 8)
#define GITEMF_HIDDEN       (1 << 9)
#define GITEMF_ANIMATED     (1 << 10)
#define GITEMF_TEX_RENDERED (1 << 11)
#define GITEMF_TEX_BUFFER0  (1 << 12)

typedef enum
{
    GITEM_SCREEN,             // 0
    GITEM_CONT,               // 1
    GITEM_CIRCLE,             // 2
    GITEM_RECT,               // 3
    GITEM_RRECT,              // 4
    GITEM_IMG,                // 5
    GITEM_LABEL,              // 6
    GITEM_LABEL_BUTTON,       // 7
    GITEM_CHECKBOX,           // 8
    GITEM_RADIO,              // 9
    GITEM_H_SLIDER,           //10
    GITEM_V_SLIDER,           //11
    GITEM_DIGIMETER,          //12
    GITEM_POPUP,              //13
    GITEM_ICON,               //14
    GITEM_MENU,               //15
    GITEM_PROGRESS_BAR,       //16
    GITEM_GAUGE,              //17
    GITEM_ROTATING_CONTAINER, //18
    GITEM_ICON_BUTTON,        //19
    GITEM_WINDOW,             //20
    GITEM_CIRCULAR_PROGRESS,  //21
    GITEM_WATCH_FACE          //22
} gitem_type_t;

#define true  1
#define false 0

typedef struct _gitem_t
{
    struct _gitem_t *gitem;
    struct _event_t *event;
    struct _event_t *animation;
    char     *text;

    float     val;
    float     step;
    float     min;
    float     max;
    float     min_angle;
    float     max_angle;
    float     angle;
    const int id;
    uint32_t  color;
    uint32_t  color_const;
    uint32_t  color2;
    uint32_t  color2_const;
    int       x;
    int       x_const;
    int       y;
    int       y_const;

    uint16_t w;
    uint16_t w_const;
    uint16_t h;
    uint16_t h_const;
    uint16_t r;
    uint16_t r_const;
    int16_t  img_asset_id;
    int16_t  img_asset_id2;
    int16_t  font_asset_id;
    uint16_t penwidth;
    uint16_t x_rot;
    uint16_t y_rot;
    uint16_t gesture_flags;

    const uint8_t  type;
    uint8_t  text_alignment; //caution nema_font takes uint32_t
    uint8_t  precision;
    bool     fill;
    bool     checked;
    bool     highlighted;
    bool     bg_image; //draw bg image if needed
} gitem_t;

typedef struct tree_node_t_
{
    gitem_t *this_;
    struct tree_node_t_ *parent;
    struct tree_node_t_ *first_child;
    struct tree_node_t_ *next;
} tree_node_t;

static nema_buffer_t create_bo_from_pointer(void *ptr, size_t size)
{
    nema_buffer_t bo;

#ifdef UNIFIED_MEMORY
    bo.base_virt = ptr;
    bo.base_phys = (uintptr_t)ptr;
    bo.size      = size;
#else
    bo = nema_buffer_create(size);
    (void)nema_buffer_map(&bo);
    nema_memcpy(bo.base_virt, ptr, size);
#endif

    return bo;
}

#define LOAD_ARRAY(array) create_bo_from_pointer((void *)(array), sizeof(array))

#endif //__GITEM_H__

