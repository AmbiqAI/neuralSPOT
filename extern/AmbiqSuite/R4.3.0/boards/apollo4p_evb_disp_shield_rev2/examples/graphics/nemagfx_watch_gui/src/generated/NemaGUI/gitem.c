//*****************************************************************************
//
//! @file gitem.c
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

#ifndef __GITEM_C__
#define __GITEM_C__

#include "event.h"

#include "gitem_tree.c"

#define GITEM_MAX GITEM_LABEL

#define CLAMP(x, low, high) ( (x) < (low) ? (low) : ( (x) > (high) ? (high) : (x) ) )

void set_hor_slider_indicator_pos(tree_node_t *node, int x)
{
    // <-----------   SLIDER   ------------->
    // filled_rect   indicator   empty_rect
    // ================[X]-------------------
    gitem_t *slider      = node->this_;
    gitem_t *empty_rect  = node->first_child->this_;
    gitem_t *filled_rect = node->first_child->next->this_;
    gitem_t *indicator   = node->first_child->next->next->this_;

    //indicator radius
    int indic_r = indicator->w>>1;

    //set indicator position
    x -= indic_r;
    indicator->x = x <= indic_r ? indic_r : x;
    indicator->x = x >= slider->w - indic_r ? slider->w - indic_r : x;

    //clip indicator position
    if ((indicator->x) < 0)
    {
        indicator->x = 0;
    }
    else if ((indicator->x) > slider->w-indicator->w)
    {
        indicator->x = slider->w-indicator->w;
    }

    //set filled_rect from start to indicator center
    filled_rect->x = indic_r;
    filled_rect->w = indicator->x; //+ indic_r - filled_rect->x;
    //set empty_rect from indicator center to finish
    empty_rect->x = indicator->x + indic_r;
    empty_rect->w = (slider->w-indic_r) - empty_rect->x;
}

void set_ver_slider_indicator_pos(tree_node_t *node, int y)
{
    // <-----------   SLIDER   ------------->
    // filled_rect   indicator   empty_rect
    // ================[X]-------------------
    gitem_t *slider      = node->this_;
    gitem_t *empty_rect  = node->first_child->this_;
    gitem_t *filled_rect = node->first_child->next->this_;
    gitem_t *indicator   = node->first_child->next->next->this_;

    //indicator radius
    int indic_r = indicator->h>>1;

    //set indicator position
    y -= indic_r;
    indicator->y = y <= indic_r ? indic_r : y;
    indicator->y = y >= slider->h - indic_r ? slider->h - indic_r : y;

    //clip indicator position
    if ((indicator->y) < 0)
    {
        indicator->y = 0;
    }
    else if ((indicator->y) > slider->h-indicator->h)
    {
        indicator->y = slider->h-indicator->h;
    }

    //set filled_rect from start to indicator center
    empty_rect->y = indic_r;
    empty_rect->h = indicator->y; //+ indic_r - filled_rect->x;
    //set empty_rect from indicator center to finish
    filled_rect->y = indicator->y + indic_r;
    filled_rect->h = slider->h - indic_r - indicator->y - indic_r;
}

void set_current_screen_by_gitem(gitem_t *git)
{
    for ( int i = 0; i < NUM_GROUPS; ++i )
    {
        screen_tree_nodes = group_tree_nodes[i];

        for ( int j = 0; j < NUM_SCREENS[i]; j++ )
        {
            if ( screen_tree_nodes[j]->this_ == git)
            {
                cur_screen_group  = i;
                cur_screen_node   = j;
                return;
            }
        }
    }
}

void set_current_popup_by_gitem(gitem_t *git)
{
    popup_node = 0;
    for (int i = 0; i < NUM_POPUPS; ++i)
    {
        if ( popup_tree_nodes[i]->this_ == git )
        {
            popup_node = popup_tree_nodes[i];

            popup_off_x = (RESX-git->w) / 2;
            popup_off_y = (RESY-git->h) / 2;

            break;
        }
    }
    if (popup_node == 0)
    {
        do_popup = 0;
    }
}

void gitem_set_flag(gitem_t *git, uint32_t flag)
{
    git->gesture_flags |= flag;
}

void gitem_unset_flag(gitem_t *git, uint32_t flag)
{
    git->gesture_flags &= ~flag;
}

