Name:
=====
 mram_program


Description:
============
 MRAM programming example.




Purpose:
========
This example demonstrates MRAM (Magnetic Random
 Access Memory) programming functionality for embedded memory management.
 The application showcases MRAM operations including programming,
 verification, and INFO region management.

Section: Key Features
=====================

 1. MRAM Programming: Implements MRAM programming
    functionality using HAL MRAM helper functions

 2. Memory Verification: Provides memory verification and integrity
    checking for reliable MRAM operations

 3. INFO Region Management: Supports INFO0-MRAM programming
    and verification for configuration data storage

 4. Low Power Operation: Optimized for low power operation
    with intelligent power management features

 5. Debug Support: Provides debug output via
    ITM/SWO interface for development and troubleshooting

Section: Functionality
======================

 The application performs the following operations:
 - Initializes system for low power operation
 - Programs MRAM at 1MB boundary with verification
 - Implements INFO0-MRAM programming and checking
 - Provides memory integrity verification
 - Supports debug output
 - Implements cache management for optimal performance

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for MRAM operations and status
 3. Observe MRAM programming and verification results
 4. Test INFO0-MRAM programming functionality
 5. Verify memory integrity and programming success

Section: Configuration
======================

 - ARB_MRAM_ADDRESS: MRAM programming address (1MB boundary)
 - ARB_MRAM_SIZE_WORDS: MRAM programming size (512 words)
 - ARB_INFO_OFFSET: INFO0-MRAM offset (0x400)
 - ITM/SWO: Output for debug messages (1MHz)
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1MHz.


******************************************************************************


