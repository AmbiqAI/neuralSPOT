/*
 * This file is part of the Arducam SPI Camera project.
 *
 * Copyright 2021 Arducam Technology co., Ltd. All Rights Reserved.
 *
 * This work is licensed under the MIT license, see the file LICENSE for
 * details.
 *
 */

#include "ArducamCamera.h"
#include "Platform.h"
#include "ns_camera.h"
#include "ns_core.h"

/// @cond

#define NS_CAMERA_MAX_WAIT 1500

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

#define SENSOR_DATA 0x48 // Camera write FIFO size[18:16]

#define BURST_FIFO_READ 0x3C  // Burst FIFO read operation
#define SINGLE_FIFO_READ 0x3D // Single FIFO read operation

#if defined(__MSP430G2553__)
    #define PREVIEW_BUF_LEN 50
#else
    #define PREVIEW_BUF_LEN 255
#endif

#define CAPRURE_MAX_NUM 0xff

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
#define CAM_REG_DEBUG_REGISTER_VALUE_H 0X0E

#define CAM_I2C_READ_MODE (1 << 0)
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
    .sdkInfo.year = 24,
    .sdkInfo.month = 6,
    .sdkInfo.day = 18,
    .sdkInfo.version = 0x020C, // V2.0.11  H bit[11:8] M bit[7:4] L bit[3:0]
};

struct cameraDefaultState {
    uint8_t cameraDefaultFormat;
    uint8_t cameraDefaultResolution;
};

void setCapture(ArducamCamera *camera);
void csHigh(ArducamCamera *camera);
void csLow(ArducamCamera *camera);
void writeReg(ArducamCamera *camera, uint8_t addr, uint8_t val);
uint8_t readReg(ArducamCamera *camera, uint8_t addr);
uint8_t busRead(ArducamCamera *camera, int address);
uint8_t busWrite(ArducamCamera *camera, int address, int value);
void clearFifoFlag(ArducamCamera *camera);
uint32_t readFifoLength(ArducamCamera *camera);
uint8_t getBit(ArducamCamera *camera, uint8_t addr, uint8_t bit);
void setFifoBurst(ArducamCamera *camera);
void setCapture(ArducamCamera *camera);
uint32_t waitI2cIdle(ArducamCamera *camera);
uint32_t imageAvailable(ArducamCamera *camera);
void flushFifo(ArducamCamera *camera);
void startCapture(ArducamCamera *camera);

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
    .supportFocus = TRUE,
    .supportSharpness = FALSE,
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
    .supportFocus = FALSE,
    .supportSharpness = TRUE,
    .deviceAddress = 0x78,
};

struct CameraInfo *CameraType[CAMERA_TYPE_NUMBER];

// struct cameraDefaultState DefaultState_5mp = {
//     .cameraDefaultFormat = CAM_IMAGE_PIX_FMT_JPG,
//     .cameraDefaultResolution = CAM_IMAGE_MODE_WQXGA2,
// };

// struct cameraDefaultState DefaultState_3mp = {
//     .cameraDefaultFormat = CAM_IMAGE_PIX_FMT_JPG,
//     .cameraDefaultResolution = CAM_IMAGE_MODE_QXGA,
// };

// struct cameraDefaultState* cameraDefaultInfo[CAMERA_TYPE_NUMBER];

uint8_t ov3640GainValue[] = {0x00, 0x10, 0x18, 0x30, 0x34, 0x38, 0x3b, 0x3f, 0x72, 0x74, 0x76,
                             0x78, 0x7a, 0x7c, 0x7e, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
                             0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

void updateCameraInfo(ArducamCamera *camera) {
    if (camera->cameraId == SENSOR_5MP_2 || camera->cameraId == SENSOR_3MP_2) {
        camera->myCameraInfo.exposureValueMax = 0xFFFF;
        camera->myCameraInfo.gainValueMax = 0xFFFF;
    }
}

void cameraInit(ArducamCamera *camera) {
    arducamSpiBegin();
    arducamCsOutputMode(camera->csPin);
    arducamSpiCsPinLow(camera->csPin);
}

void cameraGetSensorConfig(ArducamCamera *camera) {
    uint8_t cameraIdx = 0;
    camera->cameraId = readReg(camera, CAM_REG_SENSOR_ID);
    waitI2cIdle(camera);
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
    writeReg(camera, CAM_REG_SENSOR_RESET, CAM_SENSOR_RESET_ENABLE);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    cameraGetSensorConfig(camera);
    updateCameraInfo(camera);
    camera->verDateAndNumber[0] = readReg(camera, CAM_REG_YEAR_ID) & 0x3F; // year
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // ns_lp_printf("year: %d\n", camera->verDateAndNumber[0]);
    camera->verDateAndNumber[1] = readReg(camera, CAM_REG_MONTH_ID) & 0x0F; // month
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // ns_lp_printf("month: %d\n", camera->verDateAndNumber[1]);
    camera->verDateAndNumber[2] = readReg(camera, CAM_REG_DAY_ID) & 0x1F; // day
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // ns_lp_printf("day: %d\n", camera->verDateAndNumber[2]);
    camera->verDateAndNumber[3] = readReg(camera, CAM_REG_FPGA_VERSION_NUMBER) & 0xFF; // day
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // ns_lp_printf("fpga version: %d\n", camera->verDateAndNumber[3]);

    writeReg(camera, CAM_REG_DEBUG_DEVICE_ADDRESS, camera->myCameraInfo.deviceAddress);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    return CAM_ERR_SUCCESS;
}

void cameraSetCapture(ArducamCamera *camera) {
    // flushFifo(camera);
    clearFifoFlag(camera);
    startCapture(camera);
    while (getBit(camera, ARDUCHIP_TRIG, CAP_DONE_MASK) == 0)
        ;
    camera->receivedLength = readFifoLength(camera);
    camera->totalLength = camera->receivedLength;
    camera->burstFirstFlag = 0;
}

uint32_t cameraImageAvailable(ArducamCamera *camera) { return camera->receivedLength; }

CamStatus cameraSetAutoFocus(ArducamCamera *camera, uint8_t val) {
    writeReg(camera, CAM_REG_AUTO_FOCUS_CONTROL, val);                // auto focus control
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

uint8_t cameraGetAutoFocusSta(ArducamCamera *camera) {
    uint16_t sta_reg = 0x3029;
    // step 1 write sta_reg
    uint8_t register_high = (sta_reg >> 8) & 0xFF;
    uint8_t register_low = (sta_reg)&0xFF;
    writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, register_high);
    waitI2cIdle(camera);
    writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, register_low);
    waitI2cIdle(camera);

    // step 2  read real sensor register
    writeReg(camera, CAM_REG_SENSOR_RESET, CAM_I2C_READ_MODE);
    waitI2cIdle(camera); // Wait I2c Idle
    arducamDelayMs(5);   // wait read finish
                         // step 3  get value
    return readReg(camera, SENSOR_DATA);
}

CamStatus cameraSetManualFocus(ArducamCamera *camera, uint16_t val) {
    uint16_t vcm_reg_H = 0x3603;
    uint16_t vcm_reg_L = 0x3602;
    uint8_t register_high, register_low;
    // step 1 convert val to vcm code
    uint8_t code_9_4 = (val >> 4) & 0x3F;
    uint8_t code_3_0 = (val << 4) & 0xF0;

    // step 2 wirte high register
    register_high = (vcm_reg_H >> 8) & 0xFF;
    register_low = (vcm_reg_H)&0xFF;
    writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, register_high);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, register_low);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, code_9_4);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");

    // step 3 wirte low register

    register_high = (vcm_reg_L >> 8) & 0xFF;
    register_low = (vcm_reg_L)&0xFF;
    writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, register_high);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, register_low);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, code_3_0);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");

    return CAM_ERR_SUCCESS;
}

