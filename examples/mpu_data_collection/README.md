# MPU Data Capture
RPC, or remote procedure call, is a way for the code running on the EVB to call functions that reside on the laptop. It does so by communicating over a transport layer - in our case, we use serial-over-USB, where the 'usb' is the second USB port on the EVB.

It's a client/server system needing some careful staging, described below.

## Installation and Setup
You'll need to install some laptop-side software, including ERPC and it's python library.
1. Clone our [erpc fork](https://github.com/AmbiqAI/erpc)
2. Make erpc ([instructions here](https://github.com/AmbiqAI/erpc/blob/develop/README.md#building-and-installing))
3. Install erpc_python ([instructions here](https://github.com/AmbiqAI/erpc/tree/develop/erpc_python))
4. Connect an MPU6050 to the EVB ([instructions here](../../neuralspot/ns-i2c/README.md))

## Capturing Data
1. Compile and flash the NeuralSPOT mpu_data_collection example
2. Connect the second USB cable to your laptop - you'll now have 2 USB connections between the EVB and the laptop
    1. Monitor the EVB SWO printout - you should see "Press Button 0 to begin calibration"
    2. The second connection will mount as a USB TTY device. On a Mac, it'll look something like `/dev/tty.usbmodem1234561`, on PC it'll be `COMx` or similar.
    3. Look for the USB TTY device - if it doesn't pop up, there is a problem. It won't show up until "Press Button" shows up, so make sure you got that far.
3. Start the laptop-side RPC server. It should say "Wait for client to send a eRPC request"
4. Press EVB button 0

After calibration, it'll dump data until your stop the server (ctrl-c on laptop, or reset button on EVB).

### Details

#### Building mpu_data_collection

```bash
$> cd .../neuralSPOT
$> make
$> make TARGET=mpu_data_collection deploy
```

#### Running Client

Requires Python 3.6+, and I highly recommend using a venv.

```bash
$> cd neuralSPOT/neuralspot/ns-rpc/python/ns-rpc-genericdata
$> python -m generic_data -t /dev/tty.usbmodem1234561 -o myfile.csv -m server
Server started - waiting for client to send a eRPC request
```

The CSV contains 1 row per sample, each row with 7 float values, and 200 rows per second: `ax, ay, az, gx, gy, gz, temp`
