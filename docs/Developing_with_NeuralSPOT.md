# Developing with neuralSPOT

The neuralSPOT AI SDK is a collection of software libraries, tools, and examples designed to help developers create AI features for Ambiq platforms. It is open-source, OS-agnostic, and supports most Ambiq devices.

neuralSPOT is intended to be used on Ambiq platform evaluation boards (EVBs), and is primarily for developers that already have an AI model and want to deploy and refine it on Ambiq devices.

Some example use-cases:

1. **Create a demonstration application**: neuralSPOT's libraries make creating complex AI demos easier by taking care of the 'hard stuff' such as power configuration, memory layout, and peripheral (BLE, USB, Audio, and more) configuration. Building a demo around a new model can be done in hours instead of weeks.
2. **Develop, characterize, and refine a Tensorflow Lite model**: neuralSPOT includes tools to automatically analyze your TFLite model and create all the necessary code to instantiate it on Ambiq platforms. Once deployed, those same tools can characterize the model's performance and power utilization, and finally export it as a minimal static library or AmbiqSuite example.
3. **Develop and debug sensor feature extraction**: processing sensor data for use by the model (aka 'feature extraction') is a critical (and error prone) part of AI development. neuralSPOT's RPC allow you to calculate features on a laptop using Python and compare the results with the firmware version's output. This is usually a manual and tedious process.

Each of these use cases entails a different development flow - neuralSPOT accomodates this by offering various ways to develop:

1. **neuralSPOT as an application layer**: neuralSPOT can be used as an application development environment where its easy-to-use libraries, easy tensorflow integration, an rich I/O are leveraged to produce applications running on the EVB.
2. **neuralSPOT as a characterization and debug tool**: neuralSPOT's remote procedure call (RPC) support makes it easy to develop AI in a hybrid environment, where part of the code is running on Python on a PC and the rest is running on the EVB.
3. **neuralSPOT as a deployment tool**: beyond simple compilation and flashing of applications, neuralSPOT offers 3 primary ways to export entire applications or minimal AI model instantiations into other projects: [Nests](NeuralSPOT_Nests.md), minimal static libraries, and ambiqsuite examples.

Each of these flows is designed to work with the other flows - for example, you can fine tune a model, then export it to an example for demo development, then finally export the whole thing as a self-contained application ready to be integrated into a larger project.
