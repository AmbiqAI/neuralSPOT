Name:
=====
 while


Description:
============
 Example to emulate a polling loop.




Purpose:
========
This example demonstrates power analysis functionality
 for tight loop execution on the Apollo5 MCU. The application showcases
 power measurement techniques with memory optimization, assembly
 loop implementation, and baseline power consumption analysis.

Section: Key Features
=====================

 1. Tight Loop Execution: Demonstrates power consumption during
    continuous while loop execution for power analysis

 2. Power Measurement: Provides baseline for measuring current draw
    during active processing without peripheral overhead

 3. Memory Optimization: Configures memory for performance and low
    power operation during loop execution

 4. Assembly Optimization: Uses 32-byte aligned assembly loop for
    consistent and predictable execution timing

 5. UART Communication: Optional UART output for debugging and
    monitoring loop execution status

Section: Functionality
======================

 The application performs the following operations:
 - Initializes memory for performance and low power operation
 - Executes tight while loop for power consumption analysis
 - Provides baseline power measurement without peripheral overhead
 - Implements 32-byte aligned assembly loop for consistent timing
 - Supports optional UART output for debugging and monitoring
 - Enables power analysis and optimization

Section: Usage
==============

 1. Compile and download the application to target device
 2. Measure current draw during tight loop execution
 3. Monitor UART output for loop execution status (optional)
 4. Analyze power consumption baseline for optimization
 5. Compare with other power consumption scenarios

Section: Configuration
======================

 - PRINT_UART: Enable UART output for debugging (optional)
 - ALL_RETAIN: Memory retention configuration
 - Assembly Alignment: 32-byte aligned loop implementation
 - Power Measurement: Baseline current draw analysis


******************************************************************************


