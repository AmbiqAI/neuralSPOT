Name:
=====
 pdm_to_i2s


Description:
============
 An example to show PDM to I2S(controller) operation.




Purpose:
========
This example demonstrates PDM to I2S audio conversion
 functionality for digital microphone integration with external audio devices.
 The application showcases audio signal processing, converting PDM
 data from digital microphones to PCM format and forwarding it via I2S
 interface.

Section: Key Features
=====================

 1. PDM Interface: Receives audio signals from external digital microphone
    at 16 kHz sample rate with 2.048 MHz PDM clock

 2. PCM Conversion: Converts PDM data to PCM format for standard
    audio processing and transmission

 3. I2S Output: Forwards converted PCM data to external I2S devices
    for audio playback or further processing

 4. Digital Microphone Support: Compatible with external digital
    microphones requiring PDM interface

 5. Real Time Processing: Provides continuous audio streaming with
    minimal latency for live audio applications

Section: Functionality
======================

 The application performs the following operations:
 - Configures PDM interface for digital microphone audio capture
 - Captures audio data at 16 kHz sample rate with 2.048 MHz PDM clock
 - Converts PDM data to PCM format for standard audio processing
 - Forwards PCM data to external I2S devices via I2S interface
 - Implements real-time audio streaming with minimal latency
 - Provides status reporting via ITM/SWO

Section: Usage
==============

 1. Connect digital microphone to PDM interface pins (PDM0_CLK, PDM0_DATA)
 2. Connect external I2S device to I2S interface pins (SCLK, WS, DATA_IN)
 3. Compile and download the application to target device
 4. Monitor ITM output for status and configuration information
 5. Observe real-time audio conversion and streaming

Section: Configuration
======================

 - Sample Rate: 16 kHz audio sampling rate
 - PDM Clock: 2.048 MHz PDM clock frequency
 - PCM Format: Standard PCM audio format
 - I2S Interface: Controller mode I2S output
 - ITM/SWO: Output for status and results (1MHz)

 General pin connections:
 GPIO_50 PDM0_CLK      to CLK_IN of digital microphone
 GPIO_51 PDM0_DATA     to DATA_OUT of digital microphone
 I2S_CLK_GPIO_PIN      to SCLK or BIT_CLK of external I2S device
 I2S_WS_GPIO_PIN       to WS or FRAM_CLK of external I2S device
 I2S_DATA_OUT_GPIO_PIN to DATA_IN of external I2S device

 Printing takes place over the ITM at 1MHz.


******************************************************************************