tree_node_t *get_tree_node_under_cursor(tree_node_t *node, uint32_t gesture, int x, int y, int x_off, int y_off, int *click_x, int *click_y)
{
    tree_node_t *selected_node = 0;

    while (node && node->this_)
    {
        if (x >= node->this_->x + x_off &&
            y >= node->this_->y + y_off &&
            x <  node->this_->x + x_off + node->this_->w &&
            y <  node->this_->y + y_off + node->this_->h)
        {
            //first explore children
            if ( (node->this_->gesture_flags & GITEMF_STOP_RECUR) == 0 && node->first_child)
            {
                node->first_child->parent = node;
                selected_node = get_tree_node_under_cursor(node->first_child, gesture, x, y, x_off + node->this_->x, y_off + node->this_->y, click_x, click_y);
            }

            //find the last added-item that supports the gesture
            if (node->this_->gesture_flags & gesture )
            {
                selected_node = node;
                *click_x = x - (node->this_->x + x_off);
                *click_y = y - (node->this_->y + y_off);
            }
        }

        //got to next item at the same level;
        if (node->next)
        {
            node->next->parent = node->parent;
        }
        node = node->next;
    }

    return selected_node;
}

//find parent node that supports this gesture
tree_node_t *get_parent_tree_node_with_gesture(tree_node_t *node, uint32_t gesture)
{
    if (!node)
    {
        return (tree_node_t *)0;
    }

    //go to parent node
    node = node->parent;

    while (node && node->this_)
    {
        if (node->this_->gesture_flags & gesture)
        {
            return node;
        }

        //go to parent node
        node = node->parent;
    }
    return (tree_node_t *)0;
}

typedef void release_gesture_func_t  (tree_node_t *node);
typedef void press_gesture_func_t      (tree_node_t *node, int x, int y);
typedef void swipe_gesture_func_t    (tree_node_t *node, int x, int y, int dx, int dy);

void uncheck_subtree(tree_node_t *node)
{
    while (node && node->this_)
    {
        if (node->this_->type == GITEM_RADIO)
        {
            node->this_->checked = 0;
        }

        if (node->first_child)
        {
            uncheck_subtree(node->first_child);
        }

        //got to next item at the same level;
        node = node->next;
    }
}

