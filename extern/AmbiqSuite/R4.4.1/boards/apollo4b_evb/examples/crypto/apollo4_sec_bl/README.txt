Name:
=====
 apollo4_secbl


Description:
============
 A simple secondary bootloader program example template for Apollo4


Purpose:
========
This program is an example template for a secondary bootloader program for Apollo4.
It demonstrates how to access OTP key area.\n
It demonstrates how to use the Ambiq SBL OTA
framework for customer specific OTAs, e.g. to support external psram, or to support more
advanced auth/enc schemes.
<br>It demonstrates how to validate & transfer control to the real
main program image (assumed to be at address specified by MAIN_PROGRAM_ADDR_IN_FLASH in mram)
after locking the OTP area before exiting<br><br>

This example only demonstrates nonsecure boot and nonsecure OTA.\n\n

Printing takes place over the ITM at 1M Baud.<br><br>

Additional Information:
=======================
To exercise this program:\n
Flash the main program at 0x100000 (MAIN_PROGRAM_ADDR_IN_FLASH)\n
Link this program at the address suitable for SBL(0x18000) configuration\n
To test OTA - construct images using magic numbers in the range matching AM_IMAGE_MAGIC_CUST\n
To test OTP key area access - need to keep OTP->Security->PLONEXIT as 1\n\n
Memory Addresses Used:\n
======<\n

|   Address   | Description                                                                                                                |\n
|:-----------:|:---------------------------------------------------------------------------------------------------------------------------|\n
| 0x0001,8000 | Location of Sbl (unsecure secondary bootloader) at the standard apollo4 flash address.                                     |\n
| 0x000D,2000 | Location where jlink script will load the unencrypted image blob that contains header info and the hello world application |\n
| 0x0010,0000 | Location where sbl will move the executable portion of the hello_world from 0x000D,2000 + headerSize.                      |\n
| 0x000D,0000 | Location where ota pointers are loaded (by jlink script), this is where the the sbl will find pointers to the blob         |\n
| 0x4002,0264 | Address of OTA register, this is loaded by the jlink script and will point to 0x000D,0000. Used in SBL                     |\n\n

To Start:\n\n

Generate a hello world using a modified IAR linker script. Make sure the flash region (MCU_MRAM) starts at 0x00100000 as follows:\n

@code
Define regions for the various types of internal memory.

define region MCU_MRAM    = mem:[from 0x00100000 to 0x00200000];
define region MCU_TCM     = mem:[from 0x10000000 to 0x10060000];
define region SHARED_SRAM = mem:[from 0x10060000 to 0x10160000];
@endcode

Name the hello_world binary something descriptive, say hello_world_100000.bin<\n

It is easiest to just copy this over to the <em>tools/apollo4b_scripts</em> directory and work from there\n\n

Use one of the example blob creators found in <em>tools/apollo4b_scripts/examples</em>.\n
The output created by examples/firmware, examples/wired_download, or examples/wired_ota should all work\n
For example use <em>examples/firmware</em>,\n

\n
1 Copy the hello_world_bin into the examples/wired_download directory\n
2 edit/check these lines in firmware.ini:\n
- app_file = hello_world_1000000\n
- load_address = 0x100000\n
3 in a bash-shel type make, look at the Makefile for detail\n
4 Now there will be a file: hello_world_100000.ota.\n
- rename this to hello_world_100000.ota.bin, the jlink download command really wants a bin file\n
5 copy this back two levels to the tools directory\n
6 at this point you should already have the apollo4_sec_bl.bin file loaded at 0x18000\n
7 don't have any debugger running\n
8 open up an swo window to see what is happening\n
9 open a cmd window and using the jlink-blob.jlink file below type jlink --CommanderScript jlink-blob.jlink\n
10 this should restart the secondary bootloader, and you should see the sbl process the blob via debug output (SWO).\n
11 Note: now since the image is resident in the MRAM, the secondary bootloader normally will jump to the hello world image.\n
12 Note: to do a full test of the download capability, the resident image at 0x00100000 should be erased before testing\n
13 Note: (depending on your setup, you may have to type python3 instead of python)\n
14 Note: All the scripts were run using python 3.8.10. They didn't work with python 3.9.\n


******************************************************************************


