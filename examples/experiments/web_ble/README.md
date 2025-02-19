# Web BLE Example

This example instantiates a BLE Service using ns-ble, [NeuralSPOT's simplified BLE library](../../neuralspot/ns-ble/README.md).

It is designed to connect with a [WebBLE](https://developer.chrome.com/articles/bluetooth/) dashboard - for convenience, it using a pre-existing dashboard located here. This dashboards expects a service with 10 characteristics and specific UUIDs. The characteristics include read-only data sources such as Temperature and Co2, subscription (notify) characteristics such as Acceleration, and a read/write characteristic.

This example 'mocks up' this data, sending various forms of sines and cosines to the dashboard for display.

### Requirements

- A Bluetooth-capable Ambiq EVB
- A PC or Laptop with Bluetooth
- A WebBLE-compatible browser such as Chrome

## Running the example

1. Compile and deploy the example
2. Click here to launch the dashboard (give the browser BLE permissions if needed)
3. Pair the dashboard to the EVB's BLE server

### Compile and Deploy

```bash
.../neuralSPOT> make clean &&
.../neuralSPOT> make -j EVB=blue_kbr_evb # or kxr, depending on EVB
.../neuralSPOT> make TARGET=web_ble_example deploy
```

### Launch Dashboard

Simply click [here](https://arduino.github.io/ArduinoAI/NiclaSenseME-dashboard/). The javascript code for the dashboard can be found [here](https://github.com/arduino/ArduinoAI/blob/main/NiclaSenseME-dashboard/index.html).

### Pair

The EVB will show up as a Cordio device. On the dashboard, click the 'Connect' button near the top:

<img src="../../docs/images/image-20230706130007268.png" alt="image-20230706130007268" style="zoom:50%;" />

Select the 'Cordio' device, then click 'Pair'.

<img src="../../docs/images/image-20230706130129910.png" alt="image-20230706130129910" style="zoom:50%;" />

If all goes well, you should see something like this:

<img src="../../docs/images/image-20230706130220263.png" alt="image-20230706130220263" style="zoom:50%;" />

(pay no attention the the Gas Value - NaN error, this is a JS bug in the dashboard code).
