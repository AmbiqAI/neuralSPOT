Name:
=====
 iom_power_example


Description:
============
 Example that demonstrates IOM power consumption, connecting to a FRAM

Purpose:
========
FRAM is initialized with a known pattern data using Blocking IOM Write.
This example starts a 1 second timer. At each 1 second period, it initiates
reading a fixed size block from the FRAM device using Non-Blocking IOM
Read, and comparing against the predefined pattern

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
USE I2C FRAM on Cygnus: MB85RC64TA
Recommend to use 1.8V power supply voltage.



******************************************************************************


