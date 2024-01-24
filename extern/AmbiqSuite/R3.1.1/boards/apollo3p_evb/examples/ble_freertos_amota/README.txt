Name:
=====
 ble_freertos_amota


Description:
============
 ARM Cordio BLE - Ambiq Micro Over the Air (AMOTA) Example.


Purpose:
========
This example implements Ambiq Micro Over-the-Air (OTA) slave.  This
example is designed to allow loading of a binary software update from either
and iOS or Android phone running Ambiq's application.  This example works
with the Apollo3 Secure Bootloader (SBL) to place the image in flash and then
reset the Apollo3 to allow SBL to validate and install the image.

AM_DEBUG_PRINTF
WSF_TRACE_ENABLED=1

If enabled, debug messages will be sent over ITM at 1M Baud.

Additional Information:
=======================
The directory \tools\apollo3_amota\scripts contains a Makefile which will
build the OTA binary.

The directory \docs\app_notes\amota explains how to use the Ambiq iOS and
Android applications.


******************************************************************************


