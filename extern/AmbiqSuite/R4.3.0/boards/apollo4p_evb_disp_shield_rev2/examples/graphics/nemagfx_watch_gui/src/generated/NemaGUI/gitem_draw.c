//*****************************************************************************
//
//! @file gitem_draw.c
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

#include "gitem.h"
#include "nema_interpolators.h"

#define FLAG_BLEND_SRC (1 << 0)
unsigned _flags;

void nema_set_raster_color(uint32_t rgba8888);
void nema_raster_triangle_fx(int x0fx, int y0fx, int x1fx, int y1fx, int x2fx, int y2fx);

void draw_circle(gitem_t *git, int x_off, int y_off)
{
    if ( (git->color & 0xff000000) == 0)
    {
        return;
    }

    if ( (git->color & 0xff000000) == 0xff000000 )
    {
        nema_set_blend_fill(NEMA_BL_SRC);
    }
    else
    {
        nema_set_blend_fill(NEMA_BL_SIMPLE);
    }

    int r  = git->w>>1;
    int x0 = git->x + x_off + r;
    int y0 = git->y + y_off + r;

    if (git->fill)
    {
        nema_fill_circle(x0, y0, r, git->color);
    }
    else
    {
        nema_draw_circle(x0, y0, r, git->color);
    }
}

void draw_rect(gitem_t *git, int x_off, int y_off)
{
    uint32_t color = git->highlighted ? (git->color2 & 0xffffff) | (git->color & 0xff000000) : git->color;

    if ( (color & 0xff000000) == 0)
    {
        return;
    }

    if ( (color & 0xff000000) == 0xff000000 )
    {
        nema_set_blend_fill(NEMA_BL_SRC);
    }
    else
    {
        nema_set_blend_fill(NEMA_BL_SIMPLE);
    }

    if (git->fill)
    {
        nema_fill_rect(git->x + x_off, git->y + y_off, git->w, git->h, color);
    }
    else
    {
        if ( git->penwidth <= 1)
        {
            nema_draw_rect(git->x + x_off, git->y + y_off, git->w, git->h, color);
        }
        else
        {
            nema_fill_rect(git->x + x_off, git->y + y_off, git->w, git->penwidth, color);
            nema_fill_rect(git->x + x_off, git->y + y_off + git->h-git->penwidth, git->w, git->penwidth, color);
            nema_fill_rect(git->x + x_off, git->y + y_off + git->penwidth, git->penwidth, git->h - 2 * git->penwidth, color);
            nema_fill_rect(git->x + x_off + git->w-git->penwidth, git->y + y_off + git->penwidth, git->penwidth, git->h - 2 * git->penwidth, color);
        }
    }
}

void draw_rrect(gitem_t *git, int x_off, int y_off)
{
    if ( (git->color & 0xff000000) == 0)
    {
        return;
    }

    if ( (git->color & 0xff000000) == 0xff000000 )
    {
        nema_set_blend_fill(NEMA_BL_SRC);
    }
    else
    {
        nema_set_blend_fill(NEMA_BL_SIMPLE);
    }

    if (git->fill)
    {
        nema_fill_rounded_rect(git->x + x_off, git->y + y_off, git->w, git->h, git->r, git->color);
    }
    else
    {
        nema_draw_rounded_rect(git->x + x_off, git->y + y_off, git->w, git->h, git->r, git->color);
    }
}

void draw_image(gitem_t *git, int x_off, int y_off)
{
    int img_asset_id = git->highlighted && git->img_asset_id2 >= 0 ? git->img_asset_id2 : git->img_asset_id;
    if (img_asset_id < 0)
    {
        return;
    }

    if (_flags == FLAG_BLEND_SRC)
    {
        nema_set_blend_blit(NEMA_BL_SRC);
    }
    else
    {
        int opacity = git->color>>24;
        if (opacity < 0xff)
        {
            nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
            //set opacity to const color (uint32_t) needed for blending
            nema_set_const_color(opacity << 24);
        }
        else
        {
            nema_set_blend_blit(NEMA_BL_SIMPLE);
        }
    }

    //Colorize A8 images
    if ( image_assets[img_asset_id]->format == NEMA_A8)
    {
        nema_set_tex_color(image_assets[img_asset_id]->color);
    }

    nema_bind_src_tex(image_assets[img_asset_id]->bo.base_phys, image_assets[img_asset_id]->w, image_assets[img_asset_id]->h,
                      image_assets[img_asset_id]->format, image_assets[img_asset_id]->stride, image_assets[img_asset_id]->sampling_mode);
    nema_blit_rect_fit(git->x + x_off, git->y + y_off, git->w, git->h);

}

