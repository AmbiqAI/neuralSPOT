# neuralSPOT
NeuralSPOT is Ambiq's AI Enablement Library

# Building and Deploying NeuralSPOT
NeuralSPOT can be used in two ways:
1. As the 'base of operations' for your AI development. Intended for stand-alone EVB development, you can add new binary (axf) targets to the /examples directory
2. As a seed for adding NeuralSPOT to a larger project. In this mode of operations, you would use NeuralSPOT to create a stub project with everything needed to start running AI on EVBs

## Build Options
'make' - builds everything, including every target in example
'make clean' - deletes every build directory and artifact
'make nest' - creates a minimal 'nest' with a basic main.cc stub file

# Structure
/neuralspot - contains all code for NeuralSPOT libraries
/extern - contains external dependencies, including TF and AmbiqSuite
/examples - contains several examples, each of which can be compiled to a deployable axf
/make - contains makefile helpers, including neuralspot-config.mk

## Configuring NeuralSPOT
I want to make this a YAML interface, but for now it is makefile based. Options are the same you are used to from the AI repo

# The Nest
The Nest is an automatically created directory with everything you need to get TF and AmbiqSuite running together and ready to start developing AI features for your application.

# Building Nest
1. cd NeuralSpot
2. make
3. make nest
4. cd nest
5. make

### Nest Directory Contents
./Makefile
./autogen.mk
make/
	helpers
includes/
	*.h with preserved original directory structure
libs/
	*.a
src/
	main.c
	model.h

# Todo
Jlink Support - still figuring out how to do that with multiple axf targets. Maybe a cache system