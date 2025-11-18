
Name:
=====
 adc_measure


Description:
============
 Example of ADC sampling the voltage applied to a particular pin.



Purpose:
========
This example demonstrates ADC (Analog-to-Digital
 Converter) functionality for voltage measurement and sampling operations.
 The application showcases ADC initialization, configuration, and periodic
 sampling with interrupt-driven data acquisition. The example implements
 FIFO-based sampling, voltage reference calibration, and real-time voltage
 measurement display. The application provides accurate voltage readings
 with proper calibration and scaling for precise analog signal processing.

Section: Key Features
=====================

 1. ADC Initialization: Demonstrates ADC setup and
    configuration for voltage measurement operations

 2. Interrupt Driven Sampling: Implements interrupt-based ADC
    sampling with FIFO management for efficient data acquisition

 3. Voltage Measurement: Provides accurate voltage measurement with
    reference calibration and scaling for precise readings

 4. Real Time Monitoring: Displays measured voltage values in
    real-time via SWO interface for continuous monitoring

 5. Calibration Support: Implements voltage reference calibration
    for accurate and reliable measurement results

Section: Functionality
======================

 The application performs the following operations:
 - Initializes ADC with proper configuration and calibration
 - Configures ADC input pin and voltage reference settings
 - Implements interrupt-driven ADC sampling with FIFO management
 - Applies voltage reference calibration to raw ADC values
 - Calculates and displays accurate voltage measurements
 - Provides real-time monitoring via SWO debug interface
 - Implements periodic sampling for continuous voltage monitoring

Section: Usage
==============

 1. Connect voltage source to ADC input pin (default: pin 18)
 2. Compile and download the application to target device
 3. Monitor SWO output for voltage measurement results
 4. Verify accurate voltage readings and calibration

Section: Configuration
======================

 - ADC_INPUT_PIN: Configurable ADC input pin (default: pin 18)
 - ADC_SAMPLE_RATE: Configurable sampling rate and frequency
 - ADC_REFERENCE: Configurable voltage reference settings
 - AM_DEBUG_PRINTF: Enables detailed debug output via SWO

Additional Information:
=======================
 It should be noted that the pin number used for input is designated in the
 example source code via the define ADC_INPUT_PIN.

 Printing takes place over the SWO/ITM at 1MHz.


******************************************************************************


