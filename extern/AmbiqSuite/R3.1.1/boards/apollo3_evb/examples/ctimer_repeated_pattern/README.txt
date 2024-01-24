Name:
=====
 ctimer_repeated_pattern


Description:
============
 CTimer Repeated Pattern Example


Purpose:
========
This example demonstrates how to create arbitrary repeated pattern on
CTimer.  TMR0 A is used to create base timing for the pattern.  TMR0 B
is configured to terminated on TMR0. All timers are configured to run and
then synchronized off of the global timer enable.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
The patterns are output as follows:
Pin12 - TMR0 A terminate pulse
Pin13 - TMR0 B pattern



******************************************************************************


