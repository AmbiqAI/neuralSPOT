Name:
=====
 freertos_sample


Description:
============
 Example program which demonstrates using FreeRTOS



Purpose:
========
This example demonstrates FreeRTOS real-time
 operating system functionality for embedded applications. The application
 showcases FreeRTOS features including task management, timer
 interrupts, deep sleep operation, and GPIO status monitoring. The example
 provides a foundation for FreeRTOS-based embedded development with
 power management and real-time task scheduling capabilities.

Section: Key Features
=====================

 1. FreeRTOS Task Management: Implements task
    scheduling and management within FreeRTOS framework

 2. GPIO Status Monitoring: Provides GPIO-based status indication
    for task execution and system state monitoring

 3. Deep Sleep Operation: Implements deep sleep functionality
    for power optimization in embedded applications

 4. Timer Interrupt Support: Provides timer interrupt handling
    for real-time system operation

 5. Power Management: Optimized for low power operation with
    intelligent power management features

Section: Functionality
======================

 The application performs the following operations:
 - Initializes FreeRTOS with task management and scheduling
 - Implements GPIO status monitoring for system state indication
 - Provides deep sleep operation for power optimization
 - Manages timer interrupts for real-time operation
 - Supports FreeRTOS functionality demonstration
 - Implements power management and cache optimization

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor GPIO pins for task execution status
 3. Observe deep sleep entry and exit operations
 4. Monitor timer interrupt execution
 5. Test FreeRTOS task scheduling and management

Section: Configuration
======================

 - GPIO Pins: Status monitoring pins (0-4)
 - ITM: Output for debug messages
 - Power Management: Deep sleep optimization
 - Cache Configuration: I-Cache and D-Cache enablement

 The following GPIOs are configured for observation:

   GPIO           Toggles
 --------- ---------------------------------
    0     on each iteration of Hello Task loop
    1     on each iteration of Idle Task
    2     before deepsleep entry
    3     immediately following deepsleep exit
    4     on each Timer ISR entry


******************************************************************************