void draw_container(gitem_t *git, int x_off, int y_off)
{
    if (git->bg_image)
    {
        draw_image(git, x_off, y_off);
    }
    else
    {
        draw_rect(git, x_off, y_off);
    }
}

void draw_rotating_cont(gitem_t *git, int x_off, int y_off)
{
    if (git->bg_image)
    {
        int img_asset_id = git->highlighted && git->img_asset_id2 >= 0 ? git->img_asset_id2 : git->img_asset_id;

        if (img_asset_id < 0)
        {
            return;
        }

        if (_flags == FLAG_BLEND_SRC)
        {
            nema_set_blend_blit(NEMA_BL_SRC);
        }
        else
        {
            int opacity = git->color >> 24;
            if (opacity < 0xff)
            {
                nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
                nema_set_const_color(git->color);
            }
            else
            {
                nema_set_blend_blit(NEMA_BL_SIMPLE);
            }
        }

        nema_set_tex_color(git->color);
        nema_bind_src_tex(image_assets[img_asset_id]->bo.base_phys, image_assets[img_asset_id]->w, image_assets[img_asset_id]->h,
                          image_assets[img_asset_id]->format, image_assets[img_asset_id]->stride, image_assets[img_asset_id]->sampling_mode);

    }
    else
    {
        uint32_t color = git->highlighted ? (git->color2 & 0xffffff) | (git->color & 0xff000000) : git->color;

        if ( (color & 0xff000000) == 0)
        {
            return;
        }

        if ( (color & 0xff000000) == 0xff000000 )
        {
            nema_set_blend_fill(NEMA_BL_SRC);
        }
        else
        {
            nema_set_blend_fill(NEMA_BL_SIMPLE);
        }

        nema_set_raster_color(git->color);
    }

    float sin_theta = nema_sin(-git->angle);
    float cos_theta = nema_cos(-git->angle);

    //  P0-------P1
    //  | .rot    |
    //  P3-------P2
    //move rotation center to (0, 0)
    float x0 = -git->x_rot;
    float y0 = -git->y_rot;
    float x1 = x0 + git->w;
    float y1 = y0;
    float x2 = x1;
    float y2 = y1 + git->h;
    float x3 = x0;
    float y3 = y2;

    //rotate geometry
    float tmp;
    tmp =  x0*cos_theta + y0*sin_theta;    y0  = -x0*sin_theta + y0*cos_theta;    x0  = tmp;
    tmp =  x1*cos_theta + y1*sin_theta;    y1  = -x1*sin_theta + y1*cos_theta;    x1  = tmp;
    tmp =  x2*cos_theta + y2*sin_theta;    y2  = -x2*sin_theta + y2*cos_theta;    x2  = tmp;
    tmp =  x3*cos_theta + y3*sin_theta;    y3  = -x3*sin_theta + y3*cos_theta;    x3  = tmp;

    //translate rotation center to final coords
    int _trans_x = git->x + x_off;
    int _trans_y = git->y + y_off;

    x0 += _trans_x;    y0 += _trans_y;
    x1 += _trans_x;    y1 += _trans_y;
    x2 += _trans_x;    y2 += _trans_y;
    x3 += _trans_x;    y3 += _trans_y;

    nema_blit_quad_fit( x0, y0,
                        x1, y1,
                        x2, y2,
                        x3, y3);

}

void draw_label(gitem_t *git, int x_off, int y_off)
{
    if (git->fill)
    {
        draw_rect(git, x_off, y_off);
    }

    nema_bind_font(font_assets[git->font_asset_id]);

    int font_x = git->x + x_off;
    int font_y = git->y + y_off;

    nema_print(git->text, font_x, font_y, git->w, git->h, git->color2, git->text_alignment);
}

void float2str(char *str, int size, float f, int precision)
{
    //precision must be [0, 4]
    //str size should be enough

    int tmp_prec = precision;
    int multiplier = 1;
    float f_round_bias = f < 0.f ? -0.5f : 0.5f;

    while ( tmp_prec > 0 )
    {
        multiplier *= 10;
        --tmp_prec;
    }

    int tmp = (int32_t)(f * (float)multiplier + f_round_bias);
    if (tmp < 0)
    {
        tmp = -tmp;
    }

    str[size-1] = '\0';
    int i = size-2;

    if (precision > 0)
    {
        do
        {
            str[i--] = '0' + (tmp % 10);
            tmp /= 10;
            --precision;
        } while ( precision > 0 );
        str[i--] = '.';
    }

    do
    {
        str[i--] = '0' + (tmp % 10);
        tmp /= 10;
    } while (tmp != 0);

    if (i == size-2)
    {
        str[i--] = '0';
    }
    if (f < 0.f)
    {
        str[i--] = '-';
    }

    ++i;
    if (i > 0)
    {
        int j = 0;
        while ( i <= size - 1 )
        {
            str[j] = str[i];
            ++j;
            ++i;
        }
    }
}

void draw_digimeter(gitem_t *git, int x_off, int y_off)
{
    if (git->fill)
    {
        draw_rect(git, x_off, y_off);
    }

    nema_bind_font(font_assets[git->font_asset_id]);

    char text[20];

    float2str(text, 20, git->val, git->precision);

    int font_x = git->x + x_off;
    int font_y = git->y + y_off;

    nema_print(text, font_x, font_y, git->w, git->h, git->color2, NEMA_ALIGNX_CENTER | NEMA_ALIGNY_CENTER);
}

void draw_radio(gitem_t *git, int x_off, int y_off)
{
    if ( (git->color & 0xff000000) == 0)
    {
        return;
    }

    if ( (git->color & 0xff000000) == 0xff000000 )
    {
        nema_set_blend_fill(NEMA_BL_SRC);
    }
    else
    {
        nema_set_blend_fill(NEMA_BL_SIMPLE);
    }

    int r  = git->w >> 1;
    int x0 = git->x + x_off + r;
    int y0 = git->y + y_off + r;

    if (git->checked)
    {
        nema_fill_circle(x0, y0, r - 4, git->color);
    }

    nema_draw_circle(x0, y0, r, git->color);
}

void draw_checkbox(gitem_t *git, int x_off, int y_off)
{
    int tmp_fill = git->fill;
    git->fill = 0;
    draw_rect(git, x_off, y_off);
    git->fill = tmp_fill;

    if (git->checked)
    {
        if (git->fill)
        {
            if ( (git->color & 0xff000000) == 0)
            {
                return;
            }

            if ( (git->color & 0xff000000) == 0xff000000 )
            {
                nema_set_blend_fill(NEMA_BL_SRC);
            }
            else
            {
                nema_set_blend_fill(NEMA_BL_SIMPLE);
            }

            nema_fill_rect(git->x + x_off + 4, git->y + y_off + 4, git->w - 8, git->h - 8, git->color);
        }
        else
        {
            draw_container(git, x_off, y_off);
        }
    }
}

