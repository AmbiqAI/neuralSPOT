# neuralSPOT
NeuralSPOT is Ambiq's AI Enablement Library

Quick and Dirty Content Overview
1. cmake: directory containing comming .cmake helper files, including the toolchain definition and common ambiq compiler directives
2. examples: contains the 'application' you're building, today this is a basic hello_world, it doesn't even have tensorflow yet
3. extern: any external libraries go here. This is where AmbiqSuite and Tensorflow live.
	1. Only minimum code (header files and a handful of code files)
	2. Static libraries
	3. Organized by version
4. libraries: contains neuralSpot, modularized into libraries (IPC, Audio for now, power coming soon)

To build the example:
1. make a build directory, typically off of ./neuralSPOT
2. set up cmake build env by: cd to build directory, run 'cmake ..'
3. build the example: 'make'

NOTE: you need cmake 3.24



# Makefile Planning
Using recursionless makefiles
1. foo/module.mk for each subdirectory artifact
2. generate .a or .axf per each module.mk
3. top level makefile just stiches stuff together

Started on a skeleton - currently writing ns-ipc module.mk, still figuring out how to generate objects 
