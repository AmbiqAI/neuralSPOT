Name:
=====
 ctimer_stepper_synch_64bit_pattern


Description:
============
 CTimer Stepper Motor Synchronized 64 bit Pattern Example


Purpose:
========
This example demonstrates how to create arbitrary patterns on
CTimer.  TMR0 A is used to create base timing for the pattern.
and TMR1 A is configured to trigger on TMR0.
All timers are configured to run and then synchronized off of
the global timer enable.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
The patterns are output as follows:
Pin12 - TMR0 A trigger pulse
Pin18 - TMR1 A pattern



******************************************************************************


