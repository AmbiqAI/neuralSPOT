/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon Single Member PC be liable for any claim, damages or other
 *  liability, whether in an action of contract, tort or otherwise, arising
 *  from, out of or in connection with the software or the use or other dealings
 *  in the software.
 ******************************************************************************/

#ifndef NEMA_SHADERSPECIFIC_H__
#define NEMA_SHADERSPECIFIC_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"
#include "nema_programHW.h"
#include "nema_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// deprecated API call. To be removed
#define nema_blit_yuv nema_blit_yuv420_planar

void nema_blit_yuv420_planar(
            uintptr_t src_addr_Y,
            uintptr_t src_addr_U,
            uintptr_t src_addr_V,
            uint32_t src_xres,
            uint32_t src_yres,
            int src_stride,
            int dst_x,
            int dst_y);

// just an alias of 420 semiplanar
#define nema_blit_nv12 nema_blit_yuv420_semiplanar

void nema_blit_yuv420_semiplanar(
            uintptr_t src_addr_Y,
            uintptr_t src_addr_UV,
            uint32_t src_xres,
            uint32_t src_yres,
            int src_stride,
            int dst_x,
            int dst_y);

void nema_blit_yuv422_planar(
            uintptr_t src_addr_Y,
            uintptr_t src_addr_U,
            uintptr_t src_addr_V,
            uint32_t src_xres,
            uint32_t src_yres,
            int src_stride,
            int dst_x,
            int dst_y);

void nema_blit_yuv444_planar(
            uintptr_t src_addr_Y,
            uintptr_t src_addr_U,
            uintptr_t src_addr_V,
            uint32_t src_xres,
            uint32_t src_yres,
            int src_stride,
            int dst_x,
            int dst_y);

void nema_blit_yuv444_packed(
            uintptr_t src_addr_YUV,
            uint32_t src_xres,
            uint32_t src_yres,
            int src_stride,
            int dst_x,
            int dst_y);

void nema_blit_warp(uintptr_t warpBase, uint32_t warpW, uint32_t warpH, nema_tex_format_t warpMode,
                    int warpStride, int x, int y);
void nema_blit_blur(unsigned char matrix[3][3], int x, int y, int w, int h);
void nema_blit_edge(unsigned char matrix[3][3], int x, int y, int w, int h);

void nema_blit_mean(int x, int y, int w, int h);
void nema_blit_gauss(int x, int y, int w, int h);
void nema_blit_sharpen_gauss(int x, int y, int w, int h, float sharpen);
void nema_blit_sharpen_laplace(int x, int y, int w, int h, float sharpen);
void nema_blit_contrast_linear(int x, int y, int w, int h, uint8_t min, uint8_t max);
void nema_blit_color_correction(uint8_t matrix[3][3], int x, int y, int w, int h);
void nema_blit_rgb_to_ycbcr(int w, int h);
void nema_blit_median(int w, int h);
void nema_blit_hist_equalization(int w, int h, uint32_t histogram[256]);
void nema_blit_gamma(int w, int h,nema_buffer_t *bo);
void nema_blit_binary(int x, int y, int w, int h, float threshold);
void nema_blit_debayer(int w, int h);
void nema_blit_ycbcr_to_rgb(int w, int h);

void nema_blit_bayer_L8_to_RGB(int w, int h);
void nema_blit_3L8_to_RGB(int w, int h);
void nema_blit_RGB_to_3L8(int w, int h);

#ifdef __cplusplus
}
#endif

#endif