void handle_event(event_t *event, event_e triger_event)
{
    while (event && event->callback != NULL)
    {
        //if new trigger OR timer on status event
        if ( (event->trigger_event == triger_event && (event->status != EV_STATUS_RUNNING || event->re_trigger != EVENT_IGNORE) )
              || (triger_event == EVENT_TIMER && event->status == EV_STATUS_RUNNING) )
        {
            float curr_time = nema_get_time();

            switch(event->type)
            {
                case EVENT_ONESHOT:
                    event->callback(event);
                    force_display = 1;
                    break;

                case EVENT_PERIODIC:
                    if ( triger_event == EVENT_TIMER )
                    {
                        //If period has expired, execute and schedule the next execution one period later (triggered by timer)
                        if ( event->status == EV_STATUS_RUNNING && curr_time >= event->start_time)
                        {
                            event->callback(event);
                            force_display = 1;
                            event->start_time += event->period;
                        }
                    } else if ( event->re_trigger == EVENT_PAUSE && event->status != EV_STATUS_STOPPED )
                    {
                        // if re_trigger is EVENT_PAUSE and the event has already started
                        if ( event->status == EV_STATUS_RUNNING )
                        {
                            //pause
                            event->pause_time = curr_time;
                            event->status = EV_STATUS_PAUSED;
                        }
                        else
                        {
                            //resume
                            event->start_time += curr_time - event->pause_time;
                            event->status = EV_STATUS_RUNNING;
                        }
                    }
                    else
                    {
                        //start
                        event->start_time = curr_time;
                        event->status = EV_STATUS_RUNNING;
                        event->callback(event);
                        force_display = 1;
                        event->start_time += event->period;
                    }
                    break;

                case EVENT_TRANSITION:
                    if ( triger_event == EVENT_TIMER )
                    {
                        event->progress = (curr_time - event->start_time) / event->duration;

                        if ( event->progress > 1.f)
                        {
                            event->progress = 1.f;
                        }

                        event->callback(event);
                        force_display = 1;

                        if ( event->progress >= 1.f)
                        {
                            event->status = EV_STATUS_STOPPED;
                        }
                    } else if ( event->re_trigger == EVENT_PAUSE && event->status != EV_STATUS_STOPPED )
                    {
                        // if re_trigger is EVENT_PAUSE and the event has already started
                        if ( event->status == EV_STATUS_RUNNING )
                        {
                            //pause
                            event->pause_time = curr_time;
                            event->status = EV_STATUS_PAUSED;
                        }
                        else
                        {
                            //resume
                            event->start_time += curr_time - event->pause_time;
                            event->status = EV_STATUS_RUNNING;
                        }
                    }
                    else
                    {
                        event->start_time = curr_time;
                        event->status = EV_STATUS_RUNNING;
                        event->progress = 0.f;
                        event->callback(event);
                        force_display = 1;
                    }
                    break;

                case EVENT_PERIODIC_TRANSITION:
                    if ( triger_event == EVENT_TIMER )
                    {
                        // Within duration
                        if (  curr_time - event->start_time < event->duration )
                        {
                            event->progress = (curr_time - event->start_time) / event->duration;
                            event->callback(event);
                            force_display = 1;
                        }
                        else
                        {
                            // Period expired => shift start time for the next period
                            if ( curr_time - event->start_time > event->period )
                            {
                                event->start_time += event->period;
                                event->progress = (curr_time - event->start_time) / event->duration;
                                event->callback(event);
                                force_display = 1;
                            }
                            else if ( event->progress < 1.f )
                            {
                                event->progress = 1.f;
                                event->callback(event);
                                force_display = 1;
                            }
                        }
                    } else if ( event->re_trigger == EVENT_PAUSE && event->status != EV_STATUS_STOPPED )
                    {
                        // if re_trigger is EVENT_PAUSE and the event has already started
                        if ( event->status == EV_STATUS_RUNNING )
                        {
                            //pause
                            event->pause_time = curr_time;
                            event->status = EV_STATUS_PAUSED;
                        }
                        else
                        {
                            //resume
                            event->start_time += curr_time - event->pause_time;
                            event->status = EV_STATUS_RUNNING;
                        }
                    }
                    else
                    {
                        //re-trigger
                        event->start_time = curr_time;
                        event->status = EV_STATUS_RUNNING;
                        event->progress = 0.f;
                        event->callback(event);
                        force_display = 1;
                    }

                    break;

                default:
                    break;
            }
        }

        //Start timer if necessary
        if (event->status == EV_STATUS_RUNNING)
        {
            if ( animation_timer_running == 0)
            {
                nema_timer_set_periodic(animation_timer, TIMER_ANIM_PERIOD);
            }

            animation_timer_running = 1;
        }

        event = event->next;

        //check next if the event was triggered by user
        if ( triger_event == EVENT_TIMER)
        {
            return;
        }
    } //event->callback != NULL
}

void highlight_node(tree_node_t *node)
{
    node->this_->highlighted = 1;
}

void abort_press_generic(tree_node_t *node)
{
    node->this_->highlighted = 0;
}

void press_generic(tree_node_t *node, int x, int y)
{
    (void)x;
    (void)y;
    handle_event(node->this_->event, EVENT_PRESS);
}

void release_generic(tree_node_t *node)
{
    node->this_->highlighted = 0;
    handle_event(node->this_->event, EVENT_RELEASE);
}

void release_checkbox(tree_node_t *node)
{
    node->this_->highlighted = 0;
    node->this_->checked = node->this_->checked == 0;
    handle_event(node->this_->event, EVENT_RELEASE);
}

void release_radio(tree_node_t *node)
{
    node->this_->highlighted = 0;
    tree_node_t *_node = node;
    do
    {
        _node = _node->parent;
    } while ( _node->this_->type != GITEM_MENU );

    uncheck_subtree(_node->first_child);
    node->this_->checked = 1;

    release_generic(node);
}

void callback_swipe_window_momentum(event_t* event); //forward declaration

