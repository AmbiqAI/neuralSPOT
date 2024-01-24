Name:
=====
 freertos_lowpower


Description:
============
 Example of the app running under FreeRTOS.


This example implements LED task within the FreeRTOS framework. It monitors
three On-board buttons, and toggles respective on-board LEDs in response.
To save power, this application is compiled without print
statements by default. To enable them, add the following project-level
macro definitions.

AM_DEBUG_PRINTF

If enabled, debug messages will be sent over ITM.


******************************************************************************


