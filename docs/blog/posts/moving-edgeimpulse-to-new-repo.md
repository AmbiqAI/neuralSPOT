---
draft: false
date: 2023-10-20
categories:
  - Examples
  - Integration
tags:
  - audio
  - time series
  - anomaly detection
---

# NeuralSPOT's EdgeImpulse Examples Have a New Home
We've moved our EdgeImpulse examples to their [own repo](https://github.com/AmbiqAI/edgeimpulse-examples).

NeuralSPOT has long included a set of [EdgeImpulse](https://edgeimpulse.com) examples, including [anomaly detection](https://github.com/AmbiqAI/edgeimpulse-examples/tree/main/anomaly_detection) and [audio](https://github.com/AmbiqAI/edgeimpulse-examples/tree/main/hello_ambiq) use cases. Previously these were located in neuralSPOT's `project` directory because EdgeImpulse's C++ blob has its own tensorflow and CMSIS instances which conflict with neuralSPOT.

The examples were created using neuralSPOT's [Nest makefile capability](../../NeuralSPOT_Nests.md) along with neuralSPOT-specific [porting code](https://github.com/AmbiqAI/edgeimpulse-examples/tree/main/src) - the process is [documented here](../../Deploying-EI-Models-using-neuralSPOT Nests.md), and is a good example of how to integrate neuralSPOT applications with external code using Nests.

These example directories were getting a little big, and include a *lot* of small and replicated source files, so we decided to give them their [own repository](https://github.com/AmbiqAI/edgeimpulse-examples). The examples are standalone, and can be compiled without neuralSPOT (they have all the neuralSPOT components they need, exported via Nests). The porting code now exists in two places - the original projects location (for ease of development of new examples) and the new repo (to make it standalone).