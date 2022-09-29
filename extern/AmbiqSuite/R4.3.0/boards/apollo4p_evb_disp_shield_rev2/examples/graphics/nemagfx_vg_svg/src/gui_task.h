//*****************************************************************************
//
//! @file gui_task.h
//!
//! @brief Functions and variables related to the gui task.
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

#ifndef GUI_TASK_H
#define GUI_TASK_H

#include "nema_utils.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************

//Frame buffer size
#define FB_RESX (452U)
#define FB_RESY (452U)

//*****************************************************************************
//
// Debug pin.
//
//*****************************************************************************
#if defined(apollo4b_eb)
#define DEBUG_PIN_1    22
#define DEBUG_PIN_2    23
#define DEBUG_PIN_3    24
#define DEBUG_PIN_4    25
#define DEBUG_PIN_5    26
#define DEBUG_PIN_6    27
#define DEBUG_PIN_7    3
#elif defined(apollo4b_bga_evb_disp_shld)
#define DEBUG_PIN_1    25
#define DEBUG_PIN_2    74
#define DEBUG_PIN_3    75
#define DEBUG_PIN_4    76
#define DEBUG_PIN_5    77
#define DEBUG_PIN_6    78
#elif defined(apollo4p_bga_evb_disp_shld)
#define DEBUG_PIN_1    25
#define DEBUG_PIN_2    74
#define DEBUG_PIN_3    75
#define DEBUG_PIN_4    76
#define DEBUG_PIN_5    77
#define DEBUG_PIN_6    78
#elif defined(apollo4p_bga_sys_test)
#define DEBUG_PIN_1    74
#define DEBUG_PIN_2    75
#define DEBUG_PIN_3    76
#define DEBUG_PIN_4    77
#define DEBUG_PIN_5    78
#define DEBUG_PIN_6    79
#define DEBUG_PIN_7    80
#else
#define DEBUG_PIN_1    22
#define DEBUG_PIN_2    23
#define DEBUG_PIN_3    24
#define DEBUG_PIN_4    25
#define DEBUG_PIN_5    26
#define DEBUG_PIN_6    27
#define DEBUG_PIN_7    3
#endif

//*****************************************************************************
//
// Gui task handle.
//
//*****************************************************************************
extern TaskHandle_t GuiTaskHandle;

extern SemaphoreHandle_t g_semDCStart;
extern SemaphoreHandle_t g_semGPUStart;
extern SemaphoreHandle_t g_semDCEnd;
extern SemaphoreHandle_t g_semGPUEnd;

//Pointer to frame buffer
extern img_obj_t* g_pFrameBufferGPU;
extern img_obj_t* g_pFrameBufferDC;

//Texture
extern void* g_pSvgBin;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void GuiTask(void *pvParameters);

#endif // GUI_TASK_H
