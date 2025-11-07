Name:
=====
 pdm_rtt_stream


Description:
============
 An example to show PDM audio streaming to PC over RTT data logger.




Purpose:
========
This example demonstrates PDM audio capture and
 real-time streaming functionality using SEGGER RTT. The application showcases
 digital microphone audio acquisition at 16 kHz sample rate, PCM
 conversion with 24-bit resolution, and stereo channel support.

Section: Key Features
=====================

 1. PDM Audio Capture: Receives audio signals from external digital
    microphone at 16 kHz sample rate for audio recording

 2. RTT Data Streaming: Uploads captured audio data to PC via SEGGER
    RTT for real-time monitoring and analysis

 3. PCM Format Support: Converts PDM data to PCM format with 24-bit
    resolution and stereo channel support

 4. File Logging: Automatically saves captured audio to timestamped
    PCM files for offline analysis and playback

 5. Real Time Monitoring: Provides continuous audio streaming with
    minimal latency for live audio applications and debugging

Section: Functionality
======================

 The application performs the following operations:
 - Configures PDM interface for digital microphone audio capture
 - Captures audio data at 16 kHz sample rate
 - Converts PDM data to 24-bit PCM format with stereo support
 - Streams audio data to PC via SEGGER RTT
 - Logs audio data to timestamped PCM files for offline analysis
 - Provides real-time monitoring and status reporting via SWO

Section: Usage
==============

 1. Connect digital microphone to specified GPIO pins (PDM0_CLK, PDM0_DATA)
 2. Compile and download the application to target device
 3. Run 'python3 rtt_logger.py' to capture PCM data via RTT
 4. Use Audacity to import and analyze the PCM audio data
 5. Monitor SWO output for status and configuration information

Section: Configuration
======================

 - Sample Rate: 16 kHz audio sampling rate
 - PCM Format: 24-bit resolution, stereo channels
 - RTT Interface: Real-time data streaming to PC
 - SWO/ITM: Output for status and results (1MHz)

 General pin connections:
 GPIO_50 PDM0_CLK  to CLK_IN of digital microphone
 GPIO_51 PDM0_DATA to DATA_OUT of digital microphone

 Known Issue: this was tested and worked with Segger Jlink_v764e tools
 It wasn't working with Segger Jlink_v844a

 NOTE:
 In this example, RTT control block is mapped to a fixed address to facilitate
 the searching process. If the address is changed, make sure to modify the
 rtt_logger.py script to match the address, which can be get from SWO prints.

 Printing takes place over the SWO at 1MHz.
 RTT logger takes place over the SWD at 4M Baud.


******************************************************************************


