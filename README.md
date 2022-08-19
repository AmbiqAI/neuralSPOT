# NeuralSPOT
NeuralSPOT is Ambiq's AI Enablement Library. It implements an AI-centric API for common tasks such as collecting audio from Ambiq's peripherals, computing features from that audio, controlling power modes, reading accelerometer and gyroscopic data from i2c sensors, and a variety of helper functions and tools which make developing AI features on Ambiq hardware easier.

![image-20220811095223908](./docs/image-20220811095223908.png)


# Building and Deploying NeuralSPOT

NeuralSPOT is designed to be used in two ways:
1. **As the 'base of operations' for your AI development**. Intended for stand-alone EVB development, you can add new binary (axf) targets to the /examples directory
2. **As a seed for adding NeuralSPOT to a larger project**. In this mode of operations, you would use NeuralSPOT to create a stub project (a [nest](#The Nest)) with everything needed to start running AI on EVBs

## Build Options
All `make` invocations for NS must be done from the base directory ("nest" makes are different, and defined below). The primary targets are:

`make` - builds everything, including libraries and every target in examples directory
`make libraries` - builds the neuralspot and extern libraries
`make clean` - deletes every build directory and artifact
`make nest` - creates a minimal '[nest](#The Nest)' with a basic main.cc stub file

Besides targets, NeuralSPOT has a standard set of compile-time switches to help you configure the build exactly the way you need. These are set via the normal make convention, e.g. `make BOARD=apollo4b`.

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| BOARD | Defines the target SoC (currently either apollo4b or apollo4p) | apollo4p |
| EVB | Defines the EVB type (evb or evb_blue) | evb |
| BINDIR | Name of directories where binaries and build artifacts are stored. Note that there will be one build directory per binary or library being created | build |
| NESTDIR | Relative path and directory name where nest will be created | nest |
| AS_VERSION | Ambiqsuite Version | R4.1.0 |
| TF_VERSION | Tensflow Lite for Microcontrollers Version | R2.3.1 |
| TARGET | Defines what target will be loaded by `make deploy` | s2i |
| MLDEBUG | Setting to '1' turns on TF debug prints | 0 |
| AUDIO_DEBUG | Setting to '1' turns on RTT audio dump | 0 |

Note that defaults for these values are set in `./make/neuralspot_config.mk`.

# NeuralSPOT Repo Structure

NeuralSPOT consists of the neuralspot library, required external components, and examples. 

```/neuralspot - contains all code for NeuralSPOT libraries
neuralspot/ - contains neuralspot feature-specific libraries
	ns-audio/
		src/
		include_api/
		module.mk
	ns-ipc/
	ns-<some-other-feature>
	...
extern/   - contains external dependencies, including TF and AmbiqSuite
	AmbiqSuite/
		<version>/
	tensorflow/
		<version>/
/examples - contains several examples, each of which can be compiled to a deployable axf
/make     - contains makefile helpers, including neuralspot-config.mk
```



# NeuralSPOT Theory of Operations.

NeuralSPOT is a SDK for AI development on Ambiq products via an AI-friendly API. It offers a set of libraries for accessing hardware, pre-configured instances of external dependencies such as AmbiqSuite and Tensorflow Lite for Microcontrollers, and a handful of examples which compile into deployable binaries.

![image-20220811161922376](./docs/image-20220811161922376.png)



## NeuralSPOT Libraries

NeuralSPOT is continuously growing, and offers the following libraries today:

1. `ns-audio`: API for collected audio from Ambiq's audio interfaces and sending them to an AI application via several IPC methods. This library also contains audio-centric common AI feature helpers such as configurable Mel-spectogram computation.
2. `ns-peripherals`: API for controlling Ambiq's power modes, performance modes, and helpers for commonly used I/O devices such as EVB buttons.
3. `ns-harness`: a simple harness for abstracting common AmbiqSuite code, meant to be replaced when NeuralSPOT is not being used by AmbiqSuite.
4. `ns-ipc`: Common mechanisms for presenting collected sensor data to AI applications
5. (soon)`ns-experimental-drivers`: Drivers for 3rd party sensors that have not yet been incorporated into mainline AmbiqSuite.

## The Nest

The Nest is an automatically created directory with everything you need to get TF and AmbiqSuite running together and ready to start developing AI features for your application. It is created for your specific target device and only includes needed header files, along with a basic application stub with a main().

### Building Nest

Before building a nest, you must first build NeuralSPOT for your desired target. By default, the nest will be created in NeuralSPOT's root directory - set NESTDIR to change where it is built.

```bash
$> cd neuralSpot
$> make						# this builds the artifacts needed to build the nest
$> make nest			# this creates the nest, use BINDIR=<your directory> to change where
$> cd nest
$> make 					# this builds the Nest stub. 
```

### Nest Directory Contents
```bash
Makefile
autogen.mk # Automatically generated by 'make nest'
make/
	# helper scripts and make includes
includes/
	# *.h files in preserved original directory structure
libs/
	# needed *.a files for both 3rd party and neuralspot libraries
src/
	main.cc
	model.h
```

# WIP
## Adding malloc support
Malloc support is non-deterministic, and should only be used for debug environments such as erpc

It has 3 components
1. ns-malloc
2. example/basic_tf_stub modified to include it
3. ns-harness maps standard malloc/etc to ns-malloc equivalents

### Status
Added malloc and usb to ambiqsuite.a
Created ns-usb with init, send, and rx routines
Next
1. create ns-util malloc stuff, get it compiling (should be easy wrapper around freertos malloc) (DONE)
2. Create new nest in erpc, taking care to not obliterate work in current nest (DONE)
3. Modify erpc transport code to use all of the above
4. Test in main.cc