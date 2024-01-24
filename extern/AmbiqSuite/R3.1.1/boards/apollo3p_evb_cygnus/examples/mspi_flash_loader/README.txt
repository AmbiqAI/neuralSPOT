Name:
=====
 mspi_flash_loader


Description:
============
 MSPI External Flash Loading and Execution Example


Purpose:
========
This example demonstrates loading a binary image from internal
flash to MSPI external Octal flash, then executing the program using
XIP from the external flash.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
The binary must be linked to run from MSPI flash address range
(as specified by BIN_INSTALL_ADDR). The location and size of the binary
in internal flash are specified using BIN_ADDR_FLASH & BIN_SIZE

This example has been enhanced to use the new 'binary patching' feature
This example will not build if proper startup/linker files are not used.

Prepare the example as follows:
1. Generate hello_world example to load and execute at MSPI Flash XIP location 0x04000000.
i. In the /examples/hello_world/iar directory modify the FLASH region as follows:
change "define region ROMEM = mem:[from 0x0000C000 to 0x000FC000];"
to "define region ROMEM = mem:[from 0x04000000 to 0x040F0000];"
ii. Execute "make" in the /examples/hello_world/iar directory to rebuild the project.
2. Copy /examples/hello_world/iar/bin/hello_world.bin into /examples/mspi_flash_loader/
3. Create the binary with mspi_flash_loader + external executable from Step #1.
./mspi_loader_binary_combiner.py --loaderbin iar/bin/mspi_flash_loader.bin --appbin hello_world.bin --install-address 0x04000000 --flags 0x2 --outbin loader_hello_world --loader-address 0x0000C000 --chipType apollo3p
4. Open the J-Link SWO Viewer to the target board.
5. Open the J-Flash Lite program.  Select the /examples/mspi_flash_loader/loader_hello_world.bin file and program at 0x0000C000 offset.

And this example can work on:
Apollo3p_evb + Cygnus
Target hardware uses 1.8V power supply voltage.
Actual Octal flash on Cygnus board is ATXP128 (Device ID: 0x00A91F) instead of ATXP032



******************************************************************************