CamStatus
cameraTakePicture(ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format) {
    if (camera->currentPixelFormat != pixel_format) {
        camera->currentPixelFormat = pixel_format;
        writeReg(camera, CAM_REG_FORMAT, pixel_format);                   // set the data format
        NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    }

    if (camera->currentPictureMode != mode) {
        camera->currentPictureMode = mode;
        writeReg(camera, CAM_REG_CAPTURE_RESOLUTION, CAM_SET_CAPTURE_MODE | mode);
        NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    }

    setCapture(camera);
    return CAM_ERR_SUCCESS;
}

CamStatus cameratakeMultiPictures(
    ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format, uint8_t num) {
    if (camera->currentPixelFormat != pixel_format) {
        camera->currentPixelFormat = pixel_format;
        writeReg(camera, CAM_REG_FORMAT, pixel_format);                   // set the data format
        NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    }

    if (camera->currentPictureMode != mode) {
        camera->currentPictureMode = mode;
        writeReg(camera, CAM_REG_CAPTURE_RESOLUTION, CAM_SET_CAPTURE_MODE | mode);
        NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    }

    if (num > CAPRURE_MAX_NUM) {
        num = CAPRURE_MAX_NUM;
    }

    writeReg(camera, ARDUCHIP_FRAMES, num);
    setCapture(camera);
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
    camera->previewMode = TRUE;
    if (!camera->callBackFunction) {
        return CAM_ERR_NO_CALLBACK;
    }
    writeReg(camera, CAM_REG_FORMAT, CAM_IMAGE_PIX_FMT_JPG);          // set  jpeg format
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    writeReg(camera, CAM_REG_CAPTURE_RESOLUTION,
             CAM_SET_VIDEO_MODE | mode);                              // set  video mode
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    setCapture(camera);

    return CAM_ERR_SUCCESS;
}
static uint8_t callBackBuff[PREVIEW_BUF_LEN];

void cameraCaptureThread(ArducamCamera *camera) {
    if (camera->previewMode) {
        uint8_t callBackLength = readBuff(camera, callBackBuff, camera->blockSize);
        if (callBackLength != FALSE) {
            camera->callBackFunction(callBackBuff, callBackLength);
        } else {
            setCapture(camera);
        }
    }
}

