Name:
=====
 tinyusb_cdc


Description:
============
 tinyusb cdc-acm example.



Purpose:
========
This example demonstrates USB CDC-ACM (Communication
 Device Class Abstract Control Model) functionality using the TinyUSB stack.
 The application showcases USB device implementation with virtual
 serial port communication, echo functionality, and LED status indication.

Section: Key Features
=====================

 1. USB CDC ACM: Implements USB Communication Device Class
    Abstract Control Model for serial communication

 2. Echo Functionality: Echoes back input from terminal tools
    for testing and demonstration purposes

 3. TinyUSB Stack: Uses TinyUSB library for USB device
    implementation and management

 4. Serial Communication: Provides virtual serial port functionality
    for host computer communication

 5. LED Status Indication: Uses LED blinking patterns to indicate
    USB device state (mounted, suspended, etc.)

Section: Functionality
======================

 The application performs the following operations:
 - Initializes TinyUSB stack for CDC-ACM device functionality
 - Implements virtual serial port communication with host
 - Provides echo functionality for data testing and validation
 - Manages USB device state and status indication
 - Implements LED blinking patterns for device state indication
 - Supports USB suspend/resume and mount/unmount operations

Section: Usage
==============

 1. Connect USB device to host computer
 2. Compile and download the application to target device
 3. Observe LED blinking patterns for USB device state
 4. Use terminal application to communicate with virtual serial port
 5. Test echo functionality by sending data to device

Section: Configuration
======================

 - APP_BUF_SIZE: Configurable application buffer size
 - APP_BUF_COUNT: Number of application buffers
 - LED Patterns: Blinking patterns for device state indication
 - USB Descriptors: Device configuration and interface setup


******************************************************************************


