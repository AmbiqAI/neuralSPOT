# Application Notes

### 11-04-22 - CM
#### Printing and low power
A note about printf and low power: printing over ITM impacts low power in two
ways: 
1) enabling ITM prevents SoC from entering deep sleep, and 
2) ITM initialization requires crypto to be enabled. 

While ITM printing isn't something a deployed application would enable, NS does the
following to mitigate power usage during ITM:

1) ns_power_config() lets you set bNeedITM
2) ns_itm_printf_enable() will temporarily enable crypto if needed
3) ns_lp_printf() enables TPIU and ITM when needed
4) ns_deep_sleep() disables crypto, TPIU and ITM if enabled to allow full deep sleep

#### TempCo (Temperature Compensation)
TempCo support has been added to ns_power, but *has not been tested*.

This feature requires trims to be programmed into MRAM to be at least version 6.

To enable, set .bEnableTempco via power_config() struct

#### ns_core
This is a new neuralspot library (which isn't actually a library) that collects code common across all NS, such as:
- Overall NS version
- startup_gcc
- ns_state (global state across all components)

>*NOTE* As opposed to other libraries, ns_core will not produce a static library, but it will create object files that
> will be linked into NS binaries.