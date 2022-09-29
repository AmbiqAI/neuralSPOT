//*****************************************************************************
//
//! @file main.c
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

#define PRINT_I             am_util_debug_printf
#define CALCULATE_FPS

#include "am_bsp.h"
#include "am_util.h"

//Nema|GFX include files
#include "nema_core.h"
#include "nema_font.h"
#include "nema_dc_hal.h"
#include "nema_dc.h"
#include "nema_easing.h"
#include "nema_transitions.h"
#include "nema_event.h"
#include "nema_utils.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

//Nema|GUI Builder include files
#include "gitem.h"
#include "event.h"
#include "globals.c"
#include "load_image_assets.c"
#include "load_font_assets.c"
#include "gitem.c"
#include "gitem_draw.c"
#include "load_objects.c"
#include "display.c"
#include "event.c"

#include "nema_dc_mipi.h"

void loop(void)
{
    static float last_dxy      = 0.01;
    static float last_dxy_prev = 0;
    int mouse_clicked_x        = -1, mouse_clicked_y = -1;
    tree_node_t *selected_node = 0;
    tree_node_t *pressed_node  = 0;
    nema_event_t event         = {0};

    event.mouse_event       = (int)MOUSE_EVENT_LEFT_CLICK;
    event.mouse_state       = (int)MOUSE_STATE_LEFT_CLICKED;
    event.mouse_dx          = -10;
    event.mouse_dy          = 0;
    event.mouse_x           = RESX / 4 * 3;
    event.mouse_y           = RESY / 2;
    event.kb_event          = (int)KB_EVENT_NONE;
    event.kb_key            = 0;
    event.timer_id          = 0;
    event.timer_expirations = 0;

/*     nema_timer_start(animation_timer);     */

    while(1)
    {
        int cur_node_x, cur_node_y;
        force_display = 0;

        char press    = 0;
        char release  = 0;
        char press_r  = 0;
        int  mouse_dx = 0;
        int  mouse_dy = 0;
        int  timer_id = -1;
        int  timer_expirations = 0;

//        doing_transition = 1;

        if (event.mouse_x <= 2)
        {
            event.mouse_x = 2;
            event.mouse_dx = 5;
        } else if (event.mouse_x >= RESX / 4 * 3)
        {
            event.mouse_x = RESX / 4 * 3;
            event.mouse_dx = -5;
        }

        //wait for event
        if (1)
        {
            do
            {
                switch (event.mouse_event)
                {
                    case MOUSE_EVENT_LEFT_CLICK:
                        press   = 1;
                        mouse_clicked_x = RESX / 4 * 3;
                        mouse_clicked_y = RESY / 2;
                        break;
                    case MOUSE_EVENT_LEFT_RELEASE:
                        release = 1;
                        break;
                    case MOUSE_EVENT_RIGHT_CLICK:
                        press_r = 1;
                        break;
                    default:
                        break;
                }

                if (event.mouse_state == MOUSE_STATE_LEFT_CLICKED)
                {
                    mouse_dx += event.mouse_dx;
                    mouse_dy += event.mouse_dy;
                }

                if ( event.timer_id > 0 )
                {
                    timer_id = event.timer_id;
                    timer_expirations += event.timer_expirations;
                    // printf("timer %d\n", timer_id);
                }
            } while(0);
        }

        //Handle PRESS event
        if ( press )
        {
            if ( doing_transition )
            {
                //Pause Transition
                selected_node = screen_tree_nodes[cur_screen_node];

                nema_timer_stop(animation_timer);
                animation_timer_running = 0;
            }
            else
            {
                //Get Node Under Cursor
                if (do_popup)
                {
                    selected_node = get_tree_node_under_cursor( popup_node,
                                                         GITEMF_PRESS | GITEMF_RELEASE,
                                                         event.mouse_x, event.mouse_y,
                                                         popup_off_x, popup_off_y,
                                                         &cur_node_x, &cur_node_y);
                }
                else
                {
                    selected_node = get_tree_node_under_cursor( screen_tree_nodes[cur_screen_node],
                                                         GITEMF_PRESS | GITEMF_RELEASE,
                                                         event.mouse_x, event.mouse_y,
                                                         0, 0,
                                                         &cur_node_x, &cur_node_y);
                }

                //Press Selected Node
                if ( selected_node )
                {
                    highlight_node(selected_node);
                    //press gitem
                    if (selected_node->this_->gesture_flags & GITEMF_PRESS)
                    {
                        if (press_gitem[selected_node->this_->type])
                        {
                            press_gitem[selected_node->this_->type](selected_node, cur_node_x, cur_node_y);
                        }
                        else
                        {
                            press_generic(selected_node, cur_node_x, cur_node_y);
                        }
                    }
                    pressed_node = selected_node;
                    force_display = 1;
                }
            }
        }

        //Handle transition TIMER event
        if ( doing_transition && (event.mouse_state&MOUSE_STATE_LEFT_CLICKED) == 1 && timer_id == animation_timer && timer_expirations > 0 )
        {

            step += last_dxy*(float)timer_expirations;
/*             step = 0.01; */
            if (step <= 0.f || step >= 1.f)
            {
                doing_transition = 0;

                if (step >= 1.f)
                {
                    if (going_right)
                    {
                        --cur_screen_node;
                    }
                    else
                    {
                        ++cur_screen_node;
                    }
                }
                step = 0.f;
            }
            force_display = 1;
        }

        //Handle RELEASE event
        if ( release )
        {
            if ( doing_transition )
            {
                //Finish Transition

                //Start Timer
                if ( animation_timer_running == 0 )
                {
                    nema_timer_set_periodic(animation_timer, TIMER_ANIM_PERIOD);
                    animation_timer_running = 1;
                }

                if (step > 0.f && step < 1.f)
                {

                    last_dxy_prev *= 0.4f;
                    last_dxy = last_dxy_prev;

                    if (last_dxy < ANIMATION_STEP_0_1 && last_dxy > -ANIMATION_STEP_0_1)
                    {
                        if ( step > 0.5f )
                        {
                            last_dxy =  ANIMATION_STEP_0_1;
                        }
                        else
                        {
                            last_dxy = -ANIMATION_STEP_0_1;
                        }
                    }
                }
                else
                {
                    doing_transition = 0;
                }
            } else if ( pressed_node )
            {
                //Release Pressed Node

                if (pressed_node->this_->gesture_flags & GITEMF_RELEASE)
                {
                    if (release_gitem[pressed_node->this_->type])
                    {
                        release_gitem[pressed_node->this_->type](pressed_node);
                    }
                    else
                    {
                        release_generic(pressed_node);
                    }
                }
                force_display = 1;
                pressed_node = 0;
            } else if (do_popup)
            {

                if (selected_node == 0)
                {
                    int x = event.mouse_x;
                    int y = event.mouse_y;

                    //check if RELEASE is inside POPUP.
                    //If outside, close popup
                    if (x >= popup_node->this_->x + popup_off_x &&
                        y >= popup_node->this_->y + popup_off_y &&
                        x <  popup_node->this_->x + popup_off_x + popup_node->this_->w &&
                        y <  popup_node->this_->y + popup_off_y + popup_node->this_->h)
                    {
                        //PRESS is inside POPUP. Don't do anything
                    }
                    else
                    {
                        //PRESS is outside POPUP.
                        //don't close POPUP if LEFT is CLICKED
                        if ( (event.mouse_state & MOUSE_STATE_LEFT_CLICKED) == 0 )
                        {
                            do_popup = 0;
                            selected_node = screen_tree_nodes[cur_screen_node];
                            force_display = 1;
                        }
                    }
                }
            }
            selected_node = 0;
        }
        else if ( press_r )
        {
            group_effect[cur_screen_group] = (nema_transition_t)((group_effect[cur_screen_group] + 1) % NEMA_TRANS_MAX);
        }

        //SWIPING
        #define DRAG_MARGIN 5
        int margin = ((pressed_node != NULL)*DRAG_MARGIN);

        if ( pressed_node)
        {
            margin = (pressed_node->this_->gesture_flags & GITEMF_DRAG) ? 0 : margin;
        }

//        if  ( event.mouse_state&MOUSE_STATE_LEFT_CLICKED &&
//            (( nema_abs(event.mouse_x - mouse_clicked_x) > margin)  ||
//              (nema_abs(event.mouse_y - mouse_clicked_y) > margin)  ))
        if (1)
        {

            cur_node_x += mouse_dx;
            cur_node_y += mouse_dy;

            //If an item is PRESSED but doesn't support DRAG, UNPRESS it and choose new item
            if ( !selected_node || (( selected_node->this_->gesture_flags & GITEMF_DRAG) == 0
                                   && selected_node->this_->type != GITEM_SCREEN ))
            {

                if (pressed_node)
                {
                    abort_press_generic(pressed_node);
                    force_display = 1;
                    pressed_node  = NULL;
                }

                if (do_popup)
                {
                    selected_node = get_tree_node_under_cursor( popup_node,
                                                         GITEMF_DRAG,
                                                         event.mouse_x, event.mouse_y,
                                                         popup_off_x, popup_off_y,
                                                         &cur_node_x, &cur_node_y);

                    if (!popup_node)
                    {
                        selected_node = screen_tree_nodes[cur_screen_node];
                    }
                }
                else
                {
                    selected_node = get_tree_node_under_cursor(screen_tree_nodes[cur_screen_node],
                                                               GITEMF_DRAG,
                                                               event.mouse_x, event.mouse_y,
                                                               0, 0,
                                                               &cur_node_x, &cur_node_y);

                    if (!selected_node)
                    {
                        selected_node = screen_tree_nodes[cur_screen_node];
                    }
                }

            }

            //Handle DRAG
            if ( selected_node )
            {
                //if selected_node doesn't support drag, try parent nodes
                if ( (selected_node->this_->gesture_flags & GITEMF_DRAG) == 0 )
                {
                    abort_press_generic(selected_node);
                    //TODO: Discard small mouse moves
                    tree_node_t *tmp_selected_node = 0;

                    if (do_popup)
                    {
                        tmp_selected_node = get_tree_node_under_cursor( popup_node,
                                                             GITEMF_DRAG,
                                                             event.mouse_x, event.mouse_y,
                                                             popup_off_x, popup_off_y,
                                                             &cur_node_x, &cur_node_y);
                    }
                    else
                    {
                        tmp_selected_node = get_tree_node_under_cursor( screen_tree_nodes[cur_screen_node],
                                                         GITEMF_DRAG,
                                                         mouse_clicked_x, mouse_clicked_y,
                                                         0, 0,
                                                         &cur_node_x, &cur_node_y);
                    }

                    if (tmp_selected_node)
                    {
                        selected_node = tmp_selected_node;
                    }
                }

                //if selected_node is draggable, drag it
                if ( selected_node->this_->type != GITEM_SCREEN && (selected_node->this_->gesture_flags & GITEMF_DRAG) )
                {
                    //swipe gitem
                    swipe_gitem[selected_node->this_->type](selected_node,
                                                            cur_node_x, cur_node_y,
                                                            mouse_dx, mouse_dy);

                    dx_git = mouse_dx;
                    dy_git = mouse_dy;

                    force_display = 1;
                    pressed_node  = selected_node;
                }
                //else do screen transition
                else
                {
                    uint8_t layout  = group_layout[cur_screen_group];
                    int mouse_dxy   = layout ? mouse_dy : mouse_dx;
                    float step_diff = layout ? (float)mouse_dy / RESY : (float)mouse_dx / RESX;

                    if (mouse_dxy != 0)
                    {

                        if (doing_transition)
                        {
                            //Update Transition Based on DRAG
                            last_dxy_prev = last_dxy;

                            if (going_right)
                            {
                                step += (float)step_diff;
                                last_dxy = (float)step_diff;
                            }
                            else
                            {
                                step -= (float)step_diff;
                                last_dxy = -(float)step_diff;
                            }

                            if (step <= 0.f || step >= 1.f)
                            {
                                doing_transition = 0;

                                if (step >= 1.f)
                                {
                                    if (going_right)
                                    {
                                        --cur_screen_node;
                                    }
                                    else
                                    {
                                        ++cur_screen_node;
                                    }
                                }
                                step = 0.f;
                            }

                            force_display = 1;
                        }
                        else if ( !doing_transition && !do_popup && selected_node->this_->type == GITEM_SCREEN )
                        {
                            //Start New Transition
                            going_right = mouse_dxy > 0;

                            if (( ( going_right && (cur_screen_node > 0)) ||
                                 (!going_right && (cur_screen_node < NUM_SCREENS[cur_screen_group] - 1)) ) && (!TEX_BUFFER0_OCCUPIED && !TEX_BUFFER1_OCCUPIED))
                            {

                                doing_transition = 1;

                                if (going_right)
                                {
                                    display_to_buffer(cur_screen_node - 1, FRAME_BUFFERS - 2);
                                    display_to_buffer(cur_screen_node    , FRAME_BUFFERS - 1);
                                }
                                else
                                {
                                    display_to_buffer(cur_screen_node    , FRAME_BUFFERS - 2);
                                    display_to_buffer(cur_screen_node + 1, FRAME_BUFFERS - 1);
                                }

                                bind_transition_buffers();

                                if ( doing_transition  && group_effect[cur_screen_group] != NEMA_TRANS_LINEAR_H )
                                {
                                    clear_cur_screen(0);
                                }
                            }
                        }
                    } //if (mouse_dxy != 0) {
                }
            }
        } //state left clicked

        //Handle pending events

        //Animations (handled by the animation timer)
//        if (  timer_id == animation_timer )
        if (1)
        {
            uint8_t animation_timer_active_ = 0;
            for ( int i = 0; i < event_list_size; ++i )
            {
                if ( event_list[i].status == EV_STATUS_RUNNING)
                {
                    //Set timer as trigger (to be handled at execution)
                    event_e trigger = event_list[i].trigger_event;
                    event_list[i].trigger_event = EVENT_TIMER;
                    handle_event(&event_list[i], EVENT_TIMER);
                    //Reset trigger to its default value
                    event_list[i].trigger_event = trigger;
                    animation_timer_active_ = animation_timer_running;
                }
            }

            //Temporary animations (e.g. swipe window)
            for ( int i = 0; i < TEMP_ANIMATIONS_COUNT; ++i )
            {
                if ( animation_temp[i].status == EV_STATUS_RUNNING)
                {
                    animation_timer_active_ = 1;
                    animation_timer_running = 1;
                    handle_event(&animation_temp[i], EVENT_TIMER);
                }
            }

            // Stop the timer
            if ( !animation_timer_active_ && !doing_transition)
            {
                nema_timer_stop(animation_timer);
                animation_timer_running = 0;
            }
        }

        if ( force_display )
        {
            display();
            event.mouse_x += event.mouse_dx;
        }
    }//while(1)
}

int init(void)
{
    int ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; // default config
    //Initialize NemaGFX
    ret = nema_init();
    if (ret != 0)
    {
        return ret;
    }
    //Initialize Nema|dc
    ret = nemadc_init();
    if (ret != 0)
    {
        return ret;
    }

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
        uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
        uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
        pixel_format_t eFormat = FMT_RGB888;
        if ( am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0 )
        {
            return -3;
        }
        switch ( eFormat )
        {
            case FMT_RGB888:
                if ( ui8DbiWidth == 16 )
                {
                    ui32MipiCfg = MIPICFG_16RGB888_OPT0;
                }
                if ( ui8DbiWidth ==  8 )
                {
                    ui32MipiCfg = MIPICFG_8RGB888_OPT0;
                }
                break;

            case FMT_RGB565:
                if ( ui8DbiWidth == 16 )
                {
                    ui32MipiCfg = MIPICFG_16RGB565_OPT0;
                }
                if ( ui8DbiWidth ==  8 )
                {
                    ui32MipiCfg = MIPICFG_8RGB565_OPT0;
                }
                break;

            default:
                //
                // invalid color component index
                //
                return -3;
        }
    }

    uint16_t panel_resx = g_sDispCfg[g_eDispType].ui32PanelResX; //panel's max resolution
    uint16_t panel_resy = g_sDispCfg[g_eDispType].ui32PanelResY; //panel's max resolution

    uint16_t minx, miny;

    //Set the display region to center
    if ( RESX > panel_resx )
    {
        minx = 0;   // set the minimum value to 0
    }
    else
    {
        minx = (panel_resx - RESX) >> 1;
        minx = (minx >> 1) << 1;
    }

    if ( RESY > panel_resy )
    {
        miny = 0;   // set the minimum value to 0
    }
    else
    {
        miny = (panel_resy - RESY) >> 1;
        miny = (miny >> 1) << 1;
    }

    //Initialize the display
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, RESX, RESY, minx, miny);
            break;
        default:
            ; //NOP
    }

    nema_event_init(1, 0, 0, RESX, RESY);

    load_objects();
    load_image_assets();
    load_font_assets();

//    nemadc_set_layer(0, &layer[0]);
//    nemadc_send_frame_single();

    init_events();
    init_gitems();

    animation_timer = nema_timer_create();

    cur_fb_base_phys = nema_init_triple_fb(0, layer[0].baseaddr_phys,
                                              layer[1].baseaddr_phys,
                                              layer[2].baseaddr_phys);

    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
/*     nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23); */

//     nemadc_set_layer(0, &layer[0]);

    //Create Command Lists
    cl  = nema_cl_create();
    cl_screen = nema_cl_create();

    //Bind Command List
    nema_cl_bind(&cl);

    //Set Clipping Rectangle
    nema_set_clip(0, 0, RESX, RESY);

    nema_cl_submit(&cl);
    nema_wait_irq();

    nemadc_cursor_enable(1);

    display();

    return 0;
}

int watch_gui()
{
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    // Init NemaGFX, NemaDC, GUI environment (GItems, assets, events)
    int ret = init();
    if (ret)
    {
        return ret;
    }

    loop();

    nema_cl_destroy(&cl);

    return 0;
}
