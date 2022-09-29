//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! this example use two frame buffer demonstrate a digital rotating clock,
//! with Nema GPU support, the shader effect continue shows while timer passing
//! need a timer to get the accurate time past.
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

#include "am_bsp.h"
#include "nema_core.h"
#include "nema_utils.h"
#include "nema_easing.h"
#include "am_hal_global.h"
#include "nema_programHW.h"
#include "string.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"
#include "numbers_rgba.h"

// #define CUSTOM_TIME
#define RESX 384
#define RESY 192

#define PI 3.14159265f

#define DEG2RAD(a)  ((float)(a) * PI / 180.0f)
#define CHAR_ANGLE  (360.f / (PLEURES))

#define PLEURES  (10)
#define ALTITUDE (0.8f)
#define WIDTH2   (0.259936f)

static int32_t VIEWPORT_WIDTH  = RESX;
static int32_t VIEWPORT_HEIGHT = (RESX / 4 * 3);
#define ZNEAR           (1)
#define ZFAR            (15)

#define BG_COLOR        (0)

#define TEX_DIM       80
#define TEX_PIXSIZE   2

#define CLIP_YMIN     ((int32_t)(0.091f * VIEWPORT_HEIGHT)     )
#define CLIP_YMAX     ((int32_t)(0.609f * VIEWPORT_HEIGHT)     )
#define CLIP_XMIN     ((int32_t)(0.05625f * VIEWPORT_WIDTH)    )
#define CLIP_XMAX     ((int32_t)(VIEWPORT_WIDTH - CLIP_XMIN)  )

#define CLIP_SEC01    ((int32_t)(0.71875f * VIEWPORT_WIDTH) + 35 )
#define CLIP_SEC10    ((int32_t)(0.64375f * VIEWPORT_WIDTH)    )
#define CLIP_MIN01    ((int32_t)(0.5f * VIEWPORT_WIDTH)        )
#define CLIP_MIN10    ((int32_t)(VIEWPORT_WIDTH - CLIP_SEC10) )
#define CLIP_HOUR01   ((int32_t)(VIEWPORT_WIDTH - CLIP_SEC01) )

static int32_t g_i32ClipX = 0; //!< store current clipping area each time
static int32_t g_i32ClipY = 0;
static int32_t g_i32ClipW = 0;
static int32_t g_i32ClipH = 0;

static int32_t g_i32MinX = 0; //!< store previous dirty region area each time
static int32_t g_i32MinY = 0;
static int32_t g_i32MaxX = 0;
static int32_t g_i32MaxY = 0;
//
// Framebuffer Related
//

static nema_cmdlist_t g_sCL;
static nema_cmdlist_t g_sCLBlitFB;

static uintptr_t prv_fb_base_phys;
static uintptr_t cur_fb_base_phys;
static uint32_t g_ui32CurFB = 0;
static uint32_t g_ui32PrvFB = 0;

#define FRAME_BUFFERS 2
static img_obj_t g_sFB[FRAME_BUFFERS];

#ifndef DONT_USE_NEMADC
static nemadc_layer_t g_sDCLayer[FRAME_BUFFERS];
#endif

static img_obj_t g_sNumbersImg = {{0}, TEX_DIM, TEX_DIM * 10, TEX_DIM * 4, 0, NEMA_RGBA8888, NEMA_FILTER_BL};

static void
load_objects(void)
{
    for (int32_t i = 0; i < FRAME_BUFFERS; ++i)
    {
        g_sFB[i].w = RESX;
        g_sFB[i].h = RESY;
        g_sFB[i].format = NEMA_RGB565;
        g_sFB[i].stride = RESX * 2;
        g_sFB[i].bo = nema_buffer_create(g_sFB[i].stride * g_sFB[i].h);
        nema_buffer_map(&g_sFB[i].bo);

#ifndef DONT_USE_NEMADC
        g_sDCLayer[i].sizex = g_sDCLayer[i].resx = g_sFB[i].w;
        g_sDCLayer[i].sizey = g_sDCLayer[i].resy = g_sFB[i].h;
        g_sDCLayer[i].stride = g_sFB[i].stride;
        g_sDCLayer[i].format = NEMADC_RGB565;
        g_sDCLayer[i].blendmode = NEMADC_BL_SRC;
        g_sDCLayer[i].baseaddr_phys = g_sFB[i].bo.base_phys;
        g_sDCLayer[i].baseaddr_virt = g_sFB[i].bo.base_virt;
        g_sDCLayer[i].flipx_en      = 0;
        g_sDCLayer[i].flipy_en      = 0;
#endif
        printf("FB: V:%p P:0x%08x\n", (void *)g_sFB[i].bo.base_virt, g_sFB[i].bo.base_phys);
    }

    g_sNumbersImg.bo = nema_buffer_create(i32NumbersRGBALength);
    memcpy((void*)(g_sNumbersImg.bo.base_phys), g_ui8NumbersRGBA, i32NumbersRGBALength);
}

