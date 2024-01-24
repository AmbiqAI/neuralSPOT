Name:
=====
 adc_vbatt


Description:
============
 Example of ADC sampling VBATT voltage divider, BATT load, and temperature.


Purpose:
========
This example initializes the ADC, and a timer. Two times per second it
reads the VBATT voltage divider and temperature sensor and prints them.
It monitors button 0 and if pressed, it turns on the BATT LOAD resistor.
One should monitor MCU current to see when the load is on or off.

Printing takes place over the ITM at 1M Baud.


******************************************************************************