CamStatus cameraStopPreview(ArducamCamera *camera) {
    if (camera->previewMode == TRUE && camera->handle != 0) {
        camera->handle();
    }

    camera->currentPixelFormat = CAM_IMAGE_PIX_FMT_JPG;
    camera->currentPictureMode = CAM_IMAGE_MODE_QVGA;
    camera->previewMode = FALSE;
    camera->receivedLength = 0;
    camera->totalLength = 0;
    writeReg(camera, CAM_REG_FORMAT, CAM_IMAGE_PIX_FMT_JPG);          // set  jpeg format
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetImageQuality(ArducamCamera *camera, IMAGE_QUALITY qualtiy) {
    writeReg(camera, CAM_REG_IMAGE_QUALITY, qualtiy);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraReset(ArducamCamera *camera) {
    writeReg(camera, CAM_REG_SENSOR_RESET, CAM_SENSOR_RESET_ENABLE);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    camera->currentPixelFormat = -1;
    camera->currentPictureMode = -1;
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoWhiteBalanceMode(ArducamCamera *camera, CAM_WHITE_BALANCE mode) {
    writeReg(camera, CAM_REG_WHILEBALANCE_MODE_CONTROL, mode);        // set Light Mode
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoWhiteBalance(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == TRUE) {
        symbol |= 0x80;
    }
    symbol |= SET_WHILEBALANCE;
    writeReg(camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
             symbol);                                                 // white balance control
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoISOSensitive(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == TRUE) {
        symbol |= 0x80;
    }
    symbol |= SET_GAIN;
    writeReg(camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
             symbol);                                                 // auto gain control
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetISOSensitivity(ArducamCamera *camera, int iso_sense) {
    if (camera->cameraId == SENSOR_3MP_1) {
        iso_sense = ov3640GainValue[iso_sense - 1];
    }
    writeReg(camera, CAM_REG_MANUAL_GAIN_BIT_9_8,
             iso_sense >> 8); // set AGC VALUE
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    writeReg(camera, CAM_REG_MANUAL_GAIN_BIT_7_0, iso_sense & 0xff);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAutoExposure(ArducamCamera *camera, uint8_t val) {
    unsigned char symbol = 0;
    if (val == TRUE) {
        symbol |= 0x80;
    }
    symbol |= SET_EXPOSURE;
    writeReg(camera, CAM_REG_EXPOSURE_GAIN_WHILEBALANCE_CONTROL,
             symbol);                                                 // auto EXPOSURE control
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetAbsoluteExposure(ArducamCamera *camera, uint32_t exposure_time) {
    // set exposure output [19:16]
    writeReg(camera, CAM_REG_MANUAL_EXPOSURE_BIT_19_16, (uint8_t)((exposure_time >> 16) & 0xff));
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // set exposure output [15:8]
    writeReg(camera, CAM_REG_MANUAL_EXPOSURE_BIT_15_8, (uint8_t)((exposure_time >> 8) & 0xff));
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    // set exposure output [7:0]
    writeReg(camera, CAM_REG_MANUAL_EXPOSURE_BIT_7_0, (uint8_t)(exposure_time & 0xff));
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed");
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetColorEffect(ArducamCamera *camera, CAM_COLOR_FX effect) {
    writeReg(camera, CAM_REG_COLOR_EFFECT_CONTROL, effect);           // set effect
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetSaturation(ArducamCamera *camera, CAM_STAURATION_LEVEL level) {
    writeReg(camera, CAM_REG_SATURATION_CONTROL, level);              // set Saturation Level
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetEV(ArducamCamera *camera, CAM_EV_LEVEL level) {
    // set Exposure  Compensation Level
    writeReg(camera, CAM_REG_EV_CONTROL, level);
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetContrast(ArducamCamera *camera, CAM_CONTRAST_LEVEL level) {
    writeReg(camera, CAM_REG_CONTRAST_CONTROL, level);                // set Contrast Level
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
CamStatus cameraSetSharpness(ArducamCamera *camera, CAM_SHARPNESS_LEVEL level) {
    writeReg(camera, CAM_REG_SHARPNESS_CONTROL, level);               // set Brightness Level
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}

CamStatus cameraSetBrightness(ArducamCamera *camera, CAM_BRIGHTNESS_LEVEL level) {
    writeReg(camera, CAM_REG_BRIGHTNESS_CONTROL, level);              // set Brightness Level
    NS_TRY_ABORT(waitI2cIdle(camera), "Camera Wait for Idle Failed"); // Wait I2c Idle
    return CAM_ERR_SUCCESS;
}
void cameraFlushFifo(ArducamCamera *camera) { writeReg(camera, ARDUCHIP_FIFO_2, FIFO_CLEAR_MASK); }

void cameraStartCapture(ArducamCamera *camera) { writeReg(camera, ARDUCHIP_FIFO, FIFO_START_MASK); }

void cameraClearFifoFlag(ArducamCamera *camera) {
    writeReg(camera, ARDUCHIP_FIFO, FIFO_CLEAR_ID_MASK);
}

uint32_t cameraReadFifoLength(ArducamCamera *camera) {
    uint32_t len1, len2, len3, length = 0;
    len1 = readReg(camera, FIFO_SIZE1);
    len2 = readReg(camera, FIFO_SIZE2);
    len3 = readReg(camera, FIFO_SIZE3);
    length = ((len3 << 16) | (len2 << 8) | len1) & 0xffffff;
    return length;
}

uint8_t cameraGetBit(ArducamCamera *camera, uint8_t addr, uint8_t bit) {
    uint8_t temp;
    temp = readReg(camera, addr);
    temp = temp & bit;
    return temp;
}

void cameraSetFifoBurst(ArducamCamera *camera) {
    // arducamSpiTransfer(BURST_FIFO_READ);
}

uint8_t cameraReadByte(ArducamCamera *camera) {
    uint8_t data = cameraReadReg(camera, SINGLE_FIFO_READ);
    // arducamSpiCsPinLow(camera->csPin);
    // arducamSpiTransfer(SINGLE_FIFO_READ);
    // arducamSpiTransfer(0x00);
    // data = arducamSpiTransfer(0x00);
    // arducamSpiCsPinHigh(camera->csPin);
    camera->receivedLength -= 1;
    return data;
}

uint32_t cameraReadBuff(ArducamCamera *camera, uint8_t *buff, uint32_t length)

{
    if (imageAvailable(camera) == 0 || (length == 0)) {
        return 0;
    }

    if (camera->receivedLength < length) {
        length = camera->receivedLength;
    }

    // TODO Read block instead
    arducam_spi_read(buff, length, BURST_FIFO_READ, 1, camera->csPin);

    //     arducamSpiCsPinLow(camera->csPin);
    //     setFifoBurst(camera);
    if (camera->burstFirstFlag == 0) {
        camera->burstFirstFlag = 1;
        // arducamSpiTransfer(0x00);
    }

    // #ifndef arducamSpiReadBlock
    //     for (uint32_t count = 0; count < length; count++) {
    //         buff[count] = arducamSpiTransfer(0x00);
    //     }
    // #else
    //     arducamSpiReadBlock(buff, length);
    // #endif
    //     arducamSpiCsPinHigh(camera->csPin);
    camera->receivedLength -= length;
    return length;
}

void cameraWriteReg(ArducamCamera *camera, uint8_t addr, uint8_t val) {
    busWrite(camera, addr | 0x80, val);
}

uint8_t cameraReadReg(ArducamCamera *camera, uint8_t addr) {
    uint8_t data;
    data = busRead(camera, addr & 0x7F);
    // ns_lp_printf("addr %d data: %d\n", addr, data);
    return data;
}

uint8_t cameraBusWrite(ArducamCamera *camera, int address, int value) {
    // arducamSpiCsPinLow(camera->csPin);
    // arducamSpiTransfer(address);
    // arducamSpiTransfer(value);
    // arducamSpiCsPinHigh(camera->csPin);
    arducam_spi_write(&value, 1, address, 1, camera->csPin);
    // arducamDelayMs(1);
    return 1;
}

void cameraCsHigh(ArducamCamera *camera) { arducamSpiCsPinHigh(camera->csPin); }
void cameraCsLow(ArducamCamera *camera) { arducamSpiCsPinLow(camera->csPin); }

uint8_t cameraBusRead(ArducamCamera *camera, int address) {
    uint8_t value[2] = {0, 0};
    uint64_t long_address = address;
    arducam_spi_read(value, 2, long_address, 1, camera->csPin);

    // arducamSpiCsPinLow(camera->csPin);
    // arducamSpiTransfer(address);
    // value = arducamSpiTransfer(0x00);
    // value = arducamSpiTransfer(0x00);
    // arducamSpiCsPinHigh(camera->csPin);
    return value[1];
}

uint32_t cameraWaitI2cIdle(ArducamCamera *camera) {
    uint32_t count = NS_CAMERA_MAX_WAIT;
    while ((readReg(camera, CAM_REG_SENSOR_STATE) & 0X03) != CAM_REG_SENSOR_STATE_IDLE) {
        arducamDelayMs(2);
        count--;
        if (count == 0) {
            return NS_STATUS_FAILURE;
        }
    }
    return NS_STATUS_SUCCESS;
}

uint8_t cameraHeartBeat(ArducamCamera *camera) {
    return (readReg(camera, CAM_REG_SENSOR_STATE) & 0X03) == CAM_REG_SENSOR_STATE_IDLE;
}

void cameraDebugWriteRegister(ArducamCamera *camera, uint8_t *buff) {
    uint8_t register_high = buff[0];
    uint8_t register_low = buff[1];
    uint8_t value = buff[2];
    writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, register_high);
    writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, register_low);
    writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, value);
}

void cameraLowPowerOn(ArducamCamera *camera) {

    if ((camera->cameraId == SENSOR_5MP_2) || (camera->cameraId == SENSOR_3MP_2)) {
        uint16_t reg1 = 0x0028;
        uint16_t data1 = 0xD000;
        uint16_t reg2 = 0x002A;
        uint16_t data2 = 0x107E;
        uint16_t reg3 = 0x0F12;
        uint16_t data3 = 0x0001;
        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg1 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg1)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data1 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data1)&0xFF);
        waitI2cIdle(camera);

        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg2 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg2)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data2 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data2)&0xFF);
        waitI2cIdle(camera);

        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg3 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg3)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data3 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data3)&0xFF);
        waitI2cIdle(camera);

    } else {
        writeReg(camera, CAM_REG_POWER_CONTROL, 0X07);
    }
}

