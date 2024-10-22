# neuralSPOT USB Library

This library simplifies the use of USB to create CDC and WebUSB Devices. It is used by ns-rpc to connect to the remote RPC server or client, and by the Vision and Image Classification examples to connect to WebUSB applications.

## Creating a WebUSB Device

Creating a WebUSB can be done in a handful of instructions. See the [Image Classification example](../../examples/ic/README.md) for a application using this library.

#### Init Configs

```c

uint8_t my_rx_ff_buf[4096];
uint8_t my_tx_ff_buf[4096];

// WebUSB URL
static ns_tusb_desc_webusb_url_t ic_url;

// WebUSB Config
static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL,
    .desc_url = &ic_url
};
```

#### In Main

```c
    webusb_register_msg_cb(msgReceived, NULL); // See below for incoming message handling

    // instantiate the URL descriptor - this will be used by browser pop-up note
    strcpy(ic_url.url, "ambiqai.github.io/web-ble-dashboards/ic_demo/");
    ic_url.bDescriptorType = 3;
    ic_url.bScheme = 1;
    ic_url.bLength = 3 + sizeof(ic_url.url) - 1;

    // Initialize USB
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = 4096;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = 4096;
    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
```

WebUSB lets you send and receive messages across USB. The contents of the message is treated as a 'blob', and it is up to the application to decode that blob on both ends.

#### Sending a message

```c
// USB Data sent to WebUSB client
typedef struct usb_data { // App-defined 'blob'
    uint8_t type;
    uint8_t length;
    uint8_t classId;
    uint8_t confidence;
    uint8_t fps;
    uint8_t joulesEstimate;
    uint8_t cpuUtilization;
} usb_data_t;

void sendMessage(
    uint8_t type, uint8_t classId, uint8_t confidence, uint8_t fps, uint8_t joulesEstimate,
    uint8_t cpuUtilization) {
    usb_data_t data = {
        .type = type,
        .length = sizeof(usb_data_t),
        .classId = classId,
        .confidence = confidence,
        .fps = fps,
        .joulesEstimate = joulesEstimate,
        .cpuUtilization = cpuUtilization};

    // Send the data to the WebUSB client as an array of bytes
    webusb_send_data((uint8_t *)&data, sizeof(usb_data_t));
}
```

Receiving a message operates on the same principle with *one important caveat*: the sender must add a 2-byte header (0x0002) to tell the WebUSB driver how to route the packet.

#### Receiver

```c
void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // The message contains information about how to set the camera
    ns_camera_adjust_settings(buffer[0], buffer[1], buffer[2]);
}
```

#### Sender (in Javascript)

```javascript
    function sendCameraMessage() {
        console.log("sending camera message");
        let cameraMessage = new Uint8Array(6);
        // Send header
        cameraMessage[0] = 0x00;
        cameraMessage[1] = 0x02;

        // Send values
        cameraMessage[2] = contrastValue;
        cameraMessage[3] = brightnessValue;
        cameraMessage[4] = evValue;
        port.send(cameraMessage);
    }
```