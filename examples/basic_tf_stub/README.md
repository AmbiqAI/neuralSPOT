# Basic TF Stub
This example uses the Apollo4's low voltage AUDADC analog microphone interface to collect audio. Once collected, it processes the audio by extracting melscale spectograms, and passes those to a Tensorflow Lite for Microcontrollers model for inference. After invoking the model, this example processes the result and prints it out on the SWO debug interface. Optionally, it will dump the collected audio to a PC via a USB cable.

Along the way, this example uses many neuralSPOT features, including:
1. ns-audio paired with the AUDADC driver to collect audio
2. ns-ipc to use a ringbuffer to pass the audio to example application
3. ns-mfcc to compute the mel spectogram
4. ns-rpc and ns-usb to establish a remote procedure call interface to the development PC over a USB cable
5. ns-power to easily set efficient power modes
6. ns-peripherals to read the EVB buttons
7. ns-utils to provide energy measurement tools, malloc and timers for RPC

The code is structured to break out how these features are initialized and used - for example 'basic_mfcc.h' contains the init config structures needed to configure MFCC for this model.

See our [walkthrough](../../docs/Understanding-neuralSPOT-via-the-Basic-Tensorflow-Example.md) for more details.

>*NOTE* This example is used as the seed for a NeuralSPOT Deployment Nest - it includes all the NeuralSpot libraries and is based on TF.

# Building and Running the Example
This is the primary neuralSPOT example, so the makefile defaults many of its options to it.

```bash
$> cd .../neuralSPOT
$> make clean
$> make -j # makes everything
$> make deploy # flashes basic_tf_stub to the target EVB
$> make view # connects to EVB SWO via SEGGER Jlink
```
Note that 'make' without any options builds every example - you don't have to do anything special. 'make deploy' and 'make view' only apply to a single target, which is basic_tf_stub by default.

You'll need to connect an audio source to the EVB's 3.5mm audio plug. I find that powered microphones work best - I use RODE WirelessGO with good results.

Once everything is running, the application will prompt you to press Button0 on the EVB to start listening. If you've enabled RPC, there will be an additional prompt.

# Audio Data Capture using NS-RPC
RPC, or remote procedure call, is a way for the code running on the EVB to call functions that reside on the laptop. It does so by communicating over a transport layer - in our case, we use serial-over-USB, where the 'usb' is the second USB port on the EVB.

It's a client/server system needing some careful staging, described below.

## Installation and Setup
You'll need to install some laptop-side software, including ERPC's python library.

> *NOTE* for Windows, see our [Windows eRPC application note](../../docs/Application-Note-neuralSPOT-and-Windows.md)

1. Clone our erpc fork (git@github.com:AmbiqAI/erpc.git)
2. Install erpc_python (instructions here: https://github.com/AmbiqAI/erpc/tree/develop/erpc_python)
3. Install Python Libraries needed for our example Python application - see [here](../../neuralspot/ns-rpc/README.md) for a how to run it for the first time.

## Capturing Data
1. Compile and flash the NeuralSPOT basic_tf_hub example as described above. *NOTE* RPC must be enabled at compile time (look for `// #define RPC_ENABLED` in `basic_tf_stub.cc` and uncomment it).
2. Connect the second USB cable to your laptop - you'll now have 2 USB connections between the EVB and the laptop.
    1. Monitor the EVB SWO printout - you should see `Start the PC-side server, then press Button 0 to get started`.
    2. The second connection will mount as a USB TTY device. On a Mac, it'll look something like `/dev/tty.usbmodem1234561`, on PC it'll be `COMx` or similar.
    3. Look for the USB TTY device - if it doesn't pop up, there is a problem. It won't show up until "Press Button" shows up, so make sure you got that far.
3. Start the laptop-side RPC server. It should say "Wait for client to send a eRPC request"
4. Press the EVB button
5. The SWO interface will now say `Press Button 0 to start listening...`

After capturing 3 seconds, it'll dump the sample to the server. Note that the server will append the audio to a file if it already existed.

### Running the Server

Requires Python 3.6+, and the use of a virtual python environment (venv) is highly recommended.

```bash
$> cd .../neuralSPOT/ns-rpc/python/ns-rpc-genericdata
$> python -m generic_data -t /dev/tty.usbmodem1234561 -o myaudio.wav -m server # replace the /dev... with device from 2.2 above
```

The `generic_data.py` example listens on the device you specify for eRPC data dump calls, and saves them to a wav file you can listen to. The audio is
dumped to the script as a single channel stream of 16bit PCM values. This script also listens for time series data, and is an example of how you can use RPC to handle multiple types of datastreams.

In `neuralSPOT/example/basic_tf_stub/src/basic_tf_stub.cc`, the line that dumps the sample is `ns_rpc_data_sendBlockToPC(&outBlock)` which looks exactly like a function call (and the server also treats it like one, which is the magic of RPC).