void cameraLowPowerOff(ArducamCamera *camera) {
    if ((camera->cameraId == SENSOR_5MP_2) || (camera->cameraId == SENSOR_3MP_2)) {
        uint16_t reg1 = 0x0028;
        uint16_t data1 = 0xD000;
        uint16_t reg2 = 0x002A;
        uint16_t data2 = 0x107E;
        uint16_t reg3 = 0x0F12;
        uint16_t data3 = 0x0000;
        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg1 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg1)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data1 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data1)&0xFF);
        waitI2cIdle(camera);

        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg2 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg2)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data2 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data2)&0xFF);
        waitI2cIdle(camera);

        writeReg(camera, CAM_REG_DEBUG_REGISTER_HIGH, (reg3 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_LOW, (reg3)&0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE_H, (data3 >> 8) & 0xFF);
        waitI2cIdle(camera);
        writeReg(camera, CAM_REG_DEBUG_REGISTER_VALUE, (data3)&0xFF);
        waitI2cIdle(camera);

    } else {
        writeReg(camera, CAM_REG_POWER_CONTROL, 0X05);
    }
}

CamStatus reset(ArducamCamera *camera) { return camera->arducamCameraOp->reset(camera); }

CamStatus begin(ArducamCamera *camera) { return camera->arducamCameraOp->begin(camera); }

