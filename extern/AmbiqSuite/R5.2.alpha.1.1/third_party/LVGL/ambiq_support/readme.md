# Ambiq LVGL supporting

## How to add a new font

We support two approaches to adding a new font, they are:

  - `.c` source file
  - `.bin` data file 

For `.c` file, you should:

  - convert the font by LVGL font tools and save it to `.c` format.
  - compile the `.c` file in your project building process.
  - call the `lv_ambiq_A4_font_align` API in your application code to align the font bitmaps. (Currently, Only the most commonly used A4 format is supported in this API, but you can add the support to A2 or A1 format.)
  - take reference to the usage of `lv_font_montserrat_28` in this [lvgl_watch_demo](../../../boards/common4/examples/graphics/lvgl_watch_demo) example.

For `.bin` file, you should:

  - convert the font by LVGL font tools and save it to `.bin` format.
  - use the `font_realign.py` script to align the bitmaps in the font.
  - save the aligned font to your file system(usually in an EMMC card).
  - load the font in the application runtime by `lv_font_load` API(in [this](../lvgl/src/font/lv_font_loader.c) file).

    **Note**: LV_FONT_FMT_TXT_LARGE should be set to 1 if your font size is bigger than 1M Bytes.
  - take reference to the usage of `HYQiHei_font` in this [lvgl_watch_demo](../../../boards/common4/examples/graphics/lvgl_watch_demo) example.

**Why do we need to align the bitmaps in the font**

For the NemaGFX GPU, it requires each horizontal line in the font glyph to align to byte, for example, if a character is 13*13 in A4 format, the first pixel in the second row should be located in the seventh byte instead of the sixth byte.  However, this is not satisfied in the font file converted by LVGL font tools, so if you are using a font with BPP not equal to 8, an additional align operation needs to be added before this font is rendered by GPU.

**Note**: 

- **Font compression and subpixel rendering are not supported, please DO NOT USE**.
- Font bitmaps located in EMMC(or MRAM) **must be loaded to PSRAM(or SSRAM)** before being used by GPU. For font in `.c` file, the loading process has been implemented in the `lv_ambiq_A4_font_align` API, see our [example](../../../boards/common4/examples/graphics/lvgl_watch_demo) for detail. For font in `.bin` file, the `lv_font_load` API will handle this implicitly, no special operation is needed.
-   In Apollo5, you are supposed to flush the Cache lines mapped to the font bitmap region after the memcpy(called implicitly in `lv_ambiq_A4_font_align` or `lv_font_load` APIs). This is required for any texture or fonts used in your application. 



## How to add a new image texture

We support the following texture color formats:
- **BGR888**       
- **BGRA8888**     
- **BGR888_Chroma_Key**       
- **Indexed_1bit** 
- **Indexed_2bits**
- **Indexed_4bits**
- **Indexed_8bits**
- **Alpha_1bit**   
- **Alpha_2bit**   
- **Alpha_4bit**   
- **Alpha_8bit**   
- **ARGB8888**     
- **BGRA8888**     
- **RGB565**       
- **TSC6**         
- **TSC4**         
- **TSC6A**        
- **RGB888**       
- **RGBA4444** 
- **TSVG**(only for Apollo5)    

You are supposed to follow these steps to add an image texture:
- run PixPresso.exe, and open the image to be converted.
- select the color format setting in the drop-down list, you will find the PixPresso config in the following table, see the `PixPresso config` column.
- convert the image to .h/.bin file. For .h file, include it in the building process. For .bin files, save them to your file system, and load them to PSRAM(or SSRAM) when they are needed.
- Manually create a `lv_img_dec_t` variable and set the `cf` member to the value in the `lv_img_dec_t setting` column.