static float prev_hour10_f ;
static float prev_hour01_f ;
static float prev_min10_f  ;
static float prev_min01_f  ;
static float prev_sec10_f  ;
static float prev_sec01_f  ;

void
suite_init(void)
{
    load_objects();

    g_ui32CurFB = 0;
    cur_fb_base_phys = g_sFB[g_ui32CurFB].bo.base_phys;

#ifndef DONT_USE_NEMADC
    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
//    nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
//    nemadc_set_layer(0, &g_sDCLayer[0]);
#endif

    nema_clear(0);
    //
    // Create Command Lists
    //
    g_sCL  = nema_cl_create();
    g_sCLBlitFB = nema_cl_create();

    prev_hour10_f = -1.0f;
    prev_hour01_f = -1.0f;
    prev_min10_f  = -1.0f;
    prev_min01_f  = -1.0f;
    prev_sec10_f  = -1.0f;
}

typedef enum
{
    ALL = 0,
    HOUR10_CHANGED,
    HOUR01_CHANGED,
    MIN10_CHANGED,
    MIN01_CHANGED,
    SEC10_CHANGED,
    SEC01_CHANGED
}
TimeState_e;

static TimeState_e g_eState;
static int32_t g_i32CountFrames = 0;

static int32_t g_i32MaxNumber = 9;

//height2 = tan(DEG2RAD(CHAR_ANGLE)/2)*ALTITUDE;
static const double height2 = (double)((double)0.3249196962 * (double)ALTITUDE);

//    gluPerspective(40, width / (float) height, 1, 15);
static float proj_matrix[4][4] = {{ 5.3365272918, 0.0000,  0.0000,  0.0000},
                           { 0.0000, 7.1153697224,  0.0000,  0.0000},
                           { 0.0000, 0.0000, -1.0833333333, -1.0000},
                           { 0.0000, 0.0000, -2.0833333333,  0.0000}};

static float modelview_matrix[4][4] = {{ 1.0000, 0.0000,  0.0000, 0.0000},
                                { 0.0000, 1.0000,  0.0000, 0.0000},
                                { 0.0000, 0.0000,  1.0000, 0.0000},
                                { 0.0000, -0.50000, -11.8980, 1.0000}};

static float mvp_matrix[4][4];

static float v[PLEURES * 2 + 2][4];

static void
obj2Window_coords(float dstvec[4], float mvp[4][4], float vec[4])
{
    //
    // http://www.songho.ca/opengl/gl_transform.html
    // vec: Obj Coords
    //

    int32_t y = 0;
    dstvec[3] = mvp[0][3] * vec[0] + mvp[1][3] * vec[1] + mvp[2][3] * vec[2] + mvp[3][3] * vec[3];
    for (y = 0; y < 3; ++y)
    {
        //!< Clip Coords
        dstvec[y] = mvp[0][y] * vec[0] + mvp[1][y] * vec[1] + mvp[2][y] * vec[2] + mvp[3][y] * vec[3];
        //!< NDC Coords
        dstvec[y] /= dstvec[3];
    }

    //!< Window Coords
    dstvec[0] = (VIEWPORT_WIDTH  / 2) * dstvec[0] + (VIEWPORT_WIDTH  / 2);
    dstvec[1] = (VIEWPORT_HEIGHT / 2) * dstvec[1] + (VIEWPORT_HEIGHT / 2);
    dstvec[2] = ((ZFAR-ZNEAR) * dstvec[2] + (ZFAR + ZNEAR)) / 2;
}

