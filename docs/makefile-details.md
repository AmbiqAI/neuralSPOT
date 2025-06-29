# Building and Deploying NeuralSPOT
NeuralSPOT make system is can be used in two ways:

1. **As the 'base of operations' for your AI development**. Intended for stand-alone EVB development, you can add new binary (axf) targets to the /examples directory.
2. **As a seed for adding NeuralSPOT to a larger project**. In this mode of operations, you would use NeuralSPOT to create a stub project (a "nest", described below) with everything needed to start running AI on EVBs.

## Build Options
All `make` invocations for neuralSPOT must be done from the base directory ("nest" makes are different, and defined below). The primary targets are:

| Target               | Description                                                  |
| -------------------- | ------------------------------------------------------------ |
| `make`               | builds everything, including libraries and every target in examples directory |
| `make clean`         | deletes every build directory and artifact                   |
| `make libraries`     | builds the neuralspot and external component libraries       |
| `make nestall`       | creates a minimal '[nest](#The_Nest)' with a basic main.cc stub file |
| `make nest`          | creates a minimal '[nest](#The_Nest)' *without* a basic main.cc stub file and without overwriting makefiles |
| `make nestcomponent` | updates a single component in the nest                       |
| `make deploy`        | Uses jlink to deploy an application to a connected EVB       |
| `make view`          | Starts a SWO terminal interface                              |

Besides targets, NeuralSPOT has a standard set of compile-time switches to help you configure the build exactly the way you need. These are set via the normal make conpiption, e.g. `make BOARD=apollo4b`.

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| PLATFORM | Defines the target EVB (currently apollo4p_evb, apollo4p_blue_kxr_evb, apollo4p_blue_kbr_evb, apollo4l_evb, apollo4l_blue_evb, or apollo3p_evb) | apollo4p_evb |
| BINDIRROOT | Name of directories where binaries and build artifacts are stored. | build |
| EXAMPLE | Name of example to be built. By default, all examples will be built. |All|
| NESTDIR | Relative path and directory name where nest will be created | nest |
| NESTCOMP | root path to a single component to be updated for `make nestcomponent` | extern/AmbiqSuite |
| NESTEGG | name of neuralspot example used to create nest | basic_tf_stub |
| AS_VERSION | Ambiqsuite Version | R4.4.1 |
| TF_VERSION | Tensflow Lite for Microcontrollers Version | d5f819d_Aug_10_2023 |
| TOOLCHAIN | Compiler toolchain, set to 'arm' to select armclang | arm-none-eabi (GCC) |
| MLDEBUG | Setting to '1' turns on TF debug prints | 0 |
| MLPROFILE | Setting to '1' enables TFLM profiling and logs | 0 |

> **Note**  Defaults for these values are set in `./make/neuralspot_config.mk`. Ambiq EVBs are available in a number of flavors, each of which requiring slightly different config settings. For convenience, these settings can be placed in `./make/local_overrides.mk` (note that this file is ignored by git to prevent inadvertent overrides making it into the repo). To make changes to this file without tracking them in git, you can do the following:
> `$> git update-index --assume-unchanged make/local_overrides.mk`

## NeuralSPOT Nests
The Nest is an automatically created directory with everything you need to get TF and AmbiqSuite running together and ready to start developing AI features for your application. It only includes static libraries, related header files, and a basic application stub with a main(). Nests are designed to accomodate various development flows - for a deeper discussion, see [Developing with neuralSPOT](./Developing_with_NeuralSPOT.md).

### Building Nest
Before building a nest, you must first build NeuralSPOT for your desired target. By default, the nest will be created in NeuralSPOT's root directory - set NESTDIR to change where it is built.

```bash
$> cd neuralSpot
$> make		       # this builds the artifacts needed to build the nest
$> make nestall  # this creates the nest, use BINDIR=<your directory> to change where
$> cd nest
$> make          # this builds the Nest basic_tf_stub.
```

### Nest Directory Contents
```bash
Makefile
autogen.mk # Automatically generated by 'make nest'
make/      # helper scripts and make includes
includes/  # *.h files in preserved original directory structure
libs/      # needed *.a files for both 3rd party and neuralspot libraries
src/
	<nestegg sources>
srcpreserved/
	<previous contents of src - see 'upgrade' section below>
```

### Choosing which NeuralSPOT example to base Nest on
By default, the example/basic_tf_stub is used to create the Nest (meaning the files in example/basic_tf_stub/src are copied into the nest). To base the nest on another example, used the NESTEGG parameter:

```bash
$> make NESTEGG=mpu_data_collection nestall # use example/mpu_data_collection as bases for new nest
```

### Updgrading a Nest
When you create a nest, all source files (include_apis, example source code, libraries, etc.) are copied
to NESTDIR. If you are working within a previously created nest, creating a new one with the same NESTDIR
destination will overwrite these files. This is necessary for certain files (libraries, include_apis) that
are required for NeuralSPOT's functionality, but not critical for the example code.

There are 3 ways to create or update a nest:
1. `make nestall`: copies all nest components including example source code and makefiles
2. `make nest`: copies everything except example source code and makefiles (it does create a suggested makefile)
3. `make NESTCOMP=desired_component nestcomponent`: copies over only includes and libraries associated with the specified component.

To ease upgrading existing nests, `make nest` and `make nestall` will create a `$(NESTDIR)/srcpreserved` directory and copy the existing contents of `$(NESTDIR)/src` to it.

With this in mind, the nest 'full' upgrade workflow goes something like this:
1. Before upgrading, preserve any non-src work in the $(NESTDIR) (and consider contributing these changes back to NeuralSPOT, of course):
	1. Any changes to files in includes/
	2. Any changes in pack/
2. Upgrade the nest by running `make nest` with the same NESTDIR you are already using
3. Compare `$(NESTDIR)/src/preserved` to new files in `$(NESTDIR)/src`, and copy in or merge as needed

If only one component (extern/.. or neuralspot/..) needs to be updated, `make nestcomponent` can be used instead.

For example:
```bash
$> make NESTCOMP=neuralspot/ns-rpc nestcomponent # only updates ns-rpc header files and static libs
```

### Adding a target platform to an existing Nest
When a Nest is created, it includes the files for a specific PLATFORM (e.g. apollo4p_evb). More platforms can be added to a nest by running the `make nest` with the PLATFORM variable set. Platforms can coexist within a single Nest, and the makefile for the Nest "understands" PLATFORM and will choose the files accordingly.
```bash
> cd neuralSPOT
> make -j # Makes the initial libraries, defaults the platform to apollo4p_p
> make nestall # creates the initial Nest
> make PLATFORM=apollo4p_blue_kbr_evb # creates files for new platform
> make nestall PLATFORM=apollo4p_blue_kbr_evb # Adds the new platform to the Nest
> cd nest
> make # builds for apollo4p_evb
> make PLATFORM=apollo4p_blue_kbr_evb # builds for apollo4p_blue_kbr_evb
```