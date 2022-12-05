# RPC Generic Data Operations Client Example
RPC, or remote procedure calls, is a way for code running on two different CPUs (in this case, a PC and an EVB connected via USB) to call procedures between each other almost as if the procedure was running locally. It does so by communicating over a transport layer - in our case, we use serial-over-USB over second USB port on the EVB.

The `rpc-client.cc` example shows how to use `ns-rpc` when the EVB is a *client* and the PC is the *server*. In this configuration, the EVB calls remote procedures and the PC executes those procedures . This example is designed to work with `generic_data.py` (part of `ns-rpc`), a python script running on the PC that can be run as server or client.

```mermaid
sequenceDiagram
    participant EVB
    participant PC
    note over EVB: client
    note over PC: server
    EVB->>+PC: ns_rpc_data_remotePrintOnPC(string)
    note right of PC: generic_data.py prints<br/> string on terminal;
    PC->>-EVB: ns_rpc_data_success;
    EVB->>+PC: ns_rpc_data_sendBlockToPC(&outBlock);
    Note right of PC: generic_data.py converts sample<br>  in dataBlock to WAV and <br/>appends to wav file;
    PC->>-EVB: ns_rpc_data_success
    EVB->>+PC: ns_rpc_data_computeOnPC(&outBlock, &resultBlock)
    Note right of PC: generic_data.py consumes <br/> dataBlock and returns mocked<br/> up resultBlock
    PC->>-EVB: ns_rpc_data_success
```



We demonstrate the following EVB->PC procedure calls:

- `ns_rpc_data_remotePrintOnPC("Hello World\n")`: Calls ns_rpc_data_remotePrintOnPC() on the PC, which is implemented in python. The example server simply prints the string to the terminal.
- `ns_rpc_data_sendBlockToPC(&outBlock)`: Passes a generic dataBlock to the python server. `dataBlock_t` is defined as part of the GenericDataOperations RPC interface. The example server takes this block, converts it from 16 bit PCM to WAV, and saves it to a file.
- `ns_rpc_data_computeOnPC(&computeBlock, &resultBlock)`: Passes a dataBlock to the server, and gets a different dataBlock in return. The two do not have to be the same shape or data type. This RPC is intended for remote computation such as calculating MFCC remotely, or testing the EVB's inference results versus the remote results. The example populates a simple 4 byte return dataBlock and returns it, whereupon the EVB prints the `dataBlock.description` field out to the SWO terminal.

It's a client/server system needing some careful staging, described below.

## Installation and Setup
You'll need to install some laptop-side software, including ERPC's python library.

> *NOTE* for Windows, see our [Windows eRPC application note](../../docs/Application-Note-neuralSPOT-and-Windows.md)

1. Clone our erpc fork (git@github.com:AmbiqAI/erpc.git)
2. Install erpc_python (instructions here: https://github.com/AmbiqAI/erpc/tree/develop/erpc_python)
3. Install Python Libraries needed for our example Python application - see [here](../../neuralspot/ns-rpc/README.md) for a how to run it for the first time.

## Running the RPC-client example
Running RPC requires a bit of staging. The PC-side server cant start until the USB TTY interface
shows up as a device, and that doesn't happen until we start servicing TinyUSB, and even then it doesn't happen immediately.

To address this, rpc-client loop swaiting for a button press, servicing USB. This gives the user a chance to start the server then
pressing the button to let the EVB it is ready to start RPCing.

1. Connect the second USB cable to your laptop - you'll now have 2 USB connections between the EVB and the laptop
2. Compile and flash the NeuralSPOT `rpc-client` example
3. Run the `rpc-client` EVB code (it should be running once you flashed, unless you have something like GDB interrupting execution)
4. Monitor the EVB SWO printout - you should see "Start the PC-side server, then press Button 0 to get started"
5. At this point, the second USB connection should come alive and mount as a USB TTY device. On a Mac, it'll look something like `/dev/tty.usbmodem1234561`, on a windows PC it'll be `COMx` or similar. If the device doesn't pop up, there is a problem. It won't show up until "Press Button" shows up, so make sure you got that far.
6. Start the laptop-side RPC server:
```bash
$> cd neuralSPOT/neuralspot/ns-rpc/python/ns-rpc-genericdata
$> python -m generic_data -t /dev/tty.usbmodem1234561 -o myaudio.wav -m server
Server started - waiting for client to send a eRPC request
```
If all went well, you can press Button 0, which will record a 3 second audio sample and send it to the PC over the ERPC USB interface. The server will write it out to the data file you specified (in this case, `myaudio.wav`).
