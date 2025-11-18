Name:
=====
 emmc_scatter_raw_read_write


Description:
============
 eMMC scatter raw block read and write example using ADMA scatter IO APIs.




Purpose:
========
This example demonstrates ADMA-based scatter IO APIs for efficient
          block read and write operations on eMMC devices. It showcases
          synchronous and asynchronous scatter transfers, memory type testing,
          and power-optimized operation for SDIO applications.

Section: Key Features
=====================

 1. ADMA Scatter IO: Efficient block transfers using scatter/gather
 2. Synchronous/Asynchronous: Supports both sync and async operations
 3. Memory Type Testing: SSRAM and other memory types supported
 4. Power Optimization: Low-power initialization and operation
 5. Debug Output: ITM/SWO debug messages at 1MHz

Section: Functionality
======================

 - Initialize board and card host for low power
 - Configure and test SSRAM and other memory types
 - Perform scatter block read/write (sync/async)
 - Validate data integrity after transfers
 - Output debug information and status

Section: Usage
==============

 1. Build and load the example on target hardware
 2. Use defined macros to select test modes
 3. Monitor debug output via ITM/SWO
 4. Observe power and performance characteristics
 5. Validate data integrity after transfers

Section: Configuration
======================

 - Macros for test selection (see source for options)
 - ITM/SWO output for debug messages
 - Power and memory configuration via board init
 - Card host and bus width selection

Additional Information:
=======================
 - Debug output prevents deep sleep due to ITM/HFRC
 - See macro definitions for available test modes


******************************************************************************


