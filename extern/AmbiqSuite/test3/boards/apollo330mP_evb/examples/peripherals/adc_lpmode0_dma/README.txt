Name:
=====
 adc_lpmode0_dma


Description:
============
 This example takes samples with the ADC at high-speed using DMA.



Purpose:
========
This example demonstrates high-speed ADC sampling using DMA in
 Low Power Mode 0 (LPMODE0). The application showcases ADC
 configuration with internal timer triggering, efficient data transfer
 using DMA, and low-power operation for high-throughput data acquisition.
 The example implements circular buffer management and real-time data
 processing for analog signal capture.

Section: Key Features
=====================

 1. High Speed ADC Sampling: Achieves 1.2Msps sampling rate
    using internal timer and HFRC clock

 2. DMA Data Transfer: Utilizes DMA to efficiently move ADC data
    from FIFO to SRAM buffer with minimal CPU intervention

 3. Low Power Operation: Operates ADC in LPMODE0 for reduced
    power consumption during high-speed sampling

 4. Circular Buffer Management: Implements circular buffer for
    continuous data acquisition and processing

 5. Real Time Data Processing: Supports real-time data
    processing and status reporting via SWO

Section: Functionality
======================

 The application performs the following operations:
 - Configures ADC with internal timer and HFRC clock
 - Sets up DMA for efficient data transfer to SRAM buffer
 - Implements circular buffer management for continuous sampling
 - Processes and averages ADC samples in real time
 - Reports status and results via SWO/ITM

Section: Usage
==============

 1. Compile and download the application to target device
 2. Connect analog input to ADC SE0 pin (Pin 19)
 3. Monitor SWO output for sampling status and results
 4. Observe high-speed, low-power ADC operation

Section: Configuration
======================

 - ADC_SAMPLE_BUF_SIZE: Configurable ADC sample buffer size (default: 128)
 - ADC clock: Internal timer clocked by HFRC
 - SWO/ITM: Output for status and results (1MHz)


******************************************************************************


