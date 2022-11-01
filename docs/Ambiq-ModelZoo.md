# Ambiq ModelZoo

Models are the heart of AI - they're the bit with the intelligence in it. 

AI model development follows a lifecycle - first, the data that will be used to train the model must be collected and prepared, second, the model is 'trained' on that data, and finally the trained model is compressed and deployed to the endpoint devices where they'll be put to work. Each one of these phases requires significant development and engineering.

Ambiq's ModelZoo is a collection of open source endpoint AI models packaged with all the tools needed to develop the model from scratch. It is designed to be a launching point for creating customized, production-quality models fine tuned to your needs.

Each model's repository includes:

- **Data preparation scripts** which help you collect the data you need, put it into the right shape, and perform any feature extraction or other pre-processing needed before it is used to train the model. 
- **Training scripts** that specify the model architecture, train the model, and in some cases, perform training-aware model compression such as quantization and pruning
- **Inference scripts** to test the resulting model and **conversion scripts** that export it into something that can be deployed on Ambiq's hardware platforms.
- **Jupyter Notebooks** and documentation helping navigate all of the above.
- Where possible, **pre-trained deployable models**, and **power-optimized firmware** connecting to Apollo4's sensors and peripherals.

## The Models

Built on top of neuralSPOT, our models take advantage of the Apollo4 family's amazing power efficiency to accomplish common, practical endpoint AI tasks such as speech processing and health monitoring. 

### Neural Network Speech

This real-time model is actually a collection of 3 separate models that work together to implement a speech-based user interface. The Voice Activity Detector is small, efficient model that listens for speech, and ignores everything else. When it detects speech, it 'wakes up' the keyword spotter that listens for a specific keyphrase that tells the devices that it is being addressed. If the keyword is spotted, the rest of the phrase is decoded by the speech-to-intent. model, which infers the intent of the user.

### Heart Arrhythmia Classification

This real-time model analyses the signal from a single-lead ECG sensor to detect irregular heartbeats ('AFIB arrhythmia'). The model is designed to be able to detect other types of anomalies such as atrial flutter, and will be continuously extended and improved.

> *NOTE* This is demonstration model only, not intended for medical applications.

### Speech De-Noise (COMING SOON)

This real-time model processes audio containing speech, and removes non-speech noise to better isolate the main speaker's voice. The approach taken in this implementation closely mimics that described in the paper [TinyLSTMs: Efficient Neural Speech Enhancement for Hearing Aids](https://arxiv.org/abs/2005.11138) by Federov et al. Namely, a small recurrent neural network is employed to learn a denoising mask that is multiplied with the original noisy input to produce denoised output.

## A Word About Datasets

Our models are trained using publicly available datasets, each having different licensing constraints and requirements. Many of these datasets are free (or very cheap) to use for non-commercial purposes such as development and research, but restrict commercial use. Since trained models are at least partially derived from the dataset, these restrictions apply to them.

Where possible, our ModelZoo include the pre-trained model. If dataset licenses prevent that, the scripts and documentation walk through the process of acquiring the dataset and training the model.