//*****************************************************************************
//
//! @file tsvg_render_example.c
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

#include "nemagfx_vg_test.h"

#ifdef RUN_TSVG_RENDER_EXAMPLE

// Uncomment this if SVG needs to be loaded from the filesystem
//#define LOAD_SVG_FROM_FILESYSTEM

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_vg_tsvg.h"
#include "nema_vg.h"
#include "Arial_ttf.h"
//#include <stdlib.h>
#define VECTOR_FONT Arial_ttf

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#endif

#ifndef LOAD_SVG_FROM_FILESYSTEM
#include "./tsvgs/allsvg.tsvg.h"
#endif

#define RESX 452
#define RESY 452

#ifndef DEFAULT_FILENAME
#define DEFAULT_FILENAME  "tsvgs/tiger.tsvg"
#endif

nema_img_obj_t fb = {{0}, RESX, RESY, RESX*4, 0, NEMA_RGBX8888, 0};

#ifndef DONT_USE_NEMADC
nemadc_layer_t dc_layer = {(void *)0, 0, RESX, RESY, -1, 0, 0, RESX, RESY, 0xff, NEMADC_BL_SRC, 0, NEMADC_RGBA8888, 0, 0, 0, 0, 0, 0, 0};
#endif

#ifdef LOAD_SVG_FROM_FILESYSTEM

static void* load_binary(const char *filename)
{
    FILE* fp = NULL;
    size_t size;

    unsigned char* binary = NULL;
    fp = fopen(filename, "rb");
    if (!fp)
    {
        goto error;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    binary = (unsigned char*)malloc(size + 1);
    if (binary == NULL)
    {
        goto error;
    }
    if (fread(binary, 1, size, fp) != size)
    {
        goto error;
    }
    binary[size] = '\0';    // Must be null terminated.
    fclose(fp);

    return (void*)binary;

error:
    if (fp)
    {
        fclose(fp);
    }
    if (binary)
    {
        free(binary);
    }

    return NULL;
}

#endif

static void load_objects(void)
{
    //Load memory objects
    fb.bo = nema_buffer_create(fb.stride*fb.h);
    nema_buffer_map(&fb.bo);
    printf("FB: V:%p P:0x%08x\n", (void *)fb.bo.base_virt, fb.bo.base_phys);

#ifndef DONT_USE_NEMADC
    dc_layer.baseaddr_phys = fb.bo.base_phys;
    dc_layer.baseaddr_virt = fb.bo.base_virt;
#endif

    printf("FB: V:%p P:0x%08x\n", (void *)fb.bo.base_virt, fb.bo.base_phys);
}

static void destroy_objects(void)
{
    //Destroy memory objects
    nema_buffer_unmap(&fb.bo);
    nema_buffer_destroy(&fb.bo);
}

static void set_dc(void)
{
#ifndef DONT_USE_NEMADC
    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    //nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    nemadc_set_layer(0, &dc_layer);
#endif
}

int tsvg_render_example()
{
    // Initialize NemaGFX
    if ( nema_init() != 0 )
    {
        return -1;
    }

#ifndef DONT_USE_NEMADC
    //Initialize NemaDC
    if ( display_setup(RESX, RESY) != 0 )
    {
        return -2;
    }
#endif

    load_objects();
    set_dc();

#if 0
    // Use circular cl
    nema_cmdlist_t cl  = nema_cl_create_sized(1024);
    nema_cl_bind_circular(&cl);
#else
    nema_cmdlist_t cl  = nema_cl_create();
    nema_cl_bind(&cl);
#endif

    // Clear fb
    nema_bind_dst_tex(fb.bo.base_phys, fb.w, fb.h, fb.format, fb.stride);
    nema_set_clip(0, 0, RESX, RESY);
    nema_clear(0xffffffffU); //white

    // Init VG library
    nema_vg_init(RESX, RESY);

    // Bind one default font in case our tsvg contains text
    //---------------------------------------------
    nema_vg_bind_font(&VECTOR_FONT);

    nema_vg_set_tsvg_text_max_size(RESX, RESY);

#ifdef LOAD_SVG_FROM_FILESYSTEM
    char* filename = DEFAULT_FILENAME;
    if (argc > 1)
    {
        filename = argv[1];
    }

    unsigned char* binary = load_binary(filename);
    nema_vg_draw_tsvg(binary);
#else
    nema_vg_draw_tsvg(allsvg_tsvg);
#endif

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    //Start DC
    display_refresh_start();
    //Wait DC complete interrupt.
    nemadc_wait_vsync();
    //Do follow-up operations required by hardware.
    display_refresh_end();

    printf("error = %x\n", nema_vg_get_error());
    //nema_save_file("fb.rgba",RESX*RESY*4, fb.bo.base_virt);

    //---------------------------------------------

    nema_vg_deinit();

    nema_buffer_unmap(&fb.bo);
    nema_buffer_destroy(&fb.bo);

#ifdef LOAD_SVG_FROM_FILESYSTEM
    free(binary);
#endif

    nema_cl_destroy(&cl);

    return 0;
}

#endif