| **format**        | **PixPresso config** | **lv_img_dec_t setting**          | **GPU-only**   | **comment** |
|-------------------|----------------------|-----------------------------------|----------------|-------------|
| **BGR888**        | BGR24                | LV_IMG_CF_TRUE_COLOR              |                |             |
| **BGRA8888**      | BGRA8888             | LV_IMG_CF_TRUE_COLOR_ALPHA        |                |             |
| **BGR888**        | BGR24                | LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED |                | see note1   |
| **Indexed_1bit**  | LUT-2                | LV_IMG_CF_INDEXED_1BIT            | GPU-only       | see note2   |
| **Indexed_2bits** | LUT-4                | LV_IMG_CF_INDEXED_2BIT            | GPU-only       | see note2   |
| **Indexed_4bits** | LUT-16               | LV_IMG_CF_INDEXED_4BIT            | GPU-only       | see note2   |
| **Indexed_8bits** | LUT-256              | LV_IMG_CF_INDEXED_8BIT            | GPU-only       | see note2   |
| **Alpha_1bit**    | L1                   | LV_IMG_CF_ALPHA_1BIT              | GPU-only       |             |
| **Alpha_2bit**    | L2                   | LV_IMG_CF_ALPHA_2BIT              | GPU-only       |             |
| **Alpha_4bit**    | L4                   | LV_IMG_CF_ALPHA_4BIT              | GPU-only       |             |
| **Alpha_8bit**    | L8                   | LV_IMG_CF_ALPHA_8BIT              | GPU-only       |             |
| **ARGB8888**      | ARGB8888             | LV_IMG_CF_AMBIQ_ARGB8888          | GPU-only       |             |
| **BGRA8888**      | BGRA8888             | LV_IMG_CF_AMBIQ_BGRA8888          | GPU-only       |             |
| **RGB565**        | RGB565               | LV_IMG_CF_AMBIQ_RGB565            | GPU-only       |             |
| **TSC6**          | TSC6                 | LV_IMG_CF_AMBIQ_TSC6              | GPU-only       |             |
| **TSC4**          | TSC4                 | LV_IMG_CF_AMBIQ_TSC4              | GPU-only       |             |
| **TSC6A**         | TSC6A                | LV_IMG_CF_AMBIQ_TSC6A             | GPU-only       |             |
| **RGB888**        | RGB24                | LV_IMG_CF_AMBIQ_RGB24             | GPU-only       |             |
| **RGBA4444**      | RGBA4444             | LV_IMG_CF_AMBIQ_RGBA4444          | GPU-only       |             |
| **Tiny SVG**(Apollo5 only)     | TSVG             | LV_IMG_CF_AMBIQ_TSVG          | GPU-only       |             |

Note:

1. The LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED color format is actually the same as BGR888, the only difference is the chroma key is used, see the LV_COLOR_CHROMA_KEY definition.
2. For the Indexed_xbit/bits color formats, the Pixpresso software outputs two files:
   `xxxx_palette.xx.rgba` and `xxxx_indices.xx.gray`.

   Run the following command to concat the indice file and palette file for the L8 L4 L2 L1 color format:
   ```
   cat xxxx_palette.16.rgba xxxx_indices.16.gray  > xxxx_indexed_xbits.bin
   ```
   Note: put the palette file in front of the indices file.

   Run the following command to convert the bin file to `.h` file if `.h` file is used:
   ```
   xxd -i -c 16  xxxx_indexed_xbits.bin xxxx_indexed_xbits.h
   ```
3. `LV_IMG_SRC_FILE` image type **is not supported**, Image texture should be loaded from EMMC (or MRAM) to PSRAM (or SSRAM) before being used by LVGL, this process is totally **implemented by the user application**.
4. `GPU-only` means these texture formats can only be handled by GPU, handling these textures by CPU is time-consuming or even impossible. We have added assertions to help users find these performance bottlenecks. We hope our users can spot these CPU-rendering scenarios and avoid them by modifying UI design.

