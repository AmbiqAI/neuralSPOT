# From TF to EVB - testing, profiling, and deploying AI models

Developing AI models is hard. Deploying them on embedded devices is also hard. Developing AI features for embedded devices requires doing both of these hard things over and over again until everything is working and optimized. Fortunately, Ambiq's neuralSPOT Autodeploy tool is here to help.

Autodeploy is an Ambiq-specific tool which automates the creation, deployment, validation, and profiling of TFlite models on embedded platforms.

## The Old Way of Doing Things

Developing an AI feature typically involves a repetitive process: 

1. An AI developer produces a model that implements the feature more or less optimally
2. That model is converted to code that can execute on an EVB. This is a complex process that requires manually generating new code every time the model changes.
3. That code is deployed and tested on the EVB. Problems may arise from numeric mismatches, memory issues, and performance (latency) issues.
4. The AI developer analyses and debugs any issues, tweaks the model in an attempt to mitigate them, and jumps back to step 1.

Each of of these steps is mostly manual, complex, and fraught with the potential for coding errors.

Fortunately, all the information needed to automate the middle 2 steps is tucked in Tensorflow's representation of the model - all that is needed is a convenient way to extract it, and an automation flow that takes advantage of that info.

# Ambiq's AutoDeploy is Here to Help

AutoDeploy is a tool that speeds up the AI/Embedded iteration cycle by automating most of the tedious bits - given a TFLite file, the tool will convert it to code that can run on an Ambiq EVB, then run a series of tests to characterize its embedded behavior. It then generates a minimal static library suitable implementing the model for easy integration into applications.



![image-20230407163734881](/Users/carlosmorales/AmbiqDev/neuralSPOT/docs/images/image-20230407163734881.png)

### Pain Points

AutoDeploy was designed to address many common pain points:

- Code Generation Pain
  - Automatically figures out the right Tensor Arena, Resource Variable Arena, and Profile Data sizes
  - Produces a minimal TFLite for Microcontroller instance by determining which Ops are actually needed and only including those
- Model Behavior Mismatches
  - AutoDeploy runs the same model inputs on the PC and EVB, and compares the results, leading to early discovery and easier debugging of behavior differences. Configuration of the model, input/output tensors, and statistics gathering are driven by AutoDeploy using RPC.
- Model Performance Profiling
  - AutoDeploy extends the TFLite for Microcontrollers Profiling to produce detailed reports including per-layer latency, MAC count, and cache and CPU performance statistics.

# Using AutoDeploy

Using AutoDeploy is easy - just give it a TFLite model, connect an EVB, and let it go to work:

```bash
$> cd tools
$> python -m ns_autodeploy --tflite-filename=mymodel.tflite --model-name example_model
```

As part of the process, AutoDeploy generates two ready-to-deploy binary files and the source code used to generate them:

1. `neuralSPOT/projects/models/tflm_validator/` contains a small application that implements the model and uses RPC to validate and profile it.
2. `neuralSPOT/projects/models/example_model` contains a minimal static library that can be linked with applications wishing to invoke the model, and a trivial example of how to do so.