//*****************************************************************************
//
//! @file gitem_tree.c
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

#define RESX        320
#define RESY        320
#define NUM_GROUPS  1
#define NUM_POPUPS  0
#define TIMER_ANIM_PERIOD  16

int NUM_SCREENS[NUM_GROUPS] = {2};
int cur_screen_group = 0;
int cur_screen_node  = 0;

//Forward declaration of gitems and tree nodes
//---------------------------------
tree_node_t node_1Home_Screen_0;
gitem_t _1Home_Screen_0;
    tree_node_t node_6Analog_Clock_Face;
    gitem_t _6Analog_Clock_Face;
        tree_node_t node_8Minutes;
        gitem_t _8Minutes;
        tree_node_t node_7Hours;
        gitem_t _7Hours;
        tree_node_t node_9Seconds;
        gitem_t _9Seconds;
        tree_node_t node_320Label_Button1;
        gitem_t _320Label_Button1;
            tree_node_t node_321Label1;
            gitem_t _321Label1;
        tree_node_t node_295Main_Menu_Button_;
        gitem_t _295Main_Menu_Button_;
            tree_node_t node_296Label;
            gitem_t _296Label;
tree_node_t node_192Home_Screen_1;
gitem_t _192Home_Screen_1;
    tree_node_t node_194Seconds;
    gitem_t _194Seconds;
    tree_node_t node_324Minutes_1st_Digit;
    gitem_t _324Minutes_1st_Digit;
    tree_node_t node_325Minutes_2nd_Digit;
    gitem_t _325Minutes_2nd_Digit;
    tree_node_t node_329Separator;
    gitem_t _329Separator;
    tree_node_t node_326Hours_1st_Digit;
    gitem_t _326Hours_1st_Digit;
    tree_node_t node_327Hours_2nd_Digit;
    gitem_t _327Hours_2nd_Digit;
    tree_node_t node_297Main_Menu_Button_;
    gitem_t _297Main_Menu_Button_;
        tree_node_t node_298Label;
        gitem_t _298Label;
    tree_node_t node_336Year;
    gitem_t _336Year;
    tree_node_t node_333Month;
    gitem_t _333Month;
    tree_node_t node_337Day_Number;
    gitem_t _337Day_Number;
    tree_node_t node_332Comma_Separator;
    gitem_t _332Comma_Separator;
    tree_node_t node_196Day;
    gitem_t _196Day;

//---------------------------------