void release_window(tree_node_t *node)
{
    gitem_unset_flag(node->this_, GITEMF_STOP_RECUR);

    //Identify the parameters that will affect the duration
    int max = nema_abs(dx_git) > nema_abs(dy_git) ? nema_abs(dx_git) : nema_abs(dy_git);
    int dim = nema_abs(dx_git) > nema_abs(dy_git) ? node->this_->w      : node->this_->h;

    //Set event parameters (activate event)
    for ( int i = 0; i < TEMP_ANIMATIONS_COUNT; ++i )
    {
        if ( animation_temp[i].status == EV_STATUS_STOPPED || (animation_temp[i].status == EV_STATUS_RUNNING && animation_temp[i].target_tree_node == node) )
        {
            animation_temp[i].status            = EV_STATUS_RUNNING;
            animation_temp[i].duration          = 4.f * max / dim;
            animation_temp[i].type              = EVENT_TRANSITION;
            animation_temp[i].callback          = callback_swipe_window_momentum;
            animation_temp[i].start_time        = nema_get_time();
            animation_temp[i].target_tree_node  = node;
            dx_win[i] = dx_git;
            dy_win[i] = dy_git;

            //If duration == 0 animation is complete => avoid progress from getting Inf or NaN
            if ( animation_temp[i].duration == 0.f)
            {
                animation_temp[i].status = EV_STATUS_STOPPED;
            }

            break;
        }
    }

    //Set event timer
    if ( animation_timer_running == 0)
    {
        nema_timer_set_periodic(animation_timer, TIMER_ANIM_PERIOD);
    }

    animation_timer_running = 1;
}

void press_hor_slider(tree_node_t *node, int x, int y)
{
    (void)y;
    set_hor_slider_indicator_pos(node, x);

    gitem_t *slider      = node->this_;
    if (slider->event)
    {
        gitem_t *indicator   = node->first_child->next->next->this_;
        float percent = (float)indicator->x / (float)(slider->w - indicator->w);
        slider->val = percent;
        handle_event(slider->event, EVENT_VALUE_CHANGED);
    }
}

void press_ver_slider(tree_node_t *node, int x, int y)
{
    (void)x;

    set_ver_slider_indicator_pos(node, y);

    gitem_t *slider      = node->this_;
    if (slider->event)
    {
        gitem_t *indicator   = node->first_child->next->next->this_;
        float percent = (float)indicator->y / (float)(slider->h - indicator->h);
        percent = 1.f - percent;
        slider->val = percent;
        handle_event(slider->event, EVENT_VALUE_CHANGED);
    }
}

void press_gauge(tree_node_t *node, int x, int y)
{
    gitem_t *needle = node->first_child->this_;
    //Map needle x,y to the closest circle point
    float R       = needle->x_rot;
    float vX      = x - node->this_->x_rot;
    float vY      = y - node->this_->y_rot;
    float magV    = nema_sqrt(vX*vX + vY*vY);
    float temp_x  = node->this_->x_rot + vX / magV * R;
    float temp_y  = node->this_->y_rot + vY / magV * R;

    int   angle   = 0;
    float nom     = node->this_->y_rot - temp_y;
    float denom   = node->this_->x_rot - temp_x;

    //**************    TODO    ********************
    //a) optimize if-statements => merge equivalent conditions
    //b) avoid divisions with const (i.e. ..../(max_angle-min_angle)), precalculate denominator and multiply with the inverse of it
    if ( denom == 0 && (temp_y <= node->this_->y_rot))
    {
        angle = 90;
    }
    else if ( denom == 0 && (temp_y > node->this_->y_rot))
    {
        angle = 90; //will be mapped to 270 later
    }
    else
    {
        // angle = (int) nema_rad_to_deg(atan(nom/denom));
        angle = (int) nema_atan(nom / denom);
    }

    //Map angle to [0, 360]
    //1st quartile [0 , 90]
    if ( (temp_x <= node->this_->x_rot) && (temp_y <= node->this_->y_rot))
    {
        angle = angle + 0;
    //2nd quartile [90 , 180]
    } else if ( (temp_x >= node->this_->x_rot) && (temp_y <= node->this_->y_rot))
    {
        angle = angle + 180;
    //3rd quartile [180 , 270]
    } else if ( (temp_x >= node->this_->x_rot) && (temp_y >= node->this_->y_rot))
    {
        angle = angle + 180;
    //4th quartile [270 , 360]
    } else if ((temp_x <= node->this_->x_rot) && (temp_y >= node->this_->y_rot))
    {
        angle = angle + 360;
    }

    // Transform angles in a form conformant with: min_angle < angle < max_angle
    // and set gauge/needle arguments (angle, val) only when the previous condition holds
    if ( needle->min_angle >= 0 && needle->max_angle >= 0)
    {

        if ( angle >= needle->min_angle && angle <= needle->max_angle)
        {
            needle->angle    = angle;
            needle->val      = needle->max * (needle->angle-needle->min_angle) / (needle->max_angle - needle->min_angle);
            node->this_->val = needle->val;

            if (node->this_->event)
            {
                handle_event(node->this_->event, EVENT_VALUE_CHANGED);
            }
        }

    } else if ( needle->min_angle < 0 || needle->max_angle < 0)
    {

        if ( (angle >= needle->min_angle && angle <= needle->max_angle) || (angle >= needle->min_angle + 360 && angle <= needle->max_angle + 360))
        {
            needle->angle = angle;

            if ( angle >= needle->min_angle && angle <= needle->max_angle)
            {
                needle->val = (needle->max*(needle->angle - needle->min_angle) + needle->min * (needle->max_angle - needle->angle)) / (needle->max_angle - needle->min_angle);
            }

            // phase shift by 360 degrees required: min_angle += 360, max_angle += 360
            else
            {
                needle->val = (needle->max * (needle->angle - needle->min_angle - 360) + needle->min * (needle->max_angle + 360 - needle->angle)) / (needle->max_angle - needle->min_angle);
            }

            node->this_->val = needle->val;
            if (node->this_->event)
            {
                handle_event(node->this_->event, EVENT_VALUE_CHANGED);
            }
        }
    }
}

