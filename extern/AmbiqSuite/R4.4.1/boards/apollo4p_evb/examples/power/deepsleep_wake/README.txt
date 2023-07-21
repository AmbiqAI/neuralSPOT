Name:
=====
 deepsleep_wake


Description:
============
 Example that goes to deepsleep and wakes from either the RTC or GPIO.


Purpose:
========
This example configures the device to go into a deep sleep mode. Once in
deep sleep the RTC peripheral will wake the device every second, check to
see if 5 seconds has elapsed and then toggle LED1.

Alternatively, it will awake when button 0 is pressed and toggle LED0.

The example begins by printing out a banner annoucement message through
the UART, which is then completely disabled for the remainder of execution.

Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
Please note that text end-of-line is a newline (LF) character only.
Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


