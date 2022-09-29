Name:
=====
 ble_freertos_fit


Description:
============
 ARM Cordio BLE - Fit Application Example.


Purpose:
========
This example implements a BLE heart rate sensor within the FreeRTOS
framework. To minimize power usage, this application is compiled without
debug printing by default (the "lp" version of this example excludes
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


