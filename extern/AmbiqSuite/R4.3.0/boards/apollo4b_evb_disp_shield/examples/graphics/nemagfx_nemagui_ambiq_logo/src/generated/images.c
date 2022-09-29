//*****************************************************************************
//
//  images.c
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

#if (defined(SCENARIO_A) || defined(SCENARIO_B))
img_obj_t _AmbiqSmartwatch_ContactScreen_390x390_tsc4;
img_obj_t _AmbiqSmartwatch_MainScreen_390x390_tsc4;
img_obj_t _AmbiqSmartwatch_WatchFace_390x390_tsc4;
img_obj_t _HoursHand_390x390_tsc6a;
img_obj_t _MinutesHand_390x390_tsc6a;
img_obj_t _SecondsHand_390x390_tsc6a;
#include "assets/images/AmbiqSmartwatch_ContactScreen_390x390_tsc4.h"
#include "assets/images/AmbiqSmartwatch_MainScreen_390x390_tsc4.h"
#include "assets/images/AmbiqSmartwatch_WatchFace_390x390_tsc4.h"
#include "assets/images/HoursHand_390x390_tsc6a.h"
#include "assets/images/MinutesHand_390x390_tsc6a.h"
#include "assets/images/SecondsHand_390x390_tsc6a.h"

void load_image_assets(void)
{
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_ContactScreen_390x390_tsc4);
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.w = 392;
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.h = 392;
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.format = NEMA_TSC4;
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.stride = -1;
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.sampling_mode = NEMA_FILTER_BL;
    _AmbiqSmartwatch_ContactScreen_390x390_tsc4.color = 0xff000000;

    _AmbiqSmartwatch_MainScreen_390x390_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_MainScreen_390x390_tsc4);
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.w = 392;
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.h = 392;
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.format = NEMA_TSC4;
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.stride = -1;
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.sampling_mode = NEMA_FILTER_BL;
    _AmbiqSmartwatch_MainScreen_390x390_tsc4.color = 0xff000000;

    _AmbiqSmartwatch_WatchFace_390x390_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_WatchFace_390x390_tsc4);
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.w = 392;
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.h = 392;
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.format = NEMA_TSC4;
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.stride = -1;
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.sampling_mode = NEMA_FILTER_BL;
    _AmbiqSmartwatch_WatchFace_390x390_tsc4.color = 0xff000000;

    _HoursHand_390x390_tsc6a.bo = NG_LOAD_ARRAY(HoursHand_390x390_tsc6a);
    _HoursHand_390x390_tsc6a.w = 28;
    _HoursHand_390x390_tsc6a.h = 140;
    _HoursHand_390x390_tsc6a.format = NEMA_TSC6A;
    _HoursHand_390x390_tsc6a.stride = -1;
    _HoursHand_390x390_tsc6a.sampling_mode = NEMA_FILTER_BL;
    _HoursHand_390x390_tsc6a.color = 0xff000000;

    _MinutesHand_390x390_tsc6a.bo = NG_LOAD_ARRAY(MinutesHand_390x390_tsc6a);
    _MinutesHand_390x390_tsc6a.w = 28;
    _MinutesHand_390x390_tsc6a.h = 164;
    _MinutesHand_390x390_tsc6a.format = NEMA_TSC6A;
    _MinutesHand_390x390_tsc6a.stride = -1;
    _MinutesHand_390x390_tsc6a.sampling_mode = NEMA_FILTER_BL;
    _MinutesHand_390x390_tsc6a.color = 0xff000000;

    _SecondsHand_390x390_tsc6a.bo = NG_LOAD_ARRAY(SecondsHand_390x390_tsc6a);
    _SecondsHand_390x390_tsc6a.w = 40;
    _SecondsHand_390x390_tsc6a.h = 164;
    _SecondsHand_390x390_tsc6a.format = NEMA_TSC6A;
    _SecondsHand_390x390_tsc6a.stride = -1;
    _SecondsHand_390x390_tsc6a.sampling_mode = NEMA_FILTER_BL;
    _SecondsHand_390x390_tsc6a.color = 0xff000000;

}