void draw_circular_progress(gitem_t *git, int x_off, int y_off)
{
    // Draw background texture
    if (git->bg_image)
    {
        git->highlighted = 0;
        draw_image(git, x_off, y_off);
    }

    //Draw foreground texture
    int img_asset_id = git->img_asset_id2;

    if (img_asset_id < 0)
    {
        return;
    }

    int img_w = image_assets[img_asset_id]->w;
    int img_h = image_assets[img_asset_id]->h;
    nema_interpolate_tx_ty( git->x + x_off         , git->y + y_off         , 1.f,   0.f,   0.f,
                            git->x + x_off + git->w, git->y + y_off         , 1.f, img_w,   0.f,
                            git->x + x_off + git->w, git->y + y_off + git->h, 1.f, img_w, img_h,
                            img_w, img_h);

    // If necessary colorize the texture
    if (_flags == FLAG_BLEND_SRC)
    {
        nema_set_blend_blit(NEMA_BL_SRC);
    }
    else
    {
        int opacity = git->color>>24;
        if (opacity < 0xff)
        {
            nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
            nema_set_const_color(git->color);
        }
        else
        {
            nema_set_blend_blit(NEMA_BL_SIMPLE);
        }
    }
    nema_set_tex_color(git->color);
    nema_bind_src_tex(image_assets[img_asset_id]->bo.base_phys, image_assets[img_asset_id]->w, image_assets[img_asset_id]->h,
                      image_assets[img_asset_id]->format, image_assets[img_asset_id]->stride, image_assets[img_asset_id]->sampling_mode);

    int start_angle = (int32_t)(git->min_angle);
    int end_angle   = (int32_t)(git->val*(git->max_angle - git->min_angle) + git->min_angle);

    //Works also without if-sttement
    if ( start_angle < 0 || end_angle < 0)
    {
        start_angle += 360;
        end_angle   += 360;
    }

    if (end_angle < start_angle)
    {
        end_angle += 360;
    }

    int start_quartile = ((start_angle + 360 + 45)) / 90;
    int end_quartile   = ((end_angle + 360 + 45)) / 90;

    if (start_quartile == end_quartile && end_angle > (start_quartile * 90) + 45 )
    {
        end_quartile += 4;
    }

    if (end_quartile < start_quartile)
    {
        end_quartile += 4;
    }

    double start_x, start_y;
    double end_x, end_y;

    switch (start_quartile % 4)
    {
        case 0:
            start_x = 0;
            start_y = 0.5*((double)(git->h) - nema_tan(start_angle)*(double)(git->w));
            break;
        case 1:
            start_x = 0.5f*(git->w + nema_tan(start_angle-90)*git->h);
            start_y = 0;
            break;
        case 2:
           start_x = git->w;
           start_y = 0.5f*(git->h + nema_tan(start_angle)*git->w);
           break;
        case 3:
        default:
           start_x = 0.5f*(git->w - nema_tan(start_angle-90)*git->h);
           start_y = git->h;
           break;
    }

    switch (end_quartile % 4)
    {
        case 0:
            end_x = 0;
            end_y = 0.5*((double)(git->h) - nema_tan(end_angle)*(double)(git->w));
           break;
        case 1:
            end_x = 0.5f*(git->w + nema_tan(end_angle-90)*git->h);
            end_y = 0;
           break;
        case 2:
           end_x = git->w;
           end_y = 0.5f*(git->h + nema_tan(end_angle)*git->w);
           break;
        case 3:
        default:
           end_x = 0.5f*(git->w - nema_tan(end_angle-90)*git->h);
           end_y = git->h;
           break;
    }

    int i  = start_quartile;

    int x0 = (int32_t)start_x;
    int y0 = (int32_t)start_y;

    int x1 = 0;
    int y1 = 0;

    for ( ; i <= end_quartile; ++i )
    {
        if (i != end_quartile)
        {
            switch (i % 4)
            {
                case 0:
                    x1 = 0; y1 = 0;
                    break;
                case 1:
                    x1 = git->w; y1 = 0;
                    break;
                case 2:
                    x1 = git->w; y1 = git->h;
                    break;
                case 3:
                default:
                    x1 = 0; y1 = git->h;
                    break;
            }
        }
        else
        {
           x1 = (int32_t)end_x;
           y1 = (int32_t)end_y;
        }

        nema_raster_triangle_fx( nema_i2fx(git->x + x_off + git->w / 2), nema_i2fx(git->y + y_off + git->h / 2),
                                 nema_i2fx(git->x + x_off + x0),         nema_i2fx(git->y + y_off + y0),
                                 nema_i2fx(git->x + x_off + x1),         nema_i2fx(git->y + y_off + y1));

        x0 = x1;
        y0 = y1;
    }
}

