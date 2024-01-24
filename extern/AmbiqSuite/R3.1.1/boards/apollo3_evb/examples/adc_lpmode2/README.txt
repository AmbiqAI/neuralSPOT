Name:
=====
 adc_lpmode2


Description:
============
 This example takes samples with the ADC at 1Hz in lowest power mode.


Purpose:
========
To demonstrate the lowest possible power usage of the ADC.  The
example powers off the ADC between samples.  CTIMER-A1 is used to drive the
process.  The CTIMER ISR reconfigures the ADC from scratch and triggers each
sample.  The ADC ISR stores the sample and shuts down the ADC.

Additional Information:
=======================
The ADC_EXAMPLE_DEBUG flag is used to display information in the example to
show that it is operating.  This should be set to 0 for true low power
operation.

Printing takes place over the ITM at 1M Baud.



******************************************************************************


