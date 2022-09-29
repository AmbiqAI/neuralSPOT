//*****************************************************************************
//
//! @file predefined_callbacks.c
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

void callback_set_value(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;
    gitem_t     *t_git      = t_git_node->this_;

    float val = event->src_gitem->val;
    int is_percent;
    if ( event->src_gitem->type == GITEM_H_SLIDER || event->src_gitem->type == GITEM_V_SLIDER || event->src_gitem->type == GITEM_CIRCULAR_PROGRESS)
    {
        val = CLAMP(val, 0.f, 1.f);
        is_percent = 1;

    }
    else
    {
        val = CLAMP(val, t_git->min, t_git->max);
        is_percent = 0;
    }

    float abs_val  = is_percent ? (t_git->max-t_git->min) * val + t_git->min : val;
    float perc_val = is_percent ? val : (val-t_git->min) / (t_git->max - t_git->min);

    if ( t_git->val == abs_val )
    {
        return;
    }

    if ( t_git->type != GITEM_H_SLIDER && t_git->type != GITEM_V_SLIDER && t_git->type != GITEM_CIRCULAR_PROGRESS)
    {
        set_value(t_git_node, abs_val);
    }
    else
    {
        set_value(t_git_node, perc_val);
    }

    if ( t_git->event )
    {
        handle_event(t_git->event, EVENT_VALUE_CHANGED);
    }
}

void callback_count_up(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;
    gitem_t     *t_git      = t_git_node->this_;
    float cur_val = t_git->val + t_git->step;
    if (cur_val > t_git->max)
    {
        cur_val = t_git->max;
    }

    if (cur_val == t_git->val)
    {
        return;
    }

    t_git->val = cur_val;
    if ( t_git->event )
    {
        handle_event(t_git->event, EVENT_VALUE_CHANGED);
    }
}

void callback_count_down(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;
    gitem_t     *t_git      = t_git_node->this_;

    float cur_val = t_git->val - t_git->step;
    if (cur_val < t_git->min)
    {
        cur_val = t_git->min;
    }

    if (cur_val == t_git->val)
    {
        return;
    }

    t_git->val = cur_val;
    if ( t_git->event )
    {
        handle_event(t_git->event, EVENT_VALUE_CHANGED);
    }
}

void callback_show_screen(event_t* event)
{
    if ( event->effect > EV_SCREEN_EFFECT_NONE && event->effect < EV_SCREEN_EFFECT_COUNT)
    {
        if ( event->progress == 0)
        {

            switch(event->effect)
            {
                case EV_SCREEN_EFFECT_LINEAR_HOR_LEFT:
                default:
                    show_screen_effect = NEMA_TRANS_LINEAR_H;
                    going_right   = 1;
                    break;
                case EV_SCREEN_EFFECT_LINEAR_HOR_RIGHT:
                    show_screen_effect = NEMA_TRANS_LINEAR_H;
                    going_right   = 0;
                    break;
                case EV_SCREEN_EFFECT_LINEAR_VER_UP:
                    show_screen_effect = NEMA_TRANS_LINEAR_V;
                    going_right   = 1;
                    break;
                case EV_SCREEN_EFFECT_LINEAR_VER_DOWN:
                    show_screen_effect = NEMA_TRANS_LINEAR_V;
                    going_right   = 0;
                    break;
                case EV_SCREEN_EFFECT_FADE:
                    show_screen_effect = NEMA_TRANS_FADE;
                    going_right   = 1;
                    break;
                case EV_SCREEN_EFFECT_CUBE_HOR_LEFT:
                    show_screen_effect = NEMA_TRANS_CUBE_H;
                    going_right   = 0;
                    break;
                case EV_SCREEN_EFFECT_CUBE_HOR_RIGHT:
                    show_screen_effect = NEMA_TRANS_CUBE_H;
                    going_right   = 1;
                    break;
                case EV_SCREEN_EFFECT_FADE_ZOOM:
                    show_screen_effect = NEMA_TRANS_FADE_ZOOM;
                    going_right   = 1;
                    break;
            }

            //Bind transition buffers
            if ( going_right)
            {
                display_to_buffer(cur_screen_node, FRAME_BUFFERS-1);
                tree_node_t *t_git_node = event->target_tree_node;
                gitem_t     *t_git      = t_git_node->this_;
                set_current_screen_by_gitem(t_git);
                display_to_buffer(cur_screen_node, FRAME_BUFFERS-2);
            }
            else
            {
                display_to_buffer(cur_screen_node, FRAME_BUFFERS-2);
                tree_node_t *t_git_node = event->target_tree_node;
                gitem_t     *t_git      = t_git_node->this_;
                set_current_screen_by_gitem(t_git);
                display_to_buffer(cur_screen_node, FRAME_BUFFERS-1);
            }

            bind_transition_buffers();
            clear_cur_screen(0);
            show_screen = true;
        }// if ( event->progress == 0)

        step = event->progress;
        step = going_right ? step : 1.f-step;

        if ( event->progress >= 1.0)
        {
            show_screen = false;
            step = 0.f;
        }
    }
    else
    {
        tree_node_t *t_git_node = event->target_tree_node;
        gitem_t     *t_git      = t_git_node->this_;
        set_current_screen_by_gitem(t_git);
    }
}

void callback_pop_up(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;
    gitem_t     *t_git      = t_git_node->this_;
    do_popup    = 1;
    popup_gitem = t_git;
    set_current_popup_by_gitem(popup_gitem);
}

/* Round up the integer number "numToRound" to the closer multiple of "multiple"
 * (used in "show_hide_effect")
 */
static int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
    {
        return numToRound;
    }

    int remainder = numToRound % multiple;
    if (remainder == 0)
    {
        return numToRound;
    }

    return numToRound + multiple - remainder;
}

static void create_texture(tree_node_t* node)
{
    nema_cl_rewind(&cl);
    nema_cl_bind(&cl);

    //Back buffer is in RGBA5650, thus dimensions must be multiples of 4
    if ( node->this_->w_const % 4)
    {
        node->this_->w_const = roundUp(node->this_->w_const, 4);
    }
    if ( node->this_->h_const % 4)
    {
        node->this_->h_const = roundUp(node->this_->h_const, 4);
    }

    //Select free buffer
    if ( !TEX_BUFFER0_OCCUPIED || (TEX_BUFFER0_OCCUPIED && (node->this_->gesture_flags & GITEMF_TEX_BUFFER0)))
    {
        TEX_BUFFER0_OCCUPIED = 1;
        gitem_set_flag(node->this_, GITEMF_TEX_BUFFER0);
        nema_bind_dst_tex(fb[FRAME_BUFFERS-2].bo.base_phys, node->this_->w_const, node->this_->h_const,
                          fb[FRAME_BUFFERS-2].format, -1);
    }
    else
    {
        TEX_BUFFER1_OCCUPIED = 1;
        gitem_unset_flag(node->this_, GITEMF_TEX_BUFFER0);
        nema_bind_dst_tex(fb[FRAME_BUFFERS-1].bo.base_phys, node->this_->w_const, node->this_->h_const,
                          fb[FRAME_BUFFERS-1].format, -1);
    }

    gitem_to_tex(node, global_off_x, global_off_y, 0, 0, node->this_->w_const, node->this_->h_const);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);
}

static void reset_rendering_params(gitem_t* git)
{
    gitem_unset_flag(git, GITEMF_TEX_RENDERED);
    git->animation = NULL;
    if ( git->gesture_flags & GITEMF_TEX_BUFFER0)
    {
        gitem_unset_flag(git, GITEMF_TEX_BUFFER0);
        TEX_BUFFER0_OCCUPIED  = 0;
    }
    else
    {
        TEX_BUFFER1_OCCUPIED  = 0;
    }
}

void callback_show(event_t* event)
{
    tree_node_t *t_node = event->target_tree_node;
    gitem_t* t_git      = t_node->this_;

    //If this item is already shown or currently rendered and ritrigger != reset, consider this event completed
    if ( !(t_git->gesture_flags & GITEMF_HIDDEN) && !(t_git->gesture_flags & GITEMF_TEX_RENDERED) && event->re_trigger != EVENT_RESET)
    {
        event->progress = 1.f;
        return;
    }

    //If gitem is currently being hidden, cancel this event and proceed with show
    if ( t_git->animation && t_git->animation->action == ACTION_HIDE)
    {
        t_git->animation->status  = EV_STATUS_STOPPED;
        reset_rendering_params(t_git);
    }

    if ( event->effect > EV_EFFECT_NONE)
    {
        //if back buffers are occupied return
        if ( !t_git->animation && (TEX_BUFFER0_OCCUPIED && TEX_BUFFER1_OCCUPIED))
        {
            event->status  = EV_STATUS_STOPPED;
            return;
        }

        // progress = 0     => create texture if back buffer is available, assign animation to gitem
        // 0 < progress < 1 => do nothing, gitem will be rendered as texture according to its animation
        // progress = 1     => reset gitem animation, default drawing operation (not textured)
        if ( event->progress == 0.f)
        {
            if ( !TEX_BUFFER0_OCCUPIED || !TEX_BUFFER1_OCCUPIED)
            {
                t_git->animation = event;
                gitem_set_flag(t_git, GITEMF_HIDDEN);
                create_texture(t_node);
            }
            else
            {
                return;
            }
        } else if ( event->progress == 1.0)
        {
            gitem_unset_flag(t_git, GITEMF_HIDDEN);
            reset_rendering_params(t_git);
        }

    }
    else
    {
        t_git->x     = t_git->x_const;
        t_git->y     = t_git->y_const;
        t_git->w     = t_git->w_const;
        t_git->h     = t_git->h_const;
        t_git->color = t_git->color_const;
        t_git->animation = NULL;
        gitem_unset_flag(t_git, GITEMF_HIDDEN);
    }
}

void callback_hide(event_t* event)
{
        tree_node_t *t_node = event->target_tree_node;
    gitem_t* t_git      = t_node->this_;

    //If this item is already shown or currently rendered and ritrigger != reset, consider this event completed
    // if ( !(t_git->gesture_flags & GITEMF_HIDDEN) && !(t_git->gesture_flags & GITEMF_TEX_RENDERED) && event->re_trigger != EVENT_RESET){
    if ( (t_git->gesture_flags & GITEMF_HIDDEN) && !(t_git->gesture_flags & GITEMF_TEX_RENDERED) && event->re_trigger != EVENT_RESET)
    {
        event->progress = 1.f;
        return;
    }

    //If gitem is currently being hidden, cancel this event and proceed with show
    if ( t_git->animation && t_git->animation->action == ACTION_SHOW)
    {
        t_git->animation->status  = EV_STATUS_STOPPED;
        reset_rendering_params(t_git);
    }

    if ( event->effect > EV_EFFECT_NONE)
    {
        //if back buffers are occupied return
        if ( !t_git->animation && (TEX_BUFFER0_OCCUPIED && TEX_BUFFER1_OCCUPIED))
        {
            event->status  = EV_STATUS_STOPPED;
            return;
        }

        // progress = 0     => create texture if back buffer is available, assign animation to gitem
        // 0 < progress < 1 => do nothing, gitem will be rendered as texture according to its animation
        // progress = 1     => reset gitem animation, default drawing operation (not textured)
        if ( event->progress == 0.f)
        {
            if ( !TEX_BUFFER0_OCCUPIED || !TEX_BUFFER1_OCCUPIED)
            {
                t_git->animation = event;
                gitem_set_flag(t_git, GITEMF_HIDDEN);
                create_texture(t_node);
            }
            else
            {
                return;
            }
        } else if ( event->progress == 1.0)
        {
            t_git->animation = NULL;
            gitem_set_flag(t_git, GITEMF_HIDDEN);
            reset_rendering_params(t_git);
        }

    }
    else
    {
        t_git->animation = NULL;
        gitem_set_flag(t_git, GITEMF_HIDDEN);
    }
}

void callback_swipe_window_momentum(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;

    int index = 0;
    for ( int i = 0; i < TEMP_ANIMATIONS_COUNT; ++i )
    {
        if ( &animation_temp[i] == event)
        {
            index = i;
            break;
        }
    }

    int step_x = (int32_t)((1-event->progress)*dx_win[index] + dx_win[index]);
    int step_y = (int32_t)((1-event->progress)*dy_win[index] + dy_win[index]);

    int x_before = event->target_tree_node->first_child->this_->x;
    int y_before = event->target_tree_node->first_child->this_->y;

    swipe_window(t_git_node, 0, 0, step_x, step_y);

    int x_after  = event->target_tree_node->first_child->this_->x;
    int y_after  = event->target_tree_node->first_child->this_->y;

    //If position is the same as earlier, stop the event
    if ( x_before == x_after && y_before == y_after)
    {
        event->status = EV_STATUS_STOPPED;
    }
}

void callback_drag(event_t* event)
{
    tree_node_t *t_git_node = event->target_tree_node;
    gitem_t* t_git          = t_git_node->this_;

    if (t_git->type == GITEM_WINDOW)
    {
        gitem_t *subwindow = t_git_node->first_child->this_;

        if ( event->src_gitem->type == GITEM_V_SLIDER)
        {
            subwindow->y = (int32_t)((1.f - event->src_gitem->val)*(t_git->h - subwindow->h));
        }
        else if ( event->src_gitem->type == GITEM_H_SLIDER)
        {
            subwindow->x = (int32_t)(event->src_gitem->val*(t_git->w - subwindow->w));
        }
    }
}
