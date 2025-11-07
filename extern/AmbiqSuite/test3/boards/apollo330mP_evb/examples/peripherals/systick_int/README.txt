Name:
=====
 systick_int


Description:
============
 A simple example of using the SysTick interrupt.



Purpose:
========
This example demonstrates SysTick interrupt
 functionality for system timing and periodic task scheduling. The
 application showcases SysTick timer configuration, interrupt handling,
 and periodic task execution for real-time system operations.

Section: Key Features
=====================

 1. SysTick Configuration: Demonstrates SysTick
    timer setup and interrupt configuration for system timing

 2. Interrupt Service Routine: Implements SysTick interrupt
    handler for periodic task execution and timing control

 3. Periodic Task Scheduling: Provides periodic task execution
    with configurable timing intervals for system operations

 4. Real Time Monitoring: Implements real-time status monitoring
    and periodic debug output via SWO interface

 5. Timing Control: Provides precise timing control with
    configurable interrupt intervals and task scheduling

Section: Functionality
======================

 The application performs the following operations:
 - Initializes SysTick timer with configurable interrupt intervals
 - Implements SysTick interrupt service routine for periodic tasks
 - Provides periodic status monitoring and debug output
 - Implements precise timing control for system operations
 - Demonstrates proper interrupt handling and task scheduling
 - Monitors system timing and provides real-time status updates

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor SWO output for periodic timing status updates
 3. Verify SysTick interrupt operation and timing accuracy
 4. Observe periodic task execution and system timing

Section: Configuration
======================

 - INT_NUM_MS: Configurable interrupt interval (default: 50ms)
 - SYSTICK_CNT: SysTick counter value calculation
 - PNT_PERIOD_MS: Print period for status updates (default: 2000ms)
 - AM_DEBUG_PRINTF: Enables detailed debug output via SWO

 Since the clock to the core is gated during sleep, whether deep sleep or
 normal sleep, the SysTick interrupt cannot be used to wake the device.


******************************************************************************