void press_circular_progress(tree_node_t *node, int x, int y)
{
    float R       = node->this_->w >> 1;
    float vX      = x - R;
    float vY      = y - R;
    float magV    = nema_sqrt(vX*vX + vY*vY);
    float temp_x  = R + vX / magV * R;
    float temp_y  = R + vY / magV * R;

    int   angle   = 0;
    float nom     = R - temp_y;
    float denom   = R - temp_x;

    if ( denom == 0)
    {
        angle = 90; //will be mapped to 270 later
    }
    else
    {
        // angle = (int) nema_rad_to_deg(atan(nom/denom));
        angle = (int) nema_atan(nom / denom);
    }

    //Map angle to [0, 360]
    //1st quartile [0 , 90]
    if ( (temp_x <= R) && (temp_y <= R))
    {
        angle = angle + 0;
    //2nd quartile [90 , 180]
    } else if ( (temp_x >= R) && (temp_y <= R))
    {
        angle = angle + 180;
    //3rd quartile [180 , 270]
    } else if ( (temp_x >= R) && (temp_y >= R))
    {
        angle = angle + 180;
    //4th quartile [270 , 360]
    } else if ((temp_x <= R) && (temp_y >= R))
    {
        angle = angle + 360;
    }

    //Saturate to min/max values (prevent roll-over)
//    if ( nema_abs(angle-node->this_->angle) > 45)
//        return;

    node->this_->angle = angle;

    if ( node->this_->min_angle >= 0 && (angle >= node->this_->min_angle && angle <= node->this_->max_angle))
    {
        node->this_->val = (float) (angle-node->this_->min_angle) / (node->this_->max_angle - node->this_->min_angle);

        if (node->this_->event)
        {
            handle_event(node->this_->event, EVENT_VALUE_CHANGED);
        }

    //TODO: verify the code inside the following condition
    } else if ( node->this_->min_angle < 0 || node->this_->max_angle < 0)
    {
        if ( (angle + 360 >= node->this_->min_angle + 360 && angle + 360 <= node->this_->max_angle + 360))
        {
            node->this_->val = (float) (angle-node->this_->min_angle) / (node->this_->max_angle - node->this_->min_angle);
        }
        else if (angle - 360 >= node->this_->min_angle && angle - 360 <= node->this_->max_angle)
        {
            node->this_->val = (float) (angle-node->this_->min_angle-360) / (node->this_->max_angle - node->this_->min_angle);
        }

        if (node->this_->event)
        {
            handle_event(node->this_->event, EVENT_VALUE_CHANGED);
        }
    }
}

