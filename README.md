# neuralSPOT

![neuralSPOT](./docs/images/neuralspot-banner.png#gh-light-mode-only)
![neuralSPOT](./docs/images/neuralspot-banner-dark.png#gh-dark-mode-only)

[![Docs](https://github.com/AmbiqAI/neuralSPOT/actions/workflows/docs.yaml/badge.svg?branch=main)](https://github.com/AmbiqAI/neuralSPOT/actions/workflows/docs.yaml)

## Use neuralSPOT-X

[neuralSPOT-X](https://github.com/AmbiqAI/neuralspotx) is the successor to neuralSPOT. It is Ambiq's modular AI SDK: you compose reusable firmware modules, choose a toolchain and build AI applications for Ambiq silicon.

- [Documentation](https://ambiqai.github.io/neuralspotx/)
- [Getting started](https://ambiqai.github.io/neuralspotx/getting-started/)
- [Migrating from neuralSPOT](https://ambiqai.github.io/neuralspotx/getting-started/migrate-from-neuralspot/)
- [Source code](https://github.com/AmbiqAI/neuralspotx)

## neuralSPOT is in maintenance

neuralSPOT is no longer under active development. For new work, use neuralSPOT-X and the HELIA tools listed below. Security fixes may still be released; see the [releases](https://github.com/AmbiqAI/neuralSPOT/releases).

## Where things moved

| If you used | Go to |
|---|---|
| neuralSPOT libraries, examples and build system | [neuralSPOT-X](https://ambiqai.github.io/neuralspotx/) and its [migration guide](https://ambiqai.github.io/neuralspotx/getting-started/migrate-from-neuralspot/) |
| AutoDeploy for model profiling and power measurement | [heliaPROFILER](https://ambiqai.github.io/helia-profiler/) |
| AutoDeploy static library export and model code generation | [heliaAOT](https://ambiqai.github.io/helia-aot/) |
| The TFLM runtime | [heliaRT](https://ambiqai.github.io/helia-rt/) |
| Feature extraction (MFCC, filters, FFT) | [heliaDSP](https://ambiqai.github.io/helia-dsp/) |
| Audio demos (speech enhancement, keyword spotting, speaker ID) | [soundKIT](https://ambiqai.github.io/soundkit/) |
| Web dashboards for EVB data | [Tileio](https://ambiqai.github.io/tileio/) |

## Legacy neuralSPOT

The neuralSPOT source and its documentation remain in this repository; the documentation source is under [docs](./docs). The [v1.4.1 tag](https://github.com/AmbiqAI/neuralSPOT/tree/v1.4.1) has the code and documentation as released in 1.4.1. The [neuralSPOT site](https://ambiqai.github.io/neuralSPOT) no longer publishes the old documentation; links to old pages redirect to their replacement or to the site home.
