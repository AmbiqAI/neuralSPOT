Name:
=====
 rsa_sign_verify


Description:
============
 A simple example to demonstrate using runtime crypto APIs.



Purpose:
========
This example demonstrates RSA digital signature
 operations using the mbedTLS cryptographic library. The application
 showcases RSA key generation, message signing, and signature verification
 with SHA256 hashing for secure message authentication.

 This example requires 2048 words of stack space.
 The stack size is declared in the ini file and startup file.
 The user should specify this size in the apollo510-system-config.yaml file

Section: Key Features
=====================

 1. RSA Digital Signatures: Implements RSA digital signature
    generation and verification for secure message authentication

 2. SHA256 Hashing: Uses SHA256 cryptographic hash function for
    message digest generation and integrity verification

 3. Power Management: Implements intelligent crypto power control
    to keep crypto module active only during operations

 4. mbedTLS Integration: Utilizes mbedTLS cryptographic library
    for standards-compliant crypto operations

 5. Memory Optimization: Uses dynamic memory allocation for
    efficient resource management during crypto operations

Section: Functionality
======================

 The application performs the following operations:
 - Initializes mbedTLS cryptographic library and memory allocation
 - Generates RSA key pair (2048-bit) or loads pre-generated keys
 - Creates SHA256 hash of test message for digital signature
 - Signs the message hash using RSA private key
 - Verifies the signature using RSA public key
 - Implements power management to enable crypto only during operations
 - Provides debug output for cryptographic operations

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for cryptographic operation results
 3. Verify successful signature generation and verification

Section: Configuration
======================

 - ENABLE_CRYPTO_ON_OFF: Enables intelligent crypto power management
 - AM_DEBUG_PRINTF: Enables detailed debug output via ITM/SWO
 - RSA_KEY_SIZE: Configurable RSA key size (default: 2048 bits)

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1MHz.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