static void
matmult_4x4(float dstmat[4][4], float mat0[4][4], float mat1[4][4])
{
    //!< GL matrices are COLUMN WISE
    int32_t x, y;

    for (x = 0; x < 4; ++x)
    {
        for (y = 0; y < 4; ++y)
        {
            dstmat[x][y] = mat0[0][y] * mat1[x][0] + mat0[1][y] * mat1[x][1] + mat0[2][y] * mat1[x][2] + mat0[3][y] * mat1[x][3];
        }
    }
}

static inline void
calculate_MVP(void)
{
    //mvp_matrix = modelview_matrix * proj_matrix
    matmult_4x4(mvp_matrix, proj_matrix, modelview_matrix);
}

static void
rotateX(float angle)
{
    float cos0 = nema_cos(angle);
    float sin0 = nema_sin(angle);

    modelview_matrix[1][1] =  cos0;
    modelview_matrix[1][2] =  sin0;
    modelview_matrix[2][1] = -sin0;
    modelview_matrix[2][2] =  cos0;
}

static void
calculate_vertices(float front_pleura)
{
    float vec0[4] = { WIDTH2, -height2, ALTITUDE, 1.0};
    float vec1[4] = {-WIDTH2, -height2, ALTITUDE, 1.0};
    int32_t i32Id = 0;

    for (i32Id = 0; i32Id <= g_i32MaxNumber + 1; ++i32Id)
    {
        rotateX((front_pleura - i32Id)*CHAR_ANGLE);
        calculate_MVP();

        obj2Window_coords(v[i32Id * 2    ], mvp_matrix, vec0);
        obj2Window_coords(v[i32Id * 2 + 1], mvp_matrix, vec1);
    }
}

static void
draw_pleura(float front_pleura, int32_t i32Id)
{
    int32_t tex_id = i32Id;

    i32Id = i32Id % PLEURES;
    int32_t next_id = (i32Id + 1) % PLEURES;
    int32_t prev_id = (i32Id + PLEURES - 1) % PLEURES;

    float angle = (front_pleura - i32Id) * CHAR_ANGLE;
    float cos_angle = nema_cos(angle);
    uint32_t ui32Col;

    ui32Col = (uint32_t)(50.f * (cos_angle + 1.f) + 75.f);

    if ((int32_t)front_pleura == i32Id)
    {
        ui32Col += (uint32_t)(80.0 * (double)(i32Id + 1 - front_pleura));
    }
    else if ((int32_t)front_pleura == prev_id)
    {
        ui32Col += (uint32_t)(80.0 * (double)(front_pleura - prev_id));
    }
    ui32Col = ((ui32Col) << 24) | ((ui32Col) << 0) | ((ui32Col) << 8) | ((ui32Col) << 16);

    nema_set_const_color(ui32Col);
    nema_blit_subrect_quad_fit( v[     i32Id * 2 + 1][0], v[     i32Id * 2 + 1][1],
                                v[     i32Id * 2    ][0], v[     i32Id * 2    ][1],
                                v[next_id * 2    ][0], v[next_id * 2    ][1],
                                v[next_id * 2 + 1][0], v[next_id * 2 + 1][1],
                                0, tex_id * TEX_DIM, TEX_DIM, TEX_DIM);
}

static void
draw_polupleuro(float front_pleura_f)
{
    int32_t front_pleura = ((int32_t)front_pleura_f) % PLEURES;
    int32_t back_pleura = (int32_t)(front_pleura + 0.5 * PLEURES) % PLEURES;
    int32_t pleura = back_pleura;

    calculate_vertices(front_pleura_f);

    do
    {
        if (pleura <= g_i32MaxNumber)
        {
            draw_pleura(front_pleura_f, pleura);
        }
        pleura = (pleura + 1) % PLEURES;
    }
    while (pleura != front_pleura);

    pleura = back_pleura;

    do
    {
        pleura = (pleura + PLEURES - 1) % PLEURES;

        if (pleura <= g_i32MaxNumber)
        {
            draw_pleura(front_pleura_f, pleura);
        }
    }
    while (pleura != front_pleura);
}

