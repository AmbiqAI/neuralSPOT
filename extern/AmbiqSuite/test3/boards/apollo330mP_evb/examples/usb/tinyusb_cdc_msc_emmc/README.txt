Name:
=====
 tinyusb_cdc_msc_emmc


Description:
============
 TinyUSB CDC-ACM and mass storage USB example



Purpose:
========
This example demonstrates TinyUSB composite device
 functionality with CDC-ACM (Communication Device Class - Abstract Control
 Model) and mass storage capabilities using eMMC (embedded MultiMediaCard).
 The application showcases USB device implementation with dual
 functionality, real-time communication, and mass storage operations.

Section: Key Features
=====================

 1. Composite USB Device: Implements TinyUSB composite device
    with CDC-ACM and mass storage functionality

 2. CDC Communication: Provides USB CDC-ACM communication for
    terminal echo and data transfer capabilities

 3. Mass Storage: Implements mass storage device functionality
    using eMMC for file system operations

 4. eMMC Integration: Supports embedded MultiMediaCard for
    high-performance storage operations

 5. Cross Platform Support: Compatible with Windows, Linux,
    and other operating systems for mass storage functionality

Section: Functionality
======================

 The application performs the following operations:
 - Initializes TinyUSB composite device with CDC-ACM and mass storage
 - Implements USB CDC-ACM communication with terminal echo functionality
 - Provides mass storage device using eMMC for file system operations
 - Supports cross-platform compatibility for storage operations
 - Manages USB device mounting, unmounting, and power management
 - Implements USB composite device functionality

Section: Usage
==============

 1. Compile and download the application to target device
 2. Connect USB device to host computer
 3. Access CDC-ACM COM port for terminal communication
 4. Mount mass storage device for file system operations
 5. Test cross-platform compatibility and storage functionality

Section: Configuration
======================

 - APP_BUF_SIZE: Configurable buffer size for CDC communication
 - APP_BUF_COUNT: Number of application buffers
 - DISK_BLOCK_SIZE: Mass storage block size (512 bytes)
 - DISK_BLOCK_NUM: Number of disk blocks for storage


******************************************************************************


