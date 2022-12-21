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
extern "C" {
#endif

#include "ns_i2c.h"

/**
 * @brief Read 8-bit register over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr Register address
 * @param value Register Value
 * @param mask Read mask
 * @return uint32_t status
 */
uint32_t
ns_i2c_read_reg(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t *value,
                uint8_t mask);

/**
 * @brief Write 8-bit register over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr Register address
 * @param value Register Value
 * @param mask Write mask
 * @return uint32_t status
 */
uint32_t
ns_i2c_write_reg(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t value,
                 uint8_t mask);

/**
 * @brief Read sequential 8-bit registers over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr First register address
 * @param buf Buffer to store register values
 * @param size Number of registers to read
 * @return uint32_t status
 */
uint32_t
ns_i2c_read_sequential_regs(ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf,
                            uint32_t size);

/**
 * @brief Write sequential 8-bit registers over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr First register address
 * @param buf Pointer to buffer of values to be written
 * @param size Number of bytes to write
 * @return uint32_t status
 */
uint32_t
ns_i2c_write_sequential_regs(ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf,
                             uint32_t size);

#ifdef __cplusplus
}
#endif

/** @}*/
#endif // NS_I2C_REGISTER_DRIVER
