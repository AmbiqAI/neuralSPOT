Name:
=====
 tinyusb_cdc_msc


Description:
============
 tinyusb cdc-acm and mass storage USB example.



Purpose:
========
This example demonstrates composite USB device
 functionality combining CDC-ACM (Communication Device Class Abstract Control
 Model) and mass storage capabilities. The application showcases advanced
 USB device implementation with dual functionality - virtual serial port
 communication and mass storage device emulation.

Section: Key Features
=====================

 1. Composite USB Device: Implements composite USB device with
    CDC-ACM and mass storage functionality

 2. CDC ACM Communication: Provides virtual serial port
    communication with echo functionality

 3. Mass Storage Device: Implements mass storage device
    functionality for disk emulation

 4. Cross Platform Support: Compatible with Windows, Linux,
    and other operating systems

 5. LED Status Indication: Uses LED blinking patterns to indicate
    USB device state (mounted, suspended, etc.)

Section: Functionality
======================

 The application performs the following operations:
 - Initializes composite USB device with CDC-ACM and mass storage
 - Implements virtual serial port communication with echo
 - Provides mass storage device emulation for disk functionality
 - Manages USB device state and status indication
 - Implements LED blinking patterns for device state indication
 - Supports cross-platform USB device functionality

Section: Usage
==============

 1. Connect USB device to host computer
 2. Compile and download the application to target device
 3. Observe LED blinking patterns for USB device state
 4. Use terminal application to communicate with virtual serial port
 5. Access mass storage device as disk drive

Section: Configuration
======================

 - APP_BUF_SIZE: Configurable application buffer size
 - APP_BUF_COUNT: Number of application buffers
 - LED Patterns: Blinking patterns for device state indication
 - USB Descriptors: Device configuration and interface setup


******************************************************************************


