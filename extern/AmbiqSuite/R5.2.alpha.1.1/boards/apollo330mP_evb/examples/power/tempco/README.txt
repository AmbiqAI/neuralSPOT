Name:
=====
 tempco


Description:
============
 A brief demonstration of ADC temperature measurements and temperature

 compensation.



Purpose:
========
This example demonstrates temperature coefficient
 compensation functionality for temperature-dependent operations.
 The application showcases ADC temperature measurement, threshold
 monitoring, and temperature compensation with timer-based aging.

Section: Key Features
=====================

 1. Temperature Measurement: Initializes ADC for temperature sensing
    with configurable sampling intervals and thresholds

 2. Temperature Compensation: Implements temperature coefficient
    compensation for accurate temperature-dependent operations

 3. Threshold Monitoring: Checks temperature against configurable
    thresholds and updates compensation when out of range

 4. Timer Based Aging: Uses timer to age temperature values and
    avoid unnecessary measurements when temperature is stable

 5. Deep Sleep Support: Configurable temperature sensing suspension
    during deep sleep for power optimization

Section: Functionality
======================

 The application performs the following operations:
 - Initializes ADC temperature measurement and timer
 - Monitors temperature against configurable thresholds
 - Updates temperature compensation when out of range
 - Implements timer-based temperature value aging
 - Supports deep sleep temperature sensing suspension
 - Provides temperature monitoring and compensation

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM/SWO output for temperature measurements
 3. Observe temperature threshold monitoring and compensation
 4. Test deep sleep temperature sensing suspension
 5. Analyze temperature coefficient compensation accuracy

Section: Configuration
======================

 - ADC_TEMPERATURE_SLOT: ADC slot for temperature measurement   (default: 7)
 - TEMPERATURE_STALE_TIMER_INST: Timer instance for aging       (default: 13)
 - MONITOR_INTERVAL: Temperature monitoring interval            (default: 10)
 - TEMP_NUMSAMPLES: Number of temperature samples for averaging (default: 5)
 - ITM/SWO: Output for debug messages                           (default: 1MHz)
 
  1) Initialize ADC Temperature Measurement and Timer.
  2) Check for temperature, if current temperature is out of threshold,
     then call am_hal_pwrctrl_temp_update.
     If you want to suspend temperature measurement during deepsleep, set
     NO_TEMPSENSE_IN_DEEPSLEEP to true. After waking up, update the temperature
     by calling am_hal_pwrctrl_temp_update, even if the current temperature
     remains within the threshold returned from the last call to
     am_hal_pwrctrl_temp_update. 
  3) Start a timer to age current temperature value. If current temperature is not stale,
     do not measure the temperature again.
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1MHz.
 



******************************************************************************