void (*draw_gitem[]) (gitem_t *git, int x_off, int y_off) =
{
    draw_container,         //GITEM_SCREEN
    draw_container,         //GITEM_CONT
    draw_circle,            //GITEM_CIRCLE
    draw_rect,              //GITEM_RECT
    draw_rrect,             //GITEM_RRECT
    draw_image,             //GITEM_IMG
    draw_label,             //GITEM_LABEL
    draw_container,         //GITEM_LABEL_BUTTON
    draw_checkbox,          //GITEM_CHECKBOX
    draw_radio,             //GITEM_RADIO
    0,                      //GITEM_H_SLIDER
    0,                      //GITEM_V_SLIDER
    draw_digimeter,         //GITEM_DIGIMETER
    draw_container,         //GITEM_POPUP
    0,                      //GITEM_ICON
    0,                      //GITEM_MENU
    draw_rect,              //GITEM_PROGRESS_BAR
    draw_container,         //GITEM_GAUGE
    draw_rotating_cont,     //GITEM_ROTATING_CONTAINER
    draw_container,         //GITEM_ICON_BUTTON
    0,                      //GITEM_WINDOW
    draw_circular_progress, //GITEM_CIRCULAR_PROGRESS
    draw_container,         //GITEM_WATCH_FACE
};

#include "gitem_tex_renderer.c"

void draw_tree_node(tree_node_t *node, int x_off, int y_off, int x_min, int y_min, int x_max, int y_max)
{
    _flags = 0;
    if (node->parent == 0)
    {
        _flags = FLAG_BLEND_SRC;
    }
    //draw current gitem
    if ( /*node->this_->type >= 0 &&*/ draw_gitem[node->this_->type])
    {
        x_min = x_min > 0 ? x_min : 0;
        y_min = y_min > 0 ? y_min : 0;
        if (x_min < x_max && y_min < y_max)
        {
            gitem_t *git = node->this_;

            //Gitem is rendered as texture
            if ( (git->gesture_flags & GITEMF_TEX_RENDERED) && (git->animation))
            {
                //Set blending mode
                if ( (git->color & 0xff000000) == 0xff000000)
                {
                    nema_set_blend_blit(NEMA_BL_SRC);
                }
                else
                {
                    nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
                    nema_set_const_color(git->color);
                }

                //Select source buffer
                if ( git->gesture_flags & GITEMF_TEX_BUFFER0)
                {
                    nema_bind_src_tex(fb[FRAME_BUFFERS-2].bo.base_phys, node->this_->w_const, node->this_->h_const,
                                      fb[FRAME_BUFFERS-2].format, -1,
                                      fb[FRAME_BUFFERS-2].sampling_mode);
                }
                else
                {
                    nema_bind_src_tex(fb[FRAME_BUFFERS-1].bo.base_phys, node->this_->w_const, node->this_->h_const,
                                      fb[FRAME_BUFFERS-1].format, -1,
                                      fb[FRAME_BUFFERS-1].sampling_mode);
                }

                nema_set_clip(x_min, y_min, x_max-x_min, y_max-y_min);
                nema_set_tex_color(git->color);

                render_tex[git->animation->effect - EV_SCREEN_EFFECT_COUNT - 1](git);

            //Simple draw operations
            }
            else
            {
                nema_set_clip(x_min, y_min, x_max-x_min, y_max-y_min);
                draw_gitem[git->type](git, x_off, y_off);
            }
        }
    }
}

//draws tree
void draw_tree(tree_node_t *node, int x_off, int y_off, int x_min, int y_min, int x_max, int y_max)
{
    while (node && node->this_)
    {
        // a) Draw simply rendered items
        if ( (node->this_->gesture_flags&GITEMF_HIDDEN) == 0)
        {
            //draw current gitem
            draw_tree_node(node, x_off, y_off, x_min, y_min, x_max, y_max);

            //draw children if this is a container of some kind
            if (node->first_child)
            {
                int cur_x_min = x_off     + node->this_->x;
                int cur_y_min = y_off     + node->this_->y;
                int cur_x_max = cur_x_min + node->this_->w;
                int cur_y_max = cur_y_min + node->this_->h;

                if (x_min > cur_x_min)
                {
                    cur_x_min = x_min;
                }
                if (y_min > cur_y_min)
                {
                    cur_y_min = y_min;
                }
                if (x_max < cur_x_max)
                {
                    cur_x_max = x_max;
                }
                if (y_max < cur_y_max)
                {
                    cur_y_max = y_max;
                }

                draw_tree(node->first_child, x_off + node->this_->x, y_off + node->this_->y,
                          cur_x_min, cur_y_min,
                          cur_x_max, cur_y_max);
            }

        // b) Draw animated items
        } else if (node->this_->gesture_flags & GITEMF_TEX_RENDERED)
        {
            draw_tree_node(node, x_off, y_off, x_min, y_min, x_max, y_max);
        }

        //got to next item at the same level;
        node = node->next;
    }
}