static void
position_and_draw_polupleuro(int32_t i32Number, float pos, float front_pleura)
{
    g_i32MaxNumber = i32Number;
    modelview_matrix[3][0] = pos;
    draw_polupleuro(front_pleura);
}

static void
set_cur_clip_area(int32_t x, int32_t y, int32_t w, int32_t h)
{
   g_i32ClipX = x;
   g_i32ClipY = y;
   g_i32ClipW = w;
   g_i32ClipH = h;
}

static void
blit_previous_fb()
{
    if (g_i32CountFrames >= 2)
    {
        //!< new drawing bbox
        int32_t i32NewMinX = g_i32ClipX;
        int32_t i32NewMinY = g_i32ClipY;
        int32_t i32NewMaxX = g_i32ClipX + g_i32ClipW - 1;
        int32_t i32NewMaxY = g_i32ClipY + g_i32ClipH - 1;

        //!< check if new bbox is included prev bbox
        bool a = i32NewMinX > g_i32MinX;
        bool b = i32NewMinY > g_i32MinY;
        bool c = i32NewMaxX < g_i32MaxX;
        bool d = i32NewMaxY < g_i32MaxY;

        if ( !a || !b || !c || !d)
        {
            int32_t x = a ? g_i32MinX : i32NewMinX;
            int32_t y = b ? g_i32MinY : i32NewMinY;
            int32_t w = (c ? g_i32MaxX : i32NewMaxX) -
                    (a ? g_i32MinX : i32NewMinX) ;
            int32_t h = (d ? g_i32MaxY : i32NewMaxY) -
                    (b ? g_i32MinY : i32NewMinY) ;

            nema_cl_bind(&g_sCLBlitFB);
            nema_cl_rewind(&g_sCLBlitFB);

            nema_bind_dst_tex(cur_fb_base_phys, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);
            nema_bind_src_tex(prv_fb_base_phys, g_sFB[g_ui32PrvFB].w, g_sFB[g_ui32PrvFB].h, g_sFB[g_ui32PrvFB].format, g_sFB[g_ui32PrvFB].stride, NEMA_FILTER_BL);
            nema_set_clip(0, 0, RESX, RESY);
            nema_set_blend_blit(NEMA_BL_SRC);
            nema_blit_subrect(x, y, w, h, x, y);

            nema_cl_submit(&g_sCLBlitFB);
            nema_cl_wait(&g_sCLBlitFB);
        }
    }
}

#define EASE nema_ez_bounce_out

