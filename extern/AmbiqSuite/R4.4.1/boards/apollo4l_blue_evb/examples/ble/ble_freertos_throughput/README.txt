Name:
=====
 ble_freertos_throughput


Description:
============
 ARM Cordio BLE - Ambiq Micro Throughput Example.


Purpose:
========
This example implements is designed to test the data transmitting
and receiving thourghput. The example needs to be connected with the Ambiq BLE
Test APP (only Android now), then sends data to the APP by Notify method (Uplink)
or receives data from APP by Write Command method (Downlink) to check the rate
showed in the APP screen. The default MTU is 23 and PHY is 1M, you can request
the MTU exchange and PHY change through the settings of APP.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

If enabled, debug messages will be sent over ITM at 1M Baud.


******************************************************************************


