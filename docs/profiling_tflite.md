# Optimizing using Autodeploy - Case Studies

This document offers a quick guide to using neuralSPOT's Autodeploy tool to profile, deploy, and package a TFLite model for Apollo5 platforms.

## Relevant Documents

- [Autodeploy Usage](../tools/README.md) - installation and usage of Autodeploy and other neuralSPOT tools
- [From TF to EVB](./From TF to EVB - testing, profiling, and deploying AI models.md)  - deep dive into Autodeploy
- [Apollo5 and NeuralSPOT](./apollo51evb_quick_guide.md) - Apollo5 NeuralSPOT Quick Guide
- [Optimization Basics](optimizing_using_neuralspot.md) - Very basic power optimization notes

### Basics

Autodeploy is analyzes, profiles, packages, and deploys TFLite model files on Apollo devices. 

#### Installing Autodeploy Dependencies

```
cd ns-mirror
pip install .
```

#### Platform Settings

Autodeploy respects the makefile settings found in `make/local_overrides.mk`. For example, to set Apollo5 RevB EB as a target, TFLM version, and SDK3 as the AmbiqSuite version, you would add the following to the file:

```make
PLATFORM := apollo5b_eb_revb
AS_VERSION := Apollo510_SDK3_2024_09_14
TF_VERSION := Oct_08_2024_e86d97b6
```

#### Getting the Hardware Ready

Autodeploy uses RPC-over-USB to communicate with the device. Optionally, it can use a joulescope to automatically measure power - this requires GPIO connections between the EB/EVB and the Joulescope.

For EBs: one USB connection needs to be connected to PC

For EVB: both USB connections must be connected to PC

#### Running Autodeploy

```
python -m ns_autodeploy --model-name denoise --tflite-filename denoise.tflite
```

Autodeploy will then run for a while, generating numerous reports, source packages, and binaries.

## Case Study - ECG Denoiser

We'll use Ambiq's ECG Denoising model to illustrate how we use Autodeploy as part of our development process. The `denoise.tflite` file must be compatible with TFLite for Microcontrollers (TFLM), though Autodeploy will print out useful(ish) errors if this is not the case.

#### Fitting on the Device

Models require somewhere to store the model weights, and an arena in which to place activations and other tensors/state. We prefer placing these in the 'smallest place possible'. In the case of this model, both the weights (22kB) and arena (20kB) fit within the large DTCM (data tightly-coupled-memory) on the Apollo510. Autodeploy allows the placement of weights and arena in TCM (default), SRAM, MRAM (for weights), and PSRAM. Here, we'll leave the defaults as-is, but the MobilenetV3 scenario requires memory placement settings.

#### Results

Autodeploy will produce:

- A per-layer profile
- High resolution power and performance measurements
- An minimal example which can be linked into non-neuralSPOT codebases
- A minimal AmbiqSuite example which can be compiled from within AmbiqSuite.

Here, will focus on the first two.

#### Per-Layer Profile

| Event | Tag               | uSeconds | Est MACs | ARM_PMU_MVE_INST_RETIRED | ARM_PMU_MVE_INT_MAC_RETIRED | ARM_PMU_INST_RETIRED | ARM_PMU_BUS_CYCLES |
| ----- | ----------------- | -------- | -------- | ------------------------ | --------------------------- | -------------------- | ------------------ |
| 0     | RESHAPE           | 8        | 0        | 2                        | 0                           | 395                  | 1124               |
| 1     | DEPTHWISE_CONV_2D | 300      | 1792     | 14350                    | 2112                        | 57151                | 74782              |
| 2     | DEPTHWISE_CONV_2D | 290      | 1792     | 14350                    | 2112                        | 57153                | 72199              |
| 3     | CONV_2D           | 224      | 2048     | 12813                    | 2560                        | 35451                | 55559              |
| 4     | DEPTHWISE_CONV_2D | 351      | 14336    | 24782                    | 4224                        | 70854                | 87128              |
| 5     | MEAN              | 1399     | 0        | 2                        | 0                           | 261755               | 348604             |
| 6     | CONV_2D           | 6        | 32       | 49                       | 5                           | 609                  | 1281               |
| 7     | CONV_2D           | 5        | 32       | 85                       | 10                          | 730                  | 1079               |
| 8     | MINIMUM           | 13       | 0        | 10                       | 0                           | 1260                 | 3466               |
| 9     | RELU              | 7        | 0        | 2                        | 0                           | 700                  | 1384               |
| 10    | MUL               | 662      | 0        | 6                        | 0                           | 138755               | 164742             |
| 11    | CONV_2D           | 367      | 32768    | 25613                    | 5120                        | 67338                | 91408              |
| 12    | DEPTHWISE_CONV_2D | 4339     | 28672    | 12                       | 0                           | 873508               | 1081163            |
| 13    | MEAN              | 2765     | 0        | 2                        | 0                           | 520042               | 689073             |
| 14    | CONV_2D           | 5        | 128      | 85                       | 10                          | 730                  | 1078               |
| 15    | CONV_2D           | 6        | 128      | 157                      | 20                          | 972                  | 1348               |
| 16    | MINIMUM           | 12       | 0        | 10                       | 0                           | 1756                 | 2734               |
| 17    | RELU              | 7        | 0        | 2                        | 0                           | 1124                 | 1447               |
| 18    | MUL               | 1272     | 0        | 6                        | 0                           | 268353               | 316995             |
| 19    | CONV_2D           | 538      | 98304    | 38413                    | 7680                        | 99356                | 134023             |
| 20    | DEPTHWISE_CONV_2D | 6442     | 43008    | 12                       | 0                           | 1299717              | 1605489            |
| 21    | MEAN              | 4136     | 0        | 2                        | 0                           | 778197               | 1030730            |
| 22    | CONV_2D           | 6        | 288      | 169                      | 27                          | 899                  | 1276               |
| 23    | CONV_2D           | 8        | 288      | 229                      | 30                          | 1214                 | 1619               |
| 24    | MINIMUM           | 14       | 0        | 10                       | 0                           | 2252                 | 3259               |
| 25    | RELU              | 8        | 0        | 2                        | 0                           | 1508                 | 1810               |
| 26    | MUL               | 1893     | 0        | 6                        | 0                           | 398078               | 471465             |
| 27    | CONV_2D           | 816      | 196608   | 71693                    | 18432                       | 152251               | 203093             |
| 28    | CONV_2D           | 85       | 57344    | 10154                    | 3646                        | 16962                | 21124              |
| 29    | RESHAPE           | 3        | 0        | 2                        | 0                           | 400                  | 627                |

