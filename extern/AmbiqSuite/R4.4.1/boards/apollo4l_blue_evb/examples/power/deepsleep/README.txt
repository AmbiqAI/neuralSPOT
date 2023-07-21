Name:
=====
 deepsleep


Description:
============
 Example demonstrating how to enter deepsleep.


Purpose:
========
This example configures the device to go into a deep sleep mode. Once in
sleep mode the device has no ability to wake up. This example is merely to
provide the opportunity to measure deepsleep current without interrupts
interfering with the measurement.

The example begins by printing out a banner announcement message through
the UART, which is then completely disabled for the remainder of execution.

Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
Please note that text end-of-line is a newline (LF) character only.
Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


