Name:
=====
 deepsleep_wake


Description:
============
 Example that goes to deepsleep and wakes from either the RTC or GPIO.




Purpose:
========
This example demonstrates deep sleep and wake-up
 functionality for power optimization. The application
 showcases power management with RTC and GPIO wake-up sources,
 configurable memory retention, and power optimization.

Section: Key Features
=====================

 1. Deep Sleep Management: Implements deep sleep
    functionality with configurable memory retention options

 2. Multiple Wake Sources: Supports RTC and GPIO wake-up sources
    for flexible power management scenarios

 3. Power Optimization: Provides low power operation with
    LFRC clock source for minimal power consumption

 4. LED Status Indication: Uses LED toggling for wake-up event
    indication and power management monitoring

 5. Configurable Memory Retention: Supports different memory
    retention configurations for power optimization

Section: Functionality
======================

 The application performs the following operations:
 - Initializes deep sleep with configurable memory retention
 - Implements RTC wake-up every second with LED indication
 - Provides GPIO wake-up on button press or pin toggle
 - Manages power optimization with LFRC clock source
 - Supports power management features
 - Implements sleep and wake-up functionality

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor UART output at 115,200 BAUD for status messages
 3. Observe LED toggling for RTC and GPIO wake-up events
 4. Test button press or GPIO toggle for wake-up functionality
 5. Measure power consumption during deep sleep operation

Section: Configuration
======================

 - ALL_RETAIN: Memory retention configuration (0 for min, 1 for all)
 - WAKEUP_GPIO_PIN: GPIO pin for wake-up indication
 - INTERRUPT_GPIO: GPIO pin for wake-up source
 - LFRC Clock: Low-frequency RC clock for minimal power consumption

 The RTC Interrupt causes LED1 to toggle every second if LEDs are present
 on the board. Else it toggles a custom pin every second.

 This example uses LFRC as the RTC clock source for lowest power. For better
 accuracy, the XTAL could be used at a slight power increase.

 The GPIO interrupt can also wakeup the device and causes LED1 to toggle
 every time the button0 is pressed or the designated gpio is toggled from Lo2Hi.
 If Leds aren't found on the board it toggles a custom pin every second

 The ALL_RETAIN == 0 case, we requires a custom system-config.yaml different
 from the default system-config.yaml. The reason that is necessary is because
 the TCM Memory Configuration for ALL_RETAIN == 0 is AM_HAL_PWRCTRL_ITCM32K_DTCM128K
 and the start and end addresses of stack, heap and TCM falls within that addresses
 range.

 The example begins by printing out a banner annoucement message through
 the UART, which is then completely disabled for the remainder of execution.

 Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
 Please note that text end-of-line is a newline (LF) character only.
 Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


