Name:
=====
 rtc_print


Description:
============
 Example using the internal RTC.



Purpose:
========
This example demonstrates Real-Time Clock (RTC)
 functionality for timekeeping and deep sleep wake-up operations. The
 application showcases RTC configuration, time management, and periodic
 wake-up from deep sleep modes. The example implements sophisticated
 time tracking with date/time formatting, alarm functionality, and
 power management for battery-powered applications requiring accurate
 timekeeping during extended sleep periods.

Section: Key Features
=====================

 1. RTC Configuration: Demonstrates RTC setup and
    time management for accurate timekeeping operations

 2. Deep Sleep Wake Up: Implements periodic wake-up from
    deep sleep modes for power-efficient timekeeping

 3. Time Formatting: Provides date/time formatting
    with weekday and month name support for human-readable output

 4. Alarm Functionality: Implements RTC alarm capabilities for
    scheduled wake-up and event triggering

 5. Power Management: Demonstrates power-efficient operation with
    deep sleep modes and periodic wake-up for time updates

Section: Functionality
======================

 The application performs the following operations:
 - Initializes RTC with proper configuration and time settings
 - Implements periodic timer interrupt for deep sleep wake-up
 - Provides date/time formatting and display
 - Implements RTC alarm functionality for scheduled events
 - Demonstrates power management with deep sleep modes
 - Monitors sleep states and provides GPIO status output

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor SWO output for current time and date information
 3. Observe periodic wake-up from deep sleep modes
 4. Verify RTC timekeeping accuracy and alarm functionality

Section: Configuration
======================

 - PNTNUMSECS: Print interval in seconds (default: 5 seconds)
 - SWO_PRINTF: Enable SWO output for time display
 - GPIO_SLEEPSTATE: Enable GPIO output for sleep state monitoring
 - PNT_SLEEPSTATE: Enable sleep state printing



******************************************************************************


