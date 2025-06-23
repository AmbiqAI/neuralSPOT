# KWS (Keyword Spotting)
A KWS demo based on MLCommon's KWS.

# Hardware Requirements
One of:
1. Apollo5-family EVB with PDM Clickboard (available with VOSKit)
2. Apollo4-family EVB with VOSKit or analog microphone
3. Apollo3-family EVB with VOSKit

# Getting Started
> HINT: Type `make help` for a list of make options, including how to select target platform

```bash
$> cd .../neuralSPOT
$> make clean
$> make PLATFORM=apollo510_evb -j # makes everything
$> make PLATFORM=apollo510_evb TARGET=kws deploy # flashes kws to the target EVB
$> make view # connects to EVB SWO via SEGGER Jlink
```

# Running the demo
The demo's instructions will print out via SWO.