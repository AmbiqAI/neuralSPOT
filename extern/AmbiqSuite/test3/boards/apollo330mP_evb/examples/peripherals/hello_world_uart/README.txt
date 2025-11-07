Name:
=====
 hello_world_uart


Description:
============
 A simple "Hello World" example using the UART peripheral.



Purpose:
========
This example demonstrates basic UART driver functionality for
 simple serial communication. The application showcases fundamental UART
 transmission capabilities, providing a foundation for more complex UART
 applications. The example implements basic "Hello World" functionality
 with device information output.

Section: Key Features
=====================

 1. Simple UART Communication: Demonstrates basic UART driver
    functionality for serial communication

 2. Device Information Output: Provides device information and
    status messages via UART interface

 3. Configurable UART Interface: Supports flexible UART configuration

 4. Interrupt Driven Operation: Implements interrupt-based data
    handling for reliable transmission

 5. Non Blocking Operation: Supports non-blocking UART operation

Section: Functionality
======================

 The application performs the following operations:
 - Initializes UART with standard configuration
 - Implements basic "Hello World" message transmission
 - Provides device information and status output
 - Implements interrupt-driven and non-blocking data handling

Section: Usage
==============

 1. Compile and download the application to target device
 2. Connect UART pins to PC using UART/USB cable (1.8V logic)
 3. Use terminal application (Tera Term, PuTTY) to view output
 4. Observe "Hello World" message and device information
 5. Monitor UART status and transmission completion

Section: Configuration
======================

 - UART_BAUDRATE: Configurable UART communication rate (default: 115200)
 - USE_NONBLOCKING: Enable non-blocking UART operation
 - ENABLE_DEBUGGER: Enable debugger support

 To see the output of this program, run a terminal application such as
 Tera Term or PuTTY, and configure the console for UART.

 The example sleeps after it is done printing.


******************************************************************************


