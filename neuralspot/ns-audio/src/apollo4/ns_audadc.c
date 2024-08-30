//*****************************************************************************
//
//! @file audio.c
//!
//! @brief A set of libraries for capturing audio from AUDADC.
//!
//! Purpose: Utitlies for initializing the AUDADC and capturing audio to a
//! buffer
//!
//! Additional Information:
//! @RTT Streaming: Run rtt_logger.py to capture pcm raw data via PC.
//!                 Should modify -RTTAddress in rtt_logger.py to _SEGGER_RTT
//!                 address in audadc_rtt_stream.map The data saved as
//!                 stereo(L:low gain/R: high gain)
//!
//! @pcm_to_wav.py:  Convert pcm raw data to wav file.
//!                  Save mono left channel to destination file.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_PART_APOLLO4L

    #include "../ns_audadc.h"
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_audio.h"
    #include "ns_core.h"

// #define HFRC2_ADJ            0
// #define AUDADC_EXAMPLE_DEBUG 1
// #define FIFO_READ            0

    #define XTHS 0
    #define HFRC 1
    #define HFRC2 2
    #define HFRC2_ADJ 3
    #define CLK_SRC HFRC // NEW example has HFRC2_ADJ

AUDADC_Type *g_adc;
MCUCTRL_Type *g_mcuctrl;

// AUDADC Device Handle.
static void *g_AUDADCHandle;

// AUDADC DMA error flag.
static volatile bool g_bAUDADCDMAError;

    // AXI Scratch buffer
    // Need to allocate 20 Words even though we only need 16, to ensure we have 16
    // Byte alignment
    #ifndef AM_PART_APOLLO4P
static uint32_t axiScratchBuf[20];
    #endif
//*****************************************************************************
//
// AUDADC gain configuration information.
//
//*****************************************************************************

    #define PREAMP_FULL_GAIN 24 // Enable op amps for full gain range
    #define CH_A0_GAIN_DB 18
    #define CH_A1_GAIN_DB 18
    #define CH_B0_GAIN_DB 12
    #define CH_B1_GAIN_DB 12

am_hal_audadc_gain_config_t g_sAudadcGainConfig = {
    .ui32LGA = 0,      // 0 code
    .ui32HGADELTA = 0, // delta from the LGA field
    .ui32LGB = 0,      // 0 code
    .ui32HGBDELTA = 0, // delta from the LGB field
    .eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME,
};

am_hal_audadc_dma_config_t g_sAUDADCDMAConfig; // global because needed by ISR

    #if defined(AM_PART_APOLLO4P) && AUDADC_DC_OFFSET_CAL
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
    #endif // AM_PART_APOLLO4P && AUDADC_DC_OFFSET_CAL

ns_audadc_cfg_t ns_audadc_default = {
    // .clock = NS_CLKSEL_HFRC,
    // .low_power_mode = false,
    .clock = NS_CLKSEL_HFRC2_ADJ,
    .low_power_mode = true,
    .repeating_trigger_mode = true,
    .dcmp_enable = false,
};

ns_audadc_cfg_t ns_audadc_vos_default = {
    .clock = NS_CLKSEL_HFRC2_ADJ,
    .low_power_mode = true,
    .repeating_trigger_mode = false,
    .dcmp_enable = false,
};

//*****************************************************************************
//
// Configure the AUDADC SLOT.
//
//*****************************************************************************
uint32_t audadc_slot_config(ns_audio_config_t *cfg) {
    am_hal_audadc_slot_config_t AUDADCSlotConfig;

    // Set up an AUDADC slot
    AUDADCSlotConfig.eMeasToAvg = AM_HAL_AUDADC_SLOT_AVG_1;
    AUDADCSlotConfig.ePrecisionMode = AM_HAL_AUDADC_SLOT_12BIT;
    if (cfg->audadc_config->clock == NS_CLKSEL_XTHS) {
        AUDADCSlotConfig.ui32TrkCyc = 24;
    } else {
    #if defined(AM_PART_APOLLO4P)
        AUDADCSlotConfig.ui32TrkCyc = 34; // new example has '34'
    #else
        AUDADCSlotConfig.ui32TrkCyc = 30;
    #endif
    }
    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE0;
    AUDADCSlotConfig.bWindowCompare = false;
    AUDADCSlotConfig.bEnabled = true;

    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 0, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 0 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE1;
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 1, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 1 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    if (cfg->numChannels == 2) {
        AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE2;
        if (AM_HAL_STATUS_SUCCESS !=
            am_hal_audadc_configure_slot(g_AUDADCHandle, 2, &AUDADCSlotConfig)) {
            am_util_stdio_printf("Error - configuring AUDADC Slot 2 failed.\n");
            return NS_STATUS_INIT_FAILED;
        }

        AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE3;
        if (AM_HAL_STATUS_SUCCESS !=
            am_hal_audadc_configure_slot(g_AUDADCHandle, 3, &AUDADCSlotConfig)) {
            am_util_stdio_printf("Error - configuring AUDADC Slot 3 failed.\n");
            return NS_STATUS_INIT_FAILED;
        }
    }
    return NS_STATUS_SUCCESS;
}

static void audadc_pga_init(ns_audio_config_t *cfg) {
    #ifndef AM_PART_APOLLO4P
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    am_hal_daxi_control(
        AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
    #endif

    // Power up PrePGA
    am_hal_audadc_refgen_powerup();

    am_hal_audadc_pga_powerup(0);
    am_hal_audadc_pga_powerup(1);
    if (cfg->numChannels == 2) {
        am_hal_audadc_pga_powerup(2);
        am_hal_audadc_pga_powerup(3);
    }

    am_hal_audadc_gain_set(0, 2 * PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(1, 2 * PREAMP_FULL_GAIN);
    if (cfg->numChannels == 2) {
        am_hal_audadc_gain_set(2, 2 * PREAMP_FULL_GAIN);
        am_hal_audadc_gain_set(3, 2 * PREAMP_FULL_GAIN);
    }

    //  Turn on mic bias
    am_hal_audadc_micbias_powerup(24);
    am_util_delay_ms(400);
}

static void audadc_config_dma(ns_audio_config_t *cfg) {
    // Configure the AUDADC to use DMA for the sample transfer.
    g_sAUDADCDMAConfig.bDynamicPriority = true;
    g_sAUDADCDMAConfig.ePriority = AM_HAL_AUDADC_PRIOR_SERVICE_IMMED;
    g_sAUDADCDMAConfig.bDMAEnable = true;
    g_sAUDADCDMAConfig.ui32SampleCount = cfg->numSamples * cfg->numChannels;
    g_sAUDADCDMAConfig.ui32TargetAddress = (uint32_t)cfg->sampleBuffer;
    g_sAUDADCDMAConfig.ui32TargetAddressReverse =
        g_sAUDADCDMAConfig.ui32TargetAddress +
        sizeof(uint32_t) * g_sAUDADCDMAConfig.ui32SampleCount;

    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_dma(g_AUDADCHandle, &g_sAUDADCDMAConfig)) {
        ns_lp_printf("Error - configuring AUDADC DMA failed.\n");
    }

    // Reset the AUDADC DMA flags.
    // g_sVosBrd.bAUDADCDMAComplete = false;
    g_bAUDADCDMAError = false;
}

/**
 * @brief Configure and enable the AUDADC
 *
 * @param cfg - AUDADC-specific configuration
 * @return uint32_t status
 */
uint32_t audadc_config(ns_audadc_cfg_t *cfg) {

    // Set up the AUDADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.

    am_hal_audadc_config_t AUDADCConfig = {
        // Common settings
        .ePolarity = AM_HAL_AUDADC_TRIGPOL_RISING,
        .eTrigger = AM_HAL_AUDADC_TRIGSEL_SOFTWARE,
        .eRepeat = AM_HAL_AUDADC_REPEATING_SCAN,
    };

    am_hal_audadc_irtt_config_t AUDADCIrttConfig = {// only used if timer isn't used
                                                    .bIrttEnable = true,
                                                    .eClkDiv = AM_HAL_AUDADC_RPTT_CLK_DIV32};

    switch (cfg->clock) {
    case NS_CLKSEL_XTHS:
        AUDADCConfig.eClock = AM_HAL_AUDADC_CLKSEL_XTHS_24MHz;
        AUDADCIrttConfig.ui32IrttCountMax = 46; // VoS has 46
        break;
    case NS_CLKSEL_HFRC:
        AUDADCConfig.eClock = AM_HAL_AUDADC_CLKSEL_HFRC_48MHz;
        AUDADCIrttConfig.ui32IrttCountMax = 93;
        break;
    case NS_CLKSEL_HFRC2:
    case NS_CLKSEL_HFRC2_ADJ:
        AUDADCConfig.eClock = AM_HAL_AUDADC_CLKSEL_HFRC2_48MHz;
        AUDADCIrttConfig.ui32IrttCountMax = 93;
        break;
    case NS_CLKSEL_HFXTAL:
        // Not allowed for AUDADC
        return NS_STATUS_INVALID_CONFIG;
    }

    if (cfg->low_power_mode) {
        AUDADCConfig.eClockMode = AM_HAL_AUDADC_CLKMODE_LOW_POWER;
        AUDADCConfig.ePowerMode = AM_HAL_AUDADC_LPMODE1;
        AUDADCConfig.eSampMode = AM_HAL_AUDADC_SAMPMODE_LP;
    } else {
        AUDADCConfig.eClockMode = AM_HAL_AUDADC_CLKMODE_LOW_LATENCY;
        AUDADCConfig.ePowerMode = AM_HAL_AUDADC_LPMODE0;
        AUDADCConfig.eSampMode = AM_HAL_AUDADC_SAMPMODE_MED;
    }

    if ((cfg->clock == NS_CLKSEL_HFRC2) || (cfg->clock == NS_CLKSEL_HFRC2_ADJ)) {
        // Enable hfrc2.
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
        am_util_delay_us(200);
    }

    AUDADCConfig.eRepeatTrigger =
        (cfg->repeating_trigger_mode) ? AM_HAL_AUDADC_RPTTRIGSEL_INT : AM_HAL_AUDADC_RPTTRIGSEL_TMR;

    int ret = am_hal_audadc_initialize(0, &g_AUDADCHandle);
    if (AM_HAL_STATUS_SUCCESS != ret) {
        am_util_stdio_printf("Error - reservation of the AUDADC instance failed ERR %d.\n", ret);
        return NS_STATUS_INIT_FAILED;
    }

    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_power_control(g_AUDADCHandle, AM_HAL_SYSCTRL_WAKE, false)) {
        am_util_stdio_printf("Error - AUDADC power on failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    if (cfg->clock == NS_CLKSEL_HFRC2_ADJ) {
        // hfrc2 adj.
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);

        // set HF2ADJ for 24.576MHz output
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
        am_util_delay_us(500); // wait for adj to apply
    } else if (cfg->clock == NS_CLKSEL_XTHS) {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL, 0);
        am_util_delay_us(1500);
    }

    // Configure the AUDADC based on the above data structs using HAL
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure(g_AUDADCHandle, &AUDADCConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    // Set up internal repeat trigger timer
    if (cfg->repeating_trigger_mode) {
        am_hal_audadc_configure_irtt(g_AUDADCHandle, &AUDADCIrttConfig);
    } else {
        // timer6_init();
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_enable(g_AUDADCHandle)) {
        am_util_stdio_printf("Error - enabling AUDADC failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    if (cfg->repeating_trigger_mode) {
        am_hal_audadc_irtt_enable(g_AUDADCHandle);
    } else {
        // am_hal_timer_start(6);
    }

    // AUDADC DMA data config
    audadc_config_dma(g_ns_audio_config);

    if (cfg->dcmp_enable) {
        am_hal_audadc_interrupt_enable(
            g_AUDADCHandle, AM_HAL_AUDADC_INT_DERR | AM_HAL_AUDADC_INT_DCMP);
    } else {
        am_hal_audadc_interrupt_enable(
            g_AUDADCHandle,
            AM_HAL_AUDADC_INT_DERR | AM_HAL_AUDADC_INT_DCMP | AM_HAL_AUDADC_INT_FIFOOVR1);
    }

    return NS_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Interrupt handler for the AUDADC.
//
//*****************************************************************************
void am_audadc0_isr(void) {
    uint32_t ui32IntMask;

    // Read the interrupt status.
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_interrupt_status(g_AUDADCHandle, &ui32IntMask, false)) {
        am_util_stdio_printf("Error reading AUDADC interrupt status\n");
    }

    // Clear the AUDADC interrupt.
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_interrupt_clear(g_AUDADCHandle, ui32IntMask)) {
        am_util_stdio_printf("Error clearing AUDADC interrupt status\n");
    }

    // If we got a DMA complete, set the flag.
    // if (ui32IntMask & AM_HAL_AUDADC_INT_FIFOOVR1) {
    if (ui32IntMask & AM_HAL_AUDADC_INT_DCMP) {
        if (AUDADCn(0)->DMASTAT_b.DMACPL) {
            g_bAUDADCDMAError = false;

            // g_bAUDADCDMAComplete = true;
            if (g_ns_audio_config->api->version.major < 2) {
                am_hal_audadc_interrupt_service(g_AUDADCHandle, &g_sAUDADCDMAConfig);
            }

            g_ns_audio_config->callback(g_ns_audio_config, 0);
            audadc_config_dma(g_ns_audio_config);
        } else {
            if (AUDADCn(0)->DMASTAT_b.DMACPL) {
                am_util_stdio_printf("WHAT clearing AUDADC interrupt status\n");
            }
        }
    }

    // If we got a DMA error, set the flag.
    if (ui32IntMask & AM_HAL_AUDADC_INT_DERR) {
        g_bAUDADCDMAError = true;
    }
}

//*****************************************************************************
//
// Setup the AUDADC
//
//*****************************************************************************
uint32_t audadc_init(ns_audio_config_t *cfg) {

    // Power up PrePGA
    audadc_pga_init(g_ns_audio_config);

    // Configure the AUDADC
    if (audadc_config(g_ns_audio_config->audadc_config)) {
        return NS_STATUS_INIT_FAILED;
    }
    g_ns_audio_config->audioSystemHandle = g_AUDADCHandle; // now it has a real value

    // Gain setting
    g_sAudadcGainConfig.ui32LGA = CH_A0_GAIN_DB * 2 + 12;
    g_ns_audio_config->fLGAdB = CH_A0_GAIN_DB;
    // g_sAudadcGainConfig.ui32HGADELTA = g_sAudadcGainConfig.ui32LGA - (CH_A1_GAIN_DB * 2 + 12);
    g_sAudadcGainConfig.ui32HGADELTA =
        (CH_A1_GAIN_DB * 2 + 12) - g_sAudadcGainConfig.ui32LGA; // HGDelta = 12
    if (g_ns_audio_config->numChannels == 2) {
        g_sAudadcGainConfig.ui32LGB = CH_B0_GAIN_DB * 2 + 12;
        // g_sAudadcGainConfig.ui32HGBDELTA = g_sAudadcGainConfig.ui32LGB - (CH_B1_GAIN_DB * 2 +
        // 12);
        g_sAudadcGainConfig.ui32HGBDELTA = (CH_B1_GAIN_DB * 2 + 12) - g_sAudadcGainConfig.ui32LGB;
    }
    g_sAudadcGainConfig.eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME;
    am_hal_audadc_internal_pga_config(g_AUDADCHandle, &g_sAudadcGainConfig);

    // Configure the slot
    if (audadc_slot_config(g_ns_audio_config)) {
        return NS_STATUS_INIT_FAILED;
    }

    #ifdef NEW_EXAMPLE

        #if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL) && (AS_VERSION = R4_3_0)
    //
    // Calculate DC offset calibartion parameter.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_slot_dc_offset_calculate(
                                     g_AUDADCHandle, 4, g_ns_audio_config->sOffsetCalib)) {
        am_util_stdio_printf("Error - failed to calculate offset calibration parameter.\n");
    }
        #endif
    #endif
    NVIC_SetPriority(AUDADC0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(AUDADC0_IRQn);
    am_hal_interrupt_master_enable();

    return NS_STATUS_SUCCESS;
}

void audadc_deinit(ns_audio_config_t *cfg) {
    //
    // power off audadc
    //
    am_hal_audadc_interrupt_disable(g_AUDADCHandle, 0xFFFFFFFF);
    while(AUDADC->DMATOTCOUNT_b.TOTCOUNT != 0); // Ensure DMATOTCOUNT is set to 0 as part of de-initialization
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_control(g_AUDADCHandle, AM_HAL_AUDADC_REQ_DMA_DISABLE, NULL))
    {
        am_util_stdio_printf("Error - AUDADC control failed.\n");
    }
    am_hal_audadc_irtt_disable(g_AUDADCHandle);
    am_hal_audadc_disable(g_AUDADCHandle);
    am_hal_audadc_deinitialize(g_AUDADCHandle);
#ifdef POWER_CYCLE_MICBIAS
    am_hal_audadc_micbias_powerdown();
#endif
    am_hal_audadc_pga_powerdown(0);
    am_hal_audadc_pga_powerdown(1);
    am_hal_audadc_pga_powerdown(2);
    am_hal_audadc_pga_powerdown(3);
    am_hal_audadc_refgen_powerdown();
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDADC);
    am_util_delay_ms(rand() % 20);

}

#endif // AM_PART_APOLLO4L
