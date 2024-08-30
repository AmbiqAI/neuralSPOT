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

#include "ns_pdm.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_audio.h"
#include "ns_core.h"

static void *pvPDMHandle;

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
    config->sTransfer.ui32TargetAddr = (uint32_t)config->sampleBuffer;

    // Start the data transfer.
    am_hal_pdm_dma_start(pvPDMHandle, &(config->sTransfer));
}

uint32_t pdm_init(ns_audio_config_t *config) {
    ns_pdm_cfg_t *cfg = config->pdm_config;

    // Common PDM Configuration
    am_hal_pdm_config_t pdmConfig = {
        .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
        .bHighPassEnable = 0, // 0 is enabled. (Reversed..)
        .ui32HighPassCutoff = 0xB,
        .eLeftGain = AM_HAL_PDM_GAIN_P105DB,
        .eRightGain = AM_HAL_PDM_GAIN_P105DB,
        .bDataPacking = 1,
        .bInvertI2SBCLK = 0,
        .ePDMClkSource = AM_HAL_PDM_INTERNAL_CLK,
        .bPDMSampleDelay = 0,
        .bSoftMute = 0,
        .bLRSwap = 0,
    };

    // PDM Clock Config
    switch (cfg->clock_freq) {
    case NS_AUDIO_PDM_CLK_750KHZ:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_750KHZ;
        pdmConfig.ui32DecimationRate = 24;
        break;
    case NS_AUDIO_PDM_CLK_1_5MHZ:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_1_5MHZ;
        pdmConfig.ui32DecimationRate = 48; // CLK 1.5 Mhz
        break;
    default:
        ns_lp_printf("Clock not supported by PDM\n");
        return NS_STATUS_INVALID_CONFIG;
    }

    // PDM Channel Config
    if (config->numChannels == 2) {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO;
    } else {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_LEFT;
    }

    // Initialize, power-up, and configure the PDM.
    am_hal_pdm_initialize(cfg->mic, &pvPDMHandle);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDMCLK, g_AM_BSP_GPIO_PDMCLK);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM_DATA, g_AM_BSP_GPIO_PDM_DATA);

    am_hal_pdm_power_control(pvPDMHandle, AM_HAL_PDM_POWER_ON, false);

    am_hal_pdm_configure(pvPDMHandle, &pdmConfig);
    am_hal_pdm_fifo_flush(pvPDMHandle);

    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    am_hal_pdm_interrupt_enable(
        pvPDMHandle,
        (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    NVIC_EnableIRQ(PDM_IRQn);
    // NVIC_SetPriority(PDM_IRQn, NVIC_configKERNEL_INTERRUPT_PRIORITY);

    am_hal_pdm_enable(pvPDMHandle);
    pdm_trigger_dma(config);
    return NS_STATUS_SUCCESS;
}


void pdm_deinit(ns_audio_config_t *config) {
    ns_pdm_cfg_t * cfg = config->pdm_config;
    am_hal_pdm_interrupt_clear(cfg, (AM_HAL_PDM_INT_DERR  |\
                                         AM_HAL_PDM_INT_DCMP  |\
                                         AM_HAL_PDM_INT_UNDFL |\
                                         AM_HAL_PDM_INT_OVF));

    am_hal_pdm_interrupt_disable(cfg, (AM_HAL_PDM_INT_DERR  |\
                                           AM_HAL_PDM_INT_DCMP  |\
                                           AM_HAL_PDM_INT_UNDFL |\
                                           AM_HAL_PDM_INT_OVF));

    NVIC_DisableIRQ(PDM_IRQn);

    am_bsp_pdm_pins_disable(cfg->mic);

    am_hal_pdm_disable(cfg);
    am_hal_pdm_power_control(cfg, AM_HAL_PDM_POWER_OFF, false);
    am_hal_pdm_deinitialize(cfg);
}






// PDM Interrupt Service Routine (ISR)
void am_pdm0_isr(void) {
    uint32_t ui32Status;

    // Read the interrupt status.
    am_hal_pdm_interrupt_status_get(pvPDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(pvPDMHandle, ui32Status);

    if (ui32Status & AM_HAL_PDM_INT_DCMP) {
        memcpy(
            (uint8_t *)g_ns_audio_config->audioBuffer, (uint8_t *)g_ns_audio_config->sampleBuffer,
            g_ns_audio_config->numSamples * g_ns_audio_config->numChannels * 2);

        uint16_t *temp1 = (uint16_t *)g_ns_audio_config->audioBuffer;
        uint32_t *ui32PDMDatabuffer = (uint32_t *)g_ns_audio_config->sampleBuffer;
        for (uint32_t i = 0; i < g_ns_audio_config->numSamples * g_ns_audio_config->numChannels;
             i++) {

            // Mono data is packed (2 16 bit samples in 32 bit word)
            temp1[2 * i] = ui32PDMDatabuffer[i] & 0xFFFF;
            temp1[2 * i + 1] = (ui32PDMDatabuffer[i] & 0xFFFF0000) >> 16U;
        }

        g_ns_audio_config->callback(g_ns_audio_config, 0);
        AM_CRITICAL_BEGIN;
        PDMn(0)->DMATOTCOUNT = g_ns_audio_config->numSamples * g_ns_audio_config->numChannels *
                               2; // FIFO unit in bytes
        AM_CRITICAL_END;
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
}