static void
draw_frame(uint32_t ui32Hour, uint32_t ui32Min, uint32_t ui32Sec, float msec)
{
    uint32_t ui32Sec01  = ui32Sec % 10;
    uint32_t ui32Sec10  = ui32Sec / 10;
    uint32_t ui32Min01  = ui32Min % 10;
    uint32_t ui32Min10  = ui32Min / 10;
    uint32_t ui32Hour01 = ui32Hour % 10;
    uint32_t ui32Hour10 = ui32Hour / 10;

    //!< calulate time in float format
    float sec01_f  = (msec + ui32Sec01);
    float sec10_f  = ui32Sec01 != 9  ? ui32Sec10 :
                     ui32Sec10 != 5  ? (EASE(msec)     + (float)ui32Sec10):
                                   (EASE(msec)*5.f + (float)ui32Sec10);

    float min01_f  = ui32Sec   != 59 ? ui32Min01 :
                                  (EASE(msec) + ui32Min01);
    float min10_f  = ui32Min01 != 9 || ui32Sec != 59
                           ? ui32Min10 :
                             ui32Min10 != 5  ? (EASE(msec)     + ui32Min10):
                                           (EASE(msec)*5.f + ui32Min10);

    float hour01_f = ui32Sec   != 59 || ui32Min != 59
                          ? ui32Hour01 :
                            ui32Hour == 23 ? (EASE(msec)*7.f + ui32Hour01) :
                            (EASE(msec) + ui32Hour01);
    float hour10_f = ui32Sec   != 59 || ui32Min != 59 || (ui32Hour01 != 9 && ui32Hour != 23)
                          ? ui32Hour10 :
                            ui32Hour01 == 9 ? (EASE(msec)     + ui32Hour10):
                                          (EASE(msec)*8.f + ui32Hour10);

   //!< find current state and clipping area

    if (g_i32CountFrames >= 0) //!< for the first 2 cycles clear the whole screen (2 buffers)
    {
        set_cur_clip_area(0, 0, RESX, RESY);
        g_i32CountFrames++;
        g_eState = ALL;
    }
    else
    {
        if (prev_hour10_f != hour10_f)
        {
            set_cur_clip_area(CLIP_XMIN, CLIP_YMIN, CLIP_XMAX-CLIP_XMIN, CLIP_YMAX);
            g_eState = HOUR10_CHANGED;
        }
        else if (prev_hour01_f != hour01_f)
        {
            set_cur_clip_area(CLIP_HOUR01, CLIP_YMIN, CLIP_XMAX-CLIP_HOUR01, CLIP_YMAX);
            g_eState = HOUR01_CHANGED;
        }
        else if (prev_min10_f != min10_f)
        {
            set_cur_clip_area(CLIP_MIN10, CLIP_YMIN, CLIP_XMAX - CLIP_MIN10, CLIP_YMAX);
            g_eState = MIN10_CHANGED;
        }
        else if (prev_min01_f != min01_f)
        {
            set_cur_clip_area(CLIP_MIN01, CLIP_YMIN, CLIP_XMAX - CLIP_MIN01, CLIP_YMAX);
            g_eState = MIN01_CHANGED;
        }
        else if (prev_sec10_f != sec10_f)
        {
            set_cur_clip_area(CLIP_SEC10, CLIP_YMIN, CLIP_XMAX - CLIP_SEC10, CLIP_YMAX);
            g_eState = SEC10_CHANGED;
        }
        else if (prev_sec01_f != sec01_f)
        {
            set_cur_clip_area(CLIP_SEC01, CLIP_YMIN, CLIP_XMAX- CLIP_SEC01, CLIP_YMAX);
            g_eState = SEC01_CHANGED;
        }
    }

    //!< Blit previous clipping area union with current clipping area
#if (FRAME_BUFFERS > 1)
    blit_previous_fb();
#endif
    //
    // Draw current clipping area
    //
    nema_cl_bind(&g_sCL);
    nema_cl_rewind(&g_sCL);

    nema_bind_dst_tex(cur_fb_base_phys, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);
    nema_bind_src_tex(g_sNumbersImg.bo.base_phys, g_sNumbersImg.w, g_sNumbersImg.h, g_sNumbersImg.format, g_sNumbersImg.stride, g_sNumbersImg.sampling_mode);
    nema_set_clip(g_i32ClipX, g_i32ClipY, g_i32ClipW, g_i32ClipH);

    nema_clear(0);
    nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_RGB);
    nema_clear_dirty_region();
    //
    // draw and position each polupleuro only if it changes g_eState
    //
    switch (g_eState)
    {
        case ALL:
        case HOUR10_CHANGED:
        case HOUR01_CHANGED:
            position_and_draw_polupleuro(2, -1.56, hour10_f);
            position_and_draw_polupleuro(9, -1.00, hour01_f);
        case MIN10_CHANGED:
            position_and_draw_polupleuro(5, -0.28, min10_f);
        case MIN01_CHANGED:
            position_and_draw_polupleuro(9, 0.28, min01_f);
        case SEC10_CHANGED:
        case SEC01_CHANGED:
        default:
            position_and_draw_polupleuro(9, 1.56, sec01_f);
            position_and_draw_polupleuro(5, 1.00, sec10_f);
            break;
    }

    //!< Submit Command List
#if (FRAME_BUFFERS == 1)
    // If we have only 1 FB
    // DC and NemaP are operating on the same FB
    // make sure previous display update has finished
    // and then submit CL
    nemadc_wait_vsync();
#endif
    nema_cl_submit(&g_sCL);

#if (FRAME_BUFFERS > 1)
    // If we have >= 2 FBs
    // DC and NemaP are operating on different FBs
    // So we can push "wait_vsync" closer to "send_frame_single"
    nemadc_wait_vsync();
