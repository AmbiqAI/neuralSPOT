
# Edge Impulse library for HAR

This is a C++ library that lets you run the impulse for "HAR" (https://studio.edgeimpulse.com/studio/145382) on any device. It consists of the Edge Impulse inferencing SDK - with implementations of both processing and learning blocks - and your model. You will need to include this library in your project to run your impulse locally.

## Getting Started

Please refer to the following documentation to learn how to use this library:

* Deploy your model as a C++ library: https://docs.edgeimpulse.com/docs/deploy-your-model-as-a-c-library
* Running your impulse locally: https://docs.edgeimpulse.com/docs/running-your-impulse-locally-1
* C++ Inference SDK Library API Reference: https://docs.edgeimpulse.com/reference/inferencing-sdk

## Example Projects

For example projects, see 'Running your impulse locally' in the docs (https://docs.edgeimpulse.com/docs/running-your-impulse-locally-1). These pages have examples for virtually any platform under the sun including Linux, Mbed OS, Zephyr, FreeRTOS, and bare-metal on a wide range of platforms. These examples also show how to enable hardware acceleration on Cortex-M, Cortex-A, ARC DSPs and other platforms.

## License

Unless specifically indicated otherwise in a file, files are licensed under the Apache 2.0 license, as can be found in edge-impulse-sdk/LICENSE-apache-2.0.txt. Folders containing files under different permissive license than Apache 2.0 are listed below. Each folder contains its own license specified for its files.

* edge-impulse-sdk/CMSIS - Apache 2.0
* edge-impulse-sdk/dsp/kissfft - BSD-3-Clause
* edge-impulse-sdk/dsp/dct - MIT
* edge-impulse-sdk/tensorflow - Apache 2.0
* edge-impulse-sdk/third_party/flatbuffers - Apache 2.0
* edge-impulse-sdk/third_party/gemmlowp - Apache 2.0
