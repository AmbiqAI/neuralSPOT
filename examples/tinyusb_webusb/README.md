# TinyUSB WebUSB

This example demonstrates how to use the USB VENDOR device class to enable webUSB. The demo also enables the USB CDC-ACM device class. All data received is echoed back to both Serial and webUSB endpoints.

## Setup

For this example connect both of the EVB USB ports to the host computer.

## Build

```bash
$> cd .../neuralSPOT
$> make
$> make TARGET=tinyusb_webusb deploy
```

## Run

This example demonstrates webUSB as web serial with browser with webUSB support (e.g Chrome). Open a browser with webUSB support (e.g. Chrome) and navigate to [https://example.tinyusb.org/webusb-serial/](https://example.tinyusb.org/webusb-serial/).

* Click "Connect" and select target device.
* Once connected the on-board LED will turn on.
* Any characters received from either webUSB/Serial will be echo back to webUSB and Serial.

> Notes:
> * On Windows 7 and prior: You need to use Zadig tool to manually bind the webUSB interface with the WinUSB driver for Chrome to access. From windows 8 and 10, this is done automatically by firmware.
> * On Linux/macOS, udev permission may need to be updated by copying '99-tinyusb.rules' file to /etc/udev/rules.d/ then run 'sudo udevadm control --reload-rules && sudo udevadm trigger'
