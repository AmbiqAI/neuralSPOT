Name:
=====
 rng_test


Description:
============
 A simple example to demonstrate use of the mbedtls RNG.



Purpose:
========
This example demonstrates cryptographically secure random number
 generation using the mbedTLS CTR_DRBG (Counter Mode Deterministic Random
 Bit Generator) implementation. The application showcases entropy source
 management, random number generation, and power-optimized crypto operations.

 This example requires 2048 words of stack space.
 The stack size is declared in the ini file and startup file.
 The user should specify this size in the apollo510-system-config.yaml file

Section: Key Features
=====================

 1. Random Number Generation: Produces cryptographically secure
    random numbers using mbedTLS CTR_DRBG implementation

 2. Entropy Source Management: Utilizes hardware entropy sources
    for random number generation

 3. Crypto Power Control: Implements intelligent power management
    to enable crypto only during random number generation

 4. mbedTLS Integration: Uses mbedTLS cryptographic library for
    standards-compliant random number generation

 5. Memory Efficiency: Optimizes memory usage with dynamic allocation
    for crypto operations

Section: Functionality
======================

 The application performs the following operations:
 - Initializes mbedTLS entropy and CTR_DRBG random number generator
 - Seeds the random number generator with hardware entropy sources
 - Generates cryptographically secure random numbers in batches
 - Implements power management to enable crypto only during operations
 - Transitions to sleep mode after random number generation
 - Provides debug output for entropy and RNG operations

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for random number generation results
 3. Verify successful entropy collection and RNG operation

Section: Configuration
======================

 - NUM_RANDOM_BYTES: Number of random bytes to generate (default: 128)
 - AM_DEBUG_PRINTF: Enables detailed debug output via ITM/SWO
 - CTR_DRBG_ENTROPY_LEN: Entropy length for random number generation

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1MHz.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


