Name:
=====
 rtc_print


Description:
============
 Example using the internal RTC.


This example demonstrates how to interface with the RTC and prints the
time over SWO.

The example works by configuring a timer interrupt which will periodically
wake the core from deep sleep. After every interrupt, it prints the current
RTC time.



******************************************************************************