CamStatus takePicture(ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format) {
    return camera->arducamCameraOp->takePicture(camera, mode, pixel_format);
}

CamStatus takeMultiPictures(
    ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format, uint8_t num) {
    return camera->arducamCameraOp->takeMultiPictures(camera, mode, pixel_format, num);
}

CamStatus startPreview(ArducamCamera *camera, CAM_VIDEO_MODE mode) {
    return camera->arducamCameraOp->startPreview(camera, mode);
}

void captureThread(ArducamCamera *camera) { camera->arducamCameraOp->captureThread(camera); }

CamStatus stopPreview(ArducamCamera *camera) {
    return camera->arducamCameraOp->stopPreview(camera);
}

CamStatus setAutoExposure(ArducamCamera *camera, uint8_t val) {
    return camera->arducamCameraOp->setAutoExposure(camera, val);
}
CamStatus setAbsoluteExposure(ArducamCamera *camera, uint32_t val) {
    return camera->arducamCameraOp->setAbsoluteExposure(camera, val);
}
CamStatus setAutoISOSensitive(ArducamCamera *camera, uint8_t val) {
    return camera->arducamCameraOp->setAutoISOSensitive(camera, val);
}
CamStatus setISOSensitivity(ArducamCamera *camera, int iso_sense) {
    return camera->arducamCameraOp->setISOSensitivity(camera, iso_sense);
}

CamStatus setAutoWhiteBalance(ArducamCamera *camera, uint8_t val) {
    return camera->arducamCameraOp->setAutoWhiteBalance(camera, val);
}

