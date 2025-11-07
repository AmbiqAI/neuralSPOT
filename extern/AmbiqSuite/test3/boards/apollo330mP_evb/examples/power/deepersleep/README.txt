Name:
=====
 deepersleep


Description:
============
 Example that goes to "deeper" sleep.




Purpose:
========
This example demonstrates "deeper" sleep functionality
 for measuring minimum deep sleep current of the device. The application
 showcases power management with SYSDEEPERSLEEP level sleep,
 memory configuration optimization, and low power operation.

Section: Key Features
=====================

 1. Deeper Sleep Mode: Implements SYSDEEPERSLEEP level sleep
    for minimum power consumption measurement

 2. Power Measurement: Provides power measurement
    capabilities for deep sleep current evaluation

 3. Memory Configuration: Optimizes memory configuration for
    low power operation with configurable retention

 4. ROM Power Down: Enables ROM automatic power down feature
    for lowest possible power consumption

 5. UART Disable: Completely disables UART after banner message
    for power optimization

Section: Functionality
======================

 The application performs the following operations:
 - Initializes deeper sleep with optimized memory configuration
 - Implements SYSDEEPERSLEEP level sleep for minimum power
 - Provides power measurement capabilities
 - Configures ROM automatic power down for low power
 - Disables UART after banner message for power optimization
 - Implements deeper sleep functionality

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor initial UART output for banner message
 3. Measure minimum deep sleep current of the device
 4. Observe power optimization and deeper sleep operation
 5. Evaluate low power consumption capabilities

Section: Configuration
======================

 - ALL_RETAIN: Memory retention configuration (0 for min, 1 for all)
 - ROM Mode: ROM automatic power down configuration
 - Memory Configuration: TCM and SRAM configuration for power
 - UART Configuration: 115,200 BAUD, 8 bit, no parity

 Note that this example differs from deepsleep in that it goes to
 the SYSDEEPERSLEEP level of sleep, whereas deepsleep attains the
 SYSDEEPSLEEP level.

 The example begins by printing out a banner annoucement message through
 the UART, which is then completely disabled for the remainder of execution.

 Please note that text end-of-line is a newline (LF) character only.
 Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


