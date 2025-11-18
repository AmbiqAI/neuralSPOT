Name:
=====
 ecdsa_sign_verify


Description:
============
 A simple example to demonstrate using runtime crypto APIs.



Purpose:
========
This example demonstrates Elliptic Curve Digital
 Signature Algorithm (ECDSA) operations using the mbedTLS cryptographic
 library. The application showcases ECDSA key generation, message signing,
 and signature verification with SHA256 hashing for secure message
 authentication.

 This example requires 2048 words of stack space.
 The stack size is declared in the ini file and startup file.
 The user should specify this size in the apollo5x-system-config.yaml file

Section: Key Features
=====================

 1. ECDSA Digital Signatures: Implements Elliptic Curve Digital
    Signature Algorithm for secure message authentication

 2. SHA256 Hashing: Uses SHA256 cryptographic hash function for
    message digest generation and integrity verification

 3. Elliptic Curve Cryptography: Leverages elliptic curve
    mathematics for efficient and secure cryptographic operations

 4. Power Management: Implements intelligent crypto power control
    to keep crypto module active only during operations

 5. mbedTLS Integration: Utilizes mbedTLS cryptographic library
    for standards-compliant ECDSA operations

Section: Functionality
======================

 The application performs the following operations:
 - Initializes mbedTLS cryptographic library and memory allocation
 - Generates ECDSA key pair (secp256r1 curve) or loads pre-generated keys
 - Creates SHA256 hash of test message for digital signature
 - Signs the message hash using ECDSA private key
 - Verifies the signature using ECDSA public key
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
 - ECDSA_CURVE: Configurable elliptic curve (default: secp256r1)

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1MHz.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


