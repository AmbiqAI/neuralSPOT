//*****************************************************************************
//
//! @file am_hal_pdm.h
//!
//! @brief HAL implementation for the PDM module.
//!
//! @addtogroup pdm_ap510L PDM - Pulse Density Modulation
//! @ingroup apollo330P_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
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
//! CMSIS-style macro for handling a variable PDM module number.
//
//*****************************************************************************
#define PDMn(n)                             ((PDM0_Type*)(AM_REG_PDMn(n)))

//*****************************************************************************
//
//! DMA threshold minimum.
//
//! The PDM DMA works best if its threshold value is set to a multiple of 4
//! between 16 and 24, but it will technically allow threshold settings between
//! 4 and 24. This macro sets the minimum threshold value that the HAL layer
//! will allow.
//
//*****************************************************************************
#define AM_HAL_PDM_DMA_THRESHOLD_MIN        (16)

//*****************************************************************************
//
//! PDM-specific error conditions.
//
//*****************************************************************************
typedef enum
{
    //
    //! The PDM HAL will throw this error if it can't find a threshold value to
    //! match the total-count value passed in by a caller requesting a DMA
    //! transfer. The PDM hardware requires all DMA transactions to be evenly
    //! divisible in chunks of one FIFO size or smaller. Try changing your
    //! ui32TotalCount value to a more evenly divisible number.
    //
    AM_HAL_PDM_STATUS_BAD_TOTALCOUNT = AM_HAL_STATUS_MODULE_SPECIFIC_START,
}
am_hal_pdm_status_e;

//*****************************************************************************
//
//! Gain settings.
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
//! gain step size.
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
//! high pass filter enable/disable.
//
//*****************************************************************************
typedef enum
{
  AM_HAL_PDM_HIGH_PASS_ENABLE  = 0,
  AM_HAL_PDM_HIGH_PASS_DISABLE = 1
}
am_hal_pdm_highpass_filter_onoff_e;

// ****************************************************************************
//
//! @brief Macros to extract clock source and clock divider ratio from am_hal_pdm_clkspd_e.
//
// ****************************************************************************
#define AM_HAL_PDM_CRM_CLKSRC_POS   (0)
#define AM_HAL_PDM_CRM_CLKSRC_MSK   (0x000000FF)
#define AM_HAL_PDM_CRM_CLKDIV_POS   (8)
#define AM_HAL_PDM_CRM_CLKDIV_MSK   (0x0000FF00)

//*****************************************************************************
//
//! PDM internal clock speed selection.
//
//*****************************************************************************
typedef enum
{
    //! External crystal oscillator.
    AM_HAL_PDM_CLK_HFXTAL      = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_RF_XTAL_48MHz << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (0 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! HFRC 24 MHz, the actual frequency may change if HFADJ is enabled.
    AM_HAL_PDM_CLK_HFRC_24MHZ  = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_HFRC48 << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (1 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! HFRC 1.5 MHz, the actual frequency may change if HFADJ is enabled.
    AM_HAL_PDM_CLK_HFRC_1P5MHZ = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_HFRC48 << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (31 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! HFRC 500 kHz, the actual frequency may change if HFADJ is enabled.
    AM_HAL_PDM_CLK_HFRC_0P5MHZ = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_HFRC48 << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (95 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! External crystal oscillator divide 2.
    AM_HAL_PDM_CLK_HFXTAL_DIV2 = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_RF_XTAL_48MHz << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (1 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! External crystal oscillator divide 4.
    AM_HAL_PDM_CLK_HFXTAL_DIV4 = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_RF_XTAL_48MHz << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (3 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! External reference clock from GPIO 15.
    AM_HAL_PDM_CLK_EXTREF      = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_EXTREF_CLK << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (0 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! POSTDIV, an output of PLL.
    AM_HAL_PDM_CLK_PLL         = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_PLLPOSTDIV << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (0 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! POSTDIV divide 16.
    AM_HAL_PDM_CLK_PLL_DIV16   = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_PLLPOSTDIV << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (15 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! POSTDIV divide 48.
    AM_HAL_PDM_CLK_PLL_DIV48   = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_PLLPOSTDIV << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (47 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! FOUT4, an output of PLL, its frequency is equal to POSTDIV / 8.
    AM_HAL_PDM_CLK_PLL_FOUT4   = ((CRM_DSPPDM0CRM_DSPPDM0CLKSEL_PLLFOUT4 << AM_HAL_PDM_CRM_CLKSRC_POS) | \
                                  (0 << AM_HAL_PDM_CRM_CLKDIV_POS)),

    //! A virtual clock used internally or by the application when the PDMI2S0 operates in slave mode.
    AM_HAL_PDM_CLK_OFF         = 0xFF000000,
}
am_hal_pdm_clkspd_e;

//*****************************************************************************
//
//! PDM clock divider setting.
//! DIVMCLKQ REG_PDM_CORECFG1(3:2)
//! Divide down ratio for generating internal master MCLKQ.
//! Fmclkq = Fpdmclk/(DIVMCLKQ+1)
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_MCLKDIV_3 = 3,
    AM_HAL_PDM_MCLKDIV_2 = 2,
    AM_HAL_PDM_MCLKDIV_1 = 1,
    AM_HAL_PDM_MCLKDIV_0 = 0,
}
am_hal_pdm_mclkdiv_e;

//*****************************************************************************
//
//! PDMA_CKO frequency divisor. Fpdma_cko = Fmclk_l/(MCLKDIV+1)
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
    AM_HAL_PDM_PDMA_CLKO_DIV0  = 0x0,
}
am_hal_pdm_pdma_clkodiv_e;

//*****************************************************************************
//
//! PDMA_CKO clock phase delay in terms of PDMCLK period to internal sampler
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
//! Set number of PDMA_CKO cycles during gain setting changes or soft mute
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
//! PCM Channel Select.
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
//! The data flow direction after PDM convertion.
//!
//! The data can flow to memory via DMA, to external pads via I2S (a sub-module
//! of PDM), or to both destinations simultaneously.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_DATA_FLOW_TO_MEMORY = 0,
    AM_HAL_PDM_DATA_FLOW_TO_I2S    = 1,
    AM_HAL_PDM_DATA_FLOW_TO_BOTH   = 2,
}
am_hal_pdm_data_flow_direction_e;

//*****************************************************************************
//
//! The word width of PCM data if the data flows to memory.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PDM_DATA_WORD_WIDTH_24BITS = 0,
    AM_HAL_PDM_DATA_WORD_WIDTH_16BITS = 1,
}
am_hal_pdm_data_word_width_e;

//*****************************************************************************
//
//! @name PDM power state settings.
//! @{
//
//*****************************************************************************
#define AM_HAL_PDM_POWER_ON           AM_HAL_SYSCTRL_WAKE
#define AM_HAL_PDM_POWER_OFF          AM_HAL_SYSCTRL_NORMALSLEEP
//
//! @}
//

//*****************************************************************************
//
//! @name PDM interrupts.
//! @{
//
//*****************************************************************************
#define AM_HAL_PDM_INT_DERR           PDM0_INTSTAT_DERR_Msk
#define AM_HAL_PDM_INT_DCMP           PDM0_INTSTAT_DCMP_Msk
#define AM_HAL_PDM_INT_UNDFL          PDM0_INTSTAT_UNDFL_Msk
#define AM_HAL_PDM_INT_OVF            PDM0_INTSTAT_OVF_Msk
#define AM_HAL_PDM_INT_THR            PDM0_INTSTAT_THR_Msk
//
//! @}
//

//*****************************************************************************
//
//! @name PDM DMA STATE.
//! @{
//
//*****************************************************************************
#define AM_HAL_PDM_DMASTAT_DMATIP     PDM0_DMASTAT_DMATIP_Msk
#define AM_HAL_PDM_DMASTAT_DCMP       PDM0_DMASTAT_DMACPL_Msk
#define AM_HAL_PDM_DMASTAT_DMAERR     PDM0_DMASTAT_DMAERR_Msk
//
//! @}
//

//*****************************************************************************
//
//! Configuration structure for the PDM
//
//*****************************************************************************
typedef struct
{
    //
    // Clock
    //
    //! ide down ratio for generating internal master MCLKQ.
    am_hal_pdm_mclkdiv_e eClkDivider;

    //! PDMA_CKO frequency divisor.Fpdma_cko = Fmclk_l/(MCLKDIV+1)
    am_hal_pdm_pdma_clkodiv_e ePDMAClkOutDivder ;

    //! Gain
    am_hal_pdm_gain_e eLeftGain;
    am_hal_pdm_gain_e eRightGain;

    //! Fine grain step size for smooth PGA or Softmute attenuation
    //! transition0: 0.13dB1: 0.26dB
    am_hal_pdm_gain_stepsize_e eStepSize;

    //! Decimation Rate
    uint32_t ui32DecimationRate;

    //! Filters
    bool bHighPassEnable;
    //! HPGAIN: Adjust High Pass Coefficients
    uint32_t ui32HighPassCutoff;

    //! PDM Clock select.
    am_hal_pdm_clkspd_e ePDMClkSpeed;

    //! PCMPACK
    bool bDataPacking;

    //! CHSET
    am_hal_pdm_chset_e ePCMChannels;

    //! CKODLY
    am_hal_pdm_clkout_phsdly_e bPDMSampleDelay;

    //! SCYCLES
    am_hal_pdm_clkout_delay_e ui32GainChangeDelay;

    bool bSoftMute;

    bool bLRSwap;

    //! The data flow direction after PDM convertion.
    am_hal_pdm_data_flow_direction_e eDataFlowDirection;

    //! The word width of PCM data if the data flows to memory.
    am_hal_pdm_data_word_width_e eWordWidth;

    //! I2S role if I2S is enabled.
    bool bI2sMaster;
}
am_hal_pdm_config_t;

//*****************************************************************************
//
//! DMA transfer structure
//!
//! @note If the data cache is enabled, ensure that the starting address of the
//! DMA buffer is aligned to 32 bytes to meet the requirements of cache operations.
//!
//! @note If the data cache is enabled, the invalidation of the RX DMA buffer is
//! handled by the HAL in the am_hal_pdm_dma_get_buffer() function.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32TargetAddr;
    uint32_t ui32TargetAddrReverse;

    uint32_t ui32TotalCount;
}
am_hal_pdm_transfer_t;

//*****************************************************************************
//
//! @brief initialize the PDM device controller
//!
//! @param ui32Module - the index to the PDM
//! @param ppHandle   - the handle of initialized PDM instance
//!
//! This function should be called firstly before we use any other PDM HAL driver
//! functions.
//!
//! @return status    - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_initialize(uint32_t ui32Module, void **ppHandle);

//*****************************************************************************
//
//! @brief Uninitialize the PDM device controller
//!
//! @param pHandle - the handle of initialized PDM instance
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief PDM Power control function
//!
//! @param pHandle      - handle for the PDM.
//! @param ePowerState  - power state requested
//! @param bRetainState - boolean on whether to retain state
//!
//! This function allows advanced settings
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_power_control(void *pHandle, am_hal_sysctrl_power_state_e ePowerState, bool bRetainState);

//*****************************************************************************
//
//! @brief PDM configuration function
//!
//! @param pHandle  - handle for the module instance.
//! @param psConfig - pointer to the configuration structure.
//!
//! This function configures the PDM for operation.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_configure(void *pHandle, am_hal_pdm_config_t *psConfig);

//*****************************************************************************
//
//! @brief PDM enable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function enables the PDM operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_enable(void *pHandle);

//*****************************************************************************
//
//! @brief PDM disable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function disables the PDM operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_disable(void *pHandle);

//*****************************************************************************
//
//! @brief PDM Reset function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function reset the PDM module
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_reset(void *pHandle);

//*****************************************************************************
//
//! @brief PDM DMA NonBlocking Transfer Start
//!
//! @param pHandle - handle for the interface.
//! @param pDmaCfg - Pointer to the PDM DMA Config
//!
//! @note If the data cache is enabled, ensure that the starting address of the
//! DMA buffer is aligned to 32 bytes to meet the requirements of cache operations.
//!
//! @note If the data cache is enabled, the invalidation of the RX DMA buffer is
//! handled by the HAL in the am_hal_pdm_dma_get_buffer() function.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_dma_start(void *pHandle, am_hal_pdm_transfer_t *pDmaCfg);

//*****************************************************************************
//
//! @brief Stop PDM DMA Transfer.
//!
//! @param pHandle - handle for the interface.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_dma_stop(void *pHandle);

//*****************************************************************************
//
//! @brief Gets the PDM DMA NonBlocking Transfer State
//!
//! @param pHandle - handle for the interface.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_dma_state(void *pHandle);

//*****************************************************************************
//
//! @brief PDM DMA Get Buffer
//!
//! @param pHandle - handle for the interface.
//!
//! @return Pointer to the DMA Buffer
//
//*****************************************************************************
extern uint32_t am_hal_pdm_dma_get_buffer(void *pHandle);

//*****************************************************************************
//
//! @brief Flush the PDM FIFO.
//!
//! @param pHandle - handle for the interface.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_fifo_flush(void *pHandle);

//*****************************************************************************
//
//! @brief Read FIFO data.
//!
//! @param pHandle - handle for the interface.
//!
//! @return value of FIFOREAD
//
//*****************************************************************************
uint32_t am_hal_pdm_fifo_data_read(void *pHandle);

//*****************************************************************************
//
//! @brief Read FIFO data.
//!
//! @param pHandle - handle for the interface.
//! @param buffer  - Pointer to beffer for FIFOREAD
//! @param size    - Size to read
//!
//! @return 0
//
//*****************************************************************************
uint32_t am_hal_pdm_fifo_data_reads(void *pHandle, uint8_t* buffer, uint32_t size);

//*****************************************************************************
//
//! @brief Read FIFO data.
//!
//! @param pHandle - handle for the interface.
//!
//! @return value of FIFOCNT
//
//*****************************************************************************
uint32_t am_hal_pdm_fifo_count_get(void *pHandle);

//*****************************************************************************
//
//! @brief Set FIFOTHR Value
//!
//! @param pHandle - handle for the interface.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
uint32_t am_hal_pdm_fifo_threshold_setup(void *pHandle, uint32_t value);

//*****************************************************************************
//
//! @brief PDM -> I2S Passthrough enable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function enables the PDM -> I2S Passthrough operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_i2s_enable(void *pHandle);

//*****************************************************************************
//
//! @brief PDM -> I2S Passthrough disable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function disnables the PDM -> I2S Passthrough operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_i2s_disable(void *pHandle);

//*****************************************************************************
//
//! @brief PDM enable interrupts function
//!
//! @param pHandle - handle for the module instance.
//! @param ui32IntMask  - interface specific interrupt mask.
//!
//! This function enables the specific indicated interrupts (see above).
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_interrupt_enable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief PDM disable interrupts function
//!
//! @param pHandle           - handle for the module instance.
//! @param ui32IntMask - interface specific interrupt mask.
//!
//! This function disables the specified interrupts.
//!
//! @return status      - generic or interface specific status.
//!
//*****************************************************************************
extern uint32_t am_hal_pdm_interrupt_disable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief PDM interrupt clear
//!
//! @param pHandle           - handle for the module instance.
//! @param ui32IntMask - interface specific interrupt mask.
//!
//! This function clears the interrupts for the given peripheral.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_interrupt_clear(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief PDM get interrupt status
//!
//! @param pHandle      - handle for the module instance.
//! @param pui32Status  - pointer to a uint32_t to return the interrupt status
//! @param bEnabledOnly - If interrupt is enabled
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pdm_interrupt_status_get(void *pHandle, uint32_t *pui32Status, bool bEnabledOnly);

//*****************************************************************************
//
//! @brief PDM Interrupt Service Routine
//!
//! @param pHandle     - handle for the module instance.
//! @param ui32IntMask - uint32_t for interrupts to clear
//! @param psConfig    - Pointer to the PDM Config
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_pdm_interrupt_service(void *pHandle, uint32_t ui32IntMask, am_hal_pdm_transfer_t* psConfig);

//*****************************************************************************
//
//! @brief PDM DMA transfer continue function
//!
//! @param pHandle  - Handle for the module instance.
//! @param psConfig - Pointer to the PDM transfer configuration.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_pdm_dma_transfer_continue(void *pHandle, am_hal_pdm_transfer_t *pTransferCfg);


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

