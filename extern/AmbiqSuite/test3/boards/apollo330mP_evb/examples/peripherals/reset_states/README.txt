Name:
=====
 reset_states


Description:
============
 Example of various reset options in Apollo.



Purpose:
========
This example demonstrates reset state management
 and watchdog timer functionality for system monitoring and recovery.
 The application showcases various reset types including watchdog reset,
 power-on reset, external reset, and brown-out reset.

Section: Key Features
=====================

 1. Reset State Detection: Implements reset
    state detection and decoding for various reset types

 2. Watchdog Timer Management: Demonstrates watchdog timer
    configuration with interrupt and reset generation capabilities

 3. Multiple Reset Types: Supports various reset types including
    watchdog, power-on, external, and brown-out reset detection

 4. System Recovery: Implements automatic system recovery and
    restart mechanisms for operation

 5. Reset Sequence Control: Provides controlled reset sequence
    management with configurable reset behavior

Section: Functionality
======================

 The application performs the following operations:
 - Initializes watchdog timer with interrupt and reset capabilities
 - Implements reset state detection and decoding for various reset types
 - Provides controlled reset sequence management and monitoring
 - Demonstrates watchdog interrupt handling and reset generation
 - Implements system recovery and restart mechanisms
 - Monitors reset states and provides detailed status reporting

Section: Usage
==============

 1. Compile and download the application to target device
 2. Monitor ITM output for reset state information and watchdog events
 3. Observe reset sequence: Power cycle -> 5 WDT Interrupts -> WDT Reset
 4. Test external reset by pressing and releasing external reset pin
 5. Verify brown-out reset by lowering and restoring VDD_MCU

Section: Configuration
======================

 - WDT_CLOCK_SOURCE: Watchdog clock source (default: LFRC_DIV64)
 - WDT_INTERRUPT_TIMEOUT: Interrupt timeout value (default: 12 seconds)
 - WDT_RESET_TIMEOUT: Reset timeout value (default: 24 seconds)
 - RESET_SEQUENCE: Configurable reset sequence behavior

 The program will repeat the following sequence on the console after power cycling:
 Power cycle (POA Reset) - 5 WDT Interrupts - WDT Reset - 3 WDT Interrupts -
 POR Reset - 3 WDT Interrupts - repeat "POI Reset - 3 WDT Interrupts"

 Pressing then releasing external reset pin can trigger an external reset.
 Writing 1 to AIRCR.SYSRESETREQ can trigger an AIRCR SYSRESETREQ reset. (Writing
 0x05FA to VECTKEY is needed.)
 Lowering down VDD_MCU then restoring it can trigger a brown-out BOUNREG reset.

 Printing takes place over the ITM at 1MHz.



******************************************************************************


