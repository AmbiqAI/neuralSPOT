# Edge AI at Ambiq

[Ambiq](https://ambiq.com) specializes [in ultra-low-power SoCs](https://ambiq.com/apollo4-plus/) designed to make intelligent battery-powered endpoint solutions a reality. Edge is pervasive, and many edge devices incorporate AI features, including fitness activity recognition, speech-driven user interfaces, audio event detection, health monitoring, and more.

Ambiq's [ultra low power, high-performance](https://github.com/AmbiqAI/MLPerfTiny/blob/main/docs/benchmark_results.md) platforms are ideal for implementing this class of AI features, and we at Ambiq are dedicated to making implementation as easy as possible by offering developer-centric solutions for every stage of AI development.

![devcycle](/Users/carlosmorales/AmbiqDev/neuralSPOT/docs/images/devcycle.png)

## Develop Edge AI Faster with Ambiq

Ambiq offers AI Development Kits and libraries to ease your Edge AI development and accelerate time-to-deploy. These include:

| Component                                                    | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [neuralSPOT Edge](https://github.com/AmbiqAI/neuralspot-edge) | Modular components to simplify training Edge AI models via Keras |
| [PhysioKIT](https://github.com/AmbiqAI/physiokit)            | Python toolkit for processing raw ambulatory bio-signals     |
| [HeartKIT](https://github.com/AmbiqAI/heartkit)              | Heart Health AI SDK                                          |
| SoundKIT                                                     | Speech and Sound AI SDK                                      |
| [SleepKIT](https://github.com/AmbiqAI/sleepkit)              | Sleep Monitoring AI SDK                                      |

## Deploy and Optimize with Ambiq

Once a model is ready to be deployed, Ambiq offers tools to help: neuralSPOT SDK includes firmware and tools to deploy, profile, and optimize Edge AI models on Apollo EVBs.

| Component                                                    | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [neuralSPOT SDK](https://github.com/AmbiqAI/neuralSPOT/tree/main) | Firmware libraries and examples for all Ambiq platforms      |
| [neuralSPOT Autodeploy](https://github.com/AmbiqAI/neuralSPOT/blob/main/tools/autodeploy.readme.md) | Tools to deploy TFLite models onto Ambiq platforms           |
| neuralSPOT Profiler (part of autodeploy)                     | TFLite model execution per-layer firmware analytics          |
| neuralSPOT Analyzer (part of autodeploy)                     | Static analysis of TFLite models to identify unoptimized paths |

## Run Edge AI Faster

Ambiq's version of Tensorflow Lite for Microcontrollers is optimized for Apollo devices, and [is included](https://github.com/AmbiqAI/neuralSPOT/tree/main/extern/tensorflow/ns_tflm_v1_0_0) in neuralSPOT SDK. The optimized kernel source is available to our customers upon request.
