Name:
=====
 ble_freertos_fit


Description:
============
 PacketCraft BLE - Fit Application Example.


Purpose:
========
This example demonstrates BLE (Bluetooth Low Energy)
 heart rate sensor functionality within the FreeRTOS framework. The application
 showcases health monitoring capabilities with power optimization,
 real-time heart rate data transmission, and fitness tracking features.

Section: Key Features
=====================

 1. BLE Heart Rate Sensor: Implements a BLE heart rate sensor
    within the FreeRTOS framework for health monitoring applications

 2. FreeRTOS Integration: Utilizes FreeRTOS real-time operating system
    for reliable task scheduling and power management

 3. Power Optimization: Minimizes power usage with optional debug
    printing capabilities for development and testing

 4. Health Monitoring: Provides heart rate monitoring functionality
    suitable for fitness and medical applications

 5. Low Power Design: Optimized for deep sleep operation with
    intelligent power management for battery-powered devices

Section: Functionality
======================

 The application performs the following operations:
 - Initializes BLE heart rate sensor within FreeRTOS framework
 - Implements heart rate monitoring and data transmission
 - Provides power optimization and deep sleep operation
 - Supports optional debug printing for development
 - Manages health monitoring functionality
 - Implements fitness tracking capabilities

Section: Usage
==============

 1. Compile and download the application to target device
 2. Optionally enable debug printing with AM_DEBUG_PRINTF and WSF_TRACE_ENABLED=1
 3. Monitor ITM/SWO output for heart rate operations and status
 4. Test heart rate monitoring and data transmission
 5. Observe power optimization and deep sleep operation

Section: Configuration
======================

 - AM_DEBUG_PRINTF: Enable debug printing (optional)
 - WSF_TRACE_ENABLED: Enable WSF trace functionality (optional)
 - ITM/SWO: Output for debug messages (1MHz)
 - Power Management: Deep sleep optimization for low power operation

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.

 AM_DEBUG_PRINTF
 WSF_TRACE_ENABLED=1

 When defined, debug messages will be sent over ITM/SWO at 1MHz.

 Note that when these macros are defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


