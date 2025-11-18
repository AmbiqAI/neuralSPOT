Name:
=====
 mpu_example


Description:
============
 A simple example on how to configure the ARMv8.1 MPU



Purpose:
========
This example demonstrates ARMv8.1 Memory Protection
 Unit (MPU) configuration and management for secure memory access control.
 The application showcases MPU region setup, memory attribute configuration,
 and access permission management for different memory types including
 NVM (Non-Volatile Memory), SSRAM (Static SRAM), and XIPMM (Execute-in-Place
 Memory).

Section: Key Features
=====================

 1. MPU Configuration: Demonstrates ARMv8.1 MPU setup
    and region configuration for memory protection

 2. Memory Region Management: Implements multiple memory regions
    with different access permissions and attributes

 3. Access Control: Provides access permission control
    for read-only, read-write, and execute permissions

 4. Memory Attributes: Configures memory attributes for cache behavior,
    shareability, and execution control

 5. Security Features: Implements memory protection for secure
    embedded applications with proper access control

Section: Functionality
======================

 The application performs the following operations:
 - Initializes ARMv8.1 MPU with region configuration
 - Configures memory regions for NVM, SSRAM, and XIPMM memory types
 - Sets up access permissions and memory attributes for each region
 - Implements execute-never protection for data memory regions
 - Configures cache behavior and shareability attributes
 - Provides memory protection for secure embedded applications
 - Demonstrates proper MPU enablement and validation

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor debug output for MPU configuration status
 3. Verify memory region setup and access permissions
 4. Test memory protection functionality and access control

Section: Configuration
======================

 - MPU_REGIONS: Number of MPU regions to configure (default: 6)
 - NVM_REGIONS: Non-volatile memory region configuration
 - SSRAM_REGIONS: Static SRAM region configuration
 - XIPMM_REGIONS: Execute-in-Place memory region configuration


******************************************************************************


