Name:
=====
 adc_vbatt


Description:
============
 Example of ADC sampling of VBATT voltage divider, BATT load, and temp.



Purpose:
========
This example demonstrates ADC (Analog-to-Digital
 Converter) functionality for battery voltage monitoring and temperature
 sensing. The application showcases ADC configuration for VBATT voltage
 divider measurement, temperature sensor reading, and battery load control.
 The example implements voltage monitoring with configurable
 battery load switching, temperature compensation, and real-time voltage
 measurement for battery-powered applications requiring accurate power
 management and thermal monitoring.

Section: Key Features
=====================

 1. VBATT Voltage Monitoring: Implements battery voltage
    measurement using voltage divider with precise ADC sampling

 2. Temperature Sensing: Provides temperature sensor reading
    for thermal monitoring and compensation

 3. Battery Load Control: Implements configurable battery
    load switching for power consumption testing and monitoring

 4. Real Time Monitoring: Provides continuous voltage and
    temperature monitoring with periodic status updates

 5. Power Management: Demonstrates power-efficient operation
    with deep sleep modes and periodic wake-up for measurements

Section: Functionality
======================

 The application performs the following operations:
 - Initializes ADC with proper configuration for voltage and temperature sensing
 - Implements interrupt-driven ADC sampling for VBATT and temperature
 - Provides battery load control with button-triggered switching
 - Implements voltage divider measurement with calibration
 - Demonstrates temperature sensor reading and compensation
 - Monitors power consumption and provides real-time status updates

Section: Usage
==============

 1. Connect battery voltage divider and temperature sensor
 2. Compile and download the application to target device
 3. Monitor SWO output for voltage and temperature measurements
 4. Press button 0 to toggle battery load for power consumption testing

Section: Configuration
======================

 - SLOT_BATT: ADC slot for battery voltage measurement (default: 5)
 - SLOT_TEMP: ADC slot for temperature sensor (default: 7)
 - ISR_SAMPLE_COUNT: Configurable sampling frequency
 - AM_DEBUG_PRINTF: Enables detailed debug output via SWO

Additional Information:
=======================
 The expected VBATT voltage as measured for the Apollo4 EVB is ~0.65V.
 In general the measured voltage should be VDD / 3.

 Printing takes place over the SWO at 1MHz.


******************************************************************************


