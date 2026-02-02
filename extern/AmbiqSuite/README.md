# AmbiqSuite in neuralSPOT
This directory contains pared-down versions of AmbiqSuite - basically, header files, static libraries, and a handful of source files that are not part of the distributions static libs, or that have to be modified to work on our platform.

Ambiqsuite SDKs are platform-specific:
- R3.xxx is for Apollo3 (apollo3 evb and apollo3 Plus evb)
- R4.xxx is for Apollo4 (Plus and Lite and the Blue variants)
- R5.xxx is for Apollo5 (apollo510_evb, apollo510B_evb, apollo510 eb) *with the following exception*
- R5.1.alpha.xxx is for Apollo330 (and apollo510L, which is a different platform that apollo510 - I don't name 'em). These are not release yet, only in alpha stage. There is no official SDK for ap330 and ap510L, so the alphas will have to do.

SDK's a generally consitently structured, but not 100%. If you see code that converts, for example, "apollo330mP" to "apollo330P" it's because the SDK used different naming conventions in different places (there will be an apollo330bP at some point, so maybe thats why).

# Structure of extern/Ambiqsuite entries
Each Ambiqsuite version will get its own directory. Most of that directory's structure is the same as the original Ambiqsuite's, with the following exceptions.

Released Ambiqsuite SDKs have several pre-compiled static libraries that cover *most* of the SDK, but there are some files that they only compile into examples. When we need one of those files, we have to include the source. For example, the PSRAM driver is not pre-compiled, so we have to include that driver's code (the header is left in its original place in the SDK)

- `/src` is where we put any C and some H files that we need to port over. It has some internal structure that is explained below
- `/lib` is where we copy the pre-compiled SDK libs we care about. It also has some internal structure described below
- `module.mk` that neuralSPOT's makefile will pull in
- `/third_party/cordio/module.mk` is pulled in if the platform supports BLE. We leave the source files in `cordio` intact and in place, but we do some special stuff here
- `/third_party/open-amp/module.mk` for platforms that have our BLE radio - currently only Apollo330/Apollo510L.

## /src
This directory has to support all variants of a platform (with/without USB, etc) and does so with subdirectories. The `module.mk` will pull in the right subdirectories for the platform.
- `usb` contains USB files that will only be pulled in when platform has USB
- `<platform>` (e.g. apollo330P) contains source files specific to that platform.

The rest of the files are there for one of 3 reasons:
- They're not compiled into the static libs. These include devices, utils, and third party libraries such as tinyUSB and freeRTOS.
- They are configuration headers that are per-example in the SDK, but we want to be applied across all of neuralSPOT.
- They had to be modified.

## lib
This contains the SDK's pre-compiled static libs. There are two kinds of these: the HAL (hardware abstraction layer) which applies to the entire platform, and the BSP (board specification package) that applies to a specific EVB or EB.
```
/lib
  /evb
    libam_bsp.a   ## GCC lib
    libam_bsp.lib ## Armclang lib
  libam_hal.a
  libam_hal.lib
```

## The process
Generally:
1. Download the SDK zip file, unzip it somewhere outside neuralspot. The zips usually come from Jayesh for unnoficial releases, and from our content portal for official ones.
2. Run the `ambiqsuite_min_integrate.py` script (see its readme for usage instruction, and an example below). Grab some coffee, it'll take a while to chew through the new SDK
3. Inspect the `_integration_report` directory and look for any merge conflicts you need to resolve. Resolve 'em.
4. The script does its best, but every SDK release has its quirks. See below for some of them, you'll need to resolve.
5. Manual steps: see below

### SDK Updates vs. New Platforms
SDK releases generally fall into 2 categories: an update to an existing SDK for a platform, or an SDK for an entirely new platform. Updates are usually easy to port in, new platforms usually include hardware changes requiring more extensive updates. FWIW, after AP330/510L, there isn't a new platform planned until Atomiq, which is going to need a ton of changes (NPU, HBLRAM, plus maybe second core).

SDK release tend to stick to previous APIs, but sometimes they break that, mostly when upgrading third party packages.

