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

#ifdef NS_PDM1TO3_PRESENT
static const IRQn_Type g_ePdmInterrupts[] = {PDM0_IRQn, PDM1_IRQn, PDM2_IRQn, PDM3_IRQn};
#else
static const IRQn_Type g_ePdmInterrupts[] = {PDM0_IRQn};
#endif

ns_pdm_cfg_t ns_pdm_default = {
    .clock = NS_CLKSEL_PLL,
    .clock_freq = NS_AUDIO_PDM_CLK_750KHZ,
    .mic = NS_AUDIO_PDM_MICBOARD_0,
    .numBytes = NS_AUDIO_PDM_SAMPLE_16BIT,
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P)
    .left_gain = AM_HAL_PDM_GAIN_P180DB,
    .right_gain = AM_HAL_PDM_GAIN_P180DB,
    #else
    .left_gain = AM_HAL_PDM_GAIN_0DB,
    .right_gain = AM_HAL_PDM_GAIN_0DB,
    #endif
};

void pdm_trigger_dma(ns_audio_config_t *config) {

    // Configure DMA and target address.
    config->sTransfer.ui32TotalCount = 
        (config->numSamples * config->numChannels * config->pdm_config->numBytes * 2);
    config->sTransfer.ui32TargetAddr = (uint32_t)(&config->sampleBuffer[0]);
    config->sTransfer.ui32TargetAddrReverse = (uint32_t)(&config->sampleBuffer[config->sTransfer.ui32TotalCount/4]);

    // Start the data transfer.
    // print every field in sTransfer
    am_util_stdio_printf("sTransfer: %p\n", &config->sTransfer);
    am_util_stdio_printf("ui32TotalCount: %d\n", config->sTransfer.ui32TotalCount);
    am_util_stdio_printf("ui32TargetAddr: 0x%x\n", config->sTransfer.ui32TargetAddr);
    am_util_stdio_printf("ui32TargetAddrReverse: 0x%x\n", config->sTransfer.ui32TargetAddrReverse);
    am_util_delay_ms(60);
    am_hal_pdm_dma_start(pvPDMHandle, &(config->sTransfer));
}

// Ambiqsuite Alpha has a problem where if the MCU is in HIGH_PERFORMANCE2 mode during init, the PDM will not pick the right clock.
// If the MCU is in HP2, temporarily switch to HP1 during init, then switch back to HP2 after init.
uint32_t ns_power_pdm_workaround_pre(void);
uint32_t ns_power_pdm_workaround_post(void);

uint32_t pdm_init(ns_audio_config_t *config) {
    ns_pdm_cfg_t *cfg = config->pdm_config;
    // Check DMA buffer alignment
    if ((uint32_t)&config->sampleBuffer[0] % 32 != 0) {
        ns_lp_printf("DMA buffer is not aligned to 32 bytes\n");
        return NS_STATUS_INVALID_CONFIG;
    }
    // Common PDM Configuration
    am_hal_pdm_config_t pdmConfig = {
        .eStepSize = AM_HAL_PDM_GAIN_STEP_0_13DB,
        .bHighPassEnable = AM_HAL_PDM_HIGH_PASS_ENABLE,
        .ui32HighPassCutoff = 0x3,
        .eLeftGain = AM_HAL_PDM_GAIN_P120DB,
        .eRightGain = AM_HAL_PDM_GAIN_P120DB,
        .bDataPacking = 1,
        .bPDMSampleDelay = AM_HAL_PDM_CLKOUT_PHSDLY_NONE,
        .ui32GainChangeDelay = AM_HAL_PDM_CLKOUT_DELAY_NONE,
        .bSoftMute = 0,
        .bLRSwap = 0,
    };
    // PDM Gain Config
    pdmConfig.eLeftGain = cfg->left_gain;
    pdmConfig.eRightGain = cfg->right_gain;
    
    pdmConfig.eWordWidth          = AM_HAL_PDM_DATA_WORD_WIDTH_16BITS;
    pdmConfig.eDataFlowDirection  = AM_HAL_PDM_DATA_FLOW_TO_MEMORY;
    pdmConfig.bI2sMaster          = false;

    // PDM Clock Config
    // switch (cfg->clock) {
    // case NS_CLKSEL_PLL:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_PLL;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
    //     break;
    // case NS_CLKSEL_HFRC:
    //     pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC_24MHZ;
    //     pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
    //     break;
    // case NS_CLKSEL_HFRC2_ADJ:
    //     #if defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P)
    //     ns_lp_printf("HFRC2 is not supported on Apollo510L.\n");
    //     return NS_STATUS_INVALID_CONFIG;
    //     #else
    //     pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC2_31MHZ;
    //     #endif
    //     pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
    //     break;
    // case NS_CLKSEL_HFXTAL:
    //     pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFXTAL;
    //     pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_3;
    //     break;
    // case NS_CLKSEL_XTHS:
    // case NS_CLKSEL_HFRC2:
    //     ns_lp_printf("Clock not supported by PDM\n");
    //     return NS_STATUS_INVALID_CONFIG;
    // }

    // PDM Frequency Config
    // if ((cfg->clock == NS_CLKSEL_HFRC2_ADJ) || (cfg->clock == NS_CLKSEL_HFRC)) {
    //     switch (cfg->clock_freq) {
    //     case NS_AUDIO_PDM_CLK_750KHZ:
    //         pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV15;
    //         pdmConfig.ui32DecimationRate = 24;
    //         break;
    //     case NS_AUDIO_PDM_CLK_1_5MHZ:
    //         pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7;
    //         pdmConfig.ui32DecimationRate = 48;
    //         break;
    //     }
    // } if (cfg->clock == NS_CLKSEL_PLL) {
        pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV3;
        pdmConfig.ui32DecimationRate = 48;
    // } else {
    //     pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV14;
    //     pdmConfig.ui32DecimationRate = 24;
    // }

    // PDM Channel Config
    if (config->numChannels == 2) {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO;
    } else {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_LEFT;
    }


    ns_power_pdm_workaround_pre();
    // if (cfg->clock == NS_CLKSEL_PLL)
    // {
    //     #ifdef AM_PART_APOLLO510
    //         am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_SYSPLL, 12288000, NULL);
    //     #else
            //
            // Set XTAL_HS as highest priority for SYSPLL FREF.
            //
            am_hal_clkmgr_syspll_fref_priority_t sSysPllFrefPrio =
            {
                .high = AM_HAL_SYSPLL_FREFSEL_XTAL48MHz,
                .mid = AM_HAL_SYSPLL_FREFSEL_EXTREFCLK,
                .low = AM_HAL_SYSPLL_FREFSEL_HFRC192DIV4,
            };
            am_hal_clkmgr_control(AM_HAL_CLKMGR_SYPLL_FREF_PRIORITY_SET, (void*)&sSysPllFrefPrio);

            //
            // Config PLLPOSTDIV to output 24.576 MHz, hence PLLOUT4 = 3.072 MHz.
            //
            am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_PLLVCO, 245760000, NULL);
            am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_PLLPOSTDIV, 24576000, NULL);
            ns_lp_printf("PLLVCO: %d\n", 245760000);
            ns_lp_printf("PLLPOSTDIV: %d\n", 24576000);
        // #endif
    // }
    // else if (cfg->clock == NS_CLKSEL_HFRC)
    // {
    //     #if defined(AM_PART_APOLLO5A)
    //     // Start the 32MHz crystal rapidly used to adjust HFRC2.
    //     am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);
    //     am_util_delay_us(1500);

    //     // Enable HFRC2 adjustment.
    //     am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
    //     am_util_delay_us(500);
    //     #elif defined(AM_PART_APOLLO5B)
    //     am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_HFRC2, AM_HAL_CLKMGR_HFRC2_FREQ_ADJ_196P608MHZ, NULL);
    //     #endif
    // }

    // Initialize, power-up, and configure the PDM.
    ns_lp_printf("Initializing PDM %d\n", cfg->mic);
    am_bsp_pdm_pins_enable(cfg->mic);
    am_hal_pdm_initialize(cfg->mic, &pvPDMHandle);
    am_hal_pdm_power_control(pvPDMHandle, AM_HAL_PDM_POWER_ON, false);
    am_hal_pdm_configure(pvPDMHandle, &pdmConfig);

    am_hal_pdm_fifo_threshold_setup(pvPDMHandle, 16);
    am_hal_pdm_fifo_flush(pvPDMHandle);

    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    am_hal_pdm_interrupt_enable(
        // pvPDMHandle, (AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_DERR));
        pvPDMHandle, 
        (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    NVIC_SetPriority(g_ePdmInterrupts[cfg->mic],AM_IRQ_PRIORITY_DEFAULT);
    // NVIC_SetPriority(g_ePdmInterrupts[cfg->mic], 7);
    NVIC_EnableIRQ(g_ePdmInterrupts[cfg->mic]);
    am_hal_interrupt_master_enable();
    am_hal_pdm_enable(pvPDMHandle);
    pdm_trigger_dma(config);
    ns_power_pdm_workaround_post();
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

    NVIC_DisableIRQ(g_ePdmInterrupts[cfg->mic]);


    am_hal_pdm_disable(cfg);
    am_hal_pdm_power_control(cfg, AM_HAL_PDM_POWER_OFF, false);
    am_hal_pdm_deinitialize(cfg);
}


// PDM Interrupt Service Routine (ISR)
void am_pdm_isr_common(uint8_t pdmNumber) {
    uint32_t ui32Status;
    // ns_lp_printf("PDM ISR %d\n", pdmNumber);
#if configUSE_SYSVIEWER
    traceISR_ENTER();
#endif // configUSE_SYSVIEWER

    if (pdmNumber != g_ns_audio_config->pdm_config->mic) {
        return;
    }
    // Read the interrupt status.
    am_hal_pdm_interrupt_status_get(pvPDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(pvPDMHandle, ui32Status);
    am_hal_pdm_interrupt_service(pvPDMHandle, ui32Status, &(g_ns_audio_config->sTransfer));

    if (ui32Status & AM_HAL_PDM_INT_DCMP) {
    // ns_lp_printf("PDM ISR %d\n", ui32Status);


        uint32_t *ui32PDMDatabuffer = (uint32_t *)am_hal_pdm_dma_get_buffer(pvPDMHandle);

        g_ns_audio_config->callback(g_ns_audio_config, 0);

        // Re-arrange data
        uint8_t *temp1 = (uint8_t *)g_ns_audio_config->audioBuffer;
        for (uint32_t i = 0; i < g_ns_audio_config->numSamples * g_ns_audio_config->numChannels;
             i++) {
            temp1[2 * i] = (ui32PDMDatabuffer[i] & 0xFF);
            temp1[2 * i + 1] = (ui32PDMDatabuffer[i] & 0xFF00) >> 8U;
        }

    } else if (ui32Status & (AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF)) {
        uint32_t count = am_hal_pdm_fifo_count_get(pvPDMHandle);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
                volatile uint32_t ui32FifoDummy;
                for ( uint8_t i = 0; i < count; i++ )
                {
                    ui32FifoDummy = PDMn(g_ns_audio_config->pdm_config->mic)->FIFOREAD;
                }
        #pragma GCC diagnostic pop
        #endif
        am_hal_pdm_fifo_flush(pvPDMHandle);
    }

#if configUSE_SYSVIEWER
    traceISR_EXIT(); // Should be comment out when it is Apollo 3
#endif               // configUSE_SYSVIEWER
}

void am_pdm0_isr(void) { am_pdm_isr_common(0); }
#ifdef NS_PDM1TO3_PRESENT
void am_pdm1_isr(void) { am_pdm_isr_common(1); }
void am_pdm2_isr(void) { am_pdm_isr_common(2); }
#endif
