Name:
=====
 ble_freertos_eddystone_url


Description:
============
 ARM Cordio BLE - eddystone_url Example


Purpose:
========
This is a standard BLE eddystone_url example.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.



******************************************************************************