### Manual Steps
1. Modify neuralspot makefiles to include new SDK and, if needed, new platform.
1. Fix MVE include
1. Fix WSF types
1. open-amp and cordio, if needed
1. Try a compile! Fix what's not working.
1. neuralspot changes: add ns-core source for new platform if needed, fix up any HW-related changes (ns-audio, GPIOs, etc), and see if ns_power has to change
1. Delete everything in the SDK that isn't needed

### Carlos' Usual Process
1. Get to the point where I can compile (all the stuff above)
1. Test peripherals one at a time, usually starting with buttons (ns-buttons) and audio (ns-audio and ns-pdm), my 'hello world' is basic_tf_stub, which is a KWS demo.
1. Get USB up and running (rpc_evb_to_pc example)
1. Try autodeploy, first without PMU, then with PMU, then with Joulescope (excersizes GPIOs)
1. Try all of that but with armclang instead of GCC. Get that compiling and working.
1. The rest of the nice-to-haves like the power profiling tools.

I use the SDK's examples to see what changed (coremark example to see how to configure lowest power, pdm_fft to see if PDM configuration changed, etc).

### Details: neuralspot Makefiles
A new platform will likely have a set of new features. neuralSPOT handles features by setting 'feature flags' based on platforms and SDKs (different versions of SDKs may or may not support a feature). Generally:
1. Modify neuralspot_config.mk to add the platform if new
2. Set the BLE and USB features as needed

The SDK may also need platform-specific `#defines`. Look at any of the examples in the SDK (in particular, the example/gcc/Makefile) to see if anything there is platform specific. We need to define those too, and that is done in neuralspot_toolchain.mk.

### Details: SDK Header Changes
We keep most of the SDK headers unchanged, but there are a few conflicts:
1. MVE C/C++ conflict: `am_mcu_apollo.h` doesn't encapsulate a `#include <arm_mve.h>` properly. Just comment those includes out.
1. WSF Type redefinition: `wsf_types.h` defines uint32_t and int32_t in a way that causes a conflict error. Just comment those out.
1. open-amp Assert conflict: open-amp has an `assert.h` which conflicts. In R5.2.alpha.1.1, we changed the name of that file to `metal_assert.h` and changed all the includes in open-amp to that.
1. Some SDK includes have type mismatches that generate compiler warnings. I usually like to clean those up.

### Details: Third Party Recompiles
There are two `third_party` libraries that need to be compiled for neuralSPOT, and are too large to do what we did for FreeRTOS and USB (which is to copy the needed files into `/src`): Cordio (BLE stack) and open-amp (RPC library needed by some BLE stacks). For these libraries, we leave the C files intact and craft `module.mk` makefiles for each of them.

The usual process is:
1. Craft the makefiles (usually it is sufficient to copy them from a previous SDK port and modify from there if needed)
1. Compile using gcc. This produces a static lib (cordio.a and open-amp.a)
1. Copy that static lib into a new directory (`cordia/lib` and `open-amp/lib_prebuilt`)
1. Do the same for armclang.
1. Change the `module.mk` to use the lib instead of compiling (cordio in particular has a lot of files)

### Details: neuralspot
Unless the API changes, an SDK update is easy, with minimal changes to neuralspot.

If new HW lands, we usually have to update anything that touches HW:
1. ns-core contains the linker scripts and startup code. I look at any example in the new SDK to see if anything radical changed. The SDK doesn't have C++ support, so the linker scripts from those can't just be copied over. They also don't have SRAM bss. This step usuall involves a port of an existing neuralspot startup+linker script, not a copy
2. ns-power is one of the things that changes regularly in the SDK. Look at the coremark examples to see how they differ from neuralspot's ns-power, and port that in.
3. ns-audio and ns-usb are also somewhat volatile in the SDK. If neuralspot's code isn't working, compare it to the example code in pdm_fft and any of the CDC examples.
4. GPIOs and IOMs tend to change when EVB hardware changes. The clickboard IOM can move from IOM0 to IOM5, for example - the code change here is pretty simple - GPIO and IOM are parameters for many of neuralspots code.

### Details: Delete Everything not needed
We only need:
```
boards/<platform>/bsp
mcu/
third_party/
  cordio/
  open-amp/
  uecc/
devices/
utils/
CMSIS/
pack/
anything that looks like a license
```
Delete the rest. It's a lot.
