/**
 * @file ns_pdm.c
 * @author Carlos Morales
 * @brief Driver for Ambiq PDM Microphone interface
 * @version 0.1
 * @date 2023-05-17
 *
 * @copyright Copyright (c) 2023
 *
 */

void
pdm_init(ns_audio_config_t *config) {
    ns_audio_pdm_config_t *pdm_config = config->pdm_config;

    // Common PDM Configuration
    am_hal_pdm_config_t pdmConfig =
    {.eStepSize = AM_HAL_PDM_GAIN_STEP_0_13DB,
     .bHighPassEnable = AM_HAL_PDM_HIGH_PASS_ENABLE,
     .ui32HighPassCutoff = 0x3,
     .eLeftGain = AM_HAL_PDM_GAIN_0DB,
     .eRightGain = AM_HAL_PDM_GAIN_0DB,
     .bPDMSampleDelay = AM_HAL_PDM_CLKOUT_PHSDLY_NONE,
     .ui32GainChangeDelay = AM_HAL_PDM_CLKOUT_DELAY_NONE,
     .bSoftMute = 0,
     .bLRSwap = 0,
    }

    // PDM Clock Config
    switch (pdm_config->clock) {
    case NS_CLKSEL_HFRC:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC_24MHZ;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
        if (pdm_config->clock_freq == NS_AUDIO_PDM_CLK_750KHZ) {
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV15;
            pdmConfig.ui32DecimationRate = 24;
        } else {
            pdmConfig.ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7;
            pdmConfig.ui32DecimationRate = 48;
        }
        break;
    case NS_CLKSEL_HFRC2_ADJ:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC2_12MHZ;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_1;
        break;
    case NS_CLKSEL_HFXTAL:
        pdmConfig.ePDMClkSpeed = AM_HAL_PDM_CLK_HFXTAL;
        pdmConfig.eClkDivider = AM_HAL_PDM_MCLKDIV_3;
        break;
    }

    // PDM Frequency Config
    if ((pdm_config->clock == NS_CLKSEL_HFRC2_ADJ) || (pdm_config->clock == NS_CLKSEL_HFRC)) {
        switch (pdm_config->clock_freq) {
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
    if (config->num_channels == 2) {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO;
    } else {
        pdmConfig.ePCMChannels = AM_HAL_PDM_CHANNEL_LEFT;
    }

    // Initialize, power-up, and configure the PDM.
    am_hal_pdm_initialize(USE_PDM_MODULE, &g_sVosBrd.pvPDMHandle);
    am_hal_pdm_power_control(g_sVosBrd.pvPDMHandle, AM_HAL_PDM_POWER_ON, false);

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    #if USE_PDM_CLK_SRC_HFXTAL_32MHZ
    // use external XTHS, not reference clock
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);
    #endif

    #if USE_PDM_CLK_SRC_HFRC2ADJ_24MHZ
    // use external XTHS, not reference clock
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);

    // enable HFRC2
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(200); // wait for FLL to lock

    // set HF2ADJ for 24.576MHz output
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
    am_util_delay_us(500); // wait for adj to apply
    #endif                 // USE_PDM_CLK_SRC_HFRC2ADJ_24MHZ
#endif

    am_hal_pdm_configure(g_sVosBrd.pvPDMHandle, &g_sPdmConfig);

    //
    // Configure the necessary pins.
    //

#if USE_DMIC_MB0 || USE_DMIC_MB0_T5838
    am_hal_gpio_pincfg_t sGpioConfig = g_AM_BSP_GPIO_PDM0_CLK;
    sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_CLK, sGpioConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM0_DATA, g_AM_BSP_GPIO_PDM0_DATA);
#elif USE_DMIC_MB1
    am_hal_gpio_pincfg_t sGpioConfig = g_AM_BSP_GPIO_PDM2_CLK;
    sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM2_CLK, sGpioConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM2_DATA, g_AM_BSP_GPIO_PDM2_DATA);
#elif USE_DMIC_MB2
    am_hal_gpio_pincfg_t sGpioConfig = g_AM_BSP_GPIO_PDM1_CLK;
    sGpioConfig.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X;

    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM1_CLK, sGpioConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_PDM1_DATA, g_AM_BSP_GPIO_PDM1_DATA);
#endif

    // am_hal_pdm_fifo_flush(g_sVosBrd.pvPDMHandle);

    //
    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    //
    am_hal_pdm_interrupt_enable(g_sVosBrd.pvPDMHandle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP |
                                                        AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    NVIC_EnableIRQ(g_ePdmInterrupts[USE_PDM_MODULE]);
    NVIC_SetPriority(g_ePdmInterrupts[USE_PDM_MODULE], NVIC_configKERNEL_INTERRUPT_PRIORITY);
}
