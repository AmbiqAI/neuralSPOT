Name:
=====
 rng_puf_test


Description:
============
 A simple example to demonstrate use of the PUF based RNG.



Purpose:
========
This example demonstrates hardware-based random number generation
 using a Physical Unclonable Function (PUF) with an RNG as the entropy source.
 This is mapped to the One-Time Programmable (OTP) memory. The application
 showcases secure random number generation leveraging hardware entropy sources
 for unpredictable random numbers suitable for cryptographic
 applications.

Section: Key Features
=====================

 1. PUF Based RNG: Utilizes Physical Unclonable Function based RNG
    where the outputlocated in One-Time Programmable memory for
    hardware-based random number generation

 2. Power Management: Implements intelligent power control to enable
    OTP only during random number generation operations

 3. Secure Random Numbers: Provides cryptographically secure
    random numbers suitable for security applications

Section: Functionality
======================

 The application performs the following operations:
 - Initializes PUF-based hardware entropy source for random number generation
 - Reads entropy from PUF-RNG register
 - Generates cryptographically secure random numbers using hardware entropy
 - Implements power management to enable OTP only during operations
 - Transitions to sleep mode after random number generation
 - Provides debug output for hardware entropy operations

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for hardware random number generation results
 3. Verify successful entropy collection and RNG operation

Section: Configuration
======================

 - NUM_RANDOM_BYTES: Number of random bytes to generate (default: 128)
 - AM_DEBUG_PRINTF: Enables debug output via ITM/SWO

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1MHz.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


