Name:
=====
 ctimer_multi_bidirectional_stepper


Description:
============
 CTimer multiple bidirectional stepper motors Example,


Purpose:
========
This example demonstrates how to create arbitrary patterns on multiple
CTimers.  TMR6 A is used to create base timing for the patterns.  TMR0 B
TMR1 A and TMR1 B are configured to dual edge trigger on TMR6 with separate
counting patterns. All timers are configured to run and then synchronized
off of the global timer enable.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
The patterns are output as follows:
Pin12 - TMR6 A dual edge trigger pulse
Pin13 - TMR0 B positive pattern
Pin18   TMR1 A negative pattern
Pin19   TMR1 B inactive pattern


******************************************************************************


