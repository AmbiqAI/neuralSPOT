

# File ArducamCamera.h

[**File List**](files.md) **>** [**includes-api**](dir_01be6a440053b8b57cd3acfc114f4ae9.md) **>** [**ArducamCamera.h**](_arducam_camera_8h.md)

[Go to the documentation of this file](_arducam_camera_8h.md)

```C++

/*
 * This file is part of the Arducam SPI Camera project.
 *
 * Copyright 2021 Arducam Technology co., Ltd. All Rights Reserved.
 *
 * This work is licensed under the MIT license, see the file LICENSE for
 * details.
 *
 */

#ifndef __ARDUCAM_H
#define __ARDUCAM_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define TRUE 1
#define FALSE 0
// #ifndef Bool
// #define Bool char
// #endif
// typedef enum { FALSE = 0, TRUE = !FALSE } bool;


struct SdkDate {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint16_t version;
} __attribute__((packed)); // 8bit align

union SdkInfo {
    uint8_t sdkVersion[5]; 
    struct SdkDate sdkInfo;
};

struct CameraInfo {
    char *cameraId;                 
    int supportResolution;          
    int supportSpecialEffects;      
    unsigned long exposureValueMax; 
    unsigned int exposureValueMin;  
    unsigned int gainValueMax;      
    unsigned int gainValueMin;      
    unsigned char supportFocus;     
    unsigned char supportSharpness; 
    unsigned char deviceAddress;
};

typedef enum {
    CAM_ERR_SUCCESS = 0,      
    CAM_ERR_NO_CALLBACK = -1, 
} CamStatus;

typedef enum {
    CAM_IMAGE_MODE_QQVGA = 0x00,   
    CAM_IMAGE_MODE_QVGA = 0x01,    
    CAM_IMAGE_MODE_VGA = 0x02,     
    CAM_IMAGE_MODE_SVGA = 0x03,    
    CAM_IMAGE_MODE_HD = 0x04,      
    CAM_IMAGE_MODE_SXGAM = 0x05,   
    CAM_IMAGE_MODE_UXGA = 0x06,    
    CAM_IMAGE_MODE_FHD = 0x07,     
    CAM_IMAGE_MODE_QXGA = 0x08,    
    CAM_IMAGE_MODE_WQXGA2 = 0x09,  
    CAM_IMAGE_MODE_96X96 = 0x0a,   
    CAM_IMAGE_MODE_128X128 = 0x0b, 
    CAM_IMAGE_MODE_320X320 = 0x0c, 
    CAM_IMAGE_MODE_12 = 0x0d, 
    CAM_IMAGE_MODE_13 = 0x0e, 
    CAM_IMAGE_MODE_14 = 0x0f, 
    CAM_IMAGE_MODE_15 = 0x10, 
    CAM_IMAGE_MODE_NONE,
} CAM_IMAGE_MODE;

typedef enum {
    CAM_CONTRAST_LEVEL_MINUS_3 = 6, 
    CAM_CONTRAST_LEVEL_MINUS_2 = 4, 
    CAM_CONTRAST_LEVEL_MINUS_1 = 2, 
    CAM_CONTRAST_LEVEL_DEFAULT = 0, 
    CAM_CONTRAST_LEVEL_1 = 1,       
    CAM_CONTRAST_LEVEL_2 = 3,       
    CAM_CONTRAST_LEVEL_3 = 5,       
} CAM_CONTRAST_LEVEL;

typedef enum {
    CAM_EV_LEVEL_MINUS_3 = 6, 
    CAM_EV_LEVEL_MINUS_2 = 4, 
    CAM_EV_LEVEL_MINUS_1 = 2, 
    CAM_EV_LEVEL_DEFAULT = 0, 
    CAM_EV_LEVEL_1 = 1,       
    CAM_EV_LEVEL_2 = 3,       
    CAM_EV_LEVEL_3 = 5,       
} CAM_EV_LEVEL;

typedef enum {
    CAM_STAURATION_LEVEL_MINUS_3 = 6, 
    CAM_STAURATION_LEVEL_MINUS_2 = 4, 
    CAM_STAURATION_LEVEL_MINUS_1 = 2, 
    CAM_STAURATION_LEVEL_DEFAULT = 0, 
    CAM_STAURATION_LEVEL_1 = 1,       
    CAM_STAURATION_LEVEL_2 = 3,       
    CAM_STAURATION_LEVEL_3 = 5,       
} CAM_STAURATION_LEVEL;

typedef enum {
    CAM_BRIGHTNESS_LEVEL_MINUS_4 = 8, 
    CAM_BRIGHTNESS_LEVEL_MINUS_3 = 6, 
    CAM_BRIGHTNESS_LEVEL_MINUS_2 = 4, 
    CAM_BRIGHTNESS_LEVEL_MINUS_1 = 2, 
    CAM_BRIGHTNESS_LEVEL_DEFAULT = 0, 
    CAM_BRIGHTNESS_LEVEL_1 = 1,       
    CAM_BRIGHTNESS_LEVEL_2 = 3,       
    CAM_BRIGHTNESS_LEVEL_3 = 5,       
    CAM_BRIGHTNESS_LEVEL_4 = 7,       
} CAM_BRIGHTNESS_LEVEL;

typedef enum {
    CAM_SHARPNESS_LEVEL_AUTO = 0, 
    CAM_SHARPNESS_LEVEL_1,        
    CAM_SHARPNESS_LEVEL_2,        
    CAM_SHARPNESS_LEVEL_3,        
    CAM_SHARPNESS_LEVEL_4,        
    CAM_SHARPNESS_LEVEL_5,        
    CAM_SHARPNESS_LEVEL_6,        
    CAM_SHARPNESS_LEVEL_7,        
    CAM_SHARPNESS_LEVEL_8,        
} CAM_SHARPNESS_LEVEL;

typedef enum {
    CAM_VIDEO_MODE_0 = 1, 
    CAM_VIDEO_MODE_1 = 2, 
} CAM_VIDEO_MODE;

typedef enum {
    CAM_IMAGE_PIX_FMT_RGB565 = 0x02, 
    CAM_IMAGE_PIX_FMT_JPG = 0x01,    
    CAM_IMAGE_PIX_FMT_YUV = 0x03,    
    CAM_IMAGE_PIX_FMT_NONE,          
} CAM_IMAGE_PIX_FMT;

typedef enum {
    CAM_WHITE_BALANCE_MODE_DEFAULT = 0, 
    CAM_WHITE_BALANCE_MODE_SUNNY,       
    CAM_WHITE_BALANCE_MODE_OFFICE,      
    CAM_WHITE_BALANCE_MODE_CLOUDY,      
    CAM_WHITE_BALANCE_MODE_HOME,        
} CAM_WHITE_BALANCE;

typedef enum {
    CAM_COLOR_FX_NONE = 0,      
    CAM_COLOR_FX_BLUEISH,       
    CAM_COLOR_FX_REDISH,        
    CAM_COLOR_FX_BW,            
    CAM_COLOR_FX_SEPIA,         
    CAM_COLOR_FX_NEGATIVE,      
    CAM_COLOR_FX_GRASS_GREEN,   
    CAM_COLOR_FX_OVER_EXPOSURE, 
    CAM_COLOR_FX_SOLARIZE,      
} CAM_COLOR_FX;

typedef enum {
    HIGH_QUALITY = 0,
    DEFAULT_QUALITY = 1,
    LOW_QUALITY = 2,
} IMAGE_QUALITY;

enum {
    SENSOR_5MP_1 = 0x81,
    SENSOR_3MP_1 = 0x82,
    SENSOR_5MP_2 = 0x83, /* 2592x1936 */
    SENSOR_3MP_2 = 0x84,
};

typedef uint8_t (*BUFFER_CALLBACK)(
    uint8_t *buffer, uint8_t lenght); 
typedef void (*STOP_HANDLE)(void);    
typedef struct {
    int csPin;               
    uint32_t totalLength;    
    uint32_t receivedLength; 
    uint8_t blockSize;       
    uint8_t cameraId;        
    // uint8_t cameraDataFormat;                       /**< The currently set image pixel format */
    uint8_t burstFirstFlag;         
    uint8_t previewMode;            
    uint8_t currentPixelFormat;     
    uint8_t currentPictureMode;     
    struct CameraInfo myCameraInfo; 
    const struct CameraOperations *arducamCameraOp; 
    BUFFER_CALLBACK callBackFunction;               
    STOP_HANDLE handle;
    uint8_t verDateAndNumber[4]; 
    union SdkInfo *currentSDK;   
} ArducamCamera;

struct CameraOperations {
    CamStatus (*reset)(ArducamCamera *);
    CamStatus (*begin)(ArducamCamera *);
    CamStatus (*takePicture)(ArducamCamera *, CAM_IMAGE_MODE, CAM_IMAGE_PIX_FMT);
    CamStatus (*takeMultiPictures)(ArducamCamera *, CAM_IMAGE_MODE, CAM_IMAGE_PIX_FMT, uint8_t);
    CamStatus (*startPreview)(ArducamCamera *, CAM_VIDEO_MODE);
    void (*captureThread)(ArducamCamera *);
    CamStatus (*stopPreview)(ArducamCamera *);
    CamStatus (*setAutoExposure)(ArducamCamera *, uint8_t);
    CamStatus (*setAbsoluteExposure)(ArducamCamera *, uint32_t);
    CamStatus (*setAutoISOSensitive)(ArducamCamera *, uint8_t);
    CamStatus (*setISOSensitivity)(ArducamCamera *, int);
    CamStatus (*setAutoWhiteBalance)(ArducamCamera *, uint8_t);
    CamStatus (*setAutoWhiteBalanceMode)(ArducamCamera *, CAM_WHITE_BALANCE);
    CamStatus (*setColorEffect)(ArducamCamera *, CAM_COLOR_FX);
    CamStatus (*setAutoFocus)(ArducamCamera *, uint8_t);
    uint8_t (*getAutoFocusSta)(ArducamCamera *);
    CamStatus (*setManualFocus)(ArducamCamera *, uint16_t);
    CamStatus (*setSaturation)(ArducamCamera *, CAM_STAURATION_LEVEL);
    CamStatus (*setEV)(ArducamCamera *, CAM_EV_LEVEL);
    CamStatus (*setContrast)(ArducamCamera *, CAM_CONTRAST_LEVEL);
    CamStatus (*setBrightness)(ArducamCamera *, CAM_BRIGHTNESS_LEVEL);
    CamStatus (*setSharpness)(ArducamCamera *, CAM_SHARPNESS_LEVEL);
    CamStatus (*setImageQuality)(ArducamCamera *camera, IMAGE_QUALITY qualtiy);
    uint32_t (*imageAvailable)(ArducamCamera *);
    void (*csHigh)(ArducamCamera *);
    void (*csLow)(ArducamCamera *);
    uint32_t (*readBuff)(ArducamCamera *, uint8_t *, uint32_t);
    uint8_t (*readByte)(ArducamCamera *);
    void (*debugWriteRegister)(ArducamCamera *, uint8_t *);
    void (*writeReg)(ArducamCamera *, uint8_t, uint8_t);
    uint8_t (*readReg)(ArducamCamera *, uint8_t);
    uint8_t (*busRead)(ArducamCamera *, int);
    uint8_t (*busWrite)(ArducamCamera *, int, int);
    void (*flushFifo)(ArducamCamera *);
    void (*startCapture)(ArducamCamera *);
    void (*clearFifoFlag)(ArducamCamera *);
    uint32_t (*readFifoLength)(ArducamCamera *);
    uint8_t (*getBit)(ArducamCamera *, uint8_t, uint8_t);
    void (*setFifoBurst)(ArducamCamera *);
    void (*setCapture)(ArducamCamera *);
    void (*waitI2cIdle)(ArducamCamera *);
    void (*lowPowerOn)(ArducamCamera *);
    void (*lowPowerOff)(ArducamCamera *);
    void (*registerCallback)(ArducamCamera *, BUFFER_CALLBACK, uint8_t, STOP_HANDLE);
};


//**********************************************
//**********************************************
ArducamCamera createArducamCamera(int cs);

//**********************************************
//**********************************************
CamStatus reset(ArducamCamera *camera);

//**********************************************
//**********************************************

CamStatus begin(ArducamCamera *camera);

//**********************************************
CamStatus takePicture(ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format);

//**********************************************
//**********************************************
CamStatus takeMultiPictures(
    ArducamCamera *camera, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format, uint8_t number);

//**********************************************
//**********************************************
CamStatus startPreview(ArducamCamera *camera, CAM_VIDEO_MODE mode);

void captureThread(ArducamCamera *camera);

//**********************************************
//**********************************************
CamStatus stopPreview(ArducamCamera *camera);

//**********************************************
//**********************************************
CamStatus setAutoExposure(ArducamCamera *camera, uint8_t val);

//**********************************************
//**********************************************
CamStatus setAbsoluteExposure(ArducamCamera *camera, uint32_t val);

//**********************************************
//**********************************************
CamStatus setAutoISOSensitive(ArducamCamera *camera, uint8_t val);

//**********************************************
//**********************************************
CamStatus setISOSensitivity(ArducamCamera *camera, int iso_sense);

//**********************************************
//**********************************************
CamStatus setAutoWhiteBalance(ArducamCamera *camera, uint8_t val);

//**********************************************
//**********************************************
CamStatus setAutoWhiteBalanceMode(ArducamCamera *camera, CAM_WHITE_BALANCE mode);

//**********************************************
//**********************************************
CamStatus setColorEffect(ArducamCamera *camera, CAM_COLOR_FX effect);

//**********************************************
//**********************************************
CamStatus setAutoFocus(ArducamCamera *camera, uint8_t val);

//**********************************************
//**********************************************
uint8_t getAutoFocusSta(ArducamCamera *camera);

//**********************************************
//**********************************************
CamStatus setManualFocus(ArducamCamera *camera, uint16_t val);

//**********************************************
//**********************************************
CamStatus setSaturation(ArducamCamera *camera, CAM_STAURATION_LEVEL level);

//**********************************************
//**********************************************
CamStatus setEV(ArducamCamera *camera, CAM_EV_LEVEL level);

//**********************************************
//********************************************
CamStatus setContrast(ArducamCamera *camera, CAM_CONTRAST_LEVEL level);

//**********************************************
//**********************************************
CamStatus setBrightness(ArducamCamera *camera, CAM_BRIGHTNESS_LEVEL level);

//**********************************************
//**********************************************
CamStatus setSharpness(ArducamCamera *camera, CAM_SHARPNESS_LEVEL level);

//**********************************************
//**********************************************
CamStatus setImageQuality(ArducamCamera *camera, IMAGE_QUALITY qualtiy);

//**********************************************
//**********************************************
uint32_t readBuff(ArducamCamera *camera, uint8_t *buff, uint32_t length);

//**********************************************
//**********************************************
uint8_t readByte(ArducamCamera *camera);

//**********************************************
//**********************************************
void debugWriteRegister(ArducamCamera *camera, uint8_t *buff);

//**********************************************
//**********************************************
void registerCallback(
    ArducamCamera *camera, BUFFER_CALLBACK function, uint8_t blockSize, STOP_HANDLE handle);

//**********************************************
//**********************************************
void lowPowerOn(ArducamCamera *camera);

//**********************************************
//**********************************************
void lowPowerOff(ArducamCamera *camera);

//**********************************************
//**********************************************
uint8_t cameraHeartBeat(ArducamCamera *camera);

typedef enum { Camera_uninit = 0, Camera_init, Camera_open, Camera_close } CameraStatus;

struct ClassCamera {
    ArducamCamera cam;
    CameraStatus status;
};

uint8_t cameraReadReg(ArducamCamera *camera, uint8_t addr);
uint32_t cameraImageAvailable(ArducamCamera *camera);
uint32_t cameraReadFifoLength(ArducamCamera *camera);

#ifdef __cplusplus
}
#endif
#endif /*__ARDUCAM_H*/

```

