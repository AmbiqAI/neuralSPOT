Name:
=====
 reset_states


Description:
============
 Example of various reset options in Apollo.


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

The program will repeat the following sequence on the console:
(POI Reset) 5 Interrupts - (WDT Reset) 3 Interrupts - (POR Reset) 3 Interrupts

Printing takes place over the ITM at 1M Baud.



******************************************************************************


