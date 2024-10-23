# Apollo510 EVB Usage Notes

The current Apollo510 EVB is a prototype of the production version due later this year. Features include:

- Apollo510 RevB Prototype with secure bootloader installed (use BOOTLOADER=sbl when making)
- Onboard J-Link
- Compact Plus J-link debugger port (automatically chooses this port if plugged in)
- PSRAM (APS512XXB) (optionally used by autodeploy)
- Octal Flash (not used by neuralSPOT)
- 8GB eMMC (not used by neuralSPOT)
- 2 buttons and several LEDs
- A Mikrobus Header
- Numerous GPIOS

### What Works

- Making in both Armclang and GCC environments
- USB, RPC, and WebUSB
- GDB, SWO, Flash - the usual stuff

### What Doesn't Work (yet)

- PDM not yet tested (coming soon)
- Buttons not yet tested (coming soon)

## Toolchain

- Segger 8.10b - this is the earliest version with good AP5 support. GDB, SWO, and Flashing have been tested
- GCC (10 or 13) or Armclang

## Developer Notes

- We're using un-released AmbiqSuite, please keep confidential. Official AmbiqSuite SDK for the EVB expected this quarter.
- Segger identifies the AP5 as AP510NFA-CBR (in case you need to fire up the GUIs or change launch.json for GDBs - the makefiles already set the right device)
- Use the following flags for make (pro-tip: set these in local_overrides.mk to persist them):
  - AS_VERSION:=ambiqsuite_b36aab438d0f
  - PLATFORM := apollo510_evb

## Apollo5B EB

NeuralSPOT supports Apollo5a and Apollo5b EB (engineering boards), which have different peripherals and PSRAM configurations. Switching between platforms is a matter of setting the following `make` options:

- AS_VERSION:=Apollo510_SDK3_2024_09_14 `# SDK2 is also supported`
- PLATFORM:=apollo5b_eb_revb `# apollo5a_eb is also supported`
- BOOTLOADER:=sbl `# sbl for secure bootloader parts, nbl for non-sbl parts`