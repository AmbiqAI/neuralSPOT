Name:
=====
 ble_freertos_throughput


Description:
============
 PacketCraft BLE - Ambiq Micro Throughput Example.


Purpose:
========
This example demonstrates BLE (Bluetooth Low Energy)
 data throughput testing functionality for performance evaluation and
 optimization. The application showcases BLE throughput testing
 with Android test application integration, configurable parameters, and
 multiple test modes.

Section: Key Features
=====================

 1. BLE Data Throughput Testing: Provides BLE
    data throughput testing capabilities for performance evaluation

 2. Android Test Application: Integrates with Android test
    application for device discovery and connection management

 3. Configurable Parameters: Supports MTU, PHY, and bonding
    configuration for flexible testing scenarios

 4. Multiple Test Modes: Supports Downlink, Uplink, and
    Bidirectional testing modes for performance analysis

 5. Dynamic Speed Modes: Implements High Speed and Slow Speed
    modes with configurable connection intervals and latency settings

Section: Functionality
======================

 The application performs the following operations:
 - Initializes BLE throughput testing with configurable parameters
 - Implements Android test application integration
 - Provides MTU, PHY, and bonding configuration options
 - Supports Downlink, Uplink, and Bidirectional testing modes
 - Manages High Speed and Slow Speed mode switching
 - Implements BLE performance analysis

Section: Usage
==============

 1. Install 'Ambiq BLE Test.apk' application on Android phone
 2. Compile and download the application to target device
 3. Scan for 'ambiq_BLT_tester' device using Android app
 4. Configure MTU/PHY/Bonding parameters from app menu
 5. Perform Downlink/Uplink/Bidirectional throughput tests

Section: Configuration
======================

 - AM_DEBUG_PRINTF: Enable debug printing (optional)
 - WSF_TRACE_ENABLED: Enable WSF trace functionality (optional)
 - ITM/SWO: Output for debug messages (1MHz)
 - Connection Intervals: 15ms (High Speed) or 125ms (Slow Speed)
 - Latency Settings: 0 (High Speed) or 2 (Slow Speed)

 AM_DEBUG_PRINTF
 WSF_TRACE_ENABLED=1

 If enabled, debug messages will be sent over ITM at 1MHz.


******************************************************************************


