/*
 * This file is part of the Arducam SPI Camera project.
 *
 * Copyright 2021 Arducam Technology co., Ltd. All Rights Reserved.
 *
 * This work is licensed under the MIT license, see the file LICENSE for
 * details.
 *
 */

#include "arducam.h"
#include <stdbool.h>

/// @cond

#define ARDUCHIP_FRAMES 0x01
#define ARDUCHIP_TEST1 0x00  // TEST register
#define ARDUCHIP_FIFO 0x04   // FIFO and I2C control
#define ARDUCHIP_FIFO_2 0x07 // FIFO and I2C control
#define FIFO_CLEAR_ID_MASK 0x01
#define FIFO_START_MASK 0x02

#define FIFO_RDPTR_RST_MASK 0x10
#define FIFO_WRPTR_RST_MASK 0x20
#define FIFO_CLEAR_MASK 0x80

#define ARDUCHIP_TRIG 0x44 // Trigger source
#define VSYNC_MASK 0x01
#define SHUTTER_MASK 0x02
#define CAP_DONE_MASK 0x04

#define FIFO_SIZE1 0x45 // Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2 0x46 // Camera write FIFO size[15:8]
#define FIFO_SIZE3 0x47 // Camera write FIFO size[18:16]

#define BURST_FIFO_READ 0x3C  // Burst FIFO read operation
#define SINGLE_FIFO_READ 0x3D // Single FIFO read operation

#if defined(__MSP430G2553__)
    #define PREVIEW_BUF_LEN 50
#else
    #define PREVIEW_BUF_LEN 255
#endif

#define CAPTURE_MAX_NUM 0xFF

#define CAM_REG_POWER_CONTROL 0X02
#define CAM_REG_SENSOR_RESET 0X07
#define CAM_REG_FORMAT 0X20
#define CAM_REG_CAPTURE_RESOLUTION 0X21
#define CAM_REG_BRIGHTNESS_CONTROL 0X22
#define CAM_REG_CONTRAST_CONTROL 0X23
#define CAM_REG_SATURATION_CONTROL 0X24
#define CAM_REG_EV_CONTROL 0X25
#define CAM_REG_WHILEBALANCE_MODE_CONTROL 0X26
#define CAM_REG_COLOR_EFFECT_CONTROL 0X27
#define CAM_REG_SHARPNESS_CONTROL 0X28
#define CAM_REG_AUTO_FOCUS_CONTROL 0X29
#define CAM_REG_IMAGE_QUALITY 0x2A
#define CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL 0X30
#define CAM_REG_MANUAL_GAIN_BIT_9_8 0X31
#define CAM_REG_MANUAL_GAIN_BIT_7_0 0X32
#define CAM_REG_MANUAL_EXPOSURE_BIT_19_16 0X33
#define CAM_REG_MANUAL_EXPOSURE_BIT_15_8 0X34
#define CAM_REG_MANUAL_EXPOSURE_BIT_7_0 0X35
#define CAM_REG_BURST_FIFO_READ_OPERATION 0X3C
#define CAM_REG_SINGLE_FIFO_READ_OPERATION 0X3D
#define CAM_REG_SENSOR_ID 0x40
#define CAM_REG_YEAR_ID 0x41
#define CAM_REG_MONTH_ID 0x42
#define CAM_REG_DAY_ID 0x43
#define CAM_REG_SENSOR_STATE 0x44
#define CAM_REG_FPGA_VERSION_NUMBER 0x49
#define CAM_REG_DEBUG_DEVICE_ADDRESS 0X0A
#define CAM_REG_DEBUG_REGISTER_HIGH 0X0B
#define CAM_REG_DEBUG_REGISTER_LOW 0X0C
#define CAM_REG_DEBUG_REGISTER_VALUE 0X0D

#define CAM_REG_SENSOR_STATE_IDLE (1 << 1)
#define CAM_SENSOR_RESET_ENABLE (1 << 6)
#define CAM_FORMAT_BASICS (0 << 0)
#define CAM_SET_CAPTURE_MODE (0 << 7)
#define CAM_SET_VIDEO_MODE (1 << 7)

#define SET_WHILEBALANCE 0X02
#define SET_EXPOSURE 0X01
#define SET_GAIN 0X00

#define CAMERA_TYPE_NUMBER 2

// #define FORMAT_NONE                                0X00
// #define FORMAT_JPEG                                0X01
// #define FORMAT_RGB                                 0X02
// #define FORMAT_YUV                                 0X03

#define RESOLUTION_160X120 (1 << 0)
#define RESOLUTION_320X240 (1 << 1)
#define RESOLUTION_640X480 (1 << 2)
#define RESOLUTION_800X600 (1 << 3)
#define RESOLUTION_1280X720 (1 << 4)
#define RESOLUTION_1280X960 (1 << 5)
#define RESOLUTION_1600X1200 (1 << 6)
#define RESOLUTION_1920X1080 (1 << 7)
#define RESOLUTION_2048X1536 (1 << 8)
#define RESOLUTION_2592X1944 (1 << 9)
#define RESOLUTION_320x320 (1 << 10)
#define RESOLUTION_128x128 (1 << 11)
#define RESOLUTION_96x96 (1 << 12)

#define SPECIAL_NORMAL (0 << 0)
#define SPECIAL_BLUEISH (1 << 0)
#define SPECIAL_REDISH (1 << 1)
#define SPECIAL_BW (1 << 2)
#define SPECIAL_SEPIA (1 << 3)
#define SPECIAL_NEGATIVE (1 << 4)
#define SPECIAL_GREENISH (1 << 5)
#define SPECIAL_OVEREXPOSURE (1 << 6)
#define SPECIAL_SOLARIZE (1 << 7)
#define SPECIAL_YELLOWISH (1 << 8)

union SdkInfo currentSDK = {
    .sdkInfo.year = 23,
    .sdkInfo.month = 4,
    .sdkInfo.day = 19,
    .sdkInfo.version = 0x23,
};

struct cameraDefaultState {
    uint8_t cameraDefaultFormat;
    uint8_t cameraDefaultResolution;
};

struct CameraInfo CameraInfo_5MP = {
    .cameraId = "5MP",
    .supportResolution = RESOLUTION_320x320 | RESOLUTION_128x128 | RESOLUTION_96x96 |
                         RESOLUTION_320X240 | RESOLUTION_640X480 | RESOLUTION_1280X720 |
                         RESOLUTION_1600X1200 | RESOLUTION_1920X1080 | RESOLUTION_2592X1944,
    .supportSpecialEffects = SPECIAL_BLUEISH | SPECIAL_REDISH | SPECIAL_BW | SPECIAL_SEPIA |
                             SPECIAL_NEGATIVE |
                             SPECIAL_GREENISH /*| SPECIAL_OVEREXPOSURE | SPECIAL_SOLARIZE*/,
    .exposureValueMax = 30000,
    .exposureValueMin = 1,
    .gainValueMax = 1023,
    .gainValueMin = 1,
    .supportFocus = true,
    .supportSharpness = false,
    .deviceAddress = 0x78,
};

struct CameraInfo CameraInfo_3MP = {
    .cameraId = "3MP",
    .supportResolution = RESOLUTION_320x320 | RESOLUTION_128x128 | RESOLUTION_96x96 |
                         RESOLUTION_320X240 | RESOLUTION_640X480 | RESOLUTION_1280X720 |
                         RESOLUTION_1600X1200 | RESOLUTION_1920X1080 | RESOLUTION_2048X1536,
    .supportSpecialEffects = SPECIAL_BLUEISH | SPECIAL_REDISH | SPECIAL_BW | SPECIAL_SEPIA |
                             SPECIAL_NEGATIVE | SPECIAL_GREENISH | SPECIAL_YELLOWISH,
    .exposureValueMax = 30000,
    .exposureValueMin = 1,
    .gainValueMax = 1023,
    .gainValueMin = 1,
    .supportFocus = false,
    .supportSharpness = true,
    .deviceAddress = 0x78,
};

