

# File ns\_i2c.h

[**File List**](files.md) **>** [**includes-api**](dir_ead24bed0d4d5fb1b23b4266f13c1ec5.md) **>** [**ns\_i2c.h**](ns__i2c_8h.md)

[Go to the documentation of this file](ns__i2c_8h.md)

```C++

#ifndef NS_I2C
    #define NS_I2C

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_core.h"

    #define NS_I2C_V0_0_1                                                                          \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_I2C_V1_0_0                                                                          \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_I2C_OLDEST_SUPPORTED_VERSION NS_I2C_V0_0_1
    #define NS_I2C_CURRENT_VERSION NS_I2C_V1_0_0
    #define NS_I2C_API_ID 0xCA0004

extern const ns_core_api_t ns_i2c_V0_0_1;
extern const ns_core_api_t ns_i2c_V1_0_0;
extern const ns_core_api_t ns_i2c_oldest_supported_version;
extern const ns_core_api_t ns_i2c_current_version;

typedef enum { NS_I2C_STATUS_SUCCESS = 0, NS_I2C_STATUS_ERROR = 1 } ns_i2c_status_e;

typedef struct {
    const ns_core_api_t *api; 
    int8_t iom;               

    // Internal state
    void *iomHandle;             // AmbiqSuite IOM handle
    am_hal_iom_config_t sIomCfg; //  AmbiqSuite IOM config
} ns_i2c_config_t;

// I2C Transfer Flags
typedef enum { NS_I2C_XFER_WR = 0, NS_I2C_XFER_RD = (1u << 0) } ns_i2c_xfer_flag_e;

// I2C Transfer Message
typedef struct {
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
} ns_i2c_msg_t;

uint32_t ns_i2c_interface_init(ns_i2c_config_t *cfg, uint32_t speed);

uint32_t ns_i2c_read(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr);

uint32_t ns_i2c_write(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr);

uint32_t ns_i2c_write_read(
    ns_i2c_config_t *cfg, uint16_t addr, const void *writeBuf, size_t numWrite, void *readBuf,
    size_t numRead);

uint32_t ns_i2c_transfer(ns_i2c_config_t *cfg, ns_i2c_msg_t *msgs, size_t numMsgs);

    #ifdef __cplusplus
}
    #endif

#endif // NS_IO_I2C

```

