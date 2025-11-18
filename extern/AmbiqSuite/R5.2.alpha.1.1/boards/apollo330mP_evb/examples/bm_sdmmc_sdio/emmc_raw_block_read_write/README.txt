Name:
=====
 emmc_raw_block_read_write


Description:
============
 emmc raw block read and write example.



Purpose:
========
This example demonstrates eMMC (embedded MultiMediaCard)
 raw block read and write functionality using blocking PIO and DMA APIs.
 The application showcases eMMC operations with power management,
 memory testing, and performance optimization.

Section: Key Features
=====================

 1. eMMC Raw Block Operations: Implements eMMC
    raw block read and write functionality using blocking PIO and DMA APIs

 2. Power Management: Provides power down and power up functionality
    for SDIO operations with deep sleep optimization

 3. Memory Testing: Implements SSRAM and TCM testing for reliable
    memory operations and data integrity

 4. DDR50 Mode Support: Optional DDR50 mode configuration and
    testing for high-speed eMMC operations

 5. Performance Optimization: Supports ADMA mode testing for
    efficient data transfer and optimal performance

Section: Functionality
======================

 The application performs the following operations:
 - Initializes eMMC with power management and low power operation
 - Implements blocking PIO and DMA read/write operations
 - Provides SSRAM and TCM memory testing capabilities
 - Supports DDR50 mode configuration and testing
 - Manages power down and power up cycles for testing
 - Implements eMMC data transfer operations

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for eMMC operations and status
 3. Observe power management and memory testing results
 4. Test DDR50 mode configuration (optional)
 5. Verify eMMC data transfer reliability and performance

Section: Configuration
======================

 - TEST_POWER_SAVING: Enable power down/up testing (default)
 - TEST_SSRAM: Enable SSRAM testing (default)
 - TEST_TCM: Enable TCM testing (default)
 - EMMC_DDR50_TEST: Enable DDR50 mode testing (optional)
 - ITM/SWO: Output for debug messages (1MHz)
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1MHz.
 
 If ITM is not shut down, the device will never achieve deep sleep, only
 normal sleep, due to the ITM (and thus the HFRC) being enabled.
 
 The following macros can be used in this example
 
 Defined by default:
   #define TEST_POWER_SAVING
           Enables to power down and back up of the SDIO
   #define TEST_SSRAM
           Enables SSRAM testing
   #define TEST_TCM
           Enables TCM testing
 
 Not Defined by default:
   #define EMMC_DDR50_TEST
           Enables to config and test the device in DDR50 mode


******************************************************************************


