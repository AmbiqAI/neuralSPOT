Name:
=====
 watchdog


Description:
============
 Example of a basic configuration of the watchdog.


Purpose:
========
This example shows a simple configuration of the watchdog. It will print
a banner message, configure the watchdog for both interrupt and reset
generation, and immediately start the watchdog timer.
The watchdog ISR provided will 'pet' the watchdog four times, printing
a notification message from the ISR each time.
On the fifth interrupt, the watchdog will not be pet, so the 'reset'
action will eventually be allowed to occur.
On the sixth timeout event, the WDT should issue a system reset, and the
program should start over from the beginning.

Printing takes place over the ITM at 1M Baud.



******************************************************************************


