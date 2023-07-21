Name:
=====
 emmc_bm_rpmb


Description:
============
 emmc rpmb example.


Purpose:
========
This example demonstrates how to use APIs in eMMC RPMB driver
to access RPMB partition.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


