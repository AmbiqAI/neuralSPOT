
# nema_vg_font_convert

## Overview

`nema_vg_font_convert` is a tool designed to convert `.ttf` font files into vector graphic formats compatible with **NemaSDK**. These formats enable seamless integration of fonts into embedded systems and applications built with NemaSDK.

---

## Usage

`nema_vg_font_convert` is a command-line utility for generating vector font files in `.c`, and `.h` formats, compatible with the **NEMA|VG** API.

### Command Syntax
```bash
nema_vg_font_convert [OPTIONS]... [FILE]...
```

### Options
- **`-r, --range`**  
  Specify a range of characters by defining the start and end.  
  Example: `-r 0x20-0x7e` or `-r 32-127`.

- **`-a, --ascii`**  
  Include the ASCII character range. Equivalent to `-r 0x20-0x7e`.

- **`-g, --greek`**  
  Include the Greek character range. Equivalent to `-r 0x370-0x3ff`.

- **`-k, --kerning`**  
  Enable kerning information in the generated font files.

- **`-h, --help`**  
  Display help information and exit.

### Examples
1. Generate an ASCII vector font:
   ```bash
   nema_vg_font_convert -a example.ttf -o font_ascii.bin
   ```
   Output file: `font_ascii.bin`

2. Include a custom character range (32-127) with kerning enabled:
   ```bash
   nema_vg_font_convert -r 32-127 -k example.ttf -o font_custom_range.bin
   ```
   Output file: `font_custom_range.bin`

3. Add Greek characters:
   ```bash
   nema_vg_font_convert -g example.ttf -o font_greek.bin
   ```
   Output file: `font_greek.bin`

4. Display help information:
   ```bash
   nema_vg_font_convert -h
   ```

---

## System Requirements

### Tested Environment
- **OS**: Ubuntu 22.04.5 x86_64  
- **CPU**: x86_64  
- **Kernel**: 6.5.0-35-generic  

### Dependencies
- **FreeType2**: Required for font rendering.  
  [Download here](https://sourceforge.net/projects/freetype/files/) and refer to `freetype-2.xx.x/docs/INSTALL.UNIX` for installation instructions.

---

## Integration into Embedded Systems

The tool generates `.c` and `.h` files for font management, which can be integrated into embedded systems using the following steps:

1. Include the `.c` and `.h` files in your project.
2. During compilation, font vector path coordinate data is stored in the `.rodata` section of the firmware.

## Bin format
If you need to save the generated vector path coordinate data in binary format and store it in the file system, you can use the `xxd` command for conversion. For details on loading and integrating binary files, please refer to the `how_to_use.md` guide in `third_party/ThinkSi/NemaGFX_SDK/utils/fontConvert`.
