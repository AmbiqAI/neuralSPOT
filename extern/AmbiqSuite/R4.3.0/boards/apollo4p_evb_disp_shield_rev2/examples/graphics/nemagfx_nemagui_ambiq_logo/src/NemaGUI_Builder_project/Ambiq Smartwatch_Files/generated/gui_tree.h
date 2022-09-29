//*****************************************************************************
//
// @file gui_tree.h
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

#ifndef __GUI_TREE_H__
#define __GUI_TREE_H__

#include "ng_globals.h"

#ifndef EXTERN
#define EXTERN extern
#define REMEMBER_TO_UNDEFINE_EXTERN
#endif

//GItem Declaration
//---------------------------------
EXTERN gitem_screen_t _1MainScreen;
EXTERN gitem_image_t _7MainWatchFace;
EXTERN gitem_screen_t _3AnalogWatchFaceScreen;
EXTERN gitem_watchface_t _4AnalogWatchFace;
EXTERN gitem_needle_t _20HoursHand;
EXTERN gitem_needle_t _19MinutesHand;
EXTERN gitem_needle_t _18SecondsHand;
EXTERN gitem_screen_t _5DigitalWatchFaceScreen;
EXTERN gitem_image_t _10DigitalWatchFace;
EXTERN gitem_digital_clock_t _17DigitalClock;
EXTERN gitem_screen_t _6ContactScreen;
EXTERN gitem_image_t _9ContactWatchFace;

//---------------------------------

//Tree Node Declaration
EXTERN tree_node_t node_1MainScreen;
EXTERN tree_node_t node_7MainWatchFace;
EXTERN tree_node_t node_3AnalogWatchFaceScreen;
EXTERN tree_node_t node_4AnalogWatchFace;
EXTERN tree_node_t node_20HoursHand;
EXTERN tree_node_t node_19MinutesHand;
EXTERN tree_node_t node_18SecondsHand;
EXTERN tree_node_t node_5DigitalWatchFaceScreen;
EXTERN tree_node_t node_10DigitalWatchFace;
EXTERN tree_node_t node_17DigitalClock;
EXTERN tree_node_t node_6ContactScreen;
EXTERN tree_node_t node_9ContactWatchFace;

//---------------------------------

#ifdef REMEMBER_TO_UNDEFINE_EXTERN
#undef EXTERN
#undef REMEMBER_TO_UNDEFINE_EXTERN
#endif

#endif //__GUI_TREE_H__