void press_window(tree_node_t *node, int x, int y)
{
    (void)x;
    (void)y;
    gitem_unset_flag(node->this_, GITEMF_STOP_RECUR);
}

press_gesture_func_t *press_gitem[] =
{
    0,  //GITEM_SCREEN,
    0,  //GITEM_CONT,
    0,  //GITEM_CIRCLE,
    0,  //GITEM_RECT,
    0,  //GITEM_RRECT,
    0,  //GITEM_IMG,
    0,  //GITEM_LABEL,
    0,  //GITEM_LABEL_BUTTON,
    0,  //GITEM_CHECKBOX,
    0,  //GITEM_RADIO,
    press_hor_slider,   //GITEM_H_SLIDER,
    press_ver_slider,   //GITEM_V_SLIDER
    0,  //GITEM_DIGIMETER
    0,  //GITEM_POPUP,
    0,  //GITEM_ICON,
    0,  //GITEM_MENU
    0,  //GITEM_PROGRESS_BAR
    press_gauge,    //GITEM_GAUGE
    0,  //GITEM_ROTATING_CONTAINER
    0,  //GITEM_ICON_BUTTON
    press_window,   //GITEM_WINDOW
    press_circular_progress,    //GITEM_CIRCULAR_PROGRESS
    0,  //GITEM_WATCH_FACE
};

release_gesture_func_t *release_gitem[] =
{
    0,  //GITEM_SCREEN,
    0,  //GITEM_CONT,
    0,  //GITEM_CIRCLE,
    0,  //GITEM_RECT,
    0,  //GITEM_RRECT,
    0,  //GITEM_IMG,
    0,  //GITEM_LABEL,
    0,  //GITEM_LABEL_BUTTON,
    release_checkbox,   //GITEM_CHECKBOX,
    release_radio,      //GITEM_RADIO,
    0,  //GITEM_H_SLIDER,
    0,  //GITEM_V_SLIDER
    0,  //GITEM_DIGIMETER
    0,  //GITEM_POPUP,
    0,  //GITEM_ICON,
    0,  //GITEM_MENU
    0,  //GITEM_PROGRESS_BAR
    0,  //GITEM_GAUGE
    0,  //GITEM_ROTATING_CONTAINER
    0,  //GITEM_ICON_BUTTON
    release_window,     //GITEM_WINDOW
    0,  //GITEM_CIRCULAR_PROGRESS
    0,  //GITEM_WATCH_FACE
};

void swipe_window(tree_node_t *node, int x, int y, int dx, int dy)
{
    (void)x;
    (void)y;

    //node:              window
    //node->first_child: list container
    // gitem_set_flag(node->this_, GITEMF_STOP_RECUR);
    gitem_t *subwindow = node->first_child->this_;

    if (subwindow->h > node->this_->h)
    {
        int cont_y_min = subwindow->y;
        int cont_y_max = subwindow->h + cont_y_min;
        int cont_y_min_new = cont_y_min + dy;
        int cont_y_max_new = cont_y_max + dy;

        if (cont_y_max_new < node->this_->h && cont_y_min_new > 0)
        {
            //don't do anything
            cont_y_min_new = cont_y_min;
        }
        else if (cont_y_min_new > 0)
        {
            cont_y_min_new = 0;
        }
        else if (cont_y_max_new < node->this_->h)
        {
            cont_y_min_new = node->this_->h - subwindow->h;
        }

        subwindow->y = cont_y_min_new;
    }

    if (subwindow->w > node->this_->w)
    {
        int cont_x_min = subwindow->x;
        int cont_x_max = subwindow->w + cont_x_min;
        int cont_x_min_new = cont_x_min + dx;
        int cont_x_max_new = cont_x_max + dx;

        if (cont_x_max_new < node->this_->w && cont_x_min_new > 0)
        {
            //don't do anything
            cont_x_min_new = cont_x_min;
        }
        else if (cont_x_min_new > 0)
        {
            cont_x_min_new = 0;
        }
        else if (cont_x_max_new < node->this_->w)
        {
            cont_x_min_new = node->this_->w - subwindow->w;
        }

        subwindow->x = cont_x_min_new;
    }
    handle_event(node->this_->event, EVENT_VALUE_CHANGED);
}