gitem_t _1Home_Screen_0 = {0, 0, 0, 0, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 1, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    gitem_t _6Analog_Clock_Face = {0, 0, 0, 0, 0.000, 0.000, 0.000, 24.000, 0.000, 360.000, 0.000, 6, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, 5, -1, -1, 1, 160, 160, 0, 22, 0, 0, 0, 0, 0, 1 };
        gitem_t _8Minutes = {0, 0, 0, 0, 0.000, 0.000, 0.000, 100.000, 0.000, 360.000, 30.000, 8, 0xff323232, 0xff323232, 0xff000000, 0xff000000, 160, 160, 160, 160, 20, 20, 240, 240, 0, 0, 3, -1, -1, 1, 10, 120, 0, 18, 0, 0, 0, 0, 0, 1 };
        gitem_t _7Hours = {0, 0, 0, 0, 0.000, 0.000, 0.000, 100.000, 0.000, 360.000, 150.000, 7, 0xff323232, 0xff323232, 0xff000000, 0xff000000, 160, 160, 160, 160, 20, 20, 240, 240, 0, 0, 4, -1, -1, 1, 10, 120, 0, 18, 0, 0, 0, 0, 0, 1 };
        gitem_t _9Seconds = {0, 0, 0, 0, 0.000, 0.000, 0.000, 100.000, 0.000, 360.000, 90.000, 9, 0xff323232, 0xff323232, 0xff000000, 0xff000000, 160, 160, 160, 160, 30, 30, 258, 258, 0, 0, 2, -1, -1, 1, 15, 128, 0, 18, 0, 0, 0, 0, 0, 1 };
        gitem_t _320Label_Button1 = {0, 0, 0, 0, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 320, 0xff000000, 0xff000000, 0xff646400, 0xff646400, 220, 220, 0, 0, 100, 100, 30, 30, 0, 0, -1, -1, 0, 1, 0, 0, 262, 7, 0, 0, 1, 104, 0, 0 };
            char text_321Label1[] = "Begin Simul";
            gitem_t _321Label1 = {0, 0, 0, text_321Label1, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 321, 0xff969600, 0xff969600, 0xff000000, 0xff000000, 0, 0, 0, 0, 100, 100, 30, 30, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
        gitem_t _295Main_Menu_Button_ = {0, 0, 0, 0, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 295, 0x00969600, 0x00969600, 0xff646400, 0xff646400, 0, 0, 34, 34, 319, 319, 285, 285, 0, 0, -1, -1, 0, 1, 0, 0, 262, 7, 0, 0, 0, 104, 0, 0 };
            char text_296Label[] = "";
            gitem_t _296Label = {0, 0, 0, text_296Label, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 296, 0xff969600, 0xff969600, 0xffffffff, 0xffffffff, 0, 0, 0, 0, 319, 319, 285, 285, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
gitem_t _192Home_Screen_1 = {0, 0, 0, 0, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 192, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    gitem_t _194Seconds = {0, 0, 0, 0, 0.000, 0.000, 0.000, 100.000, -270.000, 90.000, -270.000, 194, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, 0, 1, -1, 1, 0, 0, 0, 21, 0, 0, 0, 0, 0, 1 };
    gitem_t _324Minutes_1st_Digit = {0, 0, 0, 0, 0.000, 1.000, 0.000, 9.000, 0.000, 0.000, 0.000, 324, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 215, 215, 100, 100, 52, 52, 108, 108, 0, 0, -1, -1, 2, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    gitem_t _325Minutes_2nd_Digit = {0, 0, 0, 0, 0.000, 1.000, 0.000, 9.000, 0.000, 0.000, 0.000, 325, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 165, 165, 100, 100, 52, 52, 108, 108, 0, 0, -1, -1, 2, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    char text_329Separator[] = ":";
    gitem_t _329Separator = {0, 0, 0, text_329Separator, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 329, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 145, 145, 99, 99, 23, 23, 108, 108, 0, 0, -1, -1, 2, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
    gitem_t _326Hours_1st_Digit = {0, 0, 0, 0, 0.000, 1.000, 0.000, 9.000, 0.000, 0.000, 0.000, 326, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 95, 95, 100, 100, 52, 52, 108, 108, 0, 0, -1, -1, 2, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    gitem_t _327Hours_2nd_Digit = {0, 0, 0, 0, 0.000, 1.000, 0.000, 9.000, 0.000, 0.000, 0.000, 327, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 45, 45, 100, 100, 52, 52, 108, 108, 0, 0, -1, -1, 2, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    gitem_t _297Main_Menu_Button_ = {0, 0, 0, 0, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 297, 0x00969600, 0x00969600, 0xff646400, 0xff646400, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, -1, -1, 0, 1, 0, 0, 262, 7, 0, 0, 0, 0, 0, 0 };
        char text_298Label[] = "";
        gitem_t _298Label = {0, 0, 0, text_298Label, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 298, 0xff969600, 0xff969600, 0xffffffff, 0xffffffff, 0, 0, 0, 0, 320, 320, 320, 320, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
    gitem_t _336Year = {0, 0, 0, 0, 2018.000, 1.000, 0.000, 9999.000, 0.000, 0.000, 0.000, 336, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 189, 189, 200, 200, 47, 47, 25, 25, 0, 0, -1, -1, 0, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    char text_333Month[] = "Sep";
    gitem_t _333Month = {0, 0, 0, text_333Month, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 333, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 149, 149, 200, 200, 40, 40, 25, 25, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
    gitem_t _337Day_Number = {0, 0, 0, 0, 31.000, 1.000, 0.000, 31.000, 0.000, 0.000, 0.000, 337, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 130, 130, 200, 200, 20, 20, 25, 25, 0, 0, -1, -1, 0, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 };
    char text_332Comma_Separator[] = ",";
    gitem_t _332Comma_Separator = {0, 0, 0, text_332Comma_Separator, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 332, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 117, 117, 200, 200, 15, 15, 25, 25, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };
    char text_196Day[] = "Tue";
    gitem_t _196Day = {0, 0, 0, text_196Day, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 196, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 83, 83, 200, 200, 40, 40, 25, 25, 0, 0, -1, -1, 0, 1, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0 };

//---------------------------------

//Build tree nodes
//---------------------------------
tree_node_t node_1Home_Screen_0 = { &_1Home_Screen_0, 0, &node_6Analog_Clock_Face, 0};
    tree_node_t node_6Analog_Clock_Face = { &_6Analog_Clock_Face, &node_1Home_Screen_0, &node_8Minutes, 0};
        tree_node_t node_8Minutes = { &_8Minutes, &node_6Analog_Clock_Face, 0, &node_7Hours};
        tree_node_t node_7Hours = { &_7Hours, &node_6Analog_Clock_Face, 0, &node_9Seconds};
        tree_node_t node_9Seconds = { &_9Seconds, &node_6Analog_Clock_Face, 0, &node_320Label_Button1};
        tree_node_t node_320Label_Button1 = { &_320Label_Button1, &node_6Analog_Clock_Face, &node_321Label1, &node_295Main_Menu_Button_};
            tree_node_t node_321Label1 = { &_321Label1, &node_320Label_Button1, 0, 0};
        tree_node_t node_295Main_Menu_Button_ = { &_295Main_Menu_Button_, &node_6Analog_Clock_Face, &node_296Label, 0};
            tree_node_t node_296Label = { &_296Label, &node_295Main_Menu_Button_, 0, 0};

//Build tree nodes
//---------------------------------
tree_node_t node_192Home_Screen_1 = { &_192Home_Screen_1, 0, &node_194Seconds, 0};
    tree_node_t node_194Seconds = { &_194Seconds, &node_192Home_Screen_1, 0, &node_324Minutes_1st_Digit};
    tree_node_t node_324Minutes_1st_Digit = { &_324Minutes_1st_Digit, &node_192Home_Screen_1, 0, &node_325Minutes_2nd_Digit};
    tree_node_t node_325Minutes_2nd_Digit = { &_325Minutes_2nd_Digit, &node_192Home_Screen_1, 0, &node_329Separator};
    tree_node_t node_329Separator = { &_329Separator, &node_192Home_Screen_1, 0, &node_326Hours_1st_Digit};
    tree_node_t node_326Hours_1st_Digit = { &_326Hours_1st_Digit, &node_192Home_Screen_1, 0, &node_327Hours_2nd_Digit};
    tree_node_t node_327Hours_2nd_Digit = { &_327Hours_2nd_Digit, &node_192Home_Screen_1, 0, &node_297Main_Menu_Button_};
    tree_node_t node_297Main_Menu_Button_ = { &_297Main_Menu_Button_, &node_192Home_Screen_1, &node_298Label, &node_336Year};
        tree_node_t node_298Label = { &_298Label, &node_297Main_Menu_Button_, 0, 0};
    tree_node_t node_336Year = { &_336Year, &node_192Home_Screen_1, 0, &node_333Month};
    tree_node_t node_333Month = { &_333Month, &node_192Home_Screen_1, 0, &node_337Day_Number};
    tree_node_t node_337Day_Number = { &_337Day_Number, &node_192Home_Screen_1, 0, &node_332Comma_Separator};
    tree_node_t node_332Comma_Separator = { &_332Comma_Separator, &node_192Home_Screen_1, 0, &node_196Day};
    tree_node_t node_196Day = { &_196Day, &node_192Home_Screen_1, 0, 0};

//---------------------------------

tree_node_t *group0_tree_nodes[] = {
    &node_1Home_Screen_0,
    &node_192Home_Screen_1,
};
tree_node_t **group_tree_nodes[NUM_GROUPS] = {group0_tree_nodes};

tree_node_t **screen_tree_nodes = group0_tree_nodes;

nema_transition_t group_effect[1] = {NEMA_TRANS_CUBE_H};

uint8_t group_layout[1] = {0};

//---------------------------------

tree_node_t *popup_tree_nodes[] = {
0};

//---------------------------------