CamStatus setAutoWhiteBalanceMode(ArducamCamera *camera, CAM_WHITE_BALANCE mode) {
    return camera->arducamCameraOp->setAutoWhiteBalanceMode(camera, mode);
}
CamStatus setColorEffect(ArducamCamera *camera, CAM_COLOR_FX effect) {
    return camera->arducamCameraOp->setColorEffect(camera, effect);
}

CamStatus setAutoFocus(ArducamCamera *camera, uint8_t val) {
    return camera->arducamCameraOp->setAutoFocus(camera, val);
}

uint8_t getAutoFocusSta(ArducamCamera *camera) {
    return camera->arducamCameraOp->getAutoFocusSta(camera);
}

CamStatus setManualFocus(ArducamCamera *camera, uint16_t val) {
    return camera->arducamCameraOp->setManualFocus(camera, val);
}

CamStatus setSaturation(ArducamCamera *camera, CAM_STAURATION_LEVEL level) {
    return camera->arducamCameraOp->setSaturation(camera, level);
}

CamStatus setEV(ArducamCamera *camera, CAM_EV_LEVEL level) {
    return camera->arducamCameraOp->setEV(camera, level);
}

CamStatus setContrast(ArducamCamera *camera, CAM_CONTRAST_LEVEL level) {
    return camera->arducamCameraOp->setContrast(camera, level);
}

CamStatus setBrightness(ArducamCamera *camera, CAM_BRIGHTNESS_LEVEL level) {
    return camera->arducamCameraOp->setBrightness(camera, level);
}

CamStatus setSharpness(ArducamCamera *camera, CAM_SHARPNESS_LEVEL level) {
    return camera->arducamCameraOp->setSharpness(camera, level);
}

uint32_t readBuff(ArducamCamera *camera, uint8_t *buff, uint32_t length) {
    return camera->arducamCameraOp->readBuff(camera, buff, length);
}

uint8_t readByte(ArducamCamera *camera) { return camera->arducamCameraOp->readByte(camera); }

void debugWriteRegister(ArducamCamera *camera, uint8_t *buff) {
    camera->arducamCameraOp->debugWriteRegister(camera, buff);
}

void registerCallback(
    ArducamCamera *camera, BUFFER_CALLBACK function, uint8_t blockSize, STOP_HANDLE handle) {
    camera->arducamCameraOp->registerCallback(camera, function, blockSize, handle);
}

void lowPowerOn(ArducamCamera *camera) { camera->arducamCameraOp->lowPowerOn(camera); }
void lowPowerOff(ArducamCamera *camera) { camera->arducamCameraOp->lowPowerOff(camera); }

void csHigh(ArducamCamera *camera) { camera->arducamCameraOp->csHigh(camera); }
void csLow(ArducamCamera *camera) { camera->arducamCameraOp->csLow(camera); }

void writeReg(ArducamCamera *camera, uint8_t addr, uint8_t val) {
    camera->arducamCameraOp->writeReg(camera, addr, val);
}

uint8_t readReg(ArducamCamera *camera, uint8_t addr) {
    return camera->arducamCameraOp->readReg(camera, addr);
}

uint8_t busRead(ArducamCamera *camera, int address) {
    return camera->arducamCameraOp->busRead(camera, address);
}

uint8_t busWrite(ArducamCamera *camera, int address, int value) {
    return camera->arducamCameraOp->busWrite(camera, address, value);
}

void startCapture(ArducamCamera *camera) { camera->arducamCameraOp->startCapture(camera); }
void clearFifoFlag(ArducamCamera *camera) { camera->arducamCameraOp->clearFifoFlag(camera); }
uint32_t readFifoLength(ArducamCamera *camera) {
    return camera->arducamCameraOp->readFifoLength(camera);
}
uint8_t getBit(ArducamCamera *camera, uint8_t addr, uint8_t bit) {
    return camera->arducamCameraOp->getBit(camera, addr, bit);
}
void setFifoBurst(ArducamCamera *camera) { camera->arducamCameraOp->setFifoBurst(camera); }
void setCapture(ArducamCamera *camera) { camera->arducamCameraOp->setCapture(camera); }
uint32_t waitI2cIdle(ArducamCamera *camera) { return camera->arducamCameraOp->waitI2cIdle(camera); }

