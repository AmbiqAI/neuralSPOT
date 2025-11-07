Name:
=====
 timer_pwm_output


Description:
============
 TIMER PWM example.



Purpose:
========
This example demonstrates Timer PWM (Pulse Width
 Modulation) functionality for LED brightness control and analog signal
 generation. The application showcases dual-timer architecture with high
 frequency PWM generation and low frequency duty cycle modulation. The
 example implements sine-wave based brightness profiles for smooth LED
 breathing effects, demonstrating timer configuration and
 interrupt-driven PWM control for precise analog signal generation.

Section: Key Features
=====================

 1. Dual Timer Architecture: Implements high-frequency PWM timer
    and low-frequency duty cycle modulation timer for control

 2. PWM Generation: Provides precise pulse width modulation with
    configurable frequency and duty cycle for analog signal generation

 3. LED Brightness Control: Demonstrates smooth LED breathing
    effects using sine-wave based brightness profiles

 4. Interrupt Driven Control: Implements timer interrupt service
    routines for real-time PWM parameter updates

 5. Low Power Operation: Optimizes for low power consumption
    with reduced DTCM size and efficient timer management

Section: Functionality
======================

 The application performs the following operations:
 - Initializes dual timer configuration for PWM and duty cycle control
 - Implements sine-wave brightness profile generation and scaling
 - Configures high-frequency PWM timer for LED control
 - Sets up low-frequency timer for duty cycle modulation
 - Implements interrupt-driven PWM parameter updates
 - Provides real-time LED brightness control and monitoring
 - Optimizes power consumption for battery-powered applications

Section: Usage
==============

 1. Connect LEDs to designated PWM output pins
 2. Compile and download the application to target device
 3. Observe LED breathing effect with smooth brightness transitions
 4. Monitor ITM output for PWM status and configuration information

Section: Configuration
======================

 - PWM_FREQ: Configurable PWM frequency (default: 200 Hz)
 - PWM_CLK: Configurable PWM clock source (default: HFRC_DIV16)
 - LED_CYCLE_TIMER: Timer for duty cycle updates (default: Timer 2)
 - TABLE_ENTRIES: Sine wave table resolution (default: 64 entries)

 NOTE: This example shows a low power operation using reduced DTCM size
 The 8K size is very near the limit with most BSPs.
 The standard stack size had to be slightly reduced for this example's
 RAM use to fit in 8K.(with IAR compiler)

 Printing takes place over the ITM at 1MHz.



******************************************************************************