#elif defined(SCENARIO_C)
    img_obj_t _AmbiqSmartwatch_MainScreen_tsc4;
    img_obj_t _AmbiqSmartwatch_WatchFace_tsc4;
    img_obj_t _AmbiqSmartwatch_ContactScreen_tsc4;
    img_obj_t _SecondsHand_tsc6a;
    img_obj_t _MinutesHand_tsc6a;
    img_obj_t _HoursHand_tsc6a;
    #include "assets/images/AmbiqSmartwatch_MainScreen_tsc4.h"
    #include "assets/images/AmbiqSmartwatch_WatchFace_tsc4.h"
    #include "assets/images/AmbiqSmartwatch_ContactScreen_tsc4.h"
    #include "assets/images/SecondsHand_tsc6a.h"
    #include "assets/images/MinutesHand_tsc6a.h"
    #include "assets/images/HoursHand_tsc6a.h"

    void load_image_assets(void)
    {
        _AmbiqSmartwatch_MainScreen_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_MainScreen_tsc4);
        _AmbiqSmartwatch_MainScreen_tsc4.w = 456;
        _AmbiqSmartwatch_MainScreen_tsc4.h = 456;
        _AmbiqSmartwatch_MainScreen_tsc4.format = NEMA_TSC4;
        _AmbiqSmartwatch_MainScreen_tsc4.stride = -1;
        _AmbiqSmartwatch_MainScreen_tsc4.sampling_mode = NEMA_FILTER_BL;
        _AmbiqSmartwatch_MainScreen_tsc4.color = 0xff000000;

        _AmbiqSmartwatch_WatchFace_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_WatchFace_tsc4);
        _AmbiqSmartwatch_WatchFace_tsc4.w = 456;
        _AmbiqSmartwatch_WatchFace_tsc4.h = 456;
        _AmbiqSmartwatch_WatchFace_tsc4.format = NEMA_TSC4;
        _AmbiqSmartwatch_WatchFace_tsc4.stride = -1;
        _AmbiqSmartwatch_WatchFace_tsc4.sampling_mode = NEMA_FILTER_BL;
        _AmbiqSmartwatch_WatchFace_tsc4.color = 0xff000000;

        _AmbiqSmartwatch_ContactScreen_tsc4.bo = NG_LOAD_ARRAY(AmbiqSmartwatch_ContactScreen_tsc4);
        _AmbiqSmartwatch_ContactScreen_tsc4.w = 456;
        _AmbiqSmartwatch_ContactScreen_tsc4.h = 456;
        _AmbiqSmartwatch_ContactScreen_tsc4.format = NEMA_TSC4;
        _AmbiqSmartwatch_ContactScreen_tsc4.stride = -1;
        _AmbiqSmartwatch_ContactScreen_tsc4.sampling_mode = NEMA_FILTER_BL;
        _AmbiqSmartwatch_ContactScreen_tsc4.color = 0xff000000;

        _SecondsHand_tsc6a.bo = NG_LOAD_ARRAY(SecondsHand_tsc6a);
        _SecondsHand_tsc6a.w = 44;
        _SecondsHand_tsc6a.h = 192;
        _SecondsHand_tsc6a.format = NEMA_TSC6A;
        _SecondsHand_tsc6a.stride = -1;
        _SecondsHand_tsc6a.sampling_mode = NEMA_FILTER_BL;
        _SecondsHand_tsc6a.color = 0xff000000;

        _MinutesHand_tsc6a.bo = NG_LOAD_ARRAY(MinutesHand_tsc6a);
        _MinutesHand_tsc6a.w = 32;
        _MinutesHand_tsc6a.h = 192;
        _MinutesHand_tsc6a.format = NEMA_TSC6A;
        _MinutesHand_tsc6a.stride = -1;
        _MinutesHand_tsc6a.sampling_mode = NEMA_FILTER_BL;
        _MinutesHand_tsc6a.color = 0xff000000;

        _HoursHand_tsc6a.bo = NG_LOAD_ARRAY(HoursHand_tsc6a);
        _HoursHand_tsc6a.w = 32;
        _HoursHand_tsc6a.h = 164;
        _HoursHand_tsc6a.format = NEMA_TSC6A;
        _HoursHand_tsc6a.stride = -1;
        _HoursHand_tsc6a.sampling_mode = NEMA_FILTER_BL;
        _HoursHand_tsc6a.color = 0xff000000;
    }
#endif