/*
 *Draws a tree_node inside the destination buffer at its native resolution
 */
void draw_sub_tree_node(tree_node_t *node, int x_off, int y_off, int x_min, int y_min, int x_max, int y_max)
{
    _flags = 0;
    if (node->parent == 0)
    {
        _flags = FLAG_BLEND_SRC;
    }

    //draw current gitem
    if (/*node->this_->type >= 0 &&*/ draw_gitem[node->this_->type])
    {
        x_min = x_min > 0 ? x_min : 0;
        y_min = y_min > 0 ? y_min : 0;
        node->this_->color = node->this_->color_const;
        if (x_min < x_max && y_min < y_max)
        {
            nema_set_clip(x_min, y_min, x_max-x_min, y_max-y_min);
            draw_gitem[node->this_->type](node->this_, x_off, y_off);
        }
    }
}

/*
 * Draws a tree_node and its children inside the destination buffer at its native resolution (uses 'draw_sub_tree_node')
 */
void gitem_to_tex(tree_node_t *node/*, event_t *event*/, int x_off, int y_off, int x_min, int y_min, int x_max, int y_max)
{
    //Draw target gitem and continue with its children
    // if ( node == event->target_tree_node){
    if ( node)
    {
        gitem_set_flag(node->this_, GITEMF_TEX_RENDERED);
        nema_set_clip(0, 0, node->this_->w_const, node->this_->h_const);

        //Draw gitem in the beggining of the dst buffer at native resolution
        node->this_->x     = 0;
        node->this_->y     = 0;
        node->this_->w     = node->this_->w_const;
        node->this_->h     = node->this_->h_const;
        node->this_->color = node->this_->color_const;

        //Clear current context
        nema_set_blend_fill(NEMA_BL_SRC);
        nema_fill_rect(0, 0, node->this_->w, node->this_->h, node->this_->color_const | 0xff000000);

        //Draw current node
        draw_gitem[node->this_->type](node->this_, 0, 0);

        //Restore previous values
        node->this_->x = node->this_->x_const;
        node->this_->y = node->this_->y_const;

        // if ( node == event->target_tree_node)
        node = node->first_child;
    }

    while (node && node->this_)
    {
        if ( (node->this_->gesture_flags&GITEMF_HIDDEN) == 0)
        {
            //draw current gitem
            draw_sub_tree_node(node, x_off, y_off, x_min, y_min, x_max, y_max);

            //draw children if this is a container of some kind
            if (node->first_child)
            {

                int cur_x_min = x_off     + node->this_->x_const;
                int cur_y_min = y_off     + node->this_->y_const;
                int cur_x_max = cur_x_min + node->this_->w_const;
                int cur_y_max = cur_y_min + node->this_->h_const;

                if (x_min > cur_x_min)
                {
                    cur_x_min = x_min;
                }
                if (y_min > cur_y_min)
                {
                    cur_y_min = y_min;
                }
                if (x_max < cur_x_max)
                {
                    cur_x_max = x_max;
                }
                if (y_max < cur_y_max)
                {
                    cur_y_max = y_max;
                }

                gitem_to_tex(node->first_child, x_off + node->this_->x_const, y_off + node->this_->y_const,
                             cur_x_min, cur_y_min,
                             cur_x_max, cur_y_max);
            }
        }

        //go to next item at the same level
        node = node->next;
    }
}
