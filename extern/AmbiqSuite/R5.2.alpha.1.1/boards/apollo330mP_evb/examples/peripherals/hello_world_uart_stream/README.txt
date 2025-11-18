Name:
=====
 hello_world_uart_stream


Description:
============
 A simple "Hello World" example using the UART Stream peripheral.



Purpose:
========
This example demonstrates basic UART streaming driver functionality
 for simple serial communication. The application showcases fundamental
 UART transmission capabilities using the streaming driver, providing
 a foundation for more complex UART applications. The example implements
 basic "Hello World" functionality with device information output.

Section: Key Features
=====================

 1. Simple UART Communication: Demonstrates basic UART streaming
    driver functionality for serial communication

 2. Device Information Output: Provides device information and
    status messages via UART interface

 3. Configurable UART Interface: Supports both VCOM (debugger
    interface) and external UART (FTDI) configurations

 4. Interrupt Driven Operation: Implements interrupt-based data
    handling for reliable transmission

 5. Callback Based Transmission: Implements callback-based
    transmission for efficient data handling

Section: Functionality
======================

 The application performs the following operations:
 - Initializes UART with streaming driver configuration
 - Implements basic "Hello World" message transmission
 - Provides device information and status output
 - Implements interrupt-driven data handling
 - Demonstrates callback-based transmission capabilities
 - Supports both VCOM and external UART configurations

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
 - USE_VCOMM: Enable VCOM interface via debugger
 - TX_BUFFER_SIZE: Configurable transmit buffer size
 - RX_BUFFER_SIZE: Configurable receive buffer size

 to use VCOM (via debugger interface) that is use AM_BSP_UART_PRINT_INST:
 #define USE_VCOMM

 If the user uses an alternate pin with an external uart (FTDI recommended),
 Don't define USE_VCOMM and define the UART_ID that is desired ensuring the RX and TX
 pins are accessible.
 The user will have to find (or set) the correct pins in the pin mapping table.
 Uart tx and rx pins are used and defined in the table.

 To see the output of this program, run a terminal application such as
 Tera Term or PuTTY, and configure the console for UART

 The example sleeps after it is done printing.


******************************************************************************