#endif

    //!< Wait for submitted Command List to finish
    nema_cl_wait(&g_sCL);

    nemadc_set_layer(0, &g_sDCLayer[g_ui32CurFB]);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_get_dirty_region(  //!< store current dirty region (In next call it will be previous dirty region)
        (int *)(&g_i32MinX),
        (int *)(&g_i32MinY),
        (int *)(&g_i32MaxX),
        (int *)(&g_i32MaxY)
        );

    prev_hour10_f = hour10_f;
    prev_hour01_f = hour01_f;
    prev_min10_f = min10_f;
    prev_min01_f = min01_f;
    prev_sec10_f = sec10_f;
    prev_sec01_f = sec01_f;
}

static inline uintptr_t
nema_swap_fb(int32_t layer)
{
    g_ui32CurFB = (g_ui32CurFB + 1) % FRAME_BUFFERS;
    return g_sFB[g_ui32CurFB].bo.base_phys;
}

#ifdef STANDALONE
int
main(void)
#else
int32_t
rotating_clock()
#endif
{
    int32_t i32Ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; //!< default config
    //
    // Initialize NemaGFX
    //
    i32Ret = nema_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }
    //
    // Initialize Nema|dc
    //
    i32Ret = nemadc_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
        uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
        uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
        pixel_format_t eFormat = FMT_RGB888;
        if (am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0)
        {
            return -3;
        }
        switch (eFormat)
        {
            case FMT_RGB888:
                if (ui8DbiWidth == 16)
                {
                    ui32MipiCfg = MIPICFG_16RGB888_OPT0;
                }
                if (ui8DbiWidth == 8)
                {
                    ui32MipiCfg = MIPICFG_8RGB888_OPT0;
                }
                break;

            case FMT_RGB565:
                if (ui8DbiWidth == 16)
                {
                    ui32MipiCfg = MIPICFG_16RGB565_OPT0;
                }
                if (ui8DbiWidth == 8)
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

    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //!< panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //!< panel's max resolution

    uint16_t i32MinX, i32MinY;

    //!< Set the display region to center
    if (RESX > ui16PanelResX)
    {
        i32MinX = 0;   //!< set the minimum value to 0
    }
    else
    {
        i32MinX = (ui16PanelResX - RESX) >> 1;
        i32MinX = (i32MinX >> 1) << 1;
    }

    if (RESY > ui16PanelResY)
    {
        i32MinY = 0;   //!< set the minimum value to 0
    }
    else
    {
        i32MinY = (ui16PanelResY - RESY) >> 1;
        i32MinY = (i32MinY >> 1) << 1;
    }
    //
    // Initialize the display
    //
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, RESX, RESY, i32MinX, i32MinY);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, RESX, RESY, i32MinX, i32MinY);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, RESX, RESY, i32MinX, i32MinY);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, RESX, RESY, i32MinX, i32MinY);
            break;
        default:
            ; //NOP
    }

    suite_init();

#ifdef CUSTOM_TIME
    static float time0 = 23.f*60.f*60.f + 59.f*60.f + 30.f;
#endif
#if ENABLE_QSPI
    uint32_t dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out( dbi_cfg | MIPICFG_SPI_HOLD  );
#endif

    while (1)
    {

#ifdef CUSTOM_TIME  //!< calculate current time
        float time = time0;
        time0 += 2.f / 60.f;

        if (time0 >= 24.f * 60.f * 60.f)
        {
            time0 = 0.f;
        }
#else
        float time = nema_get_time();
#endif

        int32_t ui32Sec  = (int32_t)time % 60;
        int32_t ui32Hour = (int32_t)(time / 60 / 60);
        int32_t ui32Min  = ((int32_t)(time / 60)) % 60;
        float msec = (time - (float)(int32_t)time);

        draw_frame(ui32Hour, ui32Min, ui32Sec, msec);

        g_ui32PrvFB = g_ui32CurFB; //!< store current buffer to previous
        prv_fb_base_phys = cur_fb_base_phys;
        cur_fb_base_phys = nema_swap_fb(0);
        nema_calculate_fps();
    }
}
