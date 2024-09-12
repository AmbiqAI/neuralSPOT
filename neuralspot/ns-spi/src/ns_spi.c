
/**
 * @file ns_spi.c
 * @author Adam Page
 * @brief Generic SPI driver
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_spi.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

uint32_t ns_spi_interface_init(ns_spi_config_t *cfg, uint32_t speed, am_hal_iom_spi_mode_e mode) {
    cfg->sIomCfg.pNBTxnBuf = NULL;
    cfg->sIomCfg.eInterfaceMode = AM_HAL_IOM_SPI_MODE;
    cfg->sIomCfg.eSpiMode = mode;
    cfg->sIomCfg.ui32NBTxnBufLength = 0;
    cfg->sIomCfg.ui32ClockFreq = speed;
    if (am_hal_iom_initialize(cfg->iom, &(cfg->iomHandle)) ||
        am_hal_iom_power_ctrl(cfg->iomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(cfg->iomHandle, &(cfg->sIomCfg))) {
        return NS_SPI_STATUS_ERROR;
    }
    am_bsp_iom_pins_enable(cfg->iom, AM_HAL_IOM_SPI_MODE);
    am_hal_iom_enable(cfg->iomHandle);

    return NS_SPI_STATUS_SUCCESS;
}

uint32_t ns_spi_read(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin) {
    uint32_t err;
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = regLen;
    Transaction.ui64Instr = reg;
    Transaction.eDirection = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes = bufLen;
    Transaction.pui32RxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32SpiChipSelect = csPin;
    err = am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction);
    if (err) {
        return err;
    }
    return NS_SPI_STATUS_SUCCESS;
}

uint32_t ns_spi_write(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin) {
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = regLen;
    Transaction.ui64Instr = reg;
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = bufLen;
    Transaction.pui32TxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32SpiChipSelect = csPin;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_SPI_STATUS_ERROR;
    }
    return NS_SPI_STATUS_SUCCESS;
}

uint32_t ns_spi_transfer(
    ns_spi_config_t *cfg, const void *txBuf, const void *rxBuf, uint32_t size, uint32_t csPin) {
    /**
     * @brief Full-duplex transfer !NOTE: Do not use for now
     *
     */
    am_hal_iom_transfer_t Transaction;
    Transaction.ui32InstrLen = 1;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_FULLDUPLEX;
    Transaction.ui32NumBytes = size;
    Transaction.pui32TxBuffer = (uint32_t *)txBuf;
    Transaction.pui32RxBuffer = (uint32_t *)rxBuf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32SpiChipSelect = csPin;
    if (am_hal_iom_spi_blocking_fullduplex(cfg->iomHandle, &Transaction)) {
        return NS_SPI_STATUS_ERROR;
    }
    return NS_SPI_STATUS_SUCCESS;
}
