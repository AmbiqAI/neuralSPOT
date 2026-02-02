# neuralSPOT Nests
neuralSPOT can export entire applications to be linked into larger projects via neuralSPOT Nests. A Nest includes every library your application used from neuralSPOT in the form ready-to-compile headers and static libraries. Nests are most useful when porting a demonstration to a non-EVB platform. To only export the bare minimum needed to run a Tensorflow Lite for Microcontroller model, it is better to use [neuralSPOT Autodeploy tools](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/From%20TF%20to%20EVB%20-%20testing,%20profiling,%20and%20deploying%20AI%20models.md).

The Nest is a fully contained instantiation of your AI application, ready to be compiled and linked into your larger project. It accomodates three major types of workflow:

1. Developing your feature within neuralSPOT and deploy it periodically to your application.
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
| `make NESTDIR=<directory> NESTEGG=<example> nestall`         | Creates an nest from scratch. It will overwrite anything already there, including code in src/ and makefiles. It will attempt to preserve existing src/ code if needed (in src/preserved) but it doesn't attempt anything fancy like code merges. |
| `make NESTDIR=<directory> NESTEGG=<example> nest`            | Copies all neuralSPOT components (libraries and external components) overwritting any components already there. It will not copy over makefiles or anything in the src/ directory. |
| `make NESTDIR=<directory> NESTCOMP=<nestcomponent> nestcomponent` | Will only copy the specified component, including all header and static library files. |

### Behind the Scenes
It is helpful to understand what actually happens behind the scenes when creating and managing nests.

Both the `nest` and `nestall` make targets do the following:

1. Create needed directories at NESTDIR
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