void swipe_hor_slider(tree_node_t *node, int x, int y, int dx, int dy)
{
    (void)dx;
    (void)dy;
    (void)y;
    set_hor_slider_indicator_pos(node, x);

    gitem_t *slider      = node->this_;
    if (slider->event)
    {
        gitem_t *indicator   = node->first_child->next->next->this_;
        float percent = (float)indicator->x / (float)(slider->w-indicator->w);
        slider->val = percent;
        handle_event(slider->event, EVENT_VALUE_CHANGED);
    }
}

void swipe_ver_slider(tree_node_t *node, int x, int y, int dx, int dy)
{
    (void)dx;
    (void)dy;
    press_ver_slider(node, x, y);
}

void swipe_gauge(tree_node_t *node, int x, int y, int dx, int dy)
{
    (void)dx;
    (void)dy;
    press_gauge(node, x, y);
}

void swipe_circular_progress(tree_node_t *node, int x, int y, int dx, int dy)
{
    (void)dx;
    (void)dy;
    press_circular_progress(node, x, y);
}

swipe_gesture_func_t *swipe_gitem[] =
{
    0,  //GITEM_SCREEN,
    0,  //GITEM_CONT,
    0,  //GITEM_CIRCLE,
    0,  //GITEM_RECT,
    0,  //GITEM_RRECT,
    0,  //GITEM_IMG,
    0,  //GITEM_LABEL,
    0,  //GITEM_BUTTON,
    0,  //GITEM_CHECKBOX,
    0,  //GITEM_RADIO,
    swipe_hor_slider,   //GITEM_H_SLIDER,
    swipe_ver_slider,   //GITEM_V_SLIDER
    0,  //GITEM_DIGIMETER
    0,  //GITEM_POPUP,
    0,  //GITEM_ICON,
    0,  //GITEM_MENU,
    0,  //GITEM_PROGRESS_BAR,
    swipe_gauge,    //GITEM_GAUGE,
    0,  //GITEM_ROTATING_CONTAINER
    0,  //GITEM_ICON_BUTTON,
    swipe_window,   //GITEM_WINDOW
    swipe_circular_progress,    //GITEM_CIRCULAR_PROGRESS
    0,  //GITEM_WATCH_FACE
};


// Generic auxiliarly functions
void set_value(tree_node_t *node, float val)
{
    gitem_t* t_git = node->this_;

    t_git->val     = val;

    if ( node->this_->type == GITEM_GAUGE)
    {
        tree_node_t *temp_node = node->first_child;
        gitem_t     *needle    = temp_node->this_;
        while ( needle->type != GITEM_ROTATING_CONTAINER )
        {
            temp_node = temp_node->next;
            needle    = temp_node->this_;
        }
        t_git->angle  = (t_git->max_angle * (t_git->val - t_git->min) + t_git->min_angle * (t_git->max - t_git->val)) / (t_git->max - t_git->min);
        needle->angle = t_git->angle;

    } else if (node->this_->type == GITEM_V_SLIDER)
    {
        gitem_t *indicator   = node->first_child->next->next->this_;
        set_ver_slider_indicator_pos(node, (int32_t)((1.f - val) * (t_git->h - indicator->h) + (indicator->h >> 1)));
    } else if (node->this_->type == GITEM_H_SLIDER)
    {
        gitem_t *indicator   = node->first_child->next->next->this_;
        set_hor_slider_indicator_pos(node, (int32_t)(val * (t_git->w - indicator->w) + (indicator->w >> 1)));
    } else if (node->this_->type == GITEM_PROGRESS_BAR)
    {
        node->first_child->this_->w = (uint16_t)(val*t_git->w);
    }
}

void set_window_source(tree_node_t *window, tree_node_t *source)
{
    window->first_child->first_child = source;
    window->first_child->this_->w    = source->this_->w;
    window->first_child->this_->h    = source->this_->h;
}

void set_opacity(tree_node_t *node, int opacity)
{
    gitem_t* t_git = node->this_;
    int bgr = t_git->color & 0x00ffffff;
    t_git->color   =  (opacity << 24) | bgr;
}

#endif // __GITEM_C__
