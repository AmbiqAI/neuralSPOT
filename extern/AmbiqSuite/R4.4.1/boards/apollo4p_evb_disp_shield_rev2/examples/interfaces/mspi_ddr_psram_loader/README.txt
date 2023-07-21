Name:
=====
 mspi_ddr_psram_loader


Description:
============
 Example demonstrating how to load and run binary image to MSPI external DDR PSRAM.


Purpose:
========
The binary must be linked to run from MSPI PSRAM address range
(as specified by BIN_INSTALL_ADDR). The location and size of the binary
in internal flash are specified using BIN_ADDR_FLASH & BIN_SIZE

This example has been enhanced to use the new 'binary patching' feature
This example will not build if proper startup/linker files are not used.

Additional Information:
=======================
Prepare the example as follows:
1. Generate hello_world example to load and execute at MSPI PSRAM XIP location 0x14000000.
i. In the /examples/hello_world/iar directory modify the MRAM region as follows:
define region MCU_MRAM    = mem:[from 0x00018000 to 0x00200000];
define region MCU_MRAM    = mem:[from 0x14000000 to 0x18000000];
ii. Execute "make" in the /examples/hello_world/iar directory to rebuild the project.
2. Copy /examples/hello_world/iar/bin/hello_world.bin into /boards/common4/examples/interfaces/mspi_ddr_psram_loader/
3. Create the binary with mspi_ddr_psram_loader + external executable from Step #1.
./mspi_loader_binary_combiner.py --loaderbin iar/bin/mspi_ddr_psram_loader.bin --appbin hello_world.bin --install-address 0x14000000 --flags 0x2 --outbin loader_hello_world --loader-address 0x00018000
4. Open the J-Link SWO Viewer to the target board.
5. Open the J-Flash Lite program.  Select the /examples/interfaces/mspi_ddr_psram_loader/loader_hello_world.bin file and program at 0x00018000 offset.

If Apollo4 EB board and DDR PSRAM daughter board are used, this example can work on:
Plugin DDR PSRAM daughter board to Apollo4 EB board, press SW2 button for 7 times to switch to the DDR PSRAM.



******************************************************************************


