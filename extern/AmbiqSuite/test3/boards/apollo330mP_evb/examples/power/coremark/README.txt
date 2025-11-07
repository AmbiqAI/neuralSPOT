Name:
=====
 coremark


Description:
============
 EEMBC COREMARK test.




Purpose:
========
This example demonstrates EEMBC CoreMark® benchmark
 functionality for microcontroller performance evaluation. The application
 showcases benchmarking with list processing, matrix manipulation,
 state machine operations, and CRC calculations.

Section: Key Features
=====================

 1. EEMBC CoreMark Benchmark: Implements official EEMBC CoreMark®
    benchmark for microcontroller performance evaluation

 2. Algorithm Implementation: Provides list processing, matrix
    manipulation, state machine, and CRC algorithms

 3. Self Checking Mechanism: Implements 16-bit CRC verification
    for benchmark correctness validation

 4. Compiler Independent: Ensures operations derive values not
    available at compile time for accurate benchmarking

 5. Performance Measurement: Provides performance
    analysis for system optimization

Section: Functionality
======================

 The application performs the following operations:
 - Executes EEMBC CoreMark® benchmark with 2250 iterations
 - Implements list processing (find and sort) algorithms
 - Provides matrix manipulation and state machine operations
 - Performs CRC calculations for self-checking validation
 - Outputs results via UART at 115,200 BAUD
 - Supports performance analysis

Section: Usage
==============

 1. Compile with optimization flags (-Omax for Arm6)
 2. Download the application to target device
 3. Monitor UART output for benchmark results
 4. Verify CRC calculations for correctness
 5. Analyze performance metrics and optimization

Section: Configuration
======================

 - ITERATIONS: 2250 benchmark iterations (specified in ambiq_core_config.h)
 - UART Output: 115,200 BAUD, 8-bit, no parity
 - Compiler Options: -Omax for optimal performance
 - Linker Options: -Omax --cpu=Cortex-M55 --lto

  The below documentation details how each coremark example i.e. coremark,
  coremark_best_cm_per_mJ, coremark_best_performance and coremark_best_power
  differs based on pre-compile macro defined.

  These macros are used to select different version of coremark as listed
  in ambiq_core_config.h i.e.

  COREMARK_DEFAULT runs ELP in retention, LP mode.
  COREMARK_BEST_PERFORMANCE runs ELP On, HP mode.
  COREMARK_BEST_CM_PER_mJ runs ELP On, LP mode.
  If there are any different configs to be run, they need to be updated under
  the respective conditional macro definitions.There are no restrictions as
  to which combinations can be run under each individual conditional macro.




******************************************************************************


