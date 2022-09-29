//*****************************************************************************
//
//! @file display.c
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

void display_to_framebuffer(int fb_id)
{
    nema_cl_rewind(&cl_screen);
    nema_cl_bind(&cl_screen);
        //Bind Framebuffer
        nema_bind_dst_tex(cur_fb_base_phys, fb[fb_id].w, fb[fb_id].h, fb[fb_id].format, -1);
        draw_tree_node(screen_tree_nodes[cur_screen_node], global_off_x, global_off_y, 0, 0, RESX, RESY);

    nema_cl_submit_no_irq(&cl_screen);

    nema_cl_rewind(&cl);
    nema_cl_bind(&cl);
        //Bind Framebuffer
        nema_bind_dst_tex(cur_fb_base_phys, fb[fb_id].w, fb[fb_id].h, fb[fb_id].format, -1);
        draw_tree(screen_tree_nodes[cur_screen_node]->first_child, global_off_x, global_off_y, 0, 0, RESX, RESY);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);
}

void display_popup()
{
    display_to_framebuffer(0);

    nema_cl_rewind(&cl);
    nema_cl_bind(&cl);
    //Bind Framebuffer
    nema_bind_dst_tex(cur_fb_base_phys, fb[0].w, fb[0].h, fb[0].format, -1);

    nema_set_blend_fill(NEMA_BL_SRC_OVER);
    nema_set_clip (0, 0, RESX, RESY);
    nema_fill_rect(0, 0, RESX, RESY, 0xd0000000);

    draw_tree(popup_node, popup_off_x, popup_off_y, 0, 0, RESX, RESY);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);
}

void display_to_buffer(int screen_node, int fb_id)
{
    uintptr_t tmp_fb_base_phys = cur_fb_base_phys;
    int       tmp_screen_node  = cur_screen_node;

    cur_fb_base_phys = fb[fb_id].bo.base_phys;
    cur_screen_node  = screen_node;
    display_to_framebuffer(fb_id);

    cur_fb_base_phys = tmp_fb_base_phys;
    cur_screen_node  = tmp_screen_node;
}

void clear_cur_screen(int wait)
{
    nema_cl_rewind(&cl_screen);
    nema_cl_bind(&cl_screen);
        //Bind Framebuffer
        nema_bind_dst_tex(cur_fb_base_phys, fb[0].w, fb[0].h, fb[0].format, -1);
        nema_set_blend_fill(NEMA_BL_SRC);
        nema_set_clip (0, 0, RESX, RESY);
        nema_fill_rect(0, 0, RESX, RESY, 0);

    if (wait)
    {
        nema_cl_submit(&cl_screen);
        nema_cl_wait(&cl_screen);
    }
    else
    {
        nema_cl_submit_no_irq(&cl_screen);
    }
}

void display_screen_animation(float cur_step, nema_transition_t effect)
{
    nema_cl_rewind(&cl);
    nema_cl_bind(&cl);
    //Bind Framebuffer
    nema_bind_dst_tex(cur_fb_base_phys, fb[0].w, fb[0].h, fb[0].format, -1);
    nema_set_clip (0, 0, RESX, RESY);

    nema_transition(effect, NEMA_TEX1, NEMA_TEX2, NEMA_BL_SRC, nema_ez_linear(cur_step), fb[0].w, fb[0].h);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);
}

void display()
{
    if (doing_transition)
    {
        display_screen_animation( going_right ? step : 1.f-step, group_effect[cur_screen_group] );
    } else if (do_popup)
    {
        display_popup();
    } else if (show_screen)
    {
        if ( step > 1.f )
        {
            step = 1.f;
        }
        display_screen_animation( step, show_screen_effect );
    }
    else
    {
        display_to_framebuffer(0);
    }

    cur_fb_base_phys = nema_swap_fb(0);
    nemadc_set_layer(0, &layer[0]);
    cur_fb_base_phys = nema_swap_fb(0);
    nemadc_set_layer(1, &layer[0]);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    #ifdef CALCULATE_FPS
    nema_calculate_fps();
    #endif

    nema_transition_t effect = group_effect[cur_screen_group];
    if ( doing_transition  && effect != NEMA_TRANS_LINEAR_H )
    {
        clear_cur_screen(0);
    }
}

void bind_transition_buffers(void)
{
    nema_cl_rewind(&cl);
    nema_cl_bind(&cl);
    nema_bind_tex(NEMA_TEX1, fb[FRAME_BUFFERS-2].bo.base_phys, fb[0].w, fb[0].h, fb[FRAME_BUFFERS-2].format, -1, NEMA_FILTER_BL);
    nema_bind_tex(NEMA_TEX2, fb[FRAME_BUFFERS-1].bo.base_phys, fb[0].w, fb[0].h, fb[FRAME_BUFFERS-2].format, -1, NEMA_FILTER_BL);
    nema_cl_submit(&cl);
    nema_cl_wait(&cl);
}
