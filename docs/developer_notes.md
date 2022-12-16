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

### 12-15-2022 - CM
NeuralSPOT is moving to version 1.0.0. As we exit Version Zero, we'll start being more formal about code changes and release schedules:
- API compatibility will be maintained across major releases
- Bug fixes and transparent code updates will be tracked via minor and revision version updates
- Outside of hotfixes and documentation updates, the `main` repo will be updated on a roughly biweekly cadence
- We'll start a persistent `development` branch to replace how we've been using the `main` branch.

Select APIs will be version checked: when calling Init() for that API, a version must be specified, and this version will be checked against a range of supported versions. We'll do our best to maintain compatibility of code updates at least one major version back from the latest released version.