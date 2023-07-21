
Name:
=====
 adc_measure


Description:
============
 Example of ADC sampling the voltage applied to a particular pin.


Purpose:
========
This example initializes the ADC and periodically samples the
voltage as applied to the ADC input configured on pin 18.

On interrupt, the samples are pulled from the FIFO, the example then
applies Vref to the sample and displays the measured voltage to SWO.

It should be noted that he pin number used for input is designated in the
example source code via the define ADC_INPUT_PIN.


******************************************************************************


