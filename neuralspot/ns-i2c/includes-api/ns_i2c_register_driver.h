/**
 * @file ns_i2c_register_driver.h
 * @author Carlos Morales
 * @brief Generic i2c driver for register-based i2c devices
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef NS_I2C_REGISTER_DRIVER
#define NS_I2C_REGISTER_DRIVER

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

typedef enum
{
    NS_I2C_STATUS_SUCCESS = 0,
    NS_I2C_STATUS_ERROR = 1
} ns_i2c_status_t;

typedef struct {
    int8_t address; ///< Device address
    int8_t iom;     ///< Apollo4 IOM port
    void *iom_handle;
    am_hal_iom_config_t sIomCfg; 
} ns_i2cDriverConfig_t;

extern uint32_t
ns_i2c_interface_init(uint32_t ui32Module, uint8_t devAddress, void **pIomHandle);

extern uint32_t
ns_i2c_write_bits(ns_i2cDriverConfig_t *h, uint8_t regAddr, uint8_t val, uint8_t bitOffset, uint8_t len);

extern uint32_t
ns_i2c_reg_write(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size);

extern uint32_t
ns_i2c_reg_read(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // NS_I2C_REGISTER_DRIVER