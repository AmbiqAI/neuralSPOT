# Developers Guide

This guide describes the process and structure for adding components to NeuralSPOT. Types of NeuralSPOT components include:

1. NeuralSPOT Libraries
2. NeuralSPOT Examples
3. External Components
4. RPC Interfaces

## Adding a NeuralSPOT Library Component

NeuralSPOT Libraries are small, task-specific components. They're located in `./neuralspot`, and are generally organized by function. For example, current components include ns-audio, ns-ipc, ns-i2c, ns-rpc, and so on. Some libraries are collections of very simple functions - ns_peripherals includes both ns-power and ns-button, components which are too simple to warrant their on libraries.

Each library compiles to a static lib (e.g. `ns-audio.a`), and has a defined API (e.g. `ns-audio.h`). When linking, only code which is invoked is actually linked into the binary.

### Library Component Structure

Every neuralspot library has a similar structure - some of this is dictated by neuralspot's makefile architecture, and some is by convention. The typical structure is as follows:

```bash
./neuralspot/
	ns-<componentName>/
		src/						# All c/cc/cpp/s/h/hpp files in here will be compiled to a static library
		include_api/		# H/HPP files defining APIs for library - files in here will be added to -I compiler path
		module.mk				# Simple makefile (see boilerplate below)
		ns-<componentName>.md # Local documentation
		build/ 					# temporary working directory for storing static libs, object files and other compile artifacts
```

## Library Component Module definition

Each component includes a local `module.mk`. These are fairly similar in structure, and tend to follow this pattern:

```make
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-<componentName>.a, $(local_src))) # only change needed
```

The only thing that normally differs between library's `module.mk` is the `<componentName>`



## Adding an Example

NeuralSPOT Examples are pared-down examples showcasing NueralSPOT functionality. They contain the application's main() function which typically initializes everthing and then executes the application loop.

For every Example, NeuralSPOT will compile all the binary artifacts needed to load the application onto an EVB, such as main.axf, main.bin, and so on. These artifacts are stored in the example's temporary build directory. 

---

** Note **



The name of the artifact is derived from the name of the source file containing `main()`, not the directory it resides in.

---



### Example Component Structure

Every neuralspot library has a similar structure - some of this is dictated by neuralspot's makefile architecture, and some is by convention. The typical structure is as follows:

```bash
./examples/
	<exampleName>/
		src/						# All c/cc/cpp/s/h/hpp files in here will be compiled to a flashable file
		module.mk				# Simple makefile (see boilerplate below)
		<exampleName>.md # Local documentation
```

## Library Component Module definition

Each component includes a local `module.mk`. These are fairly similar in structure, and tend to follow this pattern:

```make
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-<componentName>.a, $(local_src))) # only change needed
```

The only thing that normally differs between library's `module.mk` is the `<componentName>`



## Adding an External Component



## Adding an RPC Interface



