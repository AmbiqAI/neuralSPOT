Name:
=====
 ble_freertos_assettag


Description:
============
 ARM Cordio BLE - Asset tag Application Example.


Purpose:
========
This example implements a BLE asset tag application within the
FreeRTOS framework. This example can work as a slave connecting with
ble_freertos_locator. Then reports IQ data with AoA type to master after
receiving the CTE request.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


