Name:
=====
 watchdog


Description:
============
 Example of a basic configuration and usage of the watchdog.



Purpose:
========
This example demonstrates watchdog timer configuration
 and operation for system monitoring and recovery. The application showcases
 watchdog setup with both interrupt and reset generation capabilities,
 implementing a monitoring system that provides multiple
 levels of system protection. The watchdog ISR demonstrates proper
 interrupt handling and timer management, with controlled system reset
 behavior for system operation and recovery.

Section: Key Features
=====================

 1. Watchdog Configuration: Demonstrates basic watchdog setup with
    interrupt and reset generation capabilities

 2. Interrupt Handling: Implements watchdog interrupt service routine
    for monitoring and control of watchdog behavior

 3. Reset Generation: Configures watchdog to trigger system reset
    after timeout for system recovery and safety

 4. Timer Management: Provides watchdog timer control with configurable
    timeout values and clock sources

 5. System Recovery: Demonstrates automatic system recovery through
    watchdog reset functionality for operation

Section: Functionality
======================

 The application performs the following operations:
 - Configures watchdog timer with interrupt and reset capabilities
 - Sets up watchdog ISR for monitoring and control operations
 - Implements controlled watchdog petting sequence (4 times)
 - Demonstrates system reset behavior after timeout
 - Provides real-time monitoring via ITM debug interface
 - Implements system recovery mechanisms
 - Monitors and reports watchdog status and behavior

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM output for watchdog status and interrupt messages
 3. Observe watchdog petting sequence and reset behavior
 4. Verify system recovery and restart functionality

Section: Configuration
======================

 - WDT_CLOCK_SOURCE: Configurable watchdog clock source (default: LFRC_DIV64)
 - WDT_INTERRUPT_TIMEOUT: Interrupt timeout value (default: 12 seconds)
 - WDT_RESET_TIMEOUT: Reset timeout value (default: 15 seconds)
 - WDT_PET_COUNT: Number of watchdog pets before reset (default: 4)

 Printing takes place over the ITM at 1MHz.



******************************************************************************


