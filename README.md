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

Our 'externs' are special cases, not really covered by most examples
1. AS has some code, and an ton of include paths, and some libs
2. TF is a lib and some include paths

Ideas:
1. create a 'global_inc' var which is populated by extern modules.mk (and don't try to make 'em too smart)
2. Change the includes to <> and have a single top level -I per extern
	1. Don't know if this will work for extern headers including extern headers

We need to do the same for 'libs' - the subdirs know what libs should be included by examples/apps

May be easier to do recursive with the ideas listed above.

I want customers to be able to link this in easily. NS should export a linkable library with limited set of API headers ('make neuralspot' creates a standalone NS directory).