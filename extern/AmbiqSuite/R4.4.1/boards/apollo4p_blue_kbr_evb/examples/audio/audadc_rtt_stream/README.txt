Name:
=====
 audadc_rtt_stream


Description:
============
 This example uses AUDADC to capture and send audio data to PC via SEGGER RTT.


Purpose:
========
This example uses AUDADC INTTRIGTIMER to capture audio samples at 16 kHz
LPMODE1 is used for power efficiency
DMA is used to transfer samples from the AUDADC FIFO into an SRAM buffer

Additional Information:
=======================
Please set AUDADC_EXAMPLE_DEBUG to 1 when running in debug (rtt) mode.

@RTT Streaming: Run rtt_logger.py to capture pcm raw data via PC.
Should modify -RTTAddress in rtt_logger.py to _SEGGER_RTT address in audadc_rtt_stream.map
The data saved as stereo(L:low gain/R: high gain)

@pcm_to_wav.py:  Convert pcm raw data to wav file.
Save mono left channel to destination file.



******************************************************************************


