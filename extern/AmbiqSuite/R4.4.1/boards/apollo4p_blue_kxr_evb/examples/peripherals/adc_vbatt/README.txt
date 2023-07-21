Name:
=====
 adc_vbatt


Description:
============
 Example of ADC sampling VBATT voltage divider, BATT load, and temperature.


Purpose:
========
This example initializes the ADC. About two times per second it reads
the VBATT voltage divider and temperature sensor and prints the results.

It also monitors button 0 and if pressed, toggles between enabling and
disabling the BATTLOAD resistor.
One would want to monitor MCU current to see when the load is on or off.

The expected VBATT voltage as measured for the Apollo4 EVB is ~0.65V.
In general the measured voltage should be VDD / 3.

Printing takes place over the SWO at 1M Baud.


******************************************************************************


