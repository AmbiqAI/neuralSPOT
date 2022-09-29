Name:
=====
 emmc_raw_block_read_write


Description:
============
 emmc raw block read and write example.


Purpose:
========
This example demonstrates how to blocking PIO and DMA read & write
APIs with eMMC device.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


