Name:
=====
 pdm_fft


Description:
============
 An example to show basic PDM operation.




Purpose:
========
This example demonstrates PDM interface operation
 for digital microphone audio capture and real-time frequency analysis using
 multiple FFT algorithms. The application showcases audio signal
 acquisition at 16 kHz sample rate, implements various FFT techniques for
 spectral analysis, and identifies dominant frequency components in the
 captured audio.

Section: Key Features
=====================

 1. PDM Interface: Receives audio signals from digital microphone at
    16 kHz sample rate for audio input

 2. FFT Analysis: Performs various FFT algorithms including Complex FFT,
    Real FFT, and Q31 FFT for frequency domain analysis

 3. Frequency Detection: Calculates and identifies the dominant frequency
    components in the audio signal for spectral analysis

 4. Multiple FFT Algorithms: Supports cfft, rfft, rfft_optim_input,
    q31fft, and q31_optimized input for different processing requirements

 5. Real Time Processing: Provides continuous audio analysis with
    minimal latency for live frequency monitoring applications

Section: Functionality
======================

 The application performs the following operations:
 - Configures PDM interface for digital microphone audio capture
 - Captures audio data at 16 kHz sample rate
 - Implements multiple FFT algorithms for spectral analysis
 - Identifies and reports dominant frequency components
 - Provides real-time audio analysis and status reporting via SWO

Section: Usage
==============

 1. Connect digital microphone to specified GPIO pins (CLK_IN, DATA_OUT)
 2. Compile and download the application to target device
 3. Monitor SWO output for frequency analysis results
 4. Observe real-time dominant frequency detection

Section: Configuration
======================

 - Sample Rate: 16 kHz audio sampling rate
 - FFT Size: Configurable FFT size (default: 1024)
 - FFT Algorithms: cfft, rfft, rfft_optim_input, q31fft, q31_optimized
 - SWO/ITM: Output for status and results (1MHz)

 The data is captured from a digital microphone with the following pin connections:

 - **GPIO 50** to **CLK_IN** of the digital microphone
 - **GPIO 51** to **DATA_OUT** of the digital microphone

 Printing is done over the **SWO** at 1 Mbps baud rate.

 NOTE: The system configuration, such as clock source and PDM settings, is highly
       specific to the Apollo5 hardware platform, so adjust the configuration accordingly.



******************************************************************************


