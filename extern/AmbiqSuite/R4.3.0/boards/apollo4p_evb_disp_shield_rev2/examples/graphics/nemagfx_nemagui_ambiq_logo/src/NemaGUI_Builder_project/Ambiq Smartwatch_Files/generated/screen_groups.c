//*****************************************************************************
//
// @file screen_groups.c
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

#include "screen_groups.h"
#include "gui_tree.h"
#include "ng_globals.h"
#include "ng_gitem.h"
#include "nema_transitions.h"

#define NUM_GROUPS 1
#define NUM_POPUPS 0

static int screens_per_group[NUM_GROUPS] = {4};

static tree_node_t *group0_tree_nodes[] =
{
    &node_1MainScreen,
    &node_3AnalogWatchFaceScreen,
    &node_5DigitalWatchFaceScreen,
    &node_6ContactScreen,
};

static tree_node_t **group_tree_nodes[NUM_GROUPS] = {group0_tree_nodes};

static int cur_screen_group = 0;
static int cur_screen_node  = 0;

static tree_node_t *popup_tree_nodes[] =
{
    NULL
};

static tree_node_t **cur_group_tree_nodes = group0_tree_nodes;

nema_transition_t group_effect[1] = {NEMA_TRANS_LINEAR_H};

uint8_t group_layout[1] = {NG_LAYOUT_HOR};

void init_screen_groups()
{
    ng_globals_register_screen_groups(NUM_GROUPS, NUM_POPUPS, screens_per_group, group_tree_nodes, group_effect, group_layout, popup_tree_nodes, cur_screen_group, cur_screen_node, cur_group_tree_nodes);
}