This characterization table is exported as a CSV for further analysis. Just 'eyeballing' it, we can see that MVE events roughly track estimated macs per layer, though in some cases the MVE operations are much lower than the MAC operations, suggesting good targets for TFLM optimization

Of special interest, however, are layers 12 and 20. Here, there are almost no MVE operations, and these are pretty 'chunky' layers. Deeper inspection of the model reveals that these layers use `dilation`, as opposed to other depthwise convolution layers in this model.

The data scientist can now use this information to either slightly alter the neural architecture (using '0's between the filters, resulting in a larger model but possibly faster computation), looking into why TFLM isn't using MVE here, or re-architecting the model to not use dilation.

#### Power and Performance

If a joulescope is wired up, Autodeploy can use it to provide power and performance measurements by adding `--joulescope` to the command line

```
python -m ns_autodeploy --model-name denoise --tflite-filename denoise.tflite --joulescope
```

## Case Study - Mobilenet V3 Small

Mobilenet is an off-the-shelf model that is often adapted to image classification tasks. It is quite large for an edge model, so we'll use it to illustrate how to run large models.

A priori, we only know the model size (2333232kB), which can be guessed from the size of the file, but which Autodeploy can exactly determine via analysis of the network. 

The arena size is a different matter - its size is only known when the model's tensors are allocated by TFLM. This means we have to 'guess' at an arena size, and hope TFLM doesn't fail at instantiation time. This is less than ideal, but the experiments are fast, and it usually only takes a couple of iterations to find the right size.

> **Note** This guess is only needed the first time autodeploy is run on a model. Autodeploy will record the actual size after the first successful run, and use that for subsquent runs.

Since we know that the model is too large to fit in TCM, let's run an experiment placing it in MRAM. Since we can guess that large models need large activation memories, we'll try putting the arena in SRAM and making it 400kB (the default is 100kB).

```
python -m ns_autodeploy --model-name mnv3sm --tflite-filename mobilenet_v3_sm_a075_quant.tflite --model-location MRAM --arena-location SRAM --max-arena-size 400
```

Spoiler: this experiment will fail. Autodeploy will detect that TFLM wasn't able to allocate and suggest that you increase the max-arena-size. Let's max out the SRAM and see what happens.

```
python -m ns_autodeploy --model-name mnv3sm --tflite-filename mobilenet_v3_sm_a075_quant.tflite --model-location MRAM --arena-location SRAM --max-arena-size 2200
```

A minute or so later we can see that the arena size was large enough, and Autodeploy is running everything else with the detected arena size (535kB).

#### Per Layer Profiling

We'll skip this for MobilenetV3 because of its size. Autodeploy does export all 115 layers with MACs, PMU, and cycle time details which can be subsequently analyzed.

#### Detailed Optimization

For this model, we used the automatic Joulescope instrumentation to measure power and timing details, and we ran the same model in both high performance and low power modes, using GCC13 and Armclang compilers.

| mobilenet_v3_sm_a075_quant.tflite | AP510 | RevB PCM1.0 | EVB  | GCC13    | 8-Oct | 2333232 | 535  | MRAM | SRAM | 43399344 | 849.883 | 21708.563 | 25.543 | 2182.874 | 17120.087 | 7.843 |
| --------------------------------- | ----- | ----------- | ---- | -------- | ----- | ------- | ---- | ---- | ---- | -------- | ------- | --------- | ------ | -------- | --------- | ----- |
| mobilenet_v3_sm_a075_quant.tflite | AP510 | RevB PCM1.0 | EVB  | Armclang | 8-Oct | 2333232 | 535  | MRAM | SRAM | 43399344 | 800.151 | 20395     | 25.489 | 2045.89  | 16433     | 8.03  |

Here, we can see an interesting result: Armclang produces 5% faster code. We find this is true across multiple mode architectures.

> **Note** Autodeploy doesn't make 'special efforts' to minimize the power - the numbers it measures are for model optimization, and what matters there is relative performance. Data scientists may want to know how much power different models use in an apples-to-apples situation, but they'll leave the deep power optimization to the platform experts. That being said, neuralSPOT does have many power optimization tools that are not leveraged by Autodeploy.