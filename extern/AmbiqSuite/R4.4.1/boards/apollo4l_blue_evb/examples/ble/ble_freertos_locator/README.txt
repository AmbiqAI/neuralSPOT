Name:
=====
 ble_freertos_locator


Description:
============
 ARM Cordio BLE - Locator Application Example.


Purpose:
========
This example implements a BLE locater application within the FreeRTOS
framework. The locator will work as master to connect the asset tag device and
receive IQ data by sending CTE request.

Additional Information:
=======================
To enable debug printing, add the following project-level macro definitions.
Press button0 of Apollo4 EVB to scan/connect the asset tag device.
Press button1 of Apollo4 EVB to start CTE request after connection.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

When defined, debug messages will be sent over ITM/SWO at 1M Baud.

Note that when these macros are defined, the device will never achieve deep
sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


