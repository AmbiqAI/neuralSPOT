Name:
=====
 stimer


Description:
============
 Example using a STIMER with interrupts.



Purpose:
========
This example demonstrates STIMER (Secure Timer)
 functionality for precise timing and interrupt-driven operations. The
 application showcases STIMER configuration, interrupt handling, and
 periodic task execution with LED control for visual feedback. The
 example implements timing control using 32kHz crystal
 oscillator for accurate timekeeping and demonstrates proper interrupt
 service routine management for reliable system timing.

Section: Key Features
=====================

 1. STIMER Configuration: Demonstrates STIMER setup
    and interrupt configuration for precise timing operations

 2. Interrupt Driven Control: Implements STIMER interrupt service
    routine for periodic task execution and timing control

 3. LED Visual Feedback: Provides LED control for visual
    feedback of timer operation and system status

 4. Crystal Oscillator Timing: Uses 32kHz crystal oscillator
    for accurate and stable timing operations

 5. Periodic Task Execution: Implements periodic task execution
    with configurable timing intervals for system operations

Section: Functionality
======================

 The application performs the following operations:
 - Initializes STIMER with 32kHz crystal oscillator configuration
 - Implements STIMER interrupt service routine for periodic tasks
 - Provides LED visual feedback for timer operation status
 - Implements precise timing control with configurable intervals
 - Demonstrates proper interrupt handling and task scheduling
 - Monitors system timing and provides visual status updates

Section: Usage
==============

 1. Compile and download the application to target device
 2. Observe LED behavior for timer operation feedback
 3. Monitor ITM output for timing status and configuration
 4. Verify STIMER interrupt operation and timing accuracy

Section: Configuration
======================

 - WAKE_INTERVAL_IN_MS: Configurable wake interval (default: 1000ms)
 - XT_PERIOD: Crystal oscillator period (32kHz)
 - WAKE_INTERVAL: Calculated timer interval value
 - AM_IRQ_PRIORITY_DEFAULT: Configurable interrupt priority

 Printing takes place over the ITM at 1MHz.



******************************************************************************