### Image rotation
Using **Morton encoded texture**(about Morton encoding: https://en.wikipedia.org/wiki/Z-order_curve) and enabling the **tiled rendering** feature(https://en.wikipedia.org/wiki/Tiled_rendering) can significantly accelerate the image-rotating operation. 

The **tiling feature** is automatically enabled when a non-zero value is set to the `angle` attribute of `image` object.

Run the following steps to use a Morton-encoded texture.
1. In PixPresso.exe, check the `Morton encoding` checkbox while converting the source image to `.h` file or `.bin` file.
2. In your application code, set `.header.morton_order = 1` in the `lv_img_dsc_t` image description struct.


## How to change the frame buffer format setting

We have made the frame buffer format setting **independent of the `LV_COLOR_DEPTH` setting** and we support the following frame buffer format:

```
 NEMA_RGBX8888   
 NEMA_RGBA8888   
 NEMA_XRGB8888   
 NEMA_ARGB8888   
 NEMA_RGB565     
 NEMA_RGBA5650   
 NEMA_RGBA5551   
 NEMA_RGBA4444                     
 NEMA_ABGR8888   
 NEMA_XBGR8888   
 NEMA_BGRA8888   
 NEMA_BGRX8888   
 NEMA_TSC4       
 NEMA_TSC6       
 NEMA_TSC6A      
 NEMA_BGR24      
 NEMA_RGB24      
```

User should run the following steps to use a customized frame buffer:

- Allocate a buffer and create a `img_obj_t` object to specify its parameter.
- Pass the pointer of the above `img_obj_t` object to the `user_data ` area of the `lv_disp_drv_t` object, and register the display driver.

Refer to our `third_party/LVGL/ambiq_support/display_task_two_fb.c` for details.


## User config

Users are supposed to create their `lv_conf.h ` by taking [this](../lvgl/lv_conf_template.h) file as a template. 

**Note**:

- **`LV_COLOR_DEPTH` MUST be 32**, no other option is supported.
- `LV_COLOR_SCREEN_TRANSP` is not supported.



## GPU support lib

The core GPU acceleration APIs are compiled into a library.

While making this lib, `lv_conf.h` is included from ./gpu_lib_apollo4/lv_conf.h (or ./gpu_lib_apollo5/lv_conf.h). So if you have some different settings for LVGL in your project, you should make sure they are not used by this GPU-accelerating library.
For now, the following items are used, they are:

    --- LV_COLOR_DEPTH
    --- LV_USE_LARGE_COORD
    --- LV_BIG_ENDIAN_SYSTEM

## Optional panel refresh methods

We have provided three different panel refresh methods supporting various frame buffer sizes and color format settings, they are:
- **1FB**, one full-size frame buffer.
- **1.xFB**, one full size plus one partial size frame buffer.
- **2FB**, two full-size frame buffers.

Three files are provided under the `third_party/LVGL/ambiq_support` folder corresponding to each refresh mode:
- `display_task_one_fb.c`
- `display_task_one_and_partial_fb.c`
- `display_task_two_fb.c`

Select one refresh mode based on your memory budget and add one of the following macro definitions in your project to use that refresh mode. 
- `LV_AMBIQ_FB_REFRESH_ONE`
- `LV_AMBIQ_FB_REFRESH_TWO`
- `LV_AMBIQ_FB_REFRESH_ONE_AND_PARTIAL`  

In the display_task_xxx.c file, we have created frame buffers, implemented callback functions, and registered a disp_drv object as required by the LVGL porting guide. In the meanwhile, an RTOS task is also created for handling display refresh rooting, this task is created based on FreeRTOS APIs, if another RTOS is used, users are supposed to customize it.

### one full-size frame buffer

In this mode, we created a frame buffer with the same size as your display panel resolution. The GPU render operation and the frame buffer transfer operation will run in serial, resulting in a relatively low FPS.

The following configurations are open to customize:
1. `LV_AMBIQ_FB_USE_RGB565` or `LV_AMBIQ_FB_USE_RGB888`. They are the color format for the frame buffer and display panel interface. Please select **one** of these two options, if both are enabled, RGB888 will be used.
2. `LV_AMBIQ_FB_RESX` and `LV_AMBIQ_FB_RESY`, this is the panel resolution, a buffer with the size of LV_AMBIQ_FB_RESX\*LV_AMBIQ_FB_RESY\*2 or  LV_AMBIQ_FB_RESX\*LV_AMBIQ_FB_RESY\*3 bytes will be allocated if RGB565 or RGB888 is used.
3. `USE_DIRECT_MODE`, use direct mode or not. About [direct_mode](https://docs.lvgl.io/master/porting/display.html#direct-mode).
4. `USE_FULL_REFRESH`, use full refresh or not. About [full_refresh](https://docs.lvgl.io/master/porting/display.html#full-refresh).
5. `DISPLAY_REFRESH_TIMEOUT`, the timeout for the display panel refresh operation.

**Note: It is not advised to change the default `USE_DIRECT_MODE` and `USE_FULL_REFRESH` settings, only if you have a deep understanding of the entire LVGL refresh process. The default setting is optimal for the majority of user scenarios.**

### One full-size plus one partial-size frame buffer

In this mode, we will allocate two frame buffers:
-  one full-size frame buffer, size = RESX \*RESY \*PIXEL_SIZE
-  one partial size frame buffer, size = RESX \* (RESY/PARTIAL_FB_RATIO) \*PIXEL_SIZE.

The full-size one is used for display refresh operation(call it *display buffer*) and the partial one is used for GPU render operation(call it *draw buffer*). Firstly, the GPU will redraw the dirty areas one by one, writing their content to the draw buffer, then pass the dirty area's 2D location(starting position, width, height) and its draw buffer pointer to the display task. In the display task, the content of the dirty area will be copied to the display buffer, when all the dirty area is updated, the display task will trigger the MSPI(or DC) transfer operation and wait until the transfer is complete.

Using this method, GPU render operation and display refresh operation will run in parallel for most of the scenarios, However, there still exist some extreme scenarios where the dirty area overlays with the current refreshing display region, and the parallel mode will deteriorate to the serial mode.

One configuration is added compared to 1FB mode:

`PARTIAL_FB_RATIO`: this value decides the partial frame buffer size, size = RESX \* (RESY/PARTIAL_FB_RATIO) \*PIXEL_SIZE.

The other configurations share the same definition as that in 1FB mode.

**Note: `USE_DIRECT_MODE` and `USE_FULL_REFRESH` must be disabled in this mode.**

### Two full-size frame buffers

In this mode, we will allocate two full-size frame buffers. The GPU render operation and the frame buffer transfer operation will run in parallel, therefore, this mode is supposed to achieve the highest FPS among these three methods.

This mode shares the same configurable parameters as the 1FB mode.

**Note: It is not advised to change the default `USE_DIRECT_MODE` and `USE_FULL_REFRESH` settings, only if you have a deep understanding of the entire LVGL refresh process. The default settings are optimal for the majority of user scenarios.**

## Memory management

Typically, there are three different RAMs in our system, they are TCM, SSRAM, and PSRAM. TCM and SSRAM are internal memories integrated inside our chips, while the PSRAM is external memory that is connected by the MSPI interface and runs in XIP mode. These different memories have different properties as shown in the following table:

| **Memory** | **Size**                   |  **GPU Accessible** |
|------------|---------------------|---------------------|
| TCM        | small(378KB-496KB)                       |  Yes                  |
| SSRAM      | middle(1MB-3MB)   | Yes                 |
| PSRAM      | Decide by system design, typical 8-128MB |  Yes                 |

Generally speaking, they are supposed to hold different data types:
- TCM
  - Stack
  - RTOS heap(for task stack, semaphore, queue, event, etc.)
  - Other data objects are accessed **only by CPU**.
- SSRAM
  - Frame buffer
  - GPU command list
  - Other data objects are accessed **by both CPU and GPU**.
- PSRAM
  - **Image texture**
  - **Font**
  - Other data objects which usually have big sizes and less access latency requirement.

### Heap usage in LVGL
In the original LVGL implementation, all data objects are allocated from the same heap, this is easy to implement but not suitable for our Apollo chips. To take full advantage of the above-mentioned memories, we have created three heaps located in TCM, SSRAM, and PSRAM respectively, and prepared the following APIs to allocate memory for different data types.

- `lv_mem_alloc` 

    This API is used to allocate memory for all dynamic data structures in the LVGL, which are accessed only by the CPU.
    In this API, we will first try to allocate the memory from the TCM heap and if the TCM heap is exhausted, we will try to allocate it from the SSRAM heap.

    Users are allowed to reverse this order: firstly SSRAM, then TCM, if TCM is more valuable than SSRAM in their projects, see lv_mem.c for details.

- `lv_mem_ssram_alloc`

    This API is used to allocate temporary buffers used for [masking](https://docs.lvgl.io/master/overview/drawing.html?#masking) and other assets which are read directly by GPU. As these buffers are accessed by both CPU and GPU and have a high requirement for access latency, they **must be allocated from SSRAM**.

- `lv_mem_external_alloc`

    This API is used to allocate buffers for image texture and font bitmap. These data objects always have a big size and low access latency requirement, so they are allocated from PSRAM.

    Users are allowed to optimize this API to allocate a buffer from SSRAM for small-size textures or frequently used fonts. This is a useful trick to accelerate the refresh speed in some key scenarios.

- `lv_mem_font_glyph_alloc`

    This API is used to allocate a buffer for font glyph bitmap, which is accessed by GPU. If the data size is less than 4K Bytes, it will be allocated from SSRAM, otherwise, it will be allocated from PSRAM.

- `lv_mem_font_info_alloc`

    This API is used to allocate a buffer for font specification data, which is accessed only CPU. If the data size is less than 4K Bytes, it will be allocated from TCM, otherwise, it will be allocated from PSRAM.

    Users are allowed to customize this based on their memory budget.
 

### Heap usage in NemaSDK

In the NemaSDK, all dynamic memory is allocated by three APIs in `nema_hal.c`:
- `nema_host_malloc`
- `nema_buffer_create`
- `nema_buffer_create_pool`

Their usage is described as the following:
- `nema_host_malloc`

    This API is used to allocate memory for instance of the following structures: nema_cmdlist_t, nema_vg_paint_t, nema_vg_grad_t, nema_vg_path_t.  They are only accessed by the CPU, GPU will not access this data structure directly.

- `nema_buffer_create`

    This is only called in the `nema_blit_hist_equalization` API, 256bytes are allocated in `nema_blit_hist_equalization` with `nema_buffer_create`. It is safe to ignore this if nema_blit_hist_equalization is not called in your application.

- `nema_buffer_create_pool`

    Different from the above two APIs, it takes two input parameters: `nema_buffer_t nema_buffer_create_pool(int pool, int size)`.
    Two different kinds of buffers are created by this API:

    1. CL buffers


        These buffers will set the `pool` variable to `NEMA_MEM_POOL_CL_RB`, and are used to hold the GPU commands. It is suggested to allocate it from SSRAM, putting it on PSRAM or TCM will significantly slow down the GPU execution rate.

    2. block buffers

        These buffers will set the `pool` variable to `NEMA_MEM_POOL_FB_TEX`, and are used to hold the following buffers:
        - stencil buffer in NemaVG, `size=RESX*RESY`
        - LUT buffer in NemaVG, `size=256`
        - gradient buffer in NemaVG, `size=256`
        Putting these buffers into SSRAM will accelerate the GPU rendering speed. 

    **Note**: For Apollo5, the above buffers must align to 32Bytes to avoid memory consistency issues between CPU and GPU. For Apollo4, it is required to align any GPU-accessed buffers to 8Bytes. 

Users are suggested to customize the `nema_hal.c` and implement the above functions by their heap management system. Alternatively, instead of customizing the whole `nema_hal.c`, a simpler way is provided, that is, defining the following macros in the user's project setting:

- `NEMA_USE_CUSTOM_MALLOC=1`
- `NEMA_CUSTOM_MALLOC_INCLUDE="xxxx_malloc.h"`
- `NEMA_CUSTOM_MALLOC=xxx_alloc`
- `NEMA_CUSTOM_FREE=xxx_free`

A demo code about how to customize the heaps in NemaSDK is provided in `third_party/LVGL/ambiq_support/lv_ambiq_nema_hal.c` and `third_party/LVGL/ambiq_support/lv_ambiq_nema_hal.h`. 
In `lv_ambiq_nema_hal.h`, we provided the macro defines for malloc and free:
```
NEMA_CUSTOM_MALLOC=lv_ambiq_nema_hal_malloc
NEMA_CUSTOM_FREE=lv_ambiq_nema_hal_free
```
In `lv_ambiq_nema_hal.c`, we implement the `lv_ambiq_nema_hal_malloc` by `lv_mem_alloc` and `lv_mem_alloc_ssram`:
```
void* lv_ambiq_nema_hal_malloc(int pool, uint32_t size_in_byte)
{
    void* ptr;

    switch(pool)
    {
        case NEMA_MEM_POOL_CL_RB:
            ptr = lv_mem_ssram_alloc(size_in_byte);
            break;
        case NEMA_MEM_POOL_FB_TEX:
            ptr = lv_mem_ssram_alloc(size_in_byte);
            break;
        case NEMA_MEM_POOL_MISC:
            ptr = lv_mem_alloc(size_in_byte);
            break;
        default:
            ptr = NULL;
            break;        
    }
    return ptr;
}
```

**Note**: If you are allocating frame buffers or texture buffer without using the `nema_buffer_create` or `nema_buffer_create_pool` API, make sure they are aligned to 32bytes(for Apollo5) or 8bytes(for Apollo4).

## Cache management in Apollo5

In Apollo5, we should pay attention to the following scenarios to avoid memory consistency issues between CPU and GPU.
### Read/write framebuffer by CPU
Generally speaking, the frame buffer should be only accessed through LVGL APIs. However, for any reason, if users need to read or write it directly by CPU, the following steps should be taken:
```
am_hal_cachectrl_dcache_invalidate(frame_buffer_region, false);

read_write_frame_buffer()

am_hal_cachectrl_dcache_invalidate(frame_buffer_region, true);
```
### Copy image textures or font bitmaps by CPU
If users need to copy image textures or font bitmaps to SSRAM or PSRAM by CPU, the following steps should be taken:
```
copy_texture_bitmap();

am_hal_cachectrl_dcache_clean(bitmap_region);
```

**Note** All frame buffers or texture/font bitmap buffers should be aligned to 32Bytes.

## Power saving
We offer two GPU power-saving schemes:
- Frame based power saving
- Command list based power saving

### Frame based power saving
In this mode, we power on the GPU when the first command list of the current frame is ready to submit to GPU, and power off the GPU after completing the rendering of the current frame.

Enable this mode by adding the following macro definition to your project:
```
NEMA_GFX_POWERSAVE
```

### Command list based power saving
In this mode, we power up GPU when a command list is ready to submit to GPU for execution and power off it in the command list complete interrupt. This approach minimizes the GPU's activation time.

Enable this mode by adding the following macro definitions to your project:
```
NEMA_GFX_POWERSAVE
NEMA_GFX_POWEROFF_END_CL
```


## Restriction

- If the recolor feature is used, we will allocate a temporary buffer in PARAM to hold the recolored source image, this buffer has the size of W\*H\*4, W/H stands for width/hight of source image.
- Only the `LV_BLEND_MODE_NORMAL` blend mode is accelerated by GPU, all other blend modes like `LV_BLEND_MODE_ADDITIVE`,
      `LV_BLEND_MODE_SUBTRACTIVE` etc will be handled by the CPU, so they will slow down the refresh rate.
- A corner case exits where the format of image texture is restricted to BGRA8888, an assertion is added to catch this scenario, see line 263 in lv_draw_img.c. Users are supposed to change their image textures' color format to RGBA8888(**Note: Only the images used in this scenario not all of the image textures.**) or consult us for a workaround when they are trapped to this assert. 
      