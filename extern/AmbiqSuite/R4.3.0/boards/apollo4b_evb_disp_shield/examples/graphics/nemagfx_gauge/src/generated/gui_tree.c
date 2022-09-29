//*****************************************************************************
//
//! @file gui_tree.c
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

#undef EXTERN
#include "gui_tree.h"
#undef EXTERN

#include "ng_globals.h"
#include "fonts.h"
#include "images.h"

gitem_screen_t _1149Gauge = {EVENT(NULL), ng_screen_draw, GESTURES(NULL), FLAGS(0x00000036U), X(0), Y(0), W(456), H(456), ID(1149), GITEM_MAIN_SCREEN , COLOR(0xff000000U) , IMAGE(NULL)};
    gitem_gauge_t _1244Gauge1 = {EVENT(NULL), ng_gauge_draw, GESTURES(&gestures_gauge), FLAGS(0x00000040U), X(0), Y(0), W(455), H(455), ID(1244), GITEM_GAUGE , COLOR(0xff000000U) , IMAGE(&_rpm_scaled_rgba5551), NG_GITEM(&_1245Needle), CUR_VAL(0.000f), MAX_VAL(8.000000f), MIN_VAL(0.000000f), ANGLE(-90.000f), MAX_ANGLE(166.000f), MIN_ANGLE(-90.000f), X_ROT(227), Y_ROT(227), PEN_WIDTH(1)};
        gitem_image_t _1250Image1 = {EVENT(NULL), ng_image_draw, GESTURES(NULL), FLAGS(0x00000040U), X(194), Y(177), W(79), H(74), ID(1250), GITEM_IMAGE , COLOR(0xff808080U) , IMAGE(&_fuel2_a8)};
        gitem_digimeter_t _1251Digital_Meter2 = {EVENT(NULL), ng_digimeter_draw, GESTURES(NULL), FLAGS(0x00000080U), X(212), Y(256), W(92), H(29), ID(1251), GITEM_DIGITAL_METER , COLOR(0xffffffffU), TEXT_COLOR(0xffffffffU), CUR_VAL(0.000f), MAX_VAL(30.000000f), MIN_VAL(0.000000f), STEP(1.000000f), &arial14pt8b, NEMA_ALIGNX_LEFT | NEMA_ALIGNY_CENTER, DEC_PRECISION(1), INT_PRECISION(0), SUFFIX("")};
        char *_1253Label1_texts[1] = {"l/100 km"};
        nema_font_t *_1253Label1_fonts[1] = {&arial8pt8b};
        attr_text_t _1253Label1_text = {0, _1253Label1_fonts, _1253Label1_texts, NEMA_ALIGNX_CENTER | NEMA_ALIGNY_CENTER};
        gitem_label_t _1253Label1 = {EVENT(NULL), ng_label_draw, GESTURES(NULL), FLAGS(0x00000080U), X(190), Y(286), W(85), H(23), ID(1253), GITEM_LABEL , COLOR(0xffffffffU) , &_1253Label1_text, COLOR(0xffffffffU)};
        gitem_needle_t _1245Needle = {EVENT(NULL), ng_needle_draw, GESTURES(NULL), FLAGS(0x00000040U), X(227), Y(227), W(116), H(29), ID(1245), GITEM_NEEDLE , COLOR(0xff0000ffU) , IMAGE(&_speedo_needle_a8), ANGLE(-90.000f), X_ROT(190), Y_ROT(14), PEN_WIDTH(1)};


//---------------------------------


//Build tree nodes
//---------------------------------
tree_node_t node_1149Gauge = { NG_GITEM(&_1149Gauge), PARENT_NODE(NULL), FIRST_CHILD_NODE(&node_1244Gauge1), NEXT_NODE(NULL)};
    tree_node_t node_1244Gauge1 = { NG_GITEM(&_1244Gauge1), PARENT_NODE(&node_1149Gauge), FIRST_CHILD_NODE(&node_1250Image1), NEXT_NODE(NULL)};
        tree_node_t node_1250Image1 = { NG_GITEM(&_1250Image1), PARENT_NODE(&node_1244Gauge1), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_1251Digital_Meter2)};
        tree_node_t node_1251Digital_Meter2 = { NG_GITEM(&_1251Digital_Meter2), PARENT_NODE(&node_1244Gauge1), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_1253Label1)};
        tree_node_t node_1253Label1 = { NG_GITEM(&_1253Label1), PARENT_NODE(&node_1244Gauge1), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_1245Needle)};
        tree_node_t node_1245Needle = { NG_GITEM(&_1245Needle), PARENT_NODE(&node_1244Gauge1), FIRST_CHILD_NODE(NULL), NEXT_NODE(NULL)};
