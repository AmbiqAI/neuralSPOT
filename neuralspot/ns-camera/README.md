# Neuralspot Camera
This library allows the collection of images via USB from an Arducam Mega SPI camera. It has been tested on the Arducam Mega 5MP model. See below for instructions on how to wire the camera to an Apollo4 EVB. Currently, only Apollo4 is supported.

## Theory of Operations
For a simple example, see the examples/vision.cc example.

The Arducam works by taking a picture and storing it on its on-board buffer - this takes a varying amount of time (depending on shutter speed) and results in a varying amount of captured data (depending on resolution and compression). The application must poll the camera's completion register to determine when a frame has been fully captured. Once captured, the image data must be transferred to the CPU via SPI reads.

This library supports 2 ways to capture images from the camera: "pure polling" and "background".

"Pure polling" involves polling the completion SPI register in a tight loop, then reading the data via single SPI reads until all the data has been transfered. This is the native mode supported by the Arducam driver.

"Background" offloads completion polling to a timer interrupt and calls an app-defined completion callback when the image is ready for transfer. It also uses Apollo's IOM DMA engine to transfer the data (in chunks if needed) and calls an app-defined callback when the entire image is transferd.

## Taking a picture: example code
For latest code, see vision.cc.

```c
uint8_t jpgBuffer[JPG_BUFF_SIZE];
bool dmaComplete = false;
bool pictureTaken = false;

void picture_dma_complete(ns_camera_config_t *cfg) {
    ns_lp_printf("DMA Complete CB\n");
    dmaComplete = true;
}

void picture_taken_complete(ns_camera_config_t *cfg) {
    ns_lp_printf("Picture taken CB\n");
    pictureTaken = true;
}

ns_camera_config_t camera_config = {
    .api = &ns_camera_V1_0_0,
    .spiSpeed = AM_HAL_IOM_8MHZ, // can be 4MHZ or 1MHZ, but not over 8MHZ
    .cameraHw = NS_ARDUCAM,
    .imageMode = NS_CAM_IMAGE_MODE_320X320,
    .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG,
    .spiConfig = {.iom = 1}, // Only IOM1 is currently supported
    .dmaCompleteCb = picture_dma_complete,
    .pictureTakenCb = picture_taken_complete,
};

int main(void) {
    NS_TRY(ns_camera_init(&camera_config), "Camera Stop Failed\n");
    ns_stop_camera(&camera_config);

    // ... when ready to take pictures...

    // Send camera images to webusb clients
    ns_start_camera(&camera_config);
    ns_delay_us(10000);

    // Start image capture
    press_jpg_shutter_button(&camera_config);
    while (pictureTaken == false) {
        // wait for callback
        ns_deep_sleep();
    }

    // Transfer captured image
    uint8_t bufferOffset; // This is where the image data starts in jpgBuffer, will be 1 for jpg, 0 for anything else
    uint32_t length = ns_start_dma_read(&camera_config, jpgBuffer, &bufferOffset, JPG_BUFF_SIZE);
    while (dmaComplete == false) {
        ns_deep_sleep();
    }

    // Do something with the picture!
}
```

## Helper Functions
This library offers a handful of helper functions:
- camera_decode_init(...) converts a jpg to an RGB565 image
- ns_rgb565_to_rgb888(...) converts an RGB565 pixel to R, G, B 8-bit values
- ns_chop_off_trailing_zeros(...) finds the beginning of the zero pad Arducam likes to put at the end of jpgs
- ns_camera_adjust_settings(...) allows app to set brightness, ev, and contrast settings of image capture


# Wiring up an Arducam

The ns-camera component (currently) only supports SPI-base Arducam Mega 5MP (maybe 3MP too, but it hasn't been tested). To connect the Arducam to an Apollo4 EVB, make the following connections:

1. Camera GND to any EVB GND
2. Camera 5V/VDD to any EVB 5V pin
3. Camera SCK to EVB Pin 8
4. Camera MISO to EVB Pin 10
5. Camera MOSI to EVB 9
6. Camera CS to EVB 11
