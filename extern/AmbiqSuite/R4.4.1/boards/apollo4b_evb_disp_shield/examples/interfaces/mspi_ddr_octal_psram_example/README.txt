Name:
=====
 mspi_ddr_octal_psram_example


Description:
============
 Example of the MSPI operation with DDR OCTAL SPI PSRAM.


Purpose:
========
This example demonstrates MSPI DDR OCTAL operation using the PSRAM
device.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


