/**
 * @file ns_pdm.c
 * @author Ambiq
 * @brief Driver for Ambiq PDM Microphone interface
 * @version 0.1
 * @date 2023-05-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "../ns_pdm.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_audio.h"
#include "ns_core.h"

static void *pvPDMHandle;

#ifndef AM_PART_APOLLO4L
static const IRQn_Type g_ePdmInterrupts[] = {PDM0_IRQn, PDM1_IRQn, PDM2_IRQn, PDM3_IRQn};
#else
static const IRQn_Type g_ePdmInterrupts[] = {PDM0_IRQn};
#endif

ns_pdm_cfg_t ns_pdm_default = {
    .clock = NS_CLKSEL_HFRC,
    .clock_freq = NS_AUDIO_PDM_CLK_750KHZ,
    .mic = NS_AUDIO_PDM_MICBOARD_0,
    .numBytes = NS_AUDIO_PDM_SAMPLE_16BIT,
};

void pdm_trigger_dma(ns_audio_config_t *config) {

    // Configure DMA and target address.
    config->sTransfer.ui32TotalCount =
        (config->numSamples * config->numChannels * config->pdm_config->numBytes * 2);
    config->sTransfer.ui32TargetAddr = (uint32_t)(((uint32_t)config->sampleBuffer + 3) & ~0xF);
    config->sTransfer.ui32TargetAddrReverse =
        config->sTransfer.ui32TargetAddr + config->sTransfer.ui32TotalCount;

    // Start the data transfer.
    am_hal_pdm_dma_start(pvPDMHandle, &(config->sTransfer));
}

uint32_t pdm_init(ns_audio_config_t *config) {
    ns_pdm_cfg_t *cfg = config->pdm_config;

    // Common PDM Configuration
    am_hal_pdm_config_t pdmConfig = {
        .eStepSize = AM_HAL_PDM_GAIN_STEP_0_13DB,
        .bHighPassEnable = AM_HAL_PDM_HIGH_PASS_ENABLE,
        .ui32HighPassCutoff = 0x3,
        .eLeftGain = AM_HAL_PDM_GAIN_0DB,
        .eRightGain = AM_HAL_PDM_GAIN_0DB,
        .bDataPacking = 1,
        .bPDMSampleDelay = AM_HAL_PDM_CLKOUT_PHSDLY_NONE,
        .ui32GainChangeDelay = AM_HAL_PDM_CLKOUT_DELAY_NONE,
        .bSoftMute = 0,
        .bLRSwap = 0,
    };

    // PDM Clock Config
    switch (cfg->clock) {
    case NS_CLKSEL_HFRC:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC_24MHZ;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
        if (cfg->clock_freq == NS_AUDIO_PDM_CLK_750KHZ) {
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV15;
            pdmConfig.ui32DecimationRate = 24;
        } else {
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7;
            pdmConfig.ui32DecimationRate = 48;
        }
        break;
    case NS_CLKSEL_HFRC2_ADJ:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
        break;
    case NS_CLKSEL_HFXTAL:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFXTAL;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_3;
        break;
    case NS_CLKSEL_XTHS:
    case NS_CLKSEL_HFRC2:
        ns_lp_printf("Clock not supported by PDM\n");
        return NS_STATUS_INVALID_CONFIG;
    }

    // PDM Frequency Config
    if ((cfg->clock == NS_CLKSEL_HFRC2_ADJ) || (cfg->clock == NS_CLKSEL_HFRC)) {
        switch (cfg->clock_freq) {
        case NS_AUDIO_PDM_CLK_750KHZ:
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV15;
            pdmConfig.ui32DecimationRate = 24;
            break;
        case NS_AUDIO_PDM_CLK_1_5MHZ:
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7;
            pdmConfig.ui32DecimationRate = 48;
            break;
        }
    } else {
        pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV14;
        pdmConfig.ui32DecimationRate = 24;
    }

    // PDM Channel Config
    if (config->numChannels == 2) {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO;
    } else {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_LEFT;
    }

    // Initialize, power-up, and configure the PDM.
    am_hal_pdm_initialize(cfg->mic, &pvPDMHandle);
    am_hal_pdm_power_control(pvPDMHandle, AM_HAL_PDM_POWER_ON, false);

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)

    if (cfg->clock == NS_CLKSEL_HFXTAL) {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);
    } else if (cfg->clock == NS_CLKSEL_HFRC2_ADJ) {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);

        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
        am_util_delay_us(200); // wait for FLL to lock
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
        am_util_delay_us(500); // wait for adj to apply
    }
#endif

    am_hal_pdm_configure(pvPDMHandle, &pdmConfig);

    // Configure the necessary pins.
    am_hal_gpio_pincfg_t sGpioConfig;

#ifndef NS_PDM1TO3_PRESENT
    // There is only PDM0
    sGpioConfig = g_AM_BSP_GPIO_PDM0_CLK;
    sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_CLK, sGpioConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_DATA, g_AM_BSP_GPIO_PDM0_DATA);
#else
    switch (cfg->mic) {
    case NS_AUDIO_PDM_MICBOARD_0:
        sGpioConfig = g_AM_BSP_GPIO_PDM0_CLK;
        sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_CLK, sGpioConfig);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_DATA, g_AM_BSP_GPIO_PDM0_DATA);
        break;
    case NS_AUDIO_PDM_MICBOARD_1:
        sGpioConfig = g_AM_BSP_GPIO_PDM2_CLK;
        sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM2_CLK, sGpioConfig);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM2_DATA, g_AM_BSP_GPIO_PDM2_DATA);
        break;
    case NS_AUDIO_PDM_MICBOARD_2:
        sGpioConfig = g_AM_BSP_GPIO_PDM1_CLK;
        sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM1_CLK, sGpioConfig);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM1_DATA, g_AM_BSP_GPIO_PDM1_DATA);
        break;
    }
#endif

    am_hal_pdm_fifo_flush(pvPDMHandle);

    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    am_hal_pdm_interrupt_enable(
        pvPDMHandle,
        (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    // NVIC_SetPriority(g_ePdmInterrupts[cfg->mic],AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(g_ePdmInterrupts[cfg->mic], 7);
    NVIC_EnableIRQ(g_ePdmInterrupts[cfg->mic]);

    am_hal_pdm_enable(pvPDMHandle);
    pdm_trigger_dma(config);
    return NS_STATUS_SUCCESS;
}

// PDM Interrupt Service Routine (ISR)
void am_pdm_isr_common(uint8_t pdmNumber) {
    uint32_t ui32Status;

#if configUSE_SYSVIEWER
    traceISR_ENTER();
#endif // configUSE_SYSVIEWER

    if (pdmNumber != g_ns_audio_config->pdm_config->mic) {
        return;
    }
    // Read the interrupt status.
    am_hal_pdm_interrupt_status_get(pvPDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(pvPDMHandle, ui32Status);

    if (ui32Status & AM_HAL_PDM_INT_DCMP) {
        am_hal_pdm_interrupt_service(pvPDMHandle, ui32Status, &(g_ns_audio_config->sTransfer));

        uint32_t *ui32PDMDatabuffer = (uint32_t *)am_hal_pdm_dma_get_buffer(pvPDMHandle);

        g_ns_audio_config->callback(g_ns_audio_config, 0);

        // Re-arrange data
        uint8_t *temp1 = (uint8_t *)g_ns_audio_config->audioBuffer;
        for (uint32_t i = 0; i < g_ns_audio_config->numSamples * g_ns_audio_config->numChannels;
             i++) {
            temp1[2 * i] = (ui32PDMDatabuffer[i] & 0xFF00) >> 8U;
            temp1[2 * i + 1] = (ui32PDMDatabuffer[i] & 0xFF0000) >> 16U;
        }

        // #if configUSE_AAD
        //         if(g_sVosSys.ui8WosSkipFrameFlag)
        //         {
        //             g_sVosSys.ui8WosSkipFrameFlag = 0;
        //             memset(g_sVosBrd.pui32PcmBuf, 0, (g_ui32WosDiscardTimeUS / 1000) *
        //             (AM_SPP_SAMPLE_FREQ / 1000) * PCM_SAMPLE_BYTES * USE_MIC_NUM);
        //         }
        // #endif // configUSE_AAD

    } else if (ui32Status & (AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF)) {
        am_hal_pdm_fifo_flush(pvPDMHandle);
        // ns_lp_printf("am_hal_pdm_fifo_flush() %d\n", ui32Status);
    }

#if configUSE_SYSVIEWER
    traceISR_EXIT(); // Should be comment out when it is Apollo 3
#endif               // configUSE_SYSVIEWER
}

void am_pdm0_isr(void) { am_pdm_isr_common(0); }
void am_pdm1_isr(void) { am_pdm_isr_common(1); }
void am_pdm2_isr(void) { am_pdm_isr_common(2); }
