/**
 * @file ns_i2c.c
 * @author Adam Page
 * @brief Generic i2c driver
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_i2c.h"

/**
 * @brief Initialize I2C on one of the IOM (IO managers)
 * @param cfg I2C configuration
 * @param speed I2C speed in Hz
 * @return uint32_t status
 */
uint32_t ns_i2c_interface_init(ns_i2c_config_t *cfg, uint32_t speed) {
    // Initialize local state
    cfg->sIomCfg.eInterfaceMode = AM_HAL_IOM_I2C_MODE;
    cfg->sIomCfg.ui32ClockFreq = speed;
    cfg->sIomCfg.pNBTxnBuf = NULL;
    cfg->sIomCfg.ui32NBTxnBufLength = 0;

    am_hal_pwrctrl_periph_enable((am_hal_pwrctrl_periph_e)(AM_HAL_PWRCTRL_PERIPH_IOM0 + cfg->iom));

    // Initialize the IOM instance.
    // Enable power to the IOM instance.
    // Configure the IOM for Serial operation during initialization.
    // Enable the IOM.
    // HAL Success return is 0
    //
    if (am_hal_iom_initialize(cfg->iom, &(cfg->iomHandle)) ||
        am_hal_iom_power_ctrl(cfg->iomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(cfg->iomHandle, &(cfg->sIomCfg)) ||
        am_hal_iom_enable(cfg->iomHandle))
    {
        return NS_I2C_STATUS_ERROR;
    }

    // Configure the IOM pins.
    am_bsp_iom_pins_enable(cfg->iom, AM_HAL_IOM_I2C_MODE);
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Perform low-level I2C read using IOM transfer
 * @param cfg I2C configuration
 * @param buf Buffer to store read bytes
 * @param size Number of bytes to read
 * @param addr I2C device address
 */
uint32_t ns_i2c_read(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr) {
    am_hal_iom_transfer_t       Transaction;
    Transaction.ui8Priority     = 1;
    Transaction.ui32InstrLen    = 0;
    Transaction.ui64Instr       = 0;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32RxBuffer   = (uint32_t *)buf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = addr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Perform low-level I2C write using IOM transfer
 * @param cfg I2C configuration
 * @param buf Buffer of bytes to write
 * @param size Number of bytes to write
 * @param addr I2C device address
 */
uint32_t ns_i2c_write(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr) {
    am_hal_iom_transfer_t       Transaction;
    Transaction.ui8Priority     = 1;
    Transaction.ui32InstrLen    = 0;
    Transaction.ui64Instr       = 0;
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32TxBuffer   = (uint32_t *)buf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = addr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

/**
 * @brief Perform low-level I2C write followed by immediate read
 * @param cfg I2C configuration
 * @param writeBuf Write buffer
 * @param numWrite Number of bytes to write
 * @param readBuf Read buffer
 * @param numRead Number of bytes to read
 * @param addr I2C device address
 */
uint32_t ns_i2c_write_read(ns_i2c_config_t *cfg, uint16_t addr, const void *writeBuf, size_t numWrite, void *readBuf, size_t numRead) {
    ns_i2c_write(cfg, writeBuf, numWrite, addr);
    return ns_i2c_read(cfg, readBuf, numRead, addr);
}

/**
 * @brief Perform sequence of low-level I2C transfers (similar to Linux)
 * @param cfg I2C configuration
 * @param msgs I2C messages to transfer
 * @param numMsgs Number of I2C messsages
 */
uint32_t ns_i2c_transfer(ns_i2c_config_t *cfg, ns_i2c_msg_t *msgs, size_t numMsgs) {
    ns_i2c_msg_t *msg;
    uint32_t msg_len = 0;
    for (size_t i = 0; i < numMsgs; i++){
        msg = &msgs[i];
        if (msg->flags == NS_I2C_XFER_RD) {
            ns_i2c_read(cfg, msg->buf, msg->len, msg->addr);
            msg_len += msg->len;
        }
        else if (msg->flags == NS_I2C_XFER_WR) {
            ns_i2c_write(cfg, msg->buf, msg->len, msg->addr);
            msg_len += (msg->len - 1);
        }
    }
    return msg_len;
}
