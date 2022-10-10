# Developing with NeuralSPOT

NeuralSPOT is a developer-centric toolkit for creating AI-powered features for an application. Recognizing that there are almost as many development flows as there are developers, neuralSPOT includes a flexible deployment mechanism to ease integration into many types of flows - we call it the *Nest*.

The Nest is a fully contained instantiation of your feature, ready to be compiled and linked into your larger project. It accomodates three major types of workflow:

1. Developing your feature within NeuralSPOT and deploy it periodically to your application.
2. Developing your feature as a standalone project, taking updates from neuralSPOT as needed and integrating with a larger complex project.
3. Developing your feature directly within your application - similar to developing a standalone AI feature project.

## Developing Within neuralSPOT

This is the simplest approach for developing a new AI feature because it integrates closely with neuralSPOT's code base without the need for exporting include files and static libraries. Roughly, the flow is:

1. Create a project within neuralSPOT's example directory (neuralSPOT will be treated as an example)
2. Develop within that directory, compiling neuralSPOT and feature code as needed. This code can be directly deployed and run as a standalone application 
3. When ready to deploy to your application, use `make nestall` to drop in your feature in it's entirety.

## Developing as a Standalone Application

This approach involves creating a Nest from within neuralSPOT and developing your code within the nest. The flow is roughly:

1. Create a 'base' Nest by using `make nestall`
2. Develop your feature within that Nest - similar to above, this code can be directly deployed and run as a standalone application.
   1. When neuralSPOT updates or upgrades are needed use either 'make nestcomponent' to install a component, or 'make nest' to do a (almost, see below for caveats) non-destructive redeployment of neuralSPOT to the Nest.
3. When done, copy over to your application

## Developing as an Application Component

This is similar to developing as a standalone application - you'll use the same `make nestall` , `make nest` , and `make nestcomponent` features as above. The main difference will be the need to integrate nest compilation and linking into your larger application's existing code build system.

## Making and Managing Nests

As briefly alluded to below, neuralSPOT's build system can be used to create a and manage Nests via three make targets:

| Command                                                      | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `make NESTDIR=<directory> TARGET=<example> nestall`          | Creates an nest from scratch. It will overwrite anything already there, including code in src/ and makefiles. It will attempt to preserve existing src/ code if needed (in src/preserved) but it doesn't attempt anything fancy like code merges. |
| `make NESTDIR=<directory> TARGET=<example> nest`             | Copies all neuralSPOT components (libraries and external components) overwritting any components already there. It will not copy over makefiles or anything in the src/ directory. |
| `make NESTDIR=<directory> NESTCOMP=<nestcomponent> nestcomponen`t | Will only copy the specified component, including all header and static library files. |

### Behind the Scenes

While convenient, making a Nest is not that sophisticated. It is helpful to understand what actually happens behind the scenes when creating and managing nests.

Both the `nest` and `nestall` make targets do the following:

1. Create needed directories at NESTDIR - see [here](https://github.com/AmbiqAI/neuralSPOT/blob/main/README.md#nest-directory-contents) for the directory structure
2. Copy all of neuralSPOT's static libraries, including external and neuralspot libraries
3. Copies the example code specified by TARGET (if the target is `nestall`)
   1. If there are files in NESTDIR/src already (for example, if you are upgrading an existing Nest), `nestall` will copy those files into /src/preserved.
4. Create makefiles needed to compile and link the example.
   1. The `nestall` target overwrites existing Nest makefiles, while `nest` copies them over with a `.suggestedmakefile` suffix.

The `nestcomponent` target is the best way to upgrade a single component within an existing Nest (for example, picking up a new version of AmbiqSuite). It works by copying only headers and libraries that are part of that component - in practice, those that are located under the directory specified by NESTCOMP. For example:

```bash
$> make NESTCOMP=extern/AmbiqSuite/R4.3.0 nestcomponent # copies/overwrites just AS 4.3.0
$> make NESTCOMP=neuralspot/ns-harness nestcomponent # copies/overwrites the ns-harness library
```

