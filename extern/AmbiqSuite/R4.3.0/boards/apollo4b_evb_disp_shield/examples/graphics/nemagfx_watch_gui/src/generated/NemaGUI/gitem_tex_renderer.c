//*****************************************************************************
//
//! @file gitem_tex_renderer.c
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

void render_effect_fade(gitem_t* git)
{
    event_t* event = git->animation;
    // float progress = 0.f;

    // if (event->action == ACTION_SHOW)
    //     progress = event->progress;
    // else
    //     progress = 1 - event->progress;

    int opacity_const   = (git->color_const & 0xff000000)>>24;

    if (event->action == ACTION_SHOW)
    {
        int opacity_update  = (int)(nema_ez(0, opacity_const, 1.f, event->progress, event->ez_func));
        int bgr             = git->color & 0x00ffffff;
        git->color          = (opacity_update << 24) | bgr;
    }
    else
    {
        int opacity_update  = (int)(nema_ez(opacity_const, 0, 1.f, event->progress, event->ez_func));
        int bgr             = git->color & 0x00ffffff;
        git->color          = (opacity_update << 24) | bgr;
    }

    nema_blit_rect_fit(git->x, git->y, git->w, git->h);
}

void render_effect_fade_zoom(gitem_t* git)
{
    float progress = 0.f;
    event_t* event = git->animation;

    if ( event->action == ACTION_SHOW )
    {
        progress = event->progress;
    }
    else
    {
        progress = 1 - event->progress;
    }

    int opacity_const   = (git->color_const & 0xff000000)>>24;
    int opacity_update  = (int)(opacity_const*progress);
    int bgr             = git->color & 0x00ffffff;
    git->color          = (opacity_update << 24) | bgr;

    if ( event->action == ACTION_SHOW )
    {
        git->w = (int32_t)nema_ez(0, git->w_const, 1.f, event->progress, event->ez_func);
        git->h = (int32_t)nema_ez(0, git->h_const, 1.f, event->progress, event->ez_func);
        git->x = (int32_t)nema_ez(git->x_const + 0.5*git->w_const, git->x_const, 1.f, event->progress, event->ez_func);
        git->y = (int32_t)nema_ez(git->y_const + 0.5*git->h_const, git->y_const, 1.f, event->progress, event->ez_func);
    }
    else
    {
        git->w = (int32_t)nema_ez(git->w_const, 0, 1.f, event->progress, event->ez_func);
        git->h = (int32_t)nema_ez(git->h_const, 0, 1.f, event->progress, event->ez_func);
        git->x = (int32_t)nema_ez(git->x_const, git->x_const + 0.5*git->w_const, 1.f, event->progress, event->ez_func);
        git->y = (int32_t)nema_ez(git->y_const, git->y_const + 0.5*git->h_const, 1.f, event->progress, event->ez_func);
    }

    nema_blit_rect_fit(git->x, git->y, git->w, git->h);
}

void render_effect_fly(gitem_t* git)
{
    event_t* event = git->animation;
    if ( event->action == ACTION_SHOW )
    {
        git->w = (int32_t)nema_ez(0, git->w_const, 1.f, event->progress, event->ez_func);
        git->h = (int32_t)nema_ez(0, git->h_const, 1.f, event->progress, event->ez_func);
        git->x = (int32_t)nema_ez(0, git->x_const, 1.f, event->progress, event->ez_func);
        git->y = (int32_t)nema_ez(0, git->y_const, 1.f, event->progress, event->ez_func);
    }
    else
    {
        git->w = (int32_t)nema_ez(git->w_const,    0, 1.f, event->progress, event->ez_func);
        git->h = (int32_t)nema_ez(git->h_const,    0, 1.f, event->progress, event->ez_func);
        git->x = (int32_t)nema_ez(git->x_const, RESX, 1.f, event->progress, event->ez_func);
        git->y = (int32_t)nema_ez(git->y_const,    0, 1.f, event->progress, event->ez_func);
    }

    nema_blit_rect_fit(git->x, git->y, git->w, git->h);
}

void render_effect_cube_face(gitem_t *git)
{
    float box_size_2 = 0.5f;
    float FoV        = 1.1301023542f;
                   //x     y    z
    float v[18] =
    {
        -box_size_2, -box_size_2, -box_size_2,   //0
         box_size_2, -box_size_2, -box_size_2,   //1
         box_size_2,  box_size_2, -box_size_2,   //2
        -box_size_2,  box_size_2, -box_size_2,   //3
         box_size_2, -box_size_2,  box_size_2,   //4
         box_size_2,  box_size_2,  box_size_2    //5
    };

    nema_matrix4x4_t mvp;
    nema_mat4x4_load_perspective(mvp, FoV, 1, 1.f, 100.f);
    mvp[0][0] = mvp[1][1] = 2.f;

    nema_matrix4x4_t proj;
    nema_mat4x4_load_identity(proj);
    float angle = 0.f;
    if ( git->animation->action == ACTION_SHOW )
    {
        if ( git->animation->effect == EV_EFFECT_CUBE_LEFT)
        {
            angle = 90*(git->animation->progress -1);
            nema_mat4x4_rotate_Y(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_RIGHT )
        {
            angle = 90*(1-git->animation->progress);
            nema_mat4x4_rotate_Y(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_UP )
        {
            angle = 90*(1-git->animation->progress);
            nema_mat4x4_rotate_X(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_DOWN )
        {
            angle = 90*(git->animation->progress -1);
            nema_mat4x4_rotate_X(proj, angle);
        }

    }
    else
    {
        if ( git->animation->effect == EV_EFFECT_CUBE_LEFT)
        {
            angle = 90*git->animation->progress;
            nema_mat4x4_rotate_Y(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_RIGHT )
        {
            angle = -90*git->animation->progress;
            nema_mat4x4_rotate_Y(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_UP )
        {
            angle = -90*git->animation->progress;
            nema_mat4x4_rotate_X(proj, angle);
        } else if ( git->animation->effect == EV_EFFECT_CUBE_DOWN )
        {
            angle = 90*git->animation->progress;
            nema_mat4x4_rotate_X(proj, angle);
        }

    }

    //TODO: Select limits (70) dynamically
    if ( angle > 70.f)
    {
        return;
    }
    else if (angle < -70.f)
    {
        return;
    }

    nema_mat4x4_translate(proj, 0, 0, 1.5);
    nema_mat4x4_mul(mvp, mvp, proj);

    float w = 1.f;
    int i;
    for ( i = 0; i < 18; i += 3 )
    {
        nema_mat4x4_obj_to_win_coords(mvp, (float)git->x_const, (float)git->y_const, (int32_t)git->w_const, (int32_t)git->h_const,
                                      1.f, 100.f,
                                      &v[i  ], &v[i + 1], &v[i + 2], &w);
    }

    v[ 0] = (int)v[ 0]; v[ 1] = (int)v[ 1];
    v[ 3] = (int)v[ 3]; v[ 4] = (int)v[ 4];
    v[ 6] = (int)v[ 6]; v[ 7] = (int)v[ 7];
    v[ 9] = (int)v[ 9]; v[10] = (int)v[10];

    nema_blit_quad_fit(v[ 0], v[ 1],
                       v[ 3], v[ 4],
                       v[ 6], v[ 7],
                       v[ 9], v[10]);
}

void render_effect_flip_hor(gitem_t* git)
{

    float progr = git->animation->progress*0.5f;
    float hor_off = git->w_const*progr;
    float x0, x1, x2, x3, y0, y1, y2, y3;

    if ( git->animation->action == ACTION_SHOW)
    {
        x0 = git->x_const  + 0.5*git->w_const - hor_off;
        x1 = git->x_const  + 0.5*git->w_const + hor_off;
        x2 = x1;
        x3 = x0;

        y0 = git->y_const ;
        y1 = y0;
        y2 = git->y_const + git->h_const;
        y3 = y2;
    } else if ( git->animation->action == ACTION_HIDE )
    {
        x0 = git->x_const + hor_off;
        x1 = git->x_const + git->w_const - hor_off;
        x2 = x1;
        x3 = x0;

        y0 = git->y_const;
        y1 = y0;
        y2 = git->y_const + git->h_const;
        y3 = y2;
    }

    nema_blit_quad_fit(
                   x0, y0,
                   x1, y1,
                   x2, y2,
                   x3, y3);
}

void render_effect_flip_ver(gitem_t* git)
{

    float progr = git->animation->progress*0.5f;
    float ver_off = git->h_const*progr;
    float x0, x1, x2, x3, y0, y1, y2, y3;
    if ( git->animation->action == ACTION_SHOW)
    {
        x0 = git->x_const;
        x1 = git->x_const + git->w_const;
        x2 = x1;
        x3 = x0;

        y0 = git->y_const   + 0.5*git->h_const - ver_off;
        y1 = y0;
        y2 = git->y_const + git->h_const - ver_off;
        y3 = y2;
    } else if ( git->animation->action == ACTION_HIDE )
    {
        x0 = git->x_const;
        x1 = git->x_const + git->w_const;
        x2 = x1;
        x3 = x0;

        y0 = git->y_const + ver_off;
        y1 = y0;
        y2 = git->y_const + git->h_const - ver_off;
        y3 = y2;
    }

    nema_blit_quad_fit(
                   x0, y0,
                   x1, y1,
                   x2, y2,
                   x3, y3);
}

void (*render_tex[]) (gitem_t* git) =
{
    0,                       //0
    render_effect_fade,      //1
    render_effect_fade_zoom, //2
    render_effect_fly,       //3
    render_effect_cube_face, //4 cubeface left
    render_effect_cube_face, //5 cubeface right
    render_effect_cube_face, //6 cubeface up
    render_effect_cube_face, //7 cubface down
    render_effect_flip_hor,  //8
    render_effect_flip_ver,  //9
};
