/**
 * @file ns_i2c_register_driver.c
 * @author Carlos Morales
 * @brief Generic i2c driver for register-based i2c devices
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_i2c_register_driver.h"
#include "am_hal_iom.h"
#include "ns_i2c.h"

/**
 * @brief Read sequential 8-bit registers over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr First 8-bit register address
 * @param buf Buffer to store register values
 * @param size Number of registers to read
 * @return uint32_t status
 */
uint32_t ns_i2c_read_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size) {
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) || defined(AM_PART_APOLLO5A) || defined(AM_PART_APOLLO5B)
    Transaction.ui64Instr = regAddr;
#else
    Transaction.ui32Instr = (uint32_t)regAddr;
#endif
    Transaction.eDirection = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes = size;
    Transaction.pui32RxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = devAddr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Write sequential 8-bit registers over I2C
 *
 * @param cfg I2C configuration
 * @param devAddr Device address
 * @param regAddr First 8-bit register address
 * @param buf Pointer to buffer of values to be written
 * @param size Number of bytes to write
 * @return uint32_t status
 */
uint32_t ns_i2c_write_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size) {
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) || defined(AM_PART_APOLLO5A) || defined(AM_PART_APOLLO5B)
    Transaction.ui64Instr = regAddr;
#else
    Transaction.ui32Instr = (uint32_t)regAddr;
#endif
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = size;
    Transaction.pui32TxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = devAddr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction) != AM_HAL_STATUS_SUCCESS) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Read 8-bit register over I2C
 *
 * @param cfg Handle obtained from ns_i2c_interface_init
 * @param devAddr Device address
 * @param regAddr 8-bit register address
 * @param value Register value
 * @param mask Read mask
 * @return uint32_t status
 */
uint32_t ns_i2c_read_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t *value, uint8_t mask) {
    uint32_t regValue;
    if (ns_i2c_read_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
        return NS_I2C_STATUS_ERROR;
    }
    if (mask != 0xFF) {
        regValue = regValue & mask;
    }
    (*value) = (uint8_t)regValue;
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Write 8-bit register over I2C
 *
 * @param cfg Handle obtained from ns_i2c_interface_init
 * @param devAddr Device address
 * @param regAddr 8-bit register address
 * @param value Register value
 * @param mask Write mask
 * @return uint32_t status
 */
uint32_t ns_i2c_write_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t value, uint8_t mask) {
    uint32_t regValue = value;
    if (mask != 0xFF) {
        if (ns_i2c_read_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
            return NS_I2C_STATUS_ERROR;
        }
        regValue = (regValue & ~mask) | (value & mask);
    }
    if (ns_i2c_write_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}
