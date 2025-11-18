
Name:
=====
 binary_counter


Description:
============
 Example that displays the timer count on the LEDs.



Purpose:
========
This example demonstrates timer-based LED counting functionality
 with power mode switching capabilities. The application showcases timer
 interrupt handling, LED control, and dynamic power mode management. The
 example implements a binary counter using LEDs and demonstrates power
 mode transitions between HIGH and LOW performance states.

Section: Key Features
=====================

 1. Timer Configuration: Demonstrates timer setup and
    interrupt-based counting operations

 2. LED Control: Implements binary counting display using
    onboard LEDs for visual feedback

 3. Power Management: Showcases dynamic switching between
    performance modes for power optimization

 4. Sleep Mode: Utilizes deep sleep for power efficiency
    between timer events

Section: Functionality
======================

 The application performs the following operations:
 - Initializes system timer with 1-second period
 - Configures LED array for binary display output
 - Implements timer interrupt for counter increment
 - Displays binary count on LED array
 - Toggles between power modes at counter reset
 - Utilizes deep sleep between timer events

Section: Usage
==============

 1. Load and run the application
 2. Observe LED binary counting pattern
 3. Monitor power mode transitions via SWO output
 4. Verify deep sleep operation between counts

Section: Configuration
======================

 - Timer configured for 1-second interrupt
 - LED array setup based on board configuration
 - Power modes defined per device capabilities
 - SWO/ITM configured for 1MHz output

Additional Information:
=======================
 Printing takes place over the SWO/ITM at 1MHz.



******************************************************************************


