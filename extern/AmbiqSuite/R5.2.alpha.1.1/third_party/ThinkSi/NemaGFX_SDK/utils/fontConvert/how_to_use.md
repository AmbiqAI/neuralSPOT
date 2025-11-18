
# nema_font_convert

## Overview
`nema_font_convert` is a tool designed to convert `.ttf` font files into formats compatible with **NemaSDK**, offering two primary output types:
1. **C source files** (`.c` and `.h`): Font data is embedded directly into firmware.
2. **Bitmap files** (`.bin`): Font rendered as bitmap images for external storage.

These formats enable seamless integration of fonts into embedded systems and applications built with NemaSDK.

---

## Usage

`nema_font_convert` is a command-line utility for generating font files compatible with the **NEMA|gfx** graphics API.

### Command Syntax
```bash
nema_font_convert [OPTIONS]... [FILE]...
```

### Options
- **`-s, --size`**: Specify font size.
- **`-b, --bpp`**: Set bits per pixel, e.g., `-b 8`.
- **`-r, --range`**: Specify a character range, e.g., `-r 0x20-0x7e` or `-r 32-127`.
- **`-a, --ascii`**: Include ASCII range (equivalent to `-r 0x20-0x7e`).
- **`-g, --greek`**: Include Greek characters (equivalent to `-r 0x370-0x3ff`).
- **`-k, --kerning`**: Enable kerning.
- **`-i, --indices`**: Include specific glyph indices, e.g., `-i [4,17,20]`.
- **`-h, --help`**: Display help and exit.

### Examples
1. Generate an ASCII font (size 16, 8 bpp):
   ```bash
   nema_font_convert -s 16 -b 8 -a example.ttf
   ```
2. Include a custom range (32-127) with kerning:
   ```bash
   nema_font_convert -s 20 -r 32-127 -k example.ttf
   ```
3. Render Greek characters (4 bpp):
   ```bash
   nema_font_convert -s 18 -b 4 -g example.ttf
   ```
4. Add specific glyph indices:
   ```bash
   nema_font_convert -s 14 -b 8 -i [4,17,20] example.ttf
   ```

---

## System Requirements

### Tested Environment
- **OS**: Ubuntu 22.04.5 x86_64  
- **CPU**: x86_64  
- **Kernel**: 6.5.0-35-generic  

### Dependencies
- **FreeType2**: Required for font rendering. [Download here](https://sourceforge.net/projects/freetype/files/) and refer to `freetype-2.xx.x/docs/INSTALL.UNIX` for installation instructions.

---

## Integration into Embedded Systems

The tool generates `.c`, `.h`, and `.bin` files for font management. These can be integrated into embedded systems using the following methods:

### Method 1: Direct Embedding
1. Include the `.c` and `.h` files in the project.
2. During compilation, font bitmap data is stored in the `.rodata` section of the firmware.

### Method 2: Dynamic Loading
1. Include the `.c` and `.h` files in the project.
2. Define the macro `NEMA_FONT_LOAD_FROM_BIN` to store only glyph indices in `.rodata`.
3. Save the `.bin` file in external storage (e.g., EMMC, NOR Flash).
4. At runtime, dynamically load font bitmap data from the `.bin` file into GPU-accessible memory (e.g., SSRAM, PSRAM) using a file system or direct memory access. 

A demo to load the bitmap from file system:
```c
#define NEMA_FONT_LOAD_FROM_BIN

#define NEMA_FONT_IMPLEMENTATION

#include "DejaVuSerif8pt2b.h"

#undef NEMA_FONT_IMPLEMENTATION


nema_buffer_t load_bin_file(const char *filename, int length, void *buffer)
{
    FILE * f = fopen (filename, "rb");
    nema_buffer_t bo = {0};

    if (f != NULL) {
        if (length < 0) {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
        }

        if ( !buffer ) {
            bo = nema_buffer_create_pool(NEMA_MEM_POOL_ASSETS, length);
            buffer = nema_buffer_map(&bo); 
        }

        if (buffer != NULL) {
            int size = fread (buffer, 1, length, f);
            if(size != length) {
                printf("Cannot fread\n");
            }
        } else {
            printf("Cannot allocate buffer\n");
        }

        fclose (f);

        return bo;
    } else {
        printf("Can't open file %s\n", filename);
    }

    return bo;
}

void load_font{

   ......
   DejaVuSerif8pt2b.bo = load_bin_file("DejaVuSerif8pt2b.bin", -1, 0);
   ......
}

```