struct CameraInfo *CameraType[CAMERA_TYPE_NUMBER] = {
    &CameraInfo_5MP,
    &CameraInfo_3MP,
};

uint8_t ov3640GainValue[] = {0x00, 0x10, 0x18, 0x30, 0x34, 0x38, 0x3b, 0x3f, 0x72, 0x74, 0x76,
                             0x78, 0x7a, 0x7c, 0x7e, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
                             0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

void cameraWaitI2cIdle(ArducamCamera *camera);
uint8_t cameraGetBit(ArducamCamera *camera, uint8_t addr, uint8_t bit);
uint8_t cameraBusWrite(ArducamCamera *camera, int address, int value);
uint8_t cameraBusRead(ArducamCamera *camera, int address);

void cameraInit(ArducamCamera *camera, int csPin, ArducamTransport *transport) {
    camera->cameraId = false;
    camera->currentPixelFormat = CAM_IMAGE_PIX_FMT_NONE;
    camera->currentPictureMode = CAM_IMAGE_MODE_NONE;
    camera->burstFirstFlag = false;
    camera->previewMode = false;
    camera->csPin = csPin;
    camera->currentSDK = &currentSDK;
    camera->transport = transport;
}

void cameraGetSensorConfig(ArducamCamera *camera) {
    uint8_t cameraIdx = 0;
    camera->cameraId = cameraReadReg(camera, CAM_REG_SENSOR_ID);
    cameraWaitI2cIdle(camera);
    switch (camera->cameraId) {
    case SENSOR_5MP_2:
        CameraInfo_5MP.cameraId = "5MP_2";
    case SENSOR_5MP_1:
        cameraIdx = 0x00;
        break;
    case SENSOR_3MP_1:
    case SENSOR_3MP_2:
        cameraIdx = 0x01;
        break;
    }

    camera->myCameraInfo = *CameraType[cameraIdx];
    // camera->currentPixelFormat = cameraDefaultInfo[cameraIdx]->cameraDefaultFormat;
    // camera->currentPictureMode = cameraDefaultInfo[cameraIdx]->cameraDefaultResolution;
}

CamStatus cameraBegin(ArducamCamera *camera) {
    // reset cpld and camera
    cameraWriteReg(camera, CAM_REG_SENSOR_RESET, CAM_SENSOR_RESET_ENABLE);
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    cameraGetSensorConfig(camera);
    camera->verDateAndNumber[0] = cameraReadReg(camera, CAM_REG_YEAR_ID) & 0x3F; // year
    cameraWaitI2cIdle(camera);
    camera->verDateAndNumber[1] = cameraReadReg(camera, CAM_REG_MONTH_ID) & 0x0F; // month
    cameraWaitI2cIdle(camera);
    camera->verDateAndNumber[2] = cameraReadReg(camera, CAM_REG_DAY_ID) & 0x1F; // day
    cameraWaitI2cIdle(camera);
    camera->verDateAndNumber[3] = cameraReadReg(camera, CAM_REG_FPGA_VERSION_NUMBER) & 0xFF; // day
    cameraWaitI2cIdle(camera);
    cameraWriteReg(camera, CAM_REG_DEBUG_DEVICE_ADDRESS, camera->myCameraInfo.deviceAddress);
    cameraWaitI2cIdle(camera);
    return CAM_ERR_SUCCESS;
}

void cameraSetCapture(ArducamCamera *camera) {
    // cameraFlushFifo(camera);
    cameraClearFifoFlag(camera);
    cameraStartCapture(camera);
    while (cameraGetBit(camera, ARDUCHIP_TRIG, CAP_DONE_MASK) == 0)
        ;
    camera->receivedLength = cameraReadFifoLength(camera);
    camera->totalLength = camera->receivedLength;
    camera->burstFirstFlag = 0;
}

uint32_t cameraImageAvailable(ArducamCamera *camera) { return camera->receivedLength; }

CamStatus cameraSetAutoFocus(ArducamCamera *camera, uint8_t val) {
    cameraWriteReg(camera, CAM_REG_AUTO_FOCUS_CONTROL, val); // auto focus control
    cameraWaitI2cIdle(camera);                               // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus
cameraTakePicture(ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format) {
    if (camera->currentPixelFormat != pixel_format) {
        camera->currentPixelFormat = pixel_format;
        cameraWriteReg(camera, CAM_REG_FORMAT, pixel_format); // set the data format
        cameraWaitI2cIdle(camera);                            // Wait I2c Idle
    }

    if (camera->currentPictureMode != mode) {
        camera->currentPictureMode = mode;
        cameraWriteReg(camera, CAM_REG_CAPTURE_RESOLUTION, CAM_SET_CAPTURE_MODE | mode);
        cameraWaitI2cIdle(camera); // Wait I2c Idle
    }

    cameraSetCapture(camera);
    return CAM_ERR_SUCCESS;
}

CamStatus cameraTakeMultiPictures(
    ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format, uint8_t num) {
    if (camera->currentPixelFormat != pixel_format) {
        camera->currentPixelFormat = pixel_format;
        cameraWriteReg(camera, CAM_REG_FORMAT, pixel_format); // set the data format
        cameraWaitI2cIdle(camera);                            // Wait I2c Idle
    }

    if (camera->currentPictureMode != mode) {
        camera->currentPictureMode = mode;
        cameraWriteReg(camera, CAM_REG_CAPTURE_RESOLUTION, CAM_SET_CAPTURE_MODE | mode);
        cameraWaitI2cIdle(camera); // Wait I2c Idle
    }

    if (num > CAPTURE_MAX_NUM) {
        num = CAPTURE_MAX_NUM;
    }

    cameraWriteReg(camera, ARDUCHIP_FRAMES, num);
    cameraSetCapture(camera);
    return CAM_ERR_SUCCESS;
}

void cameraRegisterCallback(
    ArducamCamera *camera, BUFFER_CALLBACK function, uint8_t size, STOP_HANDLE handle) {
    camera->callBackFunction = function;
    camera->blockSize = size;
    camera->handle = handle;
}

CamStatus cameraStartPreview(ArducamCamera *camera, CAM_VIDEO_MODE mode) {

    // camera->cameraDataFormat = CAM_IMAGE_PIX_FMT_JPG;
    camera->previewMode = true;
    if (!camera->callBackFunction) {
        return CAM_ERR_NO_CALLBACK;
    }
    cameraWriteReg(camera, CAM_REG_FORMAT, CAM_IMAGE_PIX_FMT_JPG); // set  jpeg format
    cameraWaitI2cIdle(camera);                                     // Wait I2c Idle
    cameraWriteReg(
        camera, CAM_REG_CAPTURE_RESOLUTION,
        CAM_SET_VIDEO_MODE | mode); // set  video mode
    cameraWaitI2cIdle(camera);      // Wait I2c Idle
    cameraSetCapture(camera);

    return CAM_ERR_SUCCESS;
}
static uint8_t callBackBuff[PREVIEW_BUF_LEN];

void cameraCaptureThread(ArducamCamera *camera) {
    if (camera->previewMode) {
        uint8_t callBackLength = cameraReadBuff(camera, callBackBuff, camera->blockSize);
        if (callBackLength != false) {
            camera->callBackFunction(callBackBuff, callBackLength);
        } else {
            cameraSetCapture(camera);
        }
    }
}

CamStatus cameraStopPreview(ArducamCamera *camera) {
    if (camera->previewMode == true && camera->handle != 0) {
        camera->handle();
    }

    camera->currentPixelFormat = CAM_IMAGE_PIX_FMT_JPG;
    camera->currentPictureMode = CAM_IMAGE_MODE_QVGA;
    camera->previewMode = false;
    camera->receivedLength = 0;
    camera->totalLength = 0;
    cameraWriteReg(camera, CAM_REG_FORMAT, CAM_IMAGE_PIX_FMT_JPG); // set  jpeg format
    cameraWaitI2cIdle(camera);                                     // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetImageQuality(ArducamCamera *camera, IMAGE_QUALITY qualtiy) {
    cameraWriteReg(camera, CAM_REG_IMAGE_QUALITY, qualtiy);
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraReset(ArducamCamera *camera) {
    cameraWriteReg(camera, CAM_REG_SENSOR_RESET, CAM_SENSOR_RESET_ENABLE);
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoWhiteBalanceMode(ArducamCamera *camera, CAM_WHITE_BALANCE mode) {
    cameraWriteReg(camera, CAM_REG_WHILEBALANCE_MODE_CONTROL, mode); // set Light Mode
    cameraWaitI2cIdle(camera);                                       // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoWhiteBalance(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == true) {
        symbol |= 0x80;
    }
    symbol |= SET_WHILEBALANCE;
    cameraWriteReg(
        camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
        symbol);               // while balance control
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoISOSensitive(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == true) {
        symbol |= 0x80;
    }
    symbol |= SET_GAIN;
    cameraWriteReg(camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
                   symbol);    // auto gain control
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetISOSensitivity(ArducamCamera *camera, int iso_sense) {
    if (camera->cameraId == SENSOR_3MP_1) {
        iso_sense = ov3640GainValue[iso_sense - 1];
    }
    cameraWriteReg(camera, CAM_REG_MANUAL_GAIN_BIT_9_8,
                   iso_sense >> 8); // set AGC VALUE
    cameraWaitI2cIdle(camera);
    cameraWriteReg(camera, CAM_REG_MANUAL_GAIN_BIT_7_0, iso_sense & 0xff);
    cameraWaitI2cIdle(camera);
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoExposure(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == true) {
        symbol |= 0x80;
    }
    symbol |= SET_EXPOSURE;
    cameraWriteReg(
        camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
        symbol);               // auto EXPOSURE control
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAbsoluteExposure(ArducamCamera *camera, uint32_t exposure_time) {
    // set exposure output [19:16]
    cameraWriteReg(
        camera, CAM_REG_MANUAL_EXPOSURE_BIT_19_16, (uint8_t)((exposure_time >> 16) & 0xff));
    cameraWaitI2cIdle(camera);
    // set exposure output [15:8]
    cameraWriteReg(
        camera, CAM_REG_MANUAL_EXPOSURE_BIT_15_8, (uint8_t)((exposure_time >> 8) & 0xff));
    cameraWaitI2cIdle(camera);
    // set exposure output [7:0]
    cameraWriteReg(camera, CAM_REG_MANUAL_EXPOSURE_BIT_7_0, (uint8_t)(exposure_time & 0xff));
    cameraWaitI2cIdle(camera);
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetColorEffect(ArducamCamera *camera, CAM_COLOR_FX effect) {
    cameraWriteReg(camera, CAM_REG_COLOR_EFFECT_CONTROL, effect); // set effect
    cameraWaitI2cIdle(camera);                                    // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetSaturation(ArducamCamera *camera, CAM_STAURATION_LEVEL level) {
    cameraWriteReg(camera, CAM_REG_SATURATION_CONTROL, level); // set Saturation Level
    cameraWaitI2cIdle(camera);                                 // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetEV(ArducamCamera *camera, CAM_EV_LEVEL level) {
    // set Exposure  Compensation Level
    cameraWriteReg(camera, CAM_REG_EV_CONTROL, level);
    cameraWaitI2cIdle(camera); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetContrast(ArducamCamera *camera, CAM_CONTRAST_LEVEL level) {
    cameraWriteReg(camera, CAM_REG_CONTRAST_CONTROL, level); // set Contrast Level
    cameraWaitI2cIdle(camera);                               // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetSharpness(ArducamCamera *camera, CAM_SHARPNESS_LEVEL level) {
    cameraWriteReg(camera, CAM_REG_SHARPNESS_CONTROL, level); // set Brightness Level
    cameraWaitI2cIdle(camera);                                // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetBrightness(ArducamCamera *camera, CAM_BRIGHTNESS_LEVEL level) {
    cameraWriteReg(camera, CAM_REG_BRIGHTNESS_CONTROL, level); // set Brightness Level
    cameraWaitI2cIdle(camera);                                 // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
void cameraFlushFifo(ArducamCamera *camera) {
    cameraWriteReg(camera, ARDUCHIP_FIFO_2, FIFO_CLEAR_MASK);
}

void cameraStartCapture(ArducamCamera *camera) {
    cameraWriteReg(camera, ARDUCHIP_FIFO, FIFO_START_MASK);
}

void cameraClearFifoFlag(ArducamCamera *camera) {
    cameraWriteReg(camera, ARDUCHIP_FIFO, FIFO_CLEAR_ID_MASK);
}

uint32_t cameraReadFifoLength(ArducamCamera *camera) {
    uint32_t len1, len2, len3, length = 0;
    len1 = cameraReadReg(camera, FIFO_SIZE1);
    len2 = cameraReadReg(camera, FIFO_SIZE2);
    len3 = cameraReadReg(camera, FIFO_SIZE3);
    length = ((len3 << 16) | (len2 << 8) | len1) & 0xffffff;
    return length;
}

uint8_t cameraGetBit(ArducamCamera *camera, uint8_t addr, uint8_t bit) {
    uint8_t temp;
    temp = cameraReadReg(camera, addr);
    temp = temp & bit;
    return temp;
}

uint8_t cameraReadByte(ArducamCamera *camera) {
    uint8_t data = cameraReadReg(camera, SINGLE_FIFO_READ);
    camera->receivedLength -= 1;
    return data;
}

uint32_t cameraReadBuff(ArducamCamera *camera, uint8_t *buff, uint32_t length) {
    if (cameraImageAvailable(camera) == 0 || (length == 0)) {
        return 0;
    }

    if (camera->receivedLength < length) {
        length = camera->receivedLength;
    }
    camera->transport->spiRead(buff, length, BURST_FIFO_READ, 1, camera->csPin);
    if (camera->burstFirstFlag == 0) {
        camera->burstFirstFlag = 1;
        // arducamSpiTransfer(0x00);
    }
    camera->receivedLength -= length;
    return length;
}

void cameraWriteReg(ArducamCamera *camera, uint8_t addr, uint8_t val) {
    cameraBusWrite(camera, addr | 0x80, val);
}

uint8_t cameraReadReg(ArducamCamera *camera, uint8_t addr) {
    uint8_t data;
    data = cameraBusRead(camera, addr & 0x7F);
    return data;
}

uint8_t cameraBusWrite(ArducamCamera *camera, int address, int value) {
    camera->transport->spiWrite(&value, 1, address, 1, camera->csPin);
    camera->transport->delayMs(1);
    return 1;
}

uint8_t cameraBusRead(ArducamCamera *camera, int address) {
    uint8_t value[2];
    camera->transport->spiRead(value, 2, address, 1, camera->csPin);
    return value[1];
}

void cameraWaitI2cIdle(ArducamCamera *camera) {
    while ((cameraReadReg(camera, CAM_REG_SENSOR_STATE) & 0X03) != CAM_REG_SENSOR_STATE_IDLE) {
        camera->transport->delayMs(2);
    }
}

uint8_t cameraHeartBeat(ArducamCamera *camera) {
    return (cameraReadReg(camera, CAM_REG_SENSOR_STATE) & 0X03) == CAM_REG_SENSOR_STATE_IDLE;
}

void cameraDebugWriteRegister(ArducamCamera *camera, uint8_t *buff) {
    uint8_t register_high = buff[0];
    uint8_t register_low = buff[1];
    uint8_t value = buff[2];
    cameraWriteReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, register_high);
    cameraWriteReg(camera, CAM_REG_DEBUG_REGISTER_LOW, register_low);
    cameraWriteReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, value);
}

void cameraLowPowerOn(ArducamCamera *camera) {
    cameraWriteReg(camera, CAM_REG_POWER_CONTROL, 0X07);
}

void cameraLowPowerOff(ArducamCamera *camera) {
    cameraWriteReg(camera, CAM_REG_POWER_CONTROL, 0X05);
}

/// @endcond
