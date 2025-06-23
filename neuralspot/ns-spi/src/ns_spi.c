
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

#include <string.h>
#include "ns_spi.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_ambiqsuite_harness.h"

ns_spi_config_t ns_spi_config;

#define NS_SPI_DMA_MAX_XFER_SIZE 4095
uint32_t ns_spi_tcb_command_buffer[NS_SPI_DMA_MAX_XFER_SIZE + 1];



//*****************************************************************************
//
//! IOM ISRs
//! Take over correct IOM ISR.
//
//*****************************************************************************
void ns_spi_handle_iom_isr (void) {
    uint32_t ui32Status;
    // ns_lp_printf("IOM ISR\n");
    // uint32_t foo = am_hal_iom_interrupt_status_get(ns_spi_config.iomHandle, true, &ui32Status);
    // ns_lp_printf("IOM1 ISR %d, status %d\n", foo, ui32Status);
    if (!am_hal_iom_interrupt_status_get(ns_spi_config.iomHandle, true, &ui32Status)) {
        if (ui32Status) {
            am_hal_iom_interrupt_clear(ns_spi_config.iomHandle, ui32Status);
            am_hal_iom_interrupt_service(ns_spi_config.iomHandle, ui32Status);
        }
    }
}

am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_CS =
{
    .GP.cfg_b.uFuncSel             = AM_HAL_PIN_92_NCE92,
    .GP.cfg_b.eGPInput             = AM_HAL_GPIO_PIN_INPUT_NONE,
    .GP.cfg_b.eGPRdZero            = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir              = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .GP.cfg_b.eGPOutCfg            = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
    .GP.cfg_b.eDriveStrength       = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
    .GP.cfg_b.ePullup              = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.uNCE                 = AM_HAL_GPIO_NCE_IOM1CE0,
    .GP.cfg_b.eCEpol               = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
    .GP.cfg_b.uRsvd_0              = 0,
    .GP.cfg_b.ePowerSw             = AM_HAL_GPIO_PIN_POWERSW_NONE,
    .GP.cfg_b.eForceInputEn        = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.eForceOutputEn       = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.uRsvd_1              = 0,
};

//*****************************************************************************
//
// IOM1_MISO (10) - I/O Master 1 SPI MISO signal.
//
//*****************************************************************************
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_MISO =
{
    .GP.cfg_b.uFuncSel             = AM_HAL_PIN_10_M1MISO,
    .GP.cfg_b.eGPInput             = AM_HAL_GPIO_PIN_INPUT_NONE,
    .GP.cfg_b.eGPRdZero            = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir              = AM_HAL_GPIO_PIN_INTDIR_NONE,
    .GP.cfg_b.eGPOutCfg            = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eDriveStrength       = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
    .GP.cfg_b.ePullup              = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.uNCE                 = 0,
    .GP.cfg_b.eCEpol               = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
    .GP.cfg_b.uRsvd_0              = 0,
    .GP.cfg_b.ePowerSw             = AM_HAL_GPIO_PIN_POWERSW_NONE,
    .GP.cfg_b.eForceInputEn        = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.eForceOutputEn       = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.uRsvd_1              = 0,
};

//*****************************************************************************
//
// IOM1_MOSI (9) - I/O Master 1 SPI MOSI signal.
//
//*****************************************************************************
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_MOSI =
{
    .GP.cfg_b.uFuncSel             = AM_HAL_PIN_9_M1MOSI,
    .GP.cfg_b.eGPInput             = AM_HAL_GPIO_PIN_INPUT_NONE,
    .GP.cfg_b.eGPRdZero            = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir              = AM_HAL_GPIO_PIN_INTDIR_NONE,
    .GP.cfg_b.eGPOutCfg            = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eDriveStrength       = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
    .GP.cfg_b.ePullup              = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.uNCE                 = 0,
    .GP.cfg_b.eCEpol               = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
    .GP.cfg_b.uRsvd_0              = 0,
    .GP.cfg_b.ePowerSw             = AM_HAL_GPIO_PIN_POWERSW_NONE,
    .GP.cfg_b.eForceInputEn        = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.eForceOutputEn       = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.uRsvd_1              = 0,
};

//*****************************************************************************
//
// IOM1_SCK (8) - I/O Master 1 SPI SCK signal.
//
//*****************************************************************************
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_SCK =
{
    .GP.cfg_b.uFuncSel             = AM_HAL_PIN_8_M1SCK,
    .GP.cfg_b.eGPInput             = AM_HAL_GPIO_PIN_INPUT_NONE,
    .GP.cfg_b.eGPRdZero            = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir              = AM_HAL_GPIO_PIN_INTDIR_NONE,
    .GP.cfg_b.eGPOutCfg            = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eDriveStrength       = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
    .GP.cfg_b.ePullup              = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.uNCE                 = 0,
    .GP.cfg_b.eCEpol               = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
    .GP.cfg_b.uRsvd_0              = 0,
    .GP.cfg_b.ePowerSw             = AM_HAL_GPIO_PIN_POWERSW_NONE,
    .GP.cfg_b.eForceInputEn        = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.eForceOutputEn       = AM_HAL_GPIO_PIN_FORCEEN_NONE,
    .GP.cfg_b.uRsvd_1              = 0,
};


void ns_high_drive_pins_enable() {
            am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM1_SCK,  NS_AM_BSP_GPIO_IOM1_SCK);
            am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM1_MISO, NS_AM_BSP_GPIO_IOM1_MISO);
            am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM1_MOSI, NS_AM_BSP_GPIO_IOM1_MOSI);
            am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM1_CS,   NS_AM_BSP_GPIO_IOM1_CS);
};


uint32_t ns_spi_interface_init(ns_spi_config_t *cfg, uint32_t speed, am_hal_iom_spi_mode_e mode) {
    cfg->sIomCfg.pNBTxnBuf = NULL;
    cfg->sIomCfg.eInterfaceMode = AM_HAL_IOM_SPI_MODE;
    cfg->sIomCfg.eSpiMode = mode;
    cfg->sIomCfg.ui32NBTxnBufLength = 0;
    cfg->sIomCfg.ui32ClockFreq = speed;
    cfg->sIomCfg.ui32NBTxnBufLength = sizeof(ns_spi_tcb_command_buffer) / sizeof(uint32_t);
    cfg->sIomCfg.pNBTxnBuf = ns_spi_tcb_command_buffer;

    IRQn_Type gc_iomIrq = (IRQn_Type)(cfg->iom + IOMSTR0_IRQn);
    ns_lp_printf("SPI Init IOM %d, gc_iomIrq %d\n", cfg->iom, gc_iomIrq);
    // #ifdef apollo510_evb
    // ns_lp_printf("Apollo510 EVB\n");
    am_bsp_iom_pins_enable(cfg->iom, AM_HAL_IOM_SPI_MODE);
    // #else
    // ns_high_drive_pins_enable(); // High drive pins for EB
    // #endif

    // if (cfg->iom != NS_IOM_ISR) {
    //     ns_lp_printf("ns_spi_interface_init: Configured IOM %d does not match NS_IOM_ISR\n", cfg->iom, NS_IOM_ISR);
    //     return NS_SPI_STATUS_ERROR;
    // }

    if (am_hal_iom_initialize(cfg->iom, &(cfg->iomHandle)) ||
        am_hal_iom_power_ctrl(cfg->iomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(cfg->iomHandle, &(cfg->sIomCfg))) {
        ns_lp_printf("ns_spi_interface_init: Failed to initialize IOM %d\n", cfg->iom);
        return NS_SPI_STATUS_ERROR;
    }
    am_hal_iom_enable(cfg->iomHandle);
    // Store config in global
    memcpy(&ns_spi_config, cfg, sizeof(ns_spi_config_t));

    // Enable the IOM interrupt
    // ns_lp_printf("IOM %d IRQ %d\n", cfg->iom, gc_iomIrq);
    NVIC_ClearPendingIRQ(gc_iomIrq);
    NVIC_EnableIRQ(gc_iomIrq);

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
    // ns_lp_printf("SPI read of address 0x%llx, len %d\n", reg, regLen);
    err = am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction);
    if (err) {
        ns_lp_printf("SPI Read reg 0x%llx done err %d\n",reg, err);
        return err;
    }
    ns_delay_us(1000);
    return NS_SPI_STATUS_SUCCESS;
}

static void ns_spi_dma_read_complete_cb(void *pCallbackCtxt, uint32_t ui32TransactionStatus) {
    (void)pCallbackCtxt;
    (void)ui32TransactionStatus;
    // ns_lp_printf("SPI DMA done\n");
    if (ns_spi_config.cb) {
        ns_spi_config.cb(&ns_spi_config);
    }
}

uint32_t ns_spi_read_dma(
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
    // ns_lp_printf("SPI Read DMA len of %d to 0x%x IOM %d\n", bufLen, (uint32_t)buf, cfg->iom);
    err = am_hal_iom_nonblocking_transfer(
        cfg->iomHandle, &Transaction, ns_spi_dma_read_complete_cb, 0);

    // err = am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction);
    if (err) {
        ns_lp_printf("SPI Read DMA Error %d\n", err);
        // while(1);
        return err;
    }
    ns_delay_us(1000);

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
    // ns_lp_printf("SPI write of address 0x%llx\n", reg);

    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_SPI_STATUS_ERROR;
    }
    ns_delay_us(1000);

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
