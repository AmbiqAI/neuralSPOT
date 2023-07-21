Name:
=====
 ble_freertos_adv_ext


Description:
============
 ARM Cordio BLE - Advertising Extension Application Example.


Purpose:
========
This example implements advertising extension within the FreeRTOS
framework. To verify extended advertising working routines.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


