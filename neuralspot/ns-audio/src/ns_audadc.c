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

#include "ns_audadc.h"
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

//*****************************************************************************
//
// AUDADC Sample buffer.
//
//*****************************************************************************

// static uint32_t *g_ui32AUDADCSampleBuffer; //[AUDADC_MAX_SAMPLE_BUF_SIZE * 2 + 3];

//
// AUDADC Device Handle.
//
static void *g_AUDADCHandle;

//
// AUDADC DMA complete flag.
//
static volatile bool g_bAUDADCDMAComplete;

//
// AUDADC DMA error flag.
//
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
#define CH_A0_EN 1
#define CH_A1_EN 1
#define CH_B0_EN 0
#define CH_B1_EN 0

#define PREAMP_FULL_GAIN 12 // Enable op amps for full gain range
#define CH_A0_GAIN_DB 12
#define CH_A1_GAIN_DB 12
#define CH_B0_GAIN_DB 12
#define CH_B1_GAIN_DB 12

am_hal_audadc_gain_config_t g_sAudadcGainConfig = {
    .ui32LGA = 0,      // 0 code
    .ui32HGADELTA = 0, // delta from the LGA field
    .ui32LGB = 0,      // 0 code
    .ui32HGBDELTA = 0, // delta from the LGB field

    .eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME,
};

//
// Configure the AUDADC to use DMA for the sample transfer.
//
am_hal_audadc_dma_config_t g_sAUDADCDMAConfig = {
    .bDynamicPriority = true,
    .ePriority = AM_HAL_AUDADC_PRIOR_SERVICE_IMMED,
    .bDMAEnable = true,
    .ui32SampleCount = AUDADC_MAX_SAMPLE_BUF_SIZE, // updated to real value below
    .ui32TargetAddress = 0x0,
    .ui32TargetAddressReverse = 0x0,
};

//*****************************************************************************
//
// Configure the AUDADC SLOT.
//
//*****************************************************************************
uint32_t
audadc_slot_config(void) {
    am_hal_audadc_slot_config_t AUDADCSlotConfig;

    //
    // Set up an AUDADC slot
    //
    AUDADCSlotConfig.eMeasToAvg = AM_HAL_AUDADC_SLOT_AVG_1;
    AUDADCSlotConfig.ePrecisionMode = AM_HAL_AUDADC_SLOT_12BIT;
#if (CLK_SRC == XTHS)
    AUDADCSlotConfig.ui32TrkCyc = 24;
#else
    #if defined(AM_PART_APOLLO4P)
    AUDADCSlotConfig.ui32TrkCyc = 30; // new example has '34'
    #else
    AUDADCSlotConfig.ui32TrkCyc = 30;
    #endif
#endif
    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE0;
    AUDADCSlotConfig.bWindowCompare = false;
    AUDADCSlotConfig.bEnabled = true;

#if CH_A0_EN
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 0, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 0 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }
#endif
#if CH_A1_EN
    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE1;
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 1, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 1 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }
#endif
#if CH_B0_EN
    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE2;
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 2, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 2 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }
#endif
#if CH_B1_EN
    AUDADCSlotConfig.eChannel = AM_HAL_AUDADC_SLOT_CHSEL_SE3;
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_configure_slot(g_AUDADCHandle, 3, &AUDADCSlotConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC Slot 3 failed.\n");
        return NS_STATUS_INIT_FAILED;
    }
#endif
    return NS_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Configure the AUDADC.
//
//*****************************************************************************
uint32_t
audadc_config(void) {
    //
    // Set up the AUDADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    //
    am_hal_audadc_config_t AUDADCConfig = {
#if (CLK_SRC == XTHS)
        .eClock = AM_HAL_AUDADC_CLKSEL_XTHS_24MHz,
#elif (CLK_SRC == HFRC)
        .eClock = AM_HAL_AUDADC_CLKSEL_HFRC_48MHz,
#elif (CLK_SRC == HFRC2) || (CLK_SRC == HFRC2_ADJ)
        .eClock = AM_HAL_AUDADC_CLKSEL_HFRC2_48MHz,
#else
    #warning Invalid clock source.
#endif
        .ePolarity = AM_HAL_AUDADC_TRIGPOL_RISING,
        .eTrigger = AM_HAL_AUDADC_TRIGSEL_SOFTWARE,
        .eClockMode = AM_HAL_AUDADC_CLKMODE_LOW_POWER, // NEW example has _LOW_LATENCY
        .ePowerMode = AM_HAL_AUDADC_LPMODE0,           // NEW example has _LPMODE1
        .eRepeat = AM_HAL_AUDADC_REPEATING_SCAN,
        .eRepeatTrigger = AM_HAL_AUDADC_RPTTRIGSEL_INT,
        .eSampMode = AM_HAL_AUDADC_SAMPMODE_LP, // AM_HAL_AUDADC_SAMPMODE_LP, NEW example has _MED
    };

    //
    // Set up internal repeat trigger timer
    //
    am_hal_audadc_irtt_config_t AUDADCIrttConfig = {
        .bIrttEnable = true,
        .eClkDiv = AM_HAL_AUDADC_RPTT_CLK_DIV32,
    //
    // Adjust sample rate to around 16K.
    // sample rate = eClock/eClkDiv/(ui32IrttCountMax+1)
    //
#if (CLK_SRC == XTHS) // On Apollo4P_EB, XTHS is 32 MHZ.
        //.ui32IrttCountMax   = 62,
        .ui32IrttCountMax = 124, // NEW example has 62
#elif (CLK_SRC == HFRC)
        .ui32IrttCountMax = 93,
#elif (CLK_SRC == HFRC2)
        .ui32IrttCountMax = 93,
#elif (CLK_SRC == HFRC2_ADJ)
        .ui32IrttCountMax = 47,
#else
    #warning Invalid clock source.
#endif
    };

    //
    // Initialize the AUDADC and get the handle.
    //
    int ret = am_hal_audadc_initialize(0, &g_AUDADCHandle);
    if (AM_HAL_STATUS_SUCCESS != ret) {
        // if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_initialize(0, &g_AUDADCHandle)) {
        am_util_stdio_printf("Error - reservation of the AUDADC instance failed ERR %d.\n", ret);
        return NS_STATUS_INIT_FAILED;
    }
    //
    // Power on the AUDADC.
    //
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_power_control(g_AUDADCHandle, AM_HAL_SYSCTRL_WAKE, false)) {
        am_util_stdio_printf("Error - AUDADC power on failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    //#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B)
#if (CLK_SRC == HFRC2) || (CLK_SRC == HFRC2_ADJ)
    //
    // Enable hfrc2.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(200);
#endif
    //#endif

#if (CLK_SRC == HFRC2_ADJ)
    //
    // hfrc2 adj.
    //
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);
    // am_util_delay_us(1500);
    //
    //  set HF2ADJ for 24.576MHz output
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);

    am_util_delay_us(500); // wait for adj to apply
#endif

    if (AUDADCConfig.eClock == AM_HAL_AUDADC_CLKSEL_XTHS_24MHz) {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL, 0);
        am_util_delay_us(1500);
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure(g_AUDADCHandle, &AUDADCConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    //
    // Set up internal repeat trigger timer
    //
    am_hal_audadc_configure_irtt(g_AUDADCHandle, &AUDADCIrttConfig);

    //
    // Enable the AUDADC.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_enable(g_AUDADCHandle)) {
        am_util_stdio_printf("Error - enabling AUDADC failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    //
    // Enable internal repeat trigger timer
    //
    am_hal_audadc_irtt_enable(g_AUDADCHandle);

    //
    // Configure the AUDADC to use DMA for the sample transfer.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_dma(g_AUDADCHandle, &g_sAUDADCDMAConfig)) {
        am_util_stdio_printf("Error - configuring AUDADC DMA failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    //
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_audadc_interrupt_enable(g_AUDADCHandle,
                                   AM_HAL_AUDADC_INT_FIFOOVR1 | AM_HAL_AUDADC_INT_FIFOOVR2 |
                                       AM_HAL_AUDADC_INT_DERR |
                                       AM_HAL_AUDADC_INT_DCMP); //| AM_HAL_AUDADC_INT_CNVCMP |
                                                                // AM_HAL_AUDADC_INT_SCNCMP);
    return NS_STATUS_SUCCESS;
}

//*****************************************************************************
//
// When the 'g_audioRecording' flag is set, copy the latest sample to a buffer
// and set a 'ready' flag. If recording flag isn't set, discard buffer.
// If 'ready' flag is still set, the last buffer hasn't been consumed yet,
// print a debug message and overwrite.
//
//*****************************************************************************

/*void
audio_callback(void)
{
    uint32_t* pui32_buffer = (uint32_t*
)am_hal_audadc_dma_get_buffer(g_AUDADCHandle);

    if (g_audioRecording) {
        // am_util_stdio_printf("g_audio_ready = %d\n", g_audioReady);
        if (g_audioReady) {
            am_util_stdio_printf("Warning - audio buffer wasnt consumed in
time\n");
        }

        for (int i = 0; i < AUDADC_SAMPLE_BUF_SIZE; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }

        g_audioReady = true;
    }
}
*/
//*****************************************************************************
//
// Interrupt handler for the AUDADC.
//
//*****************************************************************************
void
am_audadc0_isr(void) {
    uint32_t ui32IntMask;
    //
    // Read the interrupt status.
    //
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_interrupt_status(g_AUDADCHandle, &ui32IntMask, false)) {
        am_util_stdio_printf("Error reading AUDADC interrupt status\n");
    }

    //
    // Clear the AUDADC interrupt.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_interrupt_clear(g_AUDADCHandle, ui32IntMask)) {
        am_util_stdio_printf("Error clearing AUDADC interrupt status\n");
    }

    //
    // If we got a DMA complete, set the flag.
    //
    if (ui32IntMask & AM_HAL_AUDADC_INT_FIFOOVR1) {
        if (AUDADCn(0)->DMASTAT_b.DMACPL) {
            g_bAUDADCDMAError = false;
            am_hal_audadc_interrupt_service(g_AUDADCHandle, &g_sAUDADCDMAConfig);

            g_bAUDADCDMAComplete = true;
            /*
            if (am_ai_audioIpcMode == AM_AI_AUDIO_LEGACY_MODE)
                audio_callback();
            else if (*am_ai_audioEnableRecording)
            {
                if (am_ai_audioIpcMode == AM_AI_AUDIO_CALLBACK_MODE)
                    am_ai_audioCallback();
                else
                    *am_ai_sampleReadyFlag = true;
            }*/
            g_ns_audio_config->callback(g_ns_audio_config, 0);
        }
    }

    //
    // If we got a DMA error, set the flag.
    //
    if (ui32IntMask & AM_HAL_AUDADC_INT_DERR) {
        g_bAUDADCDMAError = true;
    }
}

//*****************************************************************************
//
// Setup the AUDADC
//
//*****************************************************************************
uint32_t
audadc_init(ns_audio_config_t *cfg) {
    g_adc = (AUDADC_Type *)AUDADC_BASE;
    g_mcuctrl = (MCUCTRL_Type *)MCUCTRL_BASE;

#ifndef AM_PART_APOLLO4P
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM,
                        (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#endif

    //
    // AUDADC DMA data config:
    //   DMA buffers padded at 16B alignment.
    //
    g_sAUDADCDMAConfig.ui32SampleCount = g_ns_audio_config->numSamples;

    uint32_t ui32AUDADCDataPtr = (uint32_t)((uint32_t)(g_ns_audio_config->sampleBuffer + 3) & ~0xF);
    g_sAUDADCDMAConfig.ui32TargetAddress = ui32AUDADCDataPtr;
    g_sAUDADCDMAConfig.ui32TargetAddressReverse =
        g_sAUDADCDMAConfig.ui32TargetAddress +
        sizeof(uint32_t) * g_sAUDADCDMAConfig.ui32SampleCount;

    //
    // Power up PrePGA
    //
    am_hal_audadc_refgen_powerup();

    am_hal_audadc_pga_powerup(0);
    am_hal_audadc_pga_powerup(1);
    am_hal_audadc_pga_powerup(2);
    am_hal_audadc_pga_powerup(3);

    am_hal_audadc_gain_set(0, 2 * PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(1, 2 * PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(2, 2 * PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(3, 2 * PREAMP_FULL_GAIN);

    //
    //  Turn on mic bias
    //
    am_hal_audadc_micbias_powerup(24);
    am_util_delay_ms(400);

    //
    // Configure the AUDADC
    //
    if (audadc_config()) {
        return NS_STATUS_INIT_FAILED;
    }
    g_ns_audio_config->audioSystemHandle = g_AUDADCHandle; // wait for it to have real value

    // Gain setting
    /*g_sAudadcGainConfig.ui32LGA = (uint32_t)((float)CH_A0_GAIN_DB*2 + 12);
    g_sAudadcGainConfig.ui32HGADELTA = ((uint32_t)((float)CH_A1_GAIN_DB*2 + 12))
    - g_sAudadcGainConfig.ui32LGA; g_sAudadcGainConfig.ui32LGB =
    (uint32_t)((float)CH_B0_GAIN_DB*2 + 12); g_sAudadcGainConfig.ui32HGBDELTA =
    ((uint32_t)((float)CH_B1_GAIN_DB*2 + 12)) - g_sAudadcGainConfig.ui32LGB;
    g_sAudadcGainConfig.eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME;
    am_hal_audadc_internal_pga_config(g_AUDADCHandle, &g_sAudadcGainConfig);
*/
    // Gain setting
    g_sAudadcGainConfig.ui32LGA = CH_A0_GAIN_DB * 2 + 12;
    g_sAudadcGainConfig.ui32HGADELTA = g_sAudadcGainConfig.ui32LGA - (CH_A1_GAIN_DB * 2 + 12);
    g_sAudadcGainConfig.ui32LGB = CH_B0_GAIN_DB * 2 + 12;
    g_sAudadcGainConfig.ui32HGBDELTA = g_sAudadcGainConfig.ui32LGB - (CH_B1_GAIN_DB * 2 + 12);
    g_sAudadcGainConfig.eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME;
    am_hal_audadc_internal_pga_config(g_AUDADCHandle, &g_sAudadcGainConfig);

    // Configure the slot
    if (audadc_slot_config()) {
        return NS_STATUS_INIT_FAILED;
    }

#ifdef NEW_EXAMPLE

    #if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL) && (AS_VERSION = R4 .3.0)
    //
    // Calculate DC offset calibartion parameter.
    //
    if (AM_HAL_STATUS_SUCCESS !=
        am_hal_audadc_slot_dc_offset_calculate(g_AUDADCHandle, 4, &sOffsetCalib)) {
        am_util_stdio_printf("Error - failed to calculate offset calibartion parameter.\n");
    }
    #endif
#endif
    NVIC_SetPriority(AUDADC0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(AUDADC0_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Trigger the AUDADC sampling for the first time manually.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_sw_trigger(g_AUDADCHandle)) {
        am_util_stdio_printf("Error - triggering the AUDADC failed.\n");
        return NS_STATUS_INIT_FAILED;
    }

    return NS_STATUS_SUCCESS;
}
