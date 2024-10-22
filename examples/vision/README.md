# Vision Example

This example illustrates two neuralSPOT features: capturing images from an Arducam Mega SPI camera, and displaying those images on a webpage via WebUSB. This example is a stripped-down version of Ambiq's [EdgeImpulse FOMO demo](https://github.com/AmbiqAI/edgeimpulse-examples/tree/main/fomo/apollo5).

#### Relevant neuralSPOT Library Documents 

[WebUSB neuralSPOT Library](../../neuralspot/ns-usb/README.md)

[Camera neuralSPOT Library](../../neuralspot/ns-camera/README.md)

## Running The Demo

The vision example app captures images and sends them over USB to a javascript application. This application then displays the image. The javascript application also has basic controls for camera controls (brightness, etc).

1. Connect both USB interfaces on the EVB to a PC
2. Connect the camera to the EVB (wiring is describe in Camera doc, linked above)
3. Install a browser with WebUSB support (Chrome and Edge are known to work)
4. Compile and flash the example
5. Click on the browser notification announcing the presence of a WebUSB device - this will load the example's [companion website](https://github.com/AmbiqAI/web-ble-dashboards/blob/main/vision_demo/index.html).

### Some salient features

- Supports both RGB and JPG image formats
- Uses SPI DMA and background processing to overlap capture and transmit operations
- Illustrates bidirectional communication over WebUSB

