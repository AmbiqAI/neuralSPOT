Name:
=====
 adc_lpmode0_dma


Description:
============
 This example takes samples with the ADC at high-speed using DMA.


Purpose:
========
This example shows the CTIMER-A3 triggering repeated samples of an external
input at 1.2Msps in LPMODE0.  The example uses the CTIMER-A3 to trigger
ADC sampling.  Each data point is 128 sample average and is transferred
from the ADC FIFO into an SRAM buffer using DMA.

Printing takes place over the ITM at 1M Baud.



******************************************************************************


