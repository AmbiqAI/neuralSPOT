Name:
=====
 ble_freertos_beaconscanner


Description:
============
 ARM Cordio BLE - Beaconscanner Application Example.


Purpose:
========
This example implements a BLE beacon scanner within the FreeRTOS
framework. To minimize power usage, this application is compiled without
debug printing by default (the non-"lp" version of this example enables
them by default).

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


