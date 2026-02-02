# neuralSPOT Developer's Guide

This document covers various guidelines for creating and modifying neuralSPOT's various types of components. Its intended audience are neuralSPOT developers and contributors.

## FAQs

##### Where is main()?

neuralSPOT [examples]() each contain a `main()`, and each example produces a different binary (AXF and BIN files).

##### Where are the linker script and reset handlers?
neuralSPOT's platform-specific linker scripts and startup code are located in [neuralspot/ns-core/src/gcc](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-core/src/gcc) for the GCC toolchain and [neuralspot/ns-core/src/armclang](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-core/src/armclang) for the armclang toolchain.


##### How do the makefiles work?

This is documented within the [main makefile](https://github.com/AmbiqAI/neuralSPOT/blob/main/Makefile).

##### Where are compilation artifacts store?

By default, compilation artifacts are placed in `build\...` and mirror the location of the source of the artifact (for example, the binaries for the HAR example are placed at `build/examples/har`).

---

## Adding and Updating Components to neuralSPOT

This section describes the process and structure for adding components to neuralSPOT. Types of neuralSPOT components include:

1. neuralSPOT Libraries
2. neuralSPOT Examples
4. External Components
1. Documentation
5. RPC Interfaces

### Adding neuralSPOT Library Components

neuralSPOT Libraries are small, task-specific components. They're located in `./neuralspot`, and are generally organized by function. For example, current components include ns-audio, ns-ipc, ns-i2c, ns-rpc, and so on. Some libraries are collections of very simple functions - ns_peripherals includes both ns-power and ns-button, components which are too simple to warrant their on libraries.

Each library compiles to a static lib (e.g. `ns-audio.a`), and has a defined API (e.g. `ns-audio.h`). When linking, only code which is invoked is actually linked into the binary.

#### Library Component Structure

Every neuralspot library has a similar structure - some of this is dictated by neuralspot's makefile architecture, and some is by convention. The typical structure is as follows:

```bash
./neuralspot/
	ns-<componentName>/
		src/         # All c/cc/cpp/s/h/hpp files in here will be compiled to a static library
		include_api/ # H/HPP files defining APIs for library - files in here will be added to -I compiler path
		module.mk	 # Simple makefile (see boilerplate below)
		README.md # Local documentation
		build/       # temporary working directory for storing static libs, object files and other compile artifacts
```

#### Library Component Module definition

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
The only thing that normally differs between library's `module.mk` is the `<componentName>`, but this file can be used for more elaborate purposes (for example, ns-core's [module.mk](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-core/module.mk) conditionally includes source files based on the `TOOLCHAIN`` setting).

#### Library API Versioning
Many of the libraries have APIs - those that do are 'versioned', meaning that they have a related semantic version number. When a library's init() function is called, part of the configuration includes a desired version number. The initialization function checks that the desired version is supported, and fails if it isn't.

When adding a new library, include the following code to add version control...

##### In the Include File
```c
// using 'usb' as example
#define NS_USB_V0_0_1                                                                            \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_USB_V1_0_0                                                                            \
    { .major = 1, .minor = 0, .revision = 0 }

#define NS_USB_OLDEST_SUPPORTED_VERSION NS_USB_V0_0_1
#define NS_USB_CURRENT_VERSION NS_USB_V1_0_0
#define NS_USB_API_ID 0xCA0006 // unique for each libray

extern const ns_core_api_t ns_usb_V0_0_1;
extern const ns_core_api_t ns_usb_V1_0_0;
extern const ns_core_api_t ns_usb_oldest_supported_version;
extern const ns_core_api_t ns_usb_current_version;
```


##### In the library implementation
```c
const ns_core_api_t ns_usb_V0_0_1 = {
    .apiId = NS_USB_API_ID, .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_V1_0_0 = {
    .apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

const ns_core_api_t ns_usb_oldest_supported_version = {
    .apiId = NS_USB_API_ID, .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_current_version = {
    .apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

static ns_usb_config_t usb_config = {.api = &ns_usb_V1_0_0, // one of the above
                                     // rest of init
                                    };
main()
#ifndef NS_DISABLE_API_VALIDATION
	// other API checks

	// Version Check
    if (ns_core_check_api(cfg->api, &ns_usb_oldest_supported_version,
                          &ns_usb_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
```

### Adding neuralSPOT Examples

neuralSPOT Examples are pared-down examples showcasing neuralSPOT functionality. They contain the application's main() function which typically initializes everthing and then executes the application loop.

For every Example, neuralSPOT will compile all the binary artifacts needed to load the application onto an EVB (specifically, AXF and BIN images). These artifacts are stored in the example's temporary build directory (e.g. `build\examples\har`).

>  **Note** The name of the artifacts are derived from the example's `module.mk`,  not the directory it resides in.

#### Example Component Structure
Every neuralspot library has a similar structure - some of this is dictated by neuralspot's makefile architecture, and some is by convention. The typical structure is as follows:

```bash
./examples/
	<exampleName>/
		src/             # All c/cc/cpp/s/h/hpp files in here will be compiled to a flashable file
		module.mk        # Simple makefile (see boilerplate below)
		README.md        # Local documentation
```
> *NOTE* Each example can access all of neuralSPOT's libraries and external components, but the code in the example's `src` directory is only visible to the example itself.

#### Example Component Module definition
Each example includes a local `module.mk`. These are fairly similar in structure, and tend to follow this pattern:

```make
local_app_name := basic_tf_stub
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
```

### Adding External Components
External components are any component that may be needed by neuralSPOT, but aren't a part of it. External components include things like AmbiqSuite, Tensorflow Lite for Microcontrollers, CMSIS, and Embedded RPC.

The structure of an external component included in neuralSPOT depends on the component - for example, `extern/AmbiqSuite` doesn't include all of the SDK, instead including only the necessary header files, static libraries, and bare minimum of source files needed to compile.

neuralSPOT supports multiple versions of external components. These are stored in different subdirectories under the external component directory, and are selected by makefile flags in `make/neuralspot_config.mk`.

#### External Component Structure

Generally, the structure of an external component within neuralSPOT is:
```bash
.../extern/
	<componentName>/
		<version>/
			lib/   # Contains static libraries if there are any
			src/   # source and header files
			module.mk
```

Because external components don't all have the same structure, the module.mk is less boilerplate than that of other types of components. The critical parts of a typical module.mk are:

1. Defining the local source files that need compilation
2. Add required header files to `includes_api`
3. Adding any pre-built static libraries to `lib_prebuilt`
4. If the component generates object files, they need to be linked into a static library via the `make-library` makefile function.

#### External Component Module Definition

Here is a simple example which adds tensorflow to neuralSPOT:
```make
includes_api += $(subdirectory)/tensorflow
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include

lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite.a
```

On the other hand, here is AmbiqSuite's module.mk:
```make
# Add source files
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)

# Base AmbiqSuite include files (note the $ variables)
includes_api += $(subdirectory)/boards/$(BOARD)_$(EVB)/bsp
includes_api += $(subdirectory)/CMSIS/ARM/Include
includes_api += $(subdirectory)/CMSIS/AmbiqMicro/Include
includes_api += $(subdirectory)/devices
includes_api += $(subdirectory)/mcu/$(BOARD)
includes_api += $(subdirectory)/mcu/$(BOARD)/hal/mcu
includes_api += $(subdirectory)/utils

# Pre-built static libraries
lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.a
lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4lf_math.a

# Third-Party (FreeRTOS)
includes_api += $(subdirectory)/third_party/FreeRTOSv10.1.1/Source/include
includes_api += $(subdirectory)/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4

# Third-Party (TinyUSB)
includes_api += $(subdirectory)/third_party/tinyusb/src
includes_api += $(subdirectory)/third_party/tinyusb/src/common
includes_api += $(subdirectory)/third_party/tinyusb/src/osal
includes_api += $(subdirectory)/third_party/tinyusb/src/class/cdc
includes_api += $(subdirectory)/third_party/tinyusb/src/device

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)

# Special link and compiler overrides
LINKER_FILE := $(subdirectory)/src/linker_script.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

$(eval $(call make-library, $(local_bin)/ambiqsuite.a, $(local_src)))
```

### Adding and Updating Documentation
neuralSPOT's documentation in automatically generated from the repository using Mkdocs. Because of where our markdown docs are located, we had to locate the mkdocs definition file outside of the repository (mkdocs expects all markdown to reside in a subdirectory which is not where mkdown.yml lives, and does not support `..\` relative paths). This means we cannot use the Github deployment
command (`mkdocs gh-deploy`), and have to do so manually:

```bash
pip install mkdocs mkdocs-material mkdocs-glightbox mkdoxy
cd ..
cp neuralspot/mkdocs.yml .
mkdocs build
cd neuralspot
git checkout gh-pages
cp -R ../site .
git add .
git commit -m "update docs"
git push origin gh-pages
```

### Adding RPC Interfaces
> **LIMITATIONS** ns-rpc only works as client or server, exclusively.

RPC (Remote Procedure Call) interfaces are a way to 'call' functions that reside on another CPU (in neuralSPOT's case, on another computer entirely). Adding an RPC interface is a fairly complex process involving:

1. Defining the interface using an IDL (interface definition language)
2. Processing that definition to generate server and client code.
3. Copying the generated client code to neuralSPOT, generally as a new neuralspot library
4. Implementing the server code that will respond to the client.

neuralSPOT RPC is based on [a modified fork](https://github.com/AmbiqAI/erpc) of [EmbeddedRPC (eRPC)](https://github.com/EmbeddedRPC/erpc) which supports RPC-over-TinyUSB on our EVBs. How eRPC works, including the [specifics of IDL definition](https://github.com/EmbeddedRPC/erpc/wiki/IDL-Reference), is outside the scope of this document.

#### RPC Structure

RPC in neuralSPOT is structured as follows:

1. The relevant parts of eRPC's infrastructure reside in [`extern/erpc`](https://github.com/AmbiqAI/neuralSPOT/tree/main/extern/erpc)
2. Interface library implementations reside in [`neuralspot/ns-rpc`](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-rpc)
3. Examples of generic RPC utilization reside in [`examples/rpc-client`](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_client), and [`examples/rpc-server`](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/rpc_server).
4. Examples of practical utilization of RPC to capture data can be found in [`examples/mpu_data_collection`](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/mpu_data_collection) (for i2c) and [`examples/basic_tf_stub`](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/basic_tf_stub) (for audio)

##### NS-RPC Structure

```bash
ns-rpc/
	interfaces/ # contains IDL definition files
	includes-api/ # API for each interface
	python/ # PC-side code implementing the interface and example client/servers using it
	src/ # Code implementing the interface and wrapping it for neuralspot
```

#### Creating a New Interface

As briefly described above, creating a new interface involves multiple steps, some of which generate CPP and Python code that need to be wrapped usefully for integration with neuralSPOT.

##### Step 1 - Defining the Interface

eRPC definitions start from an ERPC file which is written in an IDL (defined [here](https://github.com/EmbeddedRPC/erpc/wiki/IDL-Reference)). eRPC doesn't have a lot of good examples in the wild, so we recommend you start from an existing definition such as [`GenericDataOperations.erpc`](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/interfaces/GenericDataOperations.erpc).

##### Step 2 - Generating the code

eRPC can generate both C and Python client and server code (of course, the Python code is only for use on the PC). You only have to generate Python if you plan on implementing Python-based PC applications. Currently, neuralSPOT only includes Python PC examples.

```bash
$> cd neuralspot/ns-rpc
$> mkdir python/foo
$> erpcgen -g py -o python/foo interfaces/foo.erpc # Generate Python client/server code
$> erpcgen -o src interfaces/foo.erpc # Generate C client/server code

```

> **NOTE** The Client C code contains prototypes for the remote procedures. This will cause linking errors if you are not implementing those procedures. If you don't plan on implementing EVB-side client procedures, delete the file or give it a non-source extension to avoid compiling it.

At this point you'll have everything you need to implement the interface.

##### Step 3 - Wrap the Interface

When the EVB is acting as a client (i.e. it is calling procedures that reside on the PC), all you really need to do is add an `init()` and a header file that includes everything needed for the application to find the remote procedures. The `init()` should initialize the USB interface and the erpc client.

When the EVB is acting as a server, the `ns-rpc` wrapper should also include implementations for the remotely called procedures. One way to implement this is to have those procedures invoke callbacks, which can be defined via the `init()`.

See the GenericDataOperations [wrapper](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/src/ns_rpc_generic_data.c) for an example of how to wrap both client and server interfaces.

##### Step 4 - Write a PC-side RPC application

Your new interface will need a PC-side application (server or client, depending on your needs). neuralSPOT includes [an example PC side application](https://github.com/AmbiqAI/neuralSPOT/blob/main/neuralspot/ns-rpc/python/ns-rpc-genericdata/generic_data.py) that can be configured as either a server or client. The code is fairly self-explanatory.

> **Note** that the eRPC Python implementation is not well documented or tested. neuralSPOT's GenericDataOperation RPC interface has been tested to work well after fixing a few bugs in our eRPC fork, but other types of interfaces have not be stressed.

#### Modifying an Existing Interface

Modifying an interface involves changing the existing interface's eRPC file, generating new interface files, and modifying the wrappers to accomodate those changes.
