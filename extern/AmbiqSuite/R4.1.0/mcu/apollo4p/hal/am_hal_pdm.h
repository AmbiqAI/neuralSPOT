//*****************************************************************************
//
//! @file am_hal_pdm.h
//!
//! @brief API for the PDM module
//!
//! @addtogroup
//! @ingroup
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_1_0-8020bdf229 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_HAL_PDM_H
#define AM_HAL_PDM_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// CMSIS-style macro for handling a variable IOS module number.
//
//*****************************************************************************
#define AM_REG_PDM_NUM_MODULES                       4
#define PDMn(n) ((PDM0_Type*)(PDM0_BASE + (n * (PDM1_BASE - PDM0_BASE))))

//*****************************************************************************
//
// DMA threshold minimum.
//
// The PDM DMA works best if its threshold value is set to a multiple of 4
// between 16 and 24, but it will technically allow threshold settings between
// 4 and 24. This macro sets the minimum threshold value that the HAL layer
// will allow.
//
//*****************************************************************************
#define AM_HAL_PDM_DMA_THRESHOLD_MIN        16

//*****************************************************************************
//
// PDM-specific error conditions.
//
//*****************************************************************************
typedef enum
{
    //
    // The PDM HAL will throw this error if it can't find a threshold value to
    // match the total-count value passed in by a caller requesting a DMA
    // transfer. The PDM hardware requires all DMA transactions to be evenly
    // divisible in chunks of one FIFO size or smaller. Try changing your
    // ui32TotalCount value to a more evenly divisible number.
    //
    AM_HAL_PDM_STATUS_BAD_TOTALCOUNT = AM_HAL_STATUS_MODULE_SPECIFIC_START,
}
am_hal_pdm_status_e;

//*****************************************************************************
//
// Gain settings.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_GAIN_P345DB = 0x1F,
    AM_HAL_PDM_GAIN_P330DB = 0x1E,
    AM_HAL_PDM_GAIN_P315DB = 0x1D,
    AM_HAL_PDM_GAIN_P300DB = 0x1C,
    AM_HAL_PDM_GAIN_P285DB = 0x1B,
    AM_HAL_PDM_GAIN_P270DB = 0x1A,
    AM_HAL_PDM_GAIN_P255DB = 0x19,
    AM_HAL_PDM_GAIN_P240DB = 0x18,
    AM_HAL_PDM_GAIN_P225DB = 0x17,
    AM_HAL_PDM_GAIN_P210DB = 0x16,
    AM_HAL_PDM_GAIN_P195DB = 0x15,
    AM_HAL_PDM_GAIN_P180DB = 0x14,
    AM_HAL_PDM_GAIN_P165DB = 0x13,
    AM_HAL_PDM_GAIN_P150DB = 0x12,
    AM_HAL_PDM_GAIN_P135DB = 0x11,
    AM_HAL_PDM_GAIN_P120DB = 0x10,
    AM_HAL_PDM_GAIN_P105DB = 0x0F,
    AM_HAL_PDM_GAIN_P90DB  = 0x0E,
    AM_HAL_PDM_GAIN_P75DB  = 0x0D,
    AM_HAL_PDM_GAIN_P60DB  = 0x0C,
    AM_HAL_PDM_GAIN_P45DB  = 0x0B,
    AM_HAL_PDM_GAIN_P30DB  = 0x0A,
    AM_HAL_PDM_GAIN_P15DB  = 0x09,
    AM_HAL_PDM_GAIN_0DB    = 0x08,
    AM_HAL_PDM_GAIN_M15DB  = 0x07,
    AM_HAL_PDM_GAIN_M30DB  = 0x06,
    AM_HAL_PDM_GAIN_M45DB  = 0x05,
    AM_HAL_PDM_GAIN_M60DB  = 0x04,
    AM_HAL_PDM_GAIN_M75DB  = 0x03,
    AM_HAL_PDM_GAIN_M90DB  = 0x02,
    AM_HAL_PDM_GAIN_M105DB = 0x01,
    AM_HAL_PDM_GAIN_M120DB = 0x00
}
am_hal_pdm_gain_e;

//*****************************************************************************
//
// gain step size.
//
//*****************************************************************************
// FIXME - CORECFG1 SELSTEP Enums missing.
typedef enum
{
  AM_HAL_PDM_GAIN_STEP_0_13DB = 0,
  AM_HAL_PDM_GAIN_STEP_0_26DB = 1
}
am_hal_pdm_gain_stepsize_e;


//*****************************************************************************
//
// high pass filter enable/disable.
//
//*****************************************************************************
typedef enum
{
  AM_HAL_PDM_HIGH_PASS_ENABLE = 0,
  AM_HAL_PDM_HIGH_PASS_DISABLE = 1
}
am_hal_pdm_highpass_filter_onoff_e;

//*****************************************************************************
//
// PDM internal clock speed selection.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ   = 0,
    AM_HAL_PDM_CLK_HFXTAL               = 1,
    AM_HAL_PDM_CLK_HFRC_24MHZ           = 2
}
am_hal_pdm_clkspd_e;

//*****************************************************************************
//
// PDM clock divider setting.
// DIVMCLKQ REG_PDM_CORECFG1(3:2)
// Divide down ratio for generating internal master MCLKQ.
// Fmclkq = Fpdmclk/(DIVMCLKQ) if DIVMCLKQ > 0;
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_MCLKDIV_3 = 3,
    AM_HAL_PDM_MCLKDIV_2 = 2,
    AM_HAL_PDM_MCLKDIV_1 = 1,
}
am_hal_pdm_mclkdiv_e;

//*****************************************************************************
//
// PDMA_CKO frequency divisor. Fpdma_cko = Fmclk_l/(MCLKDIV+1)
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_PDMA_CLKO_DIV15 = 0xF,
    AM_HAL_PDM_PDMA_CLKO_DIV14 = 0xE,
    AM_HAL_PDM_PDMA_CLKO_DIV13 = 0xD,
    AM_HAL_PDM_PDMA_CLKO_DIV12 = 0xC,
    AM_HAL_PDM_PDMA_CLKO_DIV11 = 0xB,
    AM_HAL_PDM_PDMA_CLKO_DIV10 = 0xA,
    AM_HAL_PDM_PDMA_CLKO_DIV9  = 0x9,
    AM_HAL_PDM_PDMA_CLKO_DIV8  = 0x8,
    AM_HAL_PDM_PDMA_CLKO_DIV7  = 0x7,
    AM_HAL_PDM_PDMA_CLKO_DIV6  = 0x6,
    AM_HAL_PDM_PDMA_CLKO_DIV5  = 0x5,
    AM_HAL_PDM_PDMA_CLKO_DIV4  = 0x4,
    AM_HAL_PDM_PDMA_CLKO_DIV3  = 0x3,
    AM_HAL_PDM_PDMA_CLKO_DIV2  = 0x2,
    AM_HAL_PDM_PDMA_CLKO_DIV1  = 0x1,
    AM_HAL_PDM_PDMA_CLKO_DIV0  = 0x1,   // setting DIV0 is prohibited
}
am_hal_pdm_pdma_clkodiv_e;

//*****************************************************************************
//
// PDMA_CKO clock phase delay in terms of PDMCLK period to internal sampler
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE7 = 7,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE6 = 6,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE5 = 5,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE4 = 4,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE3 = 3,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE2 = 2,
    AM_HAL_PDM_CLKOUT_PHSDLY_CYCLE1 = 1,
    AM_HAL_PDM_CLKOUT_PHSDLY_NONE   = 0
}
am_hal_pdm_clkout_phsdly_e;

//*****************************************************************************
//
// Set number of PDMA_CKO cycles during gain setting changes or soft mute
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE7 = 7,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE6 = 6,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE5 = 5,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE4 = 4,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE3 = 3,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE2 = 2,
    AM_HAL_PDM_CLKOUT_DELAY_CYCLE1 = 1,
    AM_HAL_PDM_CLKOUT_DELAY_NONE   = 0
}
am_hal_pdm_clkout_delay_e;

//*****************************************************************************
//
// PCM Channel Select.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_CHANNEL_LEFT   = PDM0_CORECFG1_PCMCHSET_MONOL,
    AM_HAL_PDM_CHANNEL_RIGHT  = PDM0_CORECFG1_PCMCHSET_MONOR,
    AM_HAL_PDM_CHANNEL_STEREO = PDM0_CORECFG1_PCMCHSET_STEREO,
}
am_hal_pdm_chset_e;

//*****************************************************************************
//
// PDM power state settings.
//
//*****************************************************************************
#define AM_HAL_PDM_POWER_ON           AM_HAL_SYSCTRL_WAKE
#define AM_HAL_PDM_POWER_OFF          AM_HAL_SYSCTRL_NORMALSLEEP

//*****************************************************************************
//
// PDM interrupts.
//
//*****************************************************************************
#define AM_HAL_PDM_INT_DERR           PDM0_INTSTAT_DERR_Msk
#define AM_HAL_PDM_INT_DCMP           PDM0_INTSTAT_DCMP_Msk
#define AM_HAL_PDM_INT_UNDFL          PDM0_INTSTAT_UNDFL_Msk
#define AM_HAL_PDM_INT_OVF            PDM0_INTSTAT_OVF_Msk
#define AM_HAL_PDM_INT_THR            PDM0_INTSTAT_THR_Msk

//*****************************************************************************
//
// PDM DMA STATE.
//
//*****************************************************************************
#define AM_HAL_PDM_DMASTAT_DMATIP     PDM0_DMASTAT_DMATIP_Msk
#define AM_HAL_PDM_DMASTAT_DCMP       PDM0_DMASTAT_DMACPL_Msk
#define AM_HAL_PDM_DMASTAT_DMAERR     PDM0_DMASTAT_DMAERR_Msk

//*****************************************************************************
//
// Configuration structure for the PDM
//
//*****************************************************************************
typedef struct
{
    //
    // Clock
    //
    //Divide down ratio for generating internal master MCLKQ.
    am_hal_pdm_mclkdiv_e eClkDivider;
    // PDMA_CKO frequency divisor.Fpdma_cko = Fmclk_l/(MCLKDIV+1)
    am_hal_pdm_pdma_clkodiv_e ePDMAClkOutDivder ;

    // Gain
    am_hal_pdm_gain_e eLeftGain;
    am_hal_pdm_gain_e eRightGain;
    // Fine grain step size for smooth PGA or Softmute attenuation
    // transition0: 0.13dB1: 0.26dB
    am_hal_pdm_gain_stepsize_e eStepSize;

    // Decimation Rate
    uint32_t ui32DecimationRate;

    // Filters
    bool bHighPassEnable;
    // HPGAIN: Adjust High Pass Coefficients
    uint32_t ui32HighPassCutoff;

    // PDM Clock select.
    am_hal_pdm_clkspd_e ePDMClkSpeed;

    // PCMPACK
    bool bDataPacking;

    // CHSET
    am_hal_pdm_chset_e ePCMChannels;

    // CKODLY
    am_hal_pdm_clkout_phsdly_e bPDMSampleDelay;

    // SCYCLES
    am_hal_pdm_clkout_delay_e ui32GainChangeDelay;

    bool bSoftMute;

    bool bLRSwap;
}
am_hal_pdm_config_t;

//*****************************************************************************
//
// DMA transfer structure
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32TargetAddr;
    uint32_t ui32TargetAddrReverse;

    uint32_t ui32TotalCount;
}
am_hal_pdm_transfer_t;


typedef struct
{
    bool bValid;
}
am_hal_pdm_register_state_t;

//*****************************************************************************
//
// Structure for handling PDM HAL state information.
//
//*****************************************************************************
typedef struct
{
    am_hal_handle_prefix_t prefix;
    am_hal_pdm_register_state_t sRegState;
    uint32_t ui32Module;

    //
    // DMA transaction Tranfer Control Buffer.
    //
    uint32_t            ui32BufferPing;
    uint32_t            ui32BufferPong;
    uint32_t            ui32BufferPtr;
}
am_hal_pdm_state_t;

// Init/De-init.
extern uint32_t am_hal_pdm_initialize(uint32_t ui32Module, void **ppHandle);
extern uint32_t am_hal_pdm_deinitialize(void *pHandle);

// Power
extern uint32_t am_hal_pdm_power_control(void *pHandle, am_hal_sysctrl_power_state_e ePowerState, bool bRetainState);

// Config
extern uint32_t am_hal_pdm_configure(void *pHandle, am_hal_pdm_config_t *psConfig);

// Enable/Disable
extern uint32_t am_hal_pdm_enable(void *pHandle);
extern uint32_t am_hal_pdm_disable(void *pHandle);
extern uint32_t am_hal_pdm_reset(void *pHandle);

// Gather PDM data.
extern uint32_t am_hal_pdm_dma_start(void *pHandle, am_hal_pdm_transfer_t *pDmaCfg);
extern uint32_t am_hal_pdm_dma_state(void *pHandle);
extern uint32_t am_hal_pdm_dma_get_buffer(void *pHandle);
extern void am_hal_pdm_dma_reset_count(uint32_t count);

// Flush the PDM FIFO.
extern uint32_t am_hal_pdm_fifo_flush(void *pHandle);

// Read FIFO data.
uint32_t am_hal_pdm_fifo_data_read(void *pHandle);

uint32_t am_hal_pdm_fifo_data_reads(void *pHandle, uint8_t* buffer, uint32_t size);

uint32_t am_hal_pdm_fifo_count_get(void *pHandle);
uint32_t am_hal_pdm_fifo_threshold_setup(void *pHandle, uint32_t value);


// I2S Passthrough
extern uint32_t am_hal_pdm_i2s_enable(void *pHandle);
extern uint32_t am_hal_pdm_i2s_disable(void *pHandle);

// Interrupts.
extern uint32_t am_hal_pdm_interrupt_enable(void *pHandle, uint32_t ui32IntMask);
extern uint32_t am_hal_pdm_interrupt_disable(void *pHandle, uint32_t ui32IntMask);
extern uint32_t am_hal_pdm_interrupt_clear(void *pHandle, uint32_t ui32IntMask);
extern uint32_t am_hal_pdm_interrupt_status_get(void *pHandle, uint32_t *pui32Status, bool bEnabledOnly);
extern uint32_t am_hal_pdm_interrupt_service(void *pHandle, uint32_t ui32IntMask, am_hal_pdm_transfer_t* psConfig);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_PDM_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

