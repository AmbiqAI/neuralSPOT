Name:
=====
 hello_world


Description:
============
 A simple "Hello World" example.



Purpose:
========
This example demonstrates fundamental device initialization and
 configuration for Apollo microcontrollers. The application showcases basic
 power management, peripheral setup, and debug interface configuration.
 The example prints device information including device ID,
 reset status, and configuration details over SWO at 1MHz for real-time
 monitoring and debugging. After completing the initialization sequence,
 the device transitions to sleep mode for power efficiency.

Section: Key Features
=====================

 1. Basic Device Initialization: Demonstrates fundamental device
    setup including power management and peripheral configuration

 2. SWO Communication: Uses SWO interface for debug output at 1MHz
    for real-time monitoring and debugging

 3. Device Information: Displays device information
    including ID, reset status, and configuration details

 4. Power Management: Implements low power initialization and
    demonstrates power management features

 5. Debug Interface: Provides debugger support with SWO configuration
    for development and testing environments

Section: Functionality
======================

 The application performs the following operations:
 - Initializes low power configuration and board setup
 - Configures OTP (One-Time Programmable) memory access
 - Retrieves and displays device information
 - Reads and reports device ID and reset status
 - Configures SWO debug interface for real-time output
 - Prints detailed device configuration and status information
 - Transitions to sleep mode for power efficiency

Section: Usage
==============

 1. Compile and download the application to target device
 2. Configure SWO viewer with 1MHz rate and SWOClock = 1000
 3. Monitor SWO output for device information and status
 4. Verify successful device initialization and configuration

Section: Configuration
======================

 - ENABLE_DEBUGGER: Enables debugger support and SWO interface
 - AM_DEBUG_PRINTF: Enables detailed debug output via SWO
 - SWO_BAUD_RATE: Configurable SWO communication rate (default: 1MHz)


******************************************************************************


