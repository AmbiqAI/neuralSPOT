/**
 * @file ns_i2c_register_driver.h
 * @author Carlos Morales
 * @brief Generic i2c driver for register-based i2c devices
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 *  \addtogroup NeuralSPOT-i2c
 *  @{
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

/// i2c Configration
typedef struct {
    int8_t address; ///< Device address
    int8_t iom;     ///< Apollo4 IOM port (only 0 and 1 supported)
    void *iom_handle; ///< AmbiqSuite IOM handle, filled in by init
    am_hal_iom_config_t sIomCfg; ///< Config for AmbiqSuite IOM, filled in by init
} ns_i2cDriverConfig_t;

/**
 * @brief Initialize an i2c interface
 *  
 * @param ui32Module Which IOM the i2c is connected to (current 0 or 1)
 * @param devAddress Address of the device connected to this i2c
 * @param pIomHandle i2c handle, initialized by init(), use this for subsequent accesses
 * @return uint32_t 
 */
extern uint32_t
ns_i2c_interface_init(uint32_t ui32Module, uint8_t devAddress, void **pIomHandle);

/**
 * @brief Write a range of bits
 * 
 * Note that the register will be read before being written
 * 
 * @param h i2c handle (from ns_i2c_interface_init())
 * @param regAddr Device register address
 * @param val Value to be written
 * @param bitOffset Offset of bit field to be written
 * @param len  Length of bit field to be written
 * @return uint32_t sucess/failure
 */
extern uint32_t
ns_i2c_write_bits(ns_i2cDriverConfig_t *h, uint8_t regAddr, uint8_t val, uint8_t bitOffset, uint8_t len);

/**
 * @brief Write one or more bytes to a register or series of registers
 * 
 * @param h i2c handle (from ns_i2c_interface_init())
 * @param regAddr Device register address of first register to be written
 * @param pBuf Pointer to buffer containing values to be written
 * @param size Number of bytes to be written
 * @return uint32_t sucess/failure
 */
extern uint32_t
ns_i2c_reg_write(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size);

/**
 * @brief Read one or more bytes to a register or series of registers
 * 
 * @param h i2c handle (from ns_i2c_interface_init())
 * @param regAddr Device register address of first register to be read
 * @param pBuf Pointer to buffer where read values will be placed
 * @param size Number of bytes to be read
 * @return uint32_t sucess/failure
 */
extern uint32_t
ns_i2c_reg_read(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_I2C_REGISTER_DRIVER