Name:
=====
 emmc_bm_fatfs


Description:
============
 eMMC bare-metal FatFs example.


Purpose:
========
This example demonstrates how to use file system with eMMC device
based on the eMMC bare-metal HAL.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


