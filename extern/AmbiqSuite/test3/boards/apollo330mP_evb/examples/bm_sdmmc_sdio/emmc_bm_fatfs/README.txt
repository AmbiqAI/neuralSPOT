Name:
=====
 emmc_bm_fatfs


Description:
============
 eMMC bare-metal FatFs example.




Purpose:
========
This example demonstrates eMMC (embedded MultiMediaCard)
 bare-metal FatFs file system functionality. The application showcases
 file system operations with eMMC bare-metal HAL, power management, and
 deep sleep optimization.

Section: Key Features
=====================

 1. eMMC Bare Metal HAL: Implements eMMC bare-metal HAL
    for direct hardware control and optimal performance

 2. FatFs File System: Provides file system
    functionality with read/write operations and file management

 3. Power Management: Implements deep sleep optimization with
    configurable wake intervals for power-efficient operation

 4. Memory Optimization: Uses aligned memory buffers for efficient
    data transfer and file system operations

 5. Debug Support: Optional debug printing with ITM/SWO interface
    for development and troubleshooting

Section: Functionality
======================

 The application performs the following operations:
 - Initializes eMMC bare-metal HAL and FatFs file system
 - Implements file read/write operations with data verification
 - Provides power management with deep sleep and wake cycles
 - Supports configurable wake intervals for testing
 - Manages file system operations
 - Implements storage management capabilities

Section: Usage
==============

 1. Compile and download the application to target device
 2. Optionally enable debug printing with AM_DEBUG_PRINTF
 3. Monitor ITM/SWO output for file system operations
 4. Observe power management and deep sleep cycles
 5. Test file system read/write operations and verification

Section: Configuration
======================

 - AM_DEBUG_PRINTF: Enable debug printing (optional)
 - ITM/SWO: Output for debug messages (1MHz)
 - EMMC_DEEP_SLEEP_TEST: Enable deep sleep testing (optional)
 - Wake Intervals: Configurable wake intervals (1s, 2s, 5s)

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.

 AM_DEBUG_PRINTF

 When defined, debug messages will be sent over ITM/SWO at 1MHz.

 Note that when these macros are defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


