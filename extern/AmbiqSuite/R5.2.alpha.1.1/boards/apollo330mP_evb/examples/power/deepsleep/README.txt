Name:
=====
 deepsleep


Description:
============
 Example demonstrating how to enter deepsleep.




Purpose:
========
This example demonstrates deep sleep functionality
 including memory optimization, ROM auto power down, and minimal DTCM retention.
 The example provides a clean environment for measuring deep sleep current
 without interrupt interference, enabling power consumption analysis for
 low-power applications.

Section: Key Features
=====================

 1. Deep Sleep Mode: Configures device to enter deep sleep mode
    for power conservation and current measurement

 2. Memory Configuration: Optimizes memory retention settings for
    minimum DTCM retention while maintaining essential functionality

 3. Power Measurement: Provides clean environment for measuring
    deep sleep current without interrupt interference

 4. ROM Auto Power Down: Enables ROM automatic power down
    feature for lowest possible power consumption

 5. UART Communication: Initializes UART for status reporting.
    Disables UART before entering sleep mode for debugging and monitoring

Section: Functionality
======================

 The application performs the following operations:
 - Initializes memory for performance and low power operation
 - Configures ROM auto power down for minimum power consumption
 - Optimizes DTCM retention settings for power efficiency
 - Places device into deep sleep mode for current measurement
 - Provides UART communication for status reporting and debugging
 - Enables power measurement environment

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor UART output for initialization and status messages
 3. Observe device entering deep sleep mode
 4. Measure current draw during deep sleep operation
 5. Verify power consumption features

Section: Configuration
======================

 - ALL_RETAIN: Memory retention configuration (0 for min TCM, 1 for all)
 - UART: Communication interface (115,200 BAUD, 8 bit, no parity)
 - ROM_MODE: ROM auto power down configuration
 - DTCM_CONFIG: DTCM memory configuration for power optimization
 
   1) Initialize memory for performance and low power
   2) Place device into deepsleep
   3) Measure current draw
 
 The example begins by printing out a banner announcement message through
 the UART, which is then completely disabled for the remainder of execution.
 
 Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
 Please note that text end-of-line is a newline (LF) character only.
 Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


