# AI and Ambiq

Ambiq specializes in ultra-low-power SoC's designed to make intelligent battery-powered endpoint solutions a reality. These days, just about every endpoint device incorporates AI features, including anomaly detection, speech-driven user interfaces, audio event detection and classification, and health monitoring.

Ambiq's [ultra low power, high-performance](https://github.com/AmbiqAI/MLPerfTiny/blob/main/docs/benchmark_results.md) platforms are ideal for implementing this class of AI features, and we at Ambiq are dedicated to making implementation as easy as possible by offering developer-centric toolkits, software libraries, and reference models to accelerate AI feature development.

## neuralSPOT - because AI is hard enough

neuralSPOT is an AI developer-focused SDK in the true sense of the word: it includes everything you need to get your AI model onto Ambiq’s platform. You’ll find libraries for talking to sensors, managing SoC peripherals, and controlling power and memory configurations, along with tools for easily debugging your model from your laptop or PC, and examples that tie it all together.

<img src="./images/image-20220927160935318.png" alt="you-are-here" style="zoom:40%;" />

For an in-depth exploration of how neuralSPOT can supercharge your AI development team, see our [white paper](./Introduction-to-neuralSPOT.md) and visit our [GitHub repository](https://github.com/AmbiqAI/neuralSPOT).

## Ambiq ModelZoo

Ambiq's ModelZoo is a collection of AI reference models built on neuralSPOT to help your team bootstrap AI model development and deployment on Apollo4 Plus. It includes open source models for speech interfaces, speech enhancement, and ECG analysis, with everything you need to reproduce our results and train your own models.

| Model                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| Voice Activity Detector                                      | A low power model that detects when speech is found in audio |
| Keyword Detection                                            | Our implementation of 'ok computer'-style trigger word detection |
| Command Words                                                | A speech-to-intent speech classifier                         |
| [Speech Enhancement](https://github.com/AmbiqAI/speech-enhancement) | A TinyLSTM-based audio model which removes noise from speech |
| [Arrhythmia Classification](https://github.com/AmbiqAI/ecg-arrhythmia) | Detect several types of heart conditions based on single-lead ECG sensors |

## Ambiq AI Benchmarks

We've benchmarked our Apollo4 Plus platform with outstanding results. Our [MLPerf-based benchmarks](https://github.com/AmbiqAI/MLPerfTiny) can be found on our benchmark repository, including instructions on how to replicate our results.

![Our results](./images/results.png)

## Accelerate Your AI Development With Ambiq's AI Tools

Whether you are creating a model from scratch, porting a model to Ambiq's platform, or optimizing your crown-jewels, Ambiq has tools to ease your journey.

