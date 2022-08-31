# Basic TF Stub
This example is used the seed for a NeuralSPOT Deployment Nest - it includes all the NeuralSpot libraries and is based on TF.

# Audio Data Capture
Everything is messy right now... see https://github.com/orgs/AmbiqAI/projects/2/views/1 for current issue backload. 

RPC, or remote procedure call, is a way for the code running on the EVB to call functions that reside on the laptop. It does so by communicating over a transport layer - in our case, we use serial-over-USB, where the 'usb' is the second USB port on the EVB.

It's a client/server system needing some careful staging, described below.

## Installation and Setup
You'll need to install some laptop-side software, including ERPC and it's python library.
1. Clone our erpc fork (git@github.com:AmbiqAI/erpc.git)
2. Make erpc (instructions here: https://github.com/AmbiqAI/erpc/blob/develop/README.md#building-and-installing)
3. Install erpc_python (instructions here: https://github.com/AmbiqAI/erpc/tree/develop/erpc_python)

## Capturing Data
1. Compile and flash the NeuralSPOT basic_tf_hub example
2. Connect the second USB cable to your laptop - you'll now have 2 USB connections between the EVB and the laptop.
    1. Monitor the EVB SWO printout - you should see "Press button to start listening..."
    2. The second connection will mount as a USB TTY device. On a Mac, it'll look something like `/dev/tty.usbmodem1234561`, on PC it'll be `COMx` or similar.
    3. Look for the USB TTY device - if it doesn't pop up, there is a problem. It won't show up until "Press Button" shows up, so make sure you got that far.

3. Start the laptop-side RPC server. It should say "Wait for client to send a eRPC request"
4. Press the EVB button

After capturing 3 seconds, it'll dump the sample to the server.

### Details

Building basic_tf_stub

```bash
$> cd .../neuralSPOT
$> make # or make BOARD=apollo4b if you're using one of those
$> make deploy # basic_tf_stub is the default for neuralSPOT deploy
```

Running Client

Requires Python 3.6+, and I highly recommend using a venv.

```bash
$> cd .../neuralSPOT/ns-rpc/python/audio 
$> python -m audio -t /dev/tty.usbmodem1234561 -o mywave.wav # replace the /dev... with device from 2.2 above
```

The `audio.py` example listens on the device you specify for eRPC audio dump calls, and saves them to a wav file you can listen to. The audio is 
dumped to the script as a single channel stream of 16bit PCM values.

In `neuralSPOT/example/basic_tf_stub/src/main.cc`, the line that dumps the sample is `ns_rpc_audio_send_buffer((uint8_t*)g_in16AudioDataBuffer, SAMPLES_IN_FRAME * sizeof(int16_t));`.

Since this is RPC-over-USB, we have to 'maintain' the USB connection, which means periodically calling `tud_task()` - main.cc calls it from the main task loop. There is also some init code needed (`ns_rpc_audio_init()`).



