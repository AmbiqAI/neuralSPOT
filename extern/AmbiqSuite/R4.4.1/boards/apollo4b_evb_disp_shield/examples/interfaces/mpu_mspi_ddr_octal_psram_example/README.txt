Name:
=====
 mpu_mspi_ddr_octal_psram_example


Description:
============
 Example of using MPU protection as workaround for DSP RAM issue.


Purpose:
========
This example demonstrates how to use MPU as workaround for DSP RAM
issue.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