uint32_t imageAvailable(ArducamCamera *camera) {
    return camera->arducamCameraOp->imageAvailable(camera);
}

void flushFifo(ArducamCamera *camera) { camera->arducamCameraOp->flushFifo(camera); }
CamStatus setImageQuality(ArducamCamera *camera, IMAGE_QUALITY qualtiy) {
    return camera->arducamCameraOp->setImageQuality(camera, qualtiy);
}

const struct CameraOperations ArducamcameraOperations = {
    .reset = cameraReset,
    .begin = cameraBegin,
    .takePicture = cameraTakePicture,
    .takeMultiPictures = cameratakeMultiPictures,
    .startPreview = cameraStartPreview,
    .captureThread = cameraCaptureThread,
    .stopPreview = cameraStopPreview,
    .setAutoExposure = cameraSetAutoExposure,
    .setAbsoluteExposure = cameraSetAbsoluteExposure,
    .setAutoISOSensitive = cameraSetAutoISOSensitive,
    .setISOSensitivity = cameraSetISOSensitivity,
    .setAutoWhiteBalance = cameraSetAutoWhiteBalance,
    .setAutoWhiteBalanceMode = cameraSetAutoWhiteBalanceMode,
    .setColorEffect = cameraSetColorEffect,
    .setAutoFocus = cameraSetAutoFocus,
    .getAutoFocusSta = cameraGetAutoFocusSta,
    .setManualFocus = cameraSetManualFocus,
    .setSaturation = cameraSetSaturation,
    .setEV = cameraSetEV,
    .setContrast = cameraSetContrast,
    .setBrightness = cameraSetBrightness,
    .setSharpness = cameraSetSharpness,
    .registerCallback = cameraRegisterCallback,
    .imageAvailable = cameraImageAvailable,
    .csHigh = cameraCsHigh,
    .csLow = cameraCsLow,
    .readBuff = cameraReadBuff,
    .readByte = cameraReadByte,
    .debugWriteRegister = cameraDebugWriteRegister,
    .writeReg = cameraWriteReg,
    .readReg = cameraReadReg,
    .busRead = cameraBusRead,
    .busWrite = cameraBusWrite,
    .flushFifo = cameraFlushFifo,
    .startCapture = cameraStartCapture,
    .clearFifoFlag = cameraClearFifoFlag,
    .readFifoLength = cameraReadFifoLength,
    .getBit = cameraGetBit,
    .setFifoBurst = cameraSetFifoBurst,
    .setCapture = cameraSetCapture,
    .waitI2cIdle = cameraWaitI2cIdle,
    .lowPowerOn = cameraLowPowerOn,
    .lowPowerOff = cameraLowPowerOff,
    .setImageQuality = cameraSetImageQuality,
};

extern ArducamCamera camera;

ArducamCamera createArducamCamera(int CS) {
    // ArducamCamera camera;
    CameraType[0] = &CameraInfo_5MP;
    CameraType[1] = &CameraInfo_3MP;
    // cameraDefaultInfo[0] = &DefaultState_5mp;
    // cameraDefaultInfo[1] = &DefaultState_3mp;
    camera.cameraId = FALSE;
    camera.currentPixelFormat = CAM_IMAGE_PIX_FMT_NONE;
    camera.currentPictureMode = CAM_IMAGE_MODE_NONE;
    camera.burstFirstFlag = FALSE;
    camera.previewMode = FALSE;
    camera.csPin = CS;
    camera.arducamCameraOp = &ArducamcameraOperations;
    camera.currentSDK = &currentSDK;
    cameraInit(&camera);
    return camera;
}

/// @endcond
