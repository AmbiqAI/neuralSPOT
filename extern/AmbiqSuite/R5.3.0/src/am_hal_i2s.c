//*****************************************************************************
//
//! @file am_hal_i2s.c
//!
//! @brief HAL implementation for the I2S module.
//!
//! @addtogroup i2s4 I2S - Inter-IC Sound
//! @ingroup apollo5b_hal
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "am_mcu_apollo.h"

//*****************************************************************************
//
//! @name I2S magic number macros for handle verification.
//! @{
//
//*****************************************************************************
#define AM_HAL_MAGIC_I2S            0x125125
#define AM_HAL_I2S_HANDLE_VALID(h)    ((h) && ((am_hal_handle_prefix_t *)(h))->s.bInit && (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_I2S))

#define AM_HAL_I2S_CHK_HANDLE(h)                                              \
    if (!AM_HAL_I2S_HANDLE_VALID(h))                                          \
    {                                                                         \
        return AM_HAL_STATUS_INVALID_HANDLE;                                  \
    }
//! @}

//*****************************************************************************
//
//! @def USE_I2S_TWO_STAGE_DMA
//! @brief Enable the two-stage DMA request pipeline feature.
//!
//! This feature is added to ease SW ISR latency requirement to reload next
//! DMA request when the current DMA request is completed.
//!
//! To disable the 2-stage DMA feature, undefine this macro. The user needs to
//! ensure that the response time of the DMACPL ISR is less than the time it
//! takes for the I2S FIFO (typically with a size of 64 words) to become filled,
//! or else data loss may occur.
//!
//! To enable the 2-stage DMA feature, define ths macro. The user has a longer
//! response time for the DMACPL ISR, which is approximately equal to the time
//! it takes to fill the entire DMA buffer.
//
//*****************************************************************************
#ifndef AM_HAL_DISABLE_I2S_TWO_STAGE_DMA
#define USE_I2S_TWO_STAGE_DMA
#else
#undef  USE_I2S_TWO_STAGE_DMA
#endif

#define LL_MUX_SWITCH_DELAY_US      (17)
#define FORCE_HFRC_ON               (*(volatile uint32_t*)0x400200C0 |=  1)
#define RELEASE_HFRC                (*(volatile uint32_t*)0x400200C0 &= ~1)
#define XTHS_IS_ENABLED             ((MCUCTRL->XTALHSCTRL & MCUCTRL_XTALHSCTRL_XTALHSPDNB_Msk) == 1)

//*****************************************************************************
//
// Set the I2S non-NCO CLKGEN mux.
//
// Disable MCLKEN to avoid clock glitches.
//
//*****************************************************************************
#define set_clkgen_mux(x)           do {                                      \
    uint32_t mclken_ef127 = I2Sn(ui32Module)->CLKCFG_b.MCLKEN;                \
    I2Sn(ui32Module)->CLKCFG_b.MCLKEN = 0;                                    \
    I2Sn(ui32Module)->CLKCFG_b.FSEL   = x;                                    \
    I2Sn(ui32Module)->CLKCFG_b.MCLKEN = mclken_ef127;                         \
} while(0)

//*****************************************************************************
//
// Set the I2S NCO CLGEN mux.
//
// Disable REFCLKEN to avoid clock glitches.
//
//*****************************************************************************
#define set_nco_clkgen_mux(x)       do {                                      \
    uint32_t refclken_771a = I2Sn(ui32Module)->CLKCFG_b.REFCLKEN;             \
    I2Sn(ui32Module)->CLKCFG_b.REFCLKEN = 0;                                  \
    I2Sn(ui32Module)->CLKCFG_b.REFFSEL  = x;                                  \
    I2Sn(ui32Module)->CLKCFG_b.REFCLKEN = refclken_771a;                      \
} while(0)


//*****************************************************************************
//
// Extract the mux selection from a given clock.
//
//*****************************************************************************
#define extract_ll_mux(x)     ((x & AM_HAL_I2S_CLK_LL_MUX_MSK)     >> AM_HAL_I2S_CLK_LL_MUX_POS)
#define extract_nco_mux(x)    ((x & AM_HAL_I2S_CLK_NCO_MUX_MSK)    >> AM_HAL_I2S_CLK_NCO_MUX_POS)
#define extract_clkgen_mux(x) ((x & AM_HAL_I2S_CLK_CLKGEN_MUX_MSK) >> AM_HAL_I2S_CLK_CLKGEN_MUX_POS)

//*****************************************************************************
//
//! Structure for I2S registers.
//
//*****************************************************************************
typedef struct
{
    bool        bValid;

    uint32_t    regI2SCTL;
    uint32_t    regI2SDATACFG;
    uint32_t    regI2SIOCFG;
    uint32_t    regAMQCFG;
    uint32_t    regI2SCLKCFG;
    uint32_t    regIPBIRPT;
    uint32_t    regRXUPPERLIMIT;
    uint32_t    regTXLOWERLIMIT;
    uint32_t    regINTEN;
    uint32_t    regI2SDMACFG;
    uint32_t    regINTDIV;
    uint32_t    regFRACDIV;
} am_hal_i2s_register_state_t;

//*****************************************************************************
//
//! Structure for handling I2S HAL state information.
//
//*****************************************************************************
typedef struct
{
    am_hal_handle_prefix_t           prefix;

    uint32_t                         ui32Module;

    am_hal_i2s_register_state_t      sRegState;
    //
    // DMA transaction Tranfer Control Buffer.
    //
    uint32_t                ui32RxBufferPing;
    uint32_t                ui32RxBufferPong;
    uint32_t                ui32TxBufferPing;
    uint32_t                ui32TxBufferPong;
    //
    // Current DMA buffer ptr.
    //
    uint32_t                ui32RxBufferPtr;
    uint32_t                ui32TxBufferPtr;
    //
    // DMA buffer size in bytes
    //
    uint32_t                ui32RxBufferSizeBytes;
    uint32_t                ui32TxBufferSizeBytes;

    am_hal_i2s_clksel_e     eClockSel;
    bool                    bInternalMclkRequired;
}am_hal_i2s_state_t;

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
am_hal_i2s_state_t          g_I2Shandles[AM_REG_I2S_NUM_MODULES];

static am_hal_clkmgr_clock_id_e
i2s_clksrc_get(am_hal_i2s_clksel_e clksel)
{
    switch (clksel)
    {
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_62MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_31MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_16MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_8MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_4MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_2MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_977kHz:
        case eAM_HAL_I2S_CLKSEL_HFRC2_APPROX_488kHz:
        case eAM_HAL_I2S_CLKSEL_NCO_HFRC2_31MHz:
            return AM_HAL_CLKMGR_CLK_ID_HFRC2;

        case eAM_HAL_I2S_CLKSEL_HFRC_48MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_24MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_12MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_6MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_3MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_1_5MHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_750kHz:
        case eAM_HAL_I2S_CLKSEL_HFRC_375kHz:
        case eAM_HAL_I2S_CLKSEL_NCO_HFRC_48MHz:
        case eAM_HAL_I2S_CLKSEL_NCO_HFRC_48MHz_GATED:
            return AM_HAL_CLKMGR_CLK_ID_HFRC;

        case eAM_HAL_I2S_CLKSEL_XTHS_EXTREF_CLK:
        case eAM_HAL_I2S_CLKSEL_XTHS_16MHz:
        case eAM_HAL_I2S_CLKSEL_XTHS_8MHz:
        case eAM_HAL_I2S_CLKSEL_XTHS_4MHz:
        case eAM_HAL_I2S_CLKSEL_XTHS_2MHz:
        case eAM_HAL_I2S_CLKSEL_XTHS_1MHz:
        case eAM_HAL_I2S_CLKSEL_XTHS_500kHz:
            //
            // Clock select for I2S XT/EXT option: 0=XTHS 1=EXTREF.
            // If user wants to use EXTREF clock from GPIO 15, XTHSMUXSEL should be set in advance.
            //
            if (CLKGEN->HF2ADJ0_b.XTHSMUXSEL == 0)
            {
                return AM_HAL_CLKMGR_CLK_ID_XTAL_HS;
            }
            else
            {
                return AM_HAL_CLKMGR_CLK_ID_EXTREF_CLK;
            }

        case eAM_HAL_I2S_CLKSEL_NCO_XTHS_32MHz:
            return AM_HAL_CLKMGR_CLK_ID_XTAL_HS;

        case eAM_HAL_I2S_CLKSEL_PLL_FOUT4:
        case eAM_HAL_I2S_CLKSEL_PLL_FOUT3:
            return AM_HAL_CLKMGR_CLK_ID_SYSPLL;

        default:
            return AM_HAL_CLKMGR_CLK_ID_MAX;
    }
}

//*****************************************************************************
//
// Set I2S LLMux with Mux Switcing Delay
//
//*****************************************************************************
static inline void
i2s_set_llmux_with_delay(uint32_t ui32Module, uint32_t targetLLMux)
{
    am_hal_syspll_mux_select_e eLLMuxSel = AM_HAL_SYSPLL_LLMUX_I2S0_SRC_CLKGEN;
    switch (targetLLMux)
    {
        case MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_PLLFOUT4:
            eLLMuxSel = (ui32Module == 0) ? AM_HAL_SYSPLL_LLMUX_I2S0_SRC_PLL_FOUT4 :
                                            AM_HAL_SYSPLL_LLMUX_I2S1_SRC_PLL_FOUT4;
            break;

        case MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_PLLFOUT3:
            eLLMuxSel = (ui32Module == 0) ? AM_HAL_SYSPLL_LLMUX_I2S0_SRC_PLL_FOUT3 :
                                            AM_HAL_SYSPLL_LLMUX_I2S1_SRC_PLL_FOUT3;
            break;

        case MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_CLKGEN:
            eLLMuxSel = (ui32Module == 0) ? AM_HAL_SYSPLL_LLMUX_I2S0_SRC_CLKGEN :
                                            AM_HAL_SYSPLL_LLMUX_I2S1_SRC_CLKGEN;
            break;
    }
    am_hal_syspll_mux_select(eLLMuxSel, LL_MUX_SWITCH_DELAY_US);
}

//*****************************************************************************
//
// Set I2S clock by selecting the NCO mux, the LL mux, and the CLKGEN mux.
//
//*****************************************************************************
static void
i2s_clock_set(uint32_t ui32Module, am_hal_i2s_clksel_e targetClk)
{
    //
    // Extract mux selctions from target clock.
    //
    uint32_t targetNco = extract_nco_mux(targetClk);
    uint32_t targetLLMux = extract_ll_mux(targetClk);
    uint32_t targetClkgen = extract_clkgen_mux(targetClk);

    //
    // Target clock is an NCO clock.
    //
    if (targetNco == AM_HAL_I2S_CLK_NCO_ENABLE)
    {
        // Set the NCO mux: 1 (NCO), 0 (non-NCO).
        I2Sn(ui32Module)->AMQCFG_b.MCLKSRC = 1;
        // Set the NCO CLKGEN mux.
        set_nco_clkgen_mux(targetClkgen);
        // Set the non-NCO CLKGEN to off.
        set_clkgen_mux(extract_clkgen_mux(eAM_HAL_I2S_CLKSEL_OFF));
    }
    //
    // Target clock is an non-NCO clock.
    //
    else
    {
        //
        // Set the NCO mux: 1 (NCO), 0 (non-NCO).
        //
        I2Sn(ui32Module)->AMQCFG_b.MCLKSRC = 0;

        //
        // Set NCO CLKGEN to off.
        //
        set_nco_clkgen_mux(extract_clkgen_mux(eAM_HAL_I2S_CLKSEL_NCO_OFF));

        //
        // Get the current LL mux status and the CLKGEN mux status.
        // Reading the LL mux status may be inaccurate if the previous LL mux switch failed.
        //
        uint32_t clkgenStatus = I2Sn(ui32Module)->CLKCFG_b.FSEL;
        uint32_t LLMuxStatus = (ui32Module == 0) ? (SYSPLLn(0)->PLLMUXCTL_b.I2S0PLLCLKSEL) : \
                                                   (SYSPLLn(0)->PLLMUXCTL_b.I2S1PLLCLKSEL);
        //
        // The LL mux doesn't require switching.
        //
        if (targetLLMux == LLMuxStatus)
        {
            // Only change CLKGEN mux.
            if ((targetLLMux == MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_CLKGEN) && (clkgenStatus != targetClkgen))
            {
                set_clkgen_mux(targetClkgen);
            }
        }
        //
        // The LL mux requires switching, both clocks (current clock and target clock) to the LL mux input must remain active.
        //
        else
        {
            //
            // Set MCLKEN = 1, which is required by CLKGEN switching in or out, so keep the restored value here.
            //
            uint32_t restoreMCLKEN = I2Sn(ui32Module)->CLKCFG_b.MCLKEN;
            I2Sn(ui32Module)->CLKCFG_b.MCLKEN = 1;

            //
            // Set an HFRC clock for CLKGEN if CLKGEN is off.
            //
            if (clkgenStatus == extract_clkgen_mux(eAM_HAL_I2S_CLKSEL_OFF))
            {
                set_clkgen_mux(extract_clkgen_mux(eAM_HAL_I2S_CLKSEL_HFRC_6MHz));
            }

            //
            // CLKGEN -> PLL_FOUT3/PLL_FOUT4
            //
            if (LLMuxStatus == MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_CLKGEN)
            {
                // Switch the LL mux first, then switch the CLKGEN to off.
                i2s_set_llmux_with_delay(ui32Module, targetLLMux);
                set_clkgen_mux(extract_clkgen_mux(eAM_HAL_I2S_CLKSEL_OFF));
            }
            //
            // PLL_FOUT3/PLL_FOUT4 -> CLKGEN
            // PLL_FOUT3 -> PLL_FOUT4
            // PLL_FOUT4 -> PLL_FOUT3
            //
            else if ((LLMuxStatus == MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_PLLFOUT3) || (LLMuxStatus == MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_PLLFOUT4))
            {
                if (targetLLMux == MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_CLKGEN)
                {
                    // Switch LL mux to CLKGEN first, then switch the CLKGEN mux.
                    i2s_set_llmux_with_delay(ui32Module, MCUCTRL_PLLMUXCTL_I2S0PLLCLKSEL_CLKGEN);
                    set_clkgen_mux(targetClkgen);
                }
                else
                {
                    i2s_set_llmux_with_delay(ui32Module, targetLLMux);
                }
            }

            //
            // Restore MCLKEN.
            //
            I2Sn(ui32Module)->CLKCFG_b.MCLKEN = restoreMCLKEN;
        }
    }
}

//*****************************************************************************
//
// I2S initialization function
//
//*****************************************************************************
uint32_t
am_hal_i2s_initialize(uint32_t ui32Module, void **ppHandle)
{
    //
    // Compile time check to ensure ENTRY_SIZE macros are defined correctly
    // incorrect definition will cause divide by 0 error at build time
    //
#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Validate the module number
    //
    if ( ui32Module >= AM_REG_I2S_NUM_MODULES )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    if (ppHandle == NULL)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if (g_I2Shandles[ui32Module].prefix.s.bInit)
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    g_I2Shandles[ui32Module].prefix.s.bInit = true;
    g_I2Shandles[ui32Module].prefix.s.bEnable = false;
    g_I2Shandles[ui32Module].prefix.s.magic = AM_HAL_MAGIC_I2S;
    //
    // Initialize the handle.
    //
    g_I2Shandles[ui32Module].ui32Module = ui32Module;
    //
    // Return the handle.
    //
    *ppHandle = (void *)&g_I2Shandles[ui32Module];
    //
    // Return the status
    //
    return AM_HAL_STATUS_SUCCESS;
} // am_hal_i2s_initialize()

//*****************************************************************************
//
// am_hal_i2s_deinitialize
//
//*****************************************************************************
uint32_t
am_hal_i2s_deinitialize(void *pHandle)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *)pHandle;
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);
    //
    // Reset the handle.
    //
    pState->prefix.s.bInit = false;
    pState->prefix.s.magic = 0;
    pState->ui32Module = 0;
    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// I2S control function
// This function allows advanced settings
//
//*****************************************************************************
uint32_t am_hal_i2s_control(void *pHandle, am_hal_i2s_request_e eReq, void *pArgs)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t*)pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Validate the parameters
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);
#endif // AM_HAL_DISABLE_API_VALIDATION

    switch (eReq)
    {
        case AM_HAL_I2S_REQ_INTSET:
            I2Sn(ui32Module)->INTSET = *((uint32_t *)pArgs);
            break;
        case AM_HAL_I2S_REQ_INTCLR:
            I2Sn(ui32Module)->INTCLR = *((uint32_t *)pArgs);
            break;
        case AM_HAL_I2S_REQ_TXFIFOCNT:
            *((uint32_t*)pArgs) = I2Sn(ui32Module)->TXFIFOSTATUS_b.TXFIFOCNT;
            break;
        case AM_HAL_I2S_REQ_READ_RXUPPERLIMIT:
            *((uint32_t*)pArgs) = I2Sn(ui32Module)->RXUPPERLIMIT;
            break;
        case AM_HAL_I2S_REQ_READ_TXLOWERLIMIT:
            *((uint32_t*)pArgs) = I2Sn(ui32Module)->TXLOWERLIMIT;
            break;
        case AM_HAL_I2S_REQ_WRITE_RXUPPERLIMIT:
            I2Sn(ui32Module)->RXUPPERLIMIT = *((uint32_t*)pArgs);
            break;
        case AM_HAL_I2S_REQ_WRITE_TXLOWERLIMIT:
            I2Sn(ui32Module)->TXLOWERLIMIT = *((uint32_t*)pArgs);
            break;
        case AM_HAL_I2S_REQ_MAX:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// I2S configuration function.
//
//*****************************************************************************
uint32_t
am_hal_i2s_configure(void *pHandle, am_hal_i2s_config_t *psConfig)
{
    uint32_t status = AM_HAL_STATUS_SUCCESS;

    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t*)pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Validate the parameters
    //
    if ((pHandle == NULL) || (psConfig == NULL) || (pState->ui32Module >= AM_REG_I2S_NUM_MODULES))
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    AM_HAL_I2S_CHK_HANDLE(pHandle);
    //
    // Configure not allowed in Enabled state
    //
    if (pState->prefix.s.bEnable)
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    am_hal_i2s_data_format_t* pI2SData = psConfig->eData;
    am_hal_i2s_io_signal_t* pI2SIOCfg  = psConfig->eIO;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ((pI2SData->ui32ChannelNumbersPhase1 > 8) || (pI2SData->ui32ChannelNumbersPhase2 > 8) || \
        (pI2SData->ui32ChannelNumbersPhase1 == 0) || (pI2SData->ui32ChannelNumbersPhase1 + pI2SData->ui32ChannelNumbersPhase2 > 8) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    if ((pI2SData->eSampleLenPhase1 != AM_HAL_I2S_SAMPLE_LENGTH_8BITS) && (pI2SData->eSampleLenPhase1 != AM_HAL_I2S_SAMPLE_LENGTH_16BITS) && \
        (pI2SData->eSampleLenPhase1 != AM_HAL_I2S_SAMPLE_LENGTH_24BITS) && (pI2SData->eSampleLenPhase1 != AM_HAL_I2S_SAMPLE_LENGTH_32BITS) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    if ((pI2SData->eSampleLenPhase2 != AM_HAL_I2S_SAMPLE_LENGTH_8BITS) && (pI2SData->eSampleLenPhase2 != AM_HAL_I2S_SAMPLE_LENGTH_16BITS) && \
        (pI2SData->eSampleLenPhase2 != AM_HAL_I2S_SAMPLE_LENGTH_24BITS) && (pI2SData->eSampleLenPhase2 != AM_HAL_I2S_SAMPLE_LENGTH_32BITS) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    if ((pI2SData->eChannelLenPhase1 != AM_HAL_I2S_FRAME_WDLEN_8BITS) && (pI2SData->eChannelLenPhase1 != AM_HAL_I2S_FRAME_WDLEN_16BITS) && \
        (pI2SData->eChannelLenPhase1 != AM_HAL_I2S_FRAME_WDLEN_24BITS) && (pI2SData->eChannelLenPhase1 != AM_HAL_I2S_FRAME_WDLEN_32BITS) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    if ((pI2SData->eChannelLenPhase2 != AM_HAL_I2S_FRAME_WDLEN_8BITS) && (pI2SData->eChannelLenPhase2 != AM_HAL_I2S_FRAME_WDLEN_16BITS) && \
        (pI2SData->eChannelLenPhase2 != AM_HAL_I2S_FRAME_WDLEN_24BITS) && (pI2SData->eChannelLenPhase2 != AM_HAL_I2S_FRAME_WDLEN_32BITS) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    uint32_t ui32FramePeriod = 0;
    uint32_t ui32FsyncPulseWidth = 0;

    //
    // 1.Reset the serial receiver or transmitter by asserting bits RXRST and/or TXRST in the I2SCTL register
    //
    I2Sn(ui32Module)->I2SCTL = _VAL2FLD(I2S0_I2SCTL_RXRST, 1) | _VAL2FLD(I2S0_I2SCTL_TXRST, 1);
    am_hal_delay_us(200);
    //
    //2.I2S IPB clocks and IO signals
    //
    uint32_t ui32OEN = (psConfig->eXfer == AM_HAL_I2S_XFER_TX || psConfig->eXfer == AM_HAL_I2S_XFER_RXTX) ? 1 : 0;

    if (pI2SData->ePhase == AM_HAL_I2S_DATA_PHASE_SINGLE)
    {
        //
        // In mono mode, frame period (in units of sclk) can be set to once or twice of the word width.
        //
        if (pI2SData->ui32ChannelNumbersPhase1 == 1)
        {
            ui32FramePeriod = AM_HAL_I2S_CH_NUM_FOR_MONO * ui32I2sWordLength[pI2SData->eChannelLenPhase1];
        }
        else
        {
            ui32FramePeriod = (pI2SData->ui32ChannelNumbersPhase1 * ui32I2sWordLength[pI2SData->eChannelLenPhase1]);
        }
    }
    else
    {
        ui32FramePeriod = ((pI2SData->ui32ChannelNumbersPhase1 * ui32I2sWordLength[pI2SData->eChannelLenPhase1]) +  \
                           (pI2SData->ui32ChannelNumbersPhase2 * ui32I2sWordLength[pI2SData->eChannelLenPhase2]) );
    }

    switch (pI2SIOCfg->sFsyncPulseCfg.eFsyncPulseType)
    {
        case AM_HAL_I2S_FSYNC_PULSE_ONE_SUBFRAME:
            ui32FsyncPulseWidth = ui32I2sWordLength[pI2SData->eChannelLenPhase1];
            break;
        case AM_HAL_I2S_FSYNC_PULSE_ONE_BIT_CLOCK:
            ui32FsyncPulseWidth = 1;
            break;
        case AM_HAL_I2S_FSYNC_PULSE_HALF_FRAME_PERIOD:
            ui32FsyncPulseWidth = ui32FramePeriod / 2;
            break;
        case AM_HAL_I2S_FSYNC_PULSE_CUSTOM:
            ui32FsyncPulseWidth = pI2SIOCfg->sFsyncPulseCfg.ui32FsyncPulseWidth;
            //
            // ui32FsyncPulseWidth should be in the range of [0, ui32FramePeriod]
            //
            if ((ui32FsyncPulseWidth == 0) || (ui32FsyncPulseWidth >= ui32FramePeriod))
            {
                return AM_HAL_STATUS_INVALID_ARG;
            }
            break;
        default:
            return AM_HAL_STATUS_INVALID_ARG;
            break;
    }

    I2Sn(ui32Module)->I2SIOCFG =
         _VAL2FLD(I2S0_I2SIOCFG_OEN,  ui32OEN)              | /*Output enable for SDATA output */
         _VAL2FLD(I2S0_I2SIOCFG_FPER, ui32FramePeriod - 1)  | /*specifying a frame period of 2x WDLEN1 cycles*/
         _VAL2FLD(I2S0_I2SIOCFG_FSP,  pI2SIOCfg->eFyncCpol) |
         _VAL2FLD(I2S0_I2SIOCFG_PRTX, pI2SIOCfg->eTxCpol)   |
         _VAL2FLD(I2S0_I2SIOCFG_MSL,  psConfig->eMode)      |
         _VAL2FLD(I2S0_I2SIOCFG_PRx,  pI2SIOCfg->eRxCpol)   |
         _VAL2FLD(I2S0_I2SIOCFG_FWID, ui32FsyncPulseWidth - 1);
    //
    // 3.Set the audio data format.
    //
    I2Sn(ui32Module)->I2SDATACFG =
         _VAL2FLD(I2S0_I2SDATACFG_SSZ1,    pI2SData->eSampleLenPhase1)             |
         _VAL2FLD(I2S0_I2SDATACFG_JUST,    pI2SData->eDataJust)                    |
         _VAL2FLD(I2S0_I2SDATACFG_WDLEN1,  pI2SData->eChannelLenPhase1)            |
         _VAL2FLD(I2S0_I2SDATACFG_FRLEN1,  (pI2SData->ui32ChannelNumbersPhase1-1)) |
         _VAL2FLD(I2S0_I2SDATACFG_SSZ2,    pI2SData->eSampleLenPhase2)             |
         _VAL2FLD(I2S0_I2SDATACFG_DATADLY, pI2SData->eDataDelay)                   |
         _VAL2FLD(I2S0_I2SDATACFG_WDLEN2,  pI2SData->eChannelLenPhase2)            |
         _VAL2FLD(I2S0_I2SDATACFG_FRLEN2,  (pI2SData->ui32ChannelNumbersPhase2-1)) |
         _VAL2FLD(I2S0_I2SDATACFG_PH,      pI2SData->ePhase);
    //
    // 4.Control the enablement of the ASRC module in the IPB core.
    //  [1..1] ASRC sub module enable. 0: Enabled. 1: Disabled/Bypassed
    //
    // It is illegal for an I2S master device to enable ASRC
    //
    if ((psConfig->eMode == AM_HAL_I2S_IO_MODE_MASTER) && (psConfig->eASRC))
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    bool bDisableASRC = ((psConfig->eMode == AM_HAL_I2S_IO_MODE_SLAVE) && (psConfig->eASRC)) ? false : true;
    I2Sn(ui32Module)->AMQCFG_b.ASRCEN = bDisableASRC;

    //
    // IPB issue fix.
    //
    I2Sn(ui32Module)->AMQCFG_b.FIX8B16B = 1;

    //
    // 5. Clock related settings.
    //
    if ((psConfig->eMode == AM_HAL_I2S_IO_MODE_MASTER) || ((psConfig->eMode == AM_HAL_I2S_IO_MODE_SLAVE) && psConfig->eASRC))
    {
        pState->bInternalMclkRequired = true;
    }
    else
    {
        pState->bInternalMclkRequired = false;
    }

    if (pState->bInternalMclkRequired)
    {
        if (APOLLO5_B0)
        {
            //
            // For B0 part, restrict I2S to use ONLY PLL as the clock source.
            //
            if ((psConfig->eClock != eAM_HAL_I2S_CLKSEL_PLL_FOUT3) && (psConfig->eClock != eAM_HAL_I2S_CLKSEL_PLL_FOUT4))
            {
                return AM_HAL_STATUS_INVALID_ARG;
            }
        }

        //
        // Not a vaild clock source.
        //
        if (AM_HAL_CLKMGR_CLK_ID_MAX == i2s_clksrc_get(psConfig->eClock))
        {
            return AM_HAL_STATUS_INVALID_ARG;
        }

        pState->eClockSel = psConfig->eClock;

        bool bUseNcoClock = (extract_nco_mux(psConfig->eClock) == AM_HAL_I2S_CLK_NCO_ENABLE) ? true : false;
        if (bUseNcoClock)
        {
            //
            // Extract the integer part of the divisor.
            //
            uint32_t ui32I = (uint32_t)(psConfig->f64NcoDiv);
            //
            // Extract the fractional part of the divisor, multiply 2^32 and round the result.
            //
            uint64_t ui64Q = (uint64_t)((psConfig->f64NcoDiv - (double)ui32I) * 0x100000000ULL + 0.5);
            //
            // If the fractional part is saturated, add one to the integer part.
            //
            if (ui64Q == 0x100000000ULL)
            {
                ui32I = ui32I + 1;
                ui64Q = 0;
            }
            //
            // nco_ref_clk frequency must be at least 4x the desired nco_mclk frequency.
            //
            if (ui32I < 4)
            {
                return AM_HAL_STATUS_INVALID_ARG;
            }

            I2Sn(ui32Module)->INTDIV  = ui32I;
            I2Sn(ui32Module)->FRACDIV = (uint32_t)ui64Q;
        }

        //
        // Internal DIV3 divider, only work for non-NCO clock.
        //
        I2Sn(ui32Module)->CLKCFG_b.DIV3 = (psConfig->eDiv3 == 0) ? 0 : 1;
    }
    else
    {
        pState->eClockSel = eAM_HAL_I2S_CLKSEL_OFF;
    }

    //
    // 6.RXTX DMA limit: FIFO 50 percent full
    //
    I2Sn(ui32Module)->RXUPPERLIMIT = 0x20;
    I2Sn(ui32Module)->TXLOWERLIMIT = 0x20;
    //
    // Return the status.
    //
    return status;
} // am_hal_i2s_configure()

//*****************************************************************************
//
// I2S DMA nonblocking transfer function
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_transfer_start(void *pHandle,  am_hal_i2s_config_t *pConfig)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);
    //
    // Poll Transmit FIFO Status register to
    // prevent the Transmit FIFO from becoming empty.
    //
    uint32_t ui32Fifocnt  = I2Sn(ui32Module)->TXFIFOSTATUS_b.TXFIFOCNT;
    uint32_t ui32Fifofull = I2Sn(ui32Module)->TXFIFOSTATUS_b.TXFIFOFULL;
    if ( ui32Fifocnt || ui32Fifofull )
    {
        return AM_HAL_STATUS_FAIL;
    }
    //
    // Enable the transmission of serial audio.
    // TXRST & RXRST must be cleared in advance!!!
    //
    if ( pConfig->eXfer == AM_HAL_I2S_XFER_RX )
    {
        I2Sn(ui32Module)->DMACFG_b.RXDMAEN = 0x1;
        I2Sn(ui32Module)->I2SCTL = _VAL2FLD(I2S0_I2SCTL_RXEN, 1);
    }
    else if ( pConfig->eXfer == AM_HAL_I2S_XFER_TX )
    {
        I2Sn(ui32Module)->DMACFG_b.TXDMAEN = 0x1;
        I2Sn(ui32Module)->I2SCTL = _VAL2FLD(I2S0_I2SCTL_TXEN, 1);
    }
    else if ( pConfig->eXfer == AM_HAL_I2S_XFER_RXTX )
    {
        I2Sn(ui32Module)->DMACFG_b.RXDMAEN = 0x1;
        I2Sn(ui32Module)->DMACFG_b.TXDMAEN = 0x1;
        I2Sn(ui32Module)->I2SCTL = _VAL2FLD(I2S0_I2SCTL_RXEN, 1) | _VAL2FLD(I2S0_I2SCTL_TXEN, 1);
    }

    #ifdef USE_I2S_TWO_STAGE_DMA
    //
    // "Pong = 0xFFFFFFFF" means that only the ping buffer works, otherwise, set the pong buffer to the next stage of DMA.
    //
    if ((pConfig->eXfer == AM_HAL_I2S_XFER_RX) && (pState->ui32RxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPtr = pState->ui32RxBufferPong;
        I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk;
    }
    else if ((pConfig->eXfer == AM_HAL_I2S_XFER_TX) && (pState->ui32TxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPtr = pState->ui32TxBufferPong;
        I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
    }
    else if ((pConfig->eXfer == AM_HAL_I2S_XFER_RXTX) && (pState->ui32TxBufferPong != 0xFFFFFFFF) && (pState->ui32RxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPtr = pState->ui32TxBufferPong;
        I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPtr = pState->ui32RxBufferPong;
        I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk | I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
    }
    #endif

    return ui32Status;
}

//*****************************************************************************
//
// am_hal_i2s_dma_transfer_continue
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_transfer_continue(void *pHandle, am_hal_i2s_config_t* psConfig, am_hal_i2s_transfer_t *pTransferCfg)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    pState->ui32RxBufferSizeBytes = pTransferCfg->ui32RxTotalCount * 4;
    pState->ui32TxBufferSizeBytes = pTransferCfg->ui32TxTotalCount * 4;
    //
    // Once completed, software must first write the DMACFG register to 0.
    //
    I2Sn(ui32Module)->DMACFG = 0x0;
    #ifdef USE_I2S_TWO_STAGE_DMA
    I2Sn(ui32Module)->DMACFG_b.NEXTDMAEN = 1;
    #endif
    //
    // Clear dma status.
    //
    I2Sn(ui32Module)->RXDMASTAT = 0x0;
    I2Sn(ui32Module)->TXDMASTAT = 0x0;
    //
    // High Priority (service immediately)
    //
    I2Sn(ui32Module)->DMACFG_b.RXDMAPRI = 0x1;
    I2Sn(ui32Module)->DMACFG_b.TXDMAPRI = 0x1;

    switch(psConfig->eXfer)
    {
        case AM_HAL_I2S_XFER_RX:
            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPtr = pTransferCfg->ui32RxTargetAddr;
            I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->RXDMAADDR   = pState->ui32RxBufferPtr = pTransferCfg->ui32RxTargetAddr;
            I2Sn(ui32Module)->RXDMATOTCNT = pState->ui32RxBufferSizeBytes >> 2;
            #endif
            I2Sn(ui32Module)->DMACFG_b.RXDMAEN = 1;
            break;

        case AM_HAL_I2S_XFER_TX:
            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPtr = pTransferCfg->ui32TxTargetAddr;
            I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->TXDMAADDR   = pState->ui32TxBufferPtr = pTransferCfg->ui32TxTargetAddr;
            I2Sn(ui32Module)->TXDMATOTCNT = pState->ui32TxBufferSizeBytes >> 2;
            #endif
            I2Sn(ui32Module)->DMACFG_b.TXDMAEN = 1;
            break;

        case AM_HAL_I2S_XFER_RXTX:
            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPtr = pTransferCfg->ui32TxTargetAddr;
            I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPtr = pTransferCfg->ui32RxTargetAddr;
            I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk | I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->TXDMAADDR   = pState->ui32TxBufferPtr = pTransferCfg->ui32TxTargetAddr;
            I2Sn(ui32Module)->TXDMATOTCNT = pTransferCfg->ui32TxTotalCount;
            I2Sn(ui32Module)->RXDMAADDR   = pState->ui32RxBufferPtr = pTransferCfg->ui32RxTargetAddr;
            I2Sn(ui32Module)->RXDMATOTCNT = pTransferCfg->ui32RxTotalCount;
            #endif
            I2Sn(ui32Module)->DMACFG_b.RXDMAEN = 1;
            I2Sn(ui32Module)->DMACFG_b.TXDMAEN = 1;
            break;
    }

    return ui32Status;
}

//*****************************************************************************
//
// Power control function.
//
//*****************************************************************************
uint32_t
am_hal_i2s_power_control(void *pHandle,
                         am_hal_sysctrl_power_state_e ePowerState,
                         bool bRetainState)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    am_hal_pwrctrl_periph_e eI2SPowerModule = ((am_hal_pwrctrl_periph_e)(AM_HAL_PWRCTRL_PERIPH_I2S0 + ui32Module));
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);
    //
    // Decode the requested power state and update I2S operation accordingly.
    //
    switch (ePowerState)
    {
        //
        // Turn on the I2S.
        //
        case AM_HAL_SYSCTRL_WAKE:
            //
            // Make sure we don't try to restore an invalid state.
            //
            if (bRetainState && !pState->sRegState.bValid)
            {
                return AM_HAL_STATUS_INVALID_OPERATION;
            }
            //
            // Enable power control.
            //
            am_hal_pwrctrl_periph_enable(eI2SPowerModule);

            if (bRetainState)
            {
                //
                // Restore I2S registers
                //
                I2Sn(ui32Module)->I2SCTL       = pState->sRegState.regI2SCTL;
                I2Sn(ui32Module)->I2SDATACFG   = pState->sRegState.regI2SDATACFG;
                I2Sn(ui32Module)->I2SIOCFG     = pState->sRegState.regI2SIOCFG;
                I2Sn(ui32Module)->AMQCFG       = pState->sRegState.regAMQCFG;
                I2Sn(ui32Module)->CLKCFG       = pState->sRegState.regI2SCLKCFG;
                I2Sn(ui32Module)->IPBIRPT      = pState->sRegState.regIPBIRPT;
                I2Sn(ui32Module)->RXUPPERLIMIT = pState->sRegState.regRXUPPERLIMIT;
                I2Sn(ui32Module)->TXLOWERLIMIT = pState->sRegState.regTXLOWERLIMIT;
                I2Sn(ui32Module)->INTEN        = pState->sRegState.regINTEN;
                I2Sn(ui32Module)->DMACFG       = pState->sRegState.regI2SDMACFG;
                I2Sn(ui32Module)->INTDIV       = pState->sRegState.regINTDIV;
                I2Sn(ui32Module)->FRACDIV      = pState->sRegState.regFRACDIV;

                uint32_t ui32Status;

                //
                // Request I2S clock source and HFRC.
                //
                am_hal_clkmgr_user_id_e userID = (ui32Module == 0) ? AM_HAL_CLKMGR_USER_ID_I2S0 : AM_HAL_CLKMGR_USER_ID_I2S1;
                ui32Status = am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);
                if (AM_HAL_STATUS_SUCCESS != ui32Status)
                {
                    return ui32Status;
                }

                if (pState->bInternalMclkRequired)
                {
                    am_hal_clkmgr_clock_id_e clockID = i2s_clksrc_get(pState->eClockSel);
                    ui32Status = am_hal_clkmgr_clock_request(clockID, userID);
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);
                        return ui32Status;
                    }

                    //
                    // Restore clock muxes after powering I2S up.
                    //
                    i2s_clock_set(ui32Module, pState->eClockSel);
                }

                pState->sRegState.bValid = false;
            }
            break;
        //
        // Turn off the I2S.
        //
        case AM_HAL_SYSCTRL_NORMALSLEEP:
        case AM_HAL_SYSCTRL_DEEPSLEEP:
            if (bRetainState)
            {
                //
                // Set I2S clock source to CLK_OFF before powering I2S down.
                //
                i2s_clock_set(ui32Module, eAM_HAL_I2S_CLKSEL_OFF);

                //
                // Release current clock source and HFRC.
                //
                am_hal_clkmgr_user_id_e userID = (ui32Module == 0) ? AM_HAL_CLKMGR_USER_ID_I2S0 : AM_HAL_CLKMGR_USER_ID_I2S1;
                am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);

                if (pState->bInternalMclkRequired)
                {
                    am_hal_clkmgr_clock_id_e clockID = i2s_clksrc_get(pState->eClockSel);
                    am_hal_clkmgr_clock_release(clockID, userID);
                }

                //
                // Save I2S Registers
                //
                pState->sRegState.regI2SCTL       = I2Sn(ui32Module)->I2SCTL & (I2S0_I2SCTL_RXEN_Msk | I2S0_I2SCTL_TXEN_Msk);
                pState->sRegState.regI2SDATACFG   = I2Sn(ui32Module)->I2SDATACFG;
                pState->sRegState.regI2SIOCFG     = I2Sn(ui32Module)->I2SIOCFG;
                pState->sRegState.regAMQCFG       = I2Sn(ui32Module)->AMQCFG;
                pState->sRegState.regI2SCLKCFG    = I2Sn(ui32Module)->CLKCFG;
                pState->sRegState.regIPBIRPT      = I2Sn(ui32Module)->IPBIRPT;
                pState->sRegState.regRXUPPERLIMIT = I2Sn(ui32Module)->RXUPPERLIMIT;
                pState->sRegState.regTXLOWERLIMIT = I2Sn(ui32Module)->TXLOWERLIMIT;
                pState->sRegState.regINTEN        = I2Sn(ui32Module)->INTEN;
                pState->sRegState.regI2SDMACFG    = I2Sn(ui32Module)->DMACFG;
                pState->sRegState.regINTDIV       = I2Sn(ui32Module)->INTDIV;
                pState->sRegState.regFRACDIV      = I2Sn(ui32Module)->FRACDIV;

                pState->sRegState.bValid = true;
            }
            //
            // Disable power control.
            //
            am_hal_pwrctrl_periph_disable(eI2SPowerModule);
            break;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }
    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Interrupt disable.
//
//*****************************************************************************
uint32_t
am_hal_i2s_interrupt_disable(void *pHandle, uint32_t ui32IntMask)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);

    I2Sn(ui32Module)->INTEN &= ~ui32IntMask;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Interrupt clear.
//
//*****************************************************************************
uint32_t
am_hal_i2s_interrupt_clear(void *pHandle, uint32_t ui32IntMask)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);

    I2Sn(ui32Module)->INTCLR = ui32IntMask;
    *(volatile uint32_t*)(&I2Sn(ui32Module)->INTSTAT);
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Returns the interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_i2s_interrupt_status_get(void *pHandle, uint32_t *pui32Status, bool bEnabledOnly)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Check the handle.
    //
    AM_HAL_I2S_CHK_HANDLE(pHandle);
    //
    // If requested, only return the interrupts that are enabled.
    //
    if ( bEnabledOnly )
    {
        *pui32Status = I2Sn(ui32Module)->INTSTAT;
        *pui32Status &= I2Sn(ui32Module)->INTEN;
    }
    else
    {
        *pui32Status = I2Sn(ui32Module)->INTSTAT;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Returns the DMA status(TXMDASTAT/RXDMASTAT)
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_status_get(void *pHandle, uint32_t *pui32Status, am_hal_i2s_xfer_dir_e xfer)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // If requested, only return the interrupts that are enabled.
    //
    if (xfer == AM_HAL_I2S_XFER_RX)
    {
        *pui32Status = I2Sn(ui32Module)->RXDMASTAT;
    }
    else if (xfer == AM_HAL_I2S_XFER_TX)
    {
      *pui32Status = I2Sn(ui32Module)->TXDMASTAT;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// I2S interrupt service routine
//
//*****************************************************************************
uint32_t am_hal_i2s_interrupt_service(void *pHandle, uint32_t ui32IntMask, am_hal_i2s_config_t* psConfig)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = ((am_hal_i2s_state_t*)pHandle)->ui32Module;

    //
    // I2S DMA error processing.
    //
    if (I2Sn(ui32Module)->TXDMASTAT & AM_HAL_I2S_STAT_DMA_TX_ERR)
    {
        am_hal_i2s_dma_error(pHandle, AM_HAL_I2S_XFER_TX);
    }

    if (I2Sn(ui32Module)->RXDMASTAT & AM_HAL_I2S_STAT_DMA_RX_ERR)
    {
        am_hal_i2s_dma_error(pHandle, AM_HAL_I2S_XFER_RX);
    }

    //
    // Reload the DMA address and the transfer size.
    //
    #ifdef USE_I2S_TWO_STAGE_DMA
    if ((ui32IntMask & AM_HAL_I2S_INT_RXDMACPL) && (pState->ui32RxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->RXDMASTAT_b.RXDMACPL = 0;
        if ((I2Sn(ui32Module)->DMAENNEXTCTRL & I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk) == 0)
        {
            I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPtr = (pState->ui32RxBufferPtr == pState->ui32RxBufferPong) ? pState->ui32RxBufferPing : pState->ui32RxBufferPong;
            I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL  |= I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk;
        }
        else
        {
            return AM_HAL_STATUS_HW_ERR;
        }

    }

    if ((ui32IntMask & AM_HAL_I2S_INT_TXDMACPL) && (pState->ui32TxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->TXDMASTAT_b.TXDMACPL = 0;
        if ((I2Sn(ui32Module)->DMAENNEXTCTRL & I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk) == 0)
        {
            I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPtr = (pState->ui32TxBufferPtr == pState->ui32TxBufferPong) ? pState->ui32TxBufferPing : pState->ui32TxBufferPong;
            I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL  |= I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
        }
        else
        {
            return AM_HAL_STATUS_HW_ERR;
        }
    }
    #else
    //
    // In the previous I2S DMA, writing DMA registers needs deasserting T/RXDMAEN first.
    //
    if ((ui32IntMask & AM_HAL_I2S_INT_RXDMACPL) && (pState->ui32RxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->DMACFG_b.RXDMAEN     = 0;
        I2Sn(ui32Module)->RXDMASTAT_b.RXDMACPL = 0;
        I2Sn(ui32Module)->RXDMAADDR            = pState->ui32RxBufferPtr = (pState->ui32RxBufferPtr == pState->ui32RxBufferPong) ? pState->ui32RxBufferPing : pState->ui32RxBufferPong;
        I2Sn(ui32Module)->RXDMATOTCNT          = pState->ui32RxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->DMACFG_b.RXDMAEN     = 1;
    }

    if ((ui32IntMask & AM_HAL_I2S_INT_TXDMACPL) && (pState->ui32TxBufferPong != 0xFFFFFFFF))
    {
        I2Sn(ui32Module)->DMACFG_b.TXDMAEN     = 0;
        I2Sn(ui32Module)->TXDMASTAT_b.TXDMACPL = 0;
        I2Sn(ui32Module)->TXDMAADDR            = pState->ui32TxBufferPtr = (pState->ui32TxBufferPtr == pState->ui32TxBufferPong) ? pState->ui32TxBufferPing : pState->ui32TxBufferPong;
        I2Sn(ui32Module)->TXDMATOTCNT          = pState->ui32TxBufferSizeBytes >> 2;
        I2Sn(ui32Module)->DMACFG_b.TXDMAEN     = 1;
    }
    #endif

    //
    // I2S FIFO interrupts handling.
    //
    if (ui32IntMask & AM_HAL_I2S_INT_IPB)
    {
        am_hal_i2s_ipb_interrupt_service(pHandle);
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// I2S IPB interrupt service routine
//
//*****************************************************************************
uint32_t am_hal_i2s_ipb_interrupt_service(void *pHandle)
{
    uint32_t ui32Module;
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t*)pHandle;
    ui32Module = pState->ui32Module;

    uint32_t ui32IntMask = I2Sn(ui32Module)->IPBIRPT;
    //
    // When the number of samples in the Transmit FIFO drops below the value in the Transmit
    // FIFO Lower Limit register (TXLOWERLIMIT)
    //
    // Transmit FIFO become empty
    //
    if (ui32IntMask & I2S0_IPBIRPT_TXEI_Msk)
    {
        //
        // clear TX_Ei
        //
        I2Sn(ui32Module)->IPBIRPT_b.TXEI = 0x0;

        //I2Sn(ui32Module)->I2SCTL_b.TXEN  = 0x0;
        //I2Sn(ui32Module)->I2SCTL_b.TXRST = 0x0;
    }

    if (ui32IntMask & I2S0_IPBIRPT_TXFFI_Msk)
    {
        I2Sn(ui32Module)->IPBIRPT_b.TXFFI = 0x0;
    }
    //
    // Receive FIFO become full
    //
    if (ui32IntMask & I2S0_IPBIRPT_RXFI_Msk)
    {
        //
        // To clear TX_Ei and RX_Fi, '0' must be written in these fields.
        // Otherwise, the interrupt signal will remain active.
        //
        I2Sn(ui32Module)->IPBIRPT_b.RXFI = 0x0;

        //I2Sn(ui32Module)->I2SCTL_b.RXEN  = 0x0;
        //I2Sn(ui32Module)->I2SCTL_b.RXRST = 0x0;
    }
    //
    // Receive fifo high limit interrupt
    //
    if (ui32IntMask & I2S0_IPBIRPT_RXFFI_Msk)
    {
        I2Sn(ui32Module)->IPBIRPT_b.RXFFI = 0x0;
    }
    //
    // Return the status.
    //
    return ui32Status;
}

//*****************************************************************************
//
// DMA transaction configuration.
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_configure(void *pHandle, am_hal_i2s_config_t* psConfig, am_hal_i2s_transfer_t *pTransferCfg)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    AM_HAL_I2S_CHK_HANDLE(pHandle);
#endif // AM_HAL_DISABLE_API_VALIDATION
    //
    // Save the buffers.
    //
    pState->ui32RxBufferPtr  = pState->ui32RxBufferPing = pTransferCfg->ui32RxTargetAddr;
    pState->ui32RxBufferPong = pTransferCfg->ui32RxTargetAddrReverse;
    pState->ui32TxBufferPtr  = pState->ui32TxBufferPing = pTransferCfg->ui32TxTargetAddr;
    pState->ui32TxBufferPong = pTransferCfg->ui32TxTargetAddrReverse;
    pState->ui32RxBufferSizeBytes = pTransferCfg->ui32RxTotalCount * 4;
    pState->ui32TxBufferSizeBytes = pTransferCfg->ui32TxTotalCount * 4;

    //
    // Save the handle.
    //
    psConfig->eTransfer      = pTransferCfg;
    //
    // Clear the interrupts
    //
    I2Sn(ui32Module)->INTCLR = AM_HAL_I2S_INT_RXDMACPL | AM_HAL_I2S_INT_TXDMACPL |
                               AM_HAL_I2S_INT_TXREQCNT | AM_HAL_I2S_INT_RXREQCNT |
                               AM_HAL_I2S_INT_IPB;

    I2Sn(ui32Module)->IPBIRPT = 0x0;

    //
    // If the USE_NEW_DMA macro is defined, set DMACFG_b.NEXTDMAEN to enable
    // this feature.
    //
    #ifdef USE_I2S_TWO_STAGE_DMA
    I2Sn(ui32Module)->DMACFG_b.NEXTDMAEN = 1;
    #else
    I2Sn(ui32Module)->DMACFG_b.NEXTDMAEN = 0;
    #endif

    //
    // Load the DMA address and the transfer size.
    // Enable I2S interrupts.
    //
    switch ( psConfig->eXfer )
    {
        case AM_HAL_I2S_XFER_RX:

            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPing;
            I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->RXDMAADDR   = pState->ui32RxBufferPing;
            I2Sn(ui32Module)->RXDMATOTCNT = pState->ui32RxBufferSizeBytes >> 2;
            #endif

            I2Sn(ui32Module)->IPBIRPT = AM_HAL_I2S_INT_IPBIRPT_RXDMA;
            I2Sn(ui32Module)->INTEN   = AM_HAL_I2S_INT_RXDMACPL;
            break;

        case AM_HAL_I2S_XFER_TX:
            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPing;
            I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->TXDMAADDR   = pState->ui32TxBufferPing;
            I2Sn(ui32Module)->TXDMATOTCNT = pState->ui32TxBufferSizeBytes >> 2;
            #endif

            I2Sn(ui32Module)->IPBIRPT = AM_HAL_I2S_INT_IPBIRPT_TXDMA;
            I2Sn(ui32Module)->INTEN   = AM_HAL_I2S_INT_TXDMACPL;
            break;

        case AM_HAL_I2S_XFER_RXTX:
            #ifdef USE_I2S_TWO_STAGE_DMA
            I2Sn(ui32Module)->TXDMAADDRNEXT   = pState->ui32TxBufferPing;
            I2Sn(ui32Module)->TXDMATOTCNTNEXT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->RXDMAADDRNEXT   = pState->ui32RxBufferPing;
            I2Sn(ui32Module)->RXDMATOTCNTNEXT = pState->ui32RxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->DMAENNEXTCTRL   = I2S0_DMAENNEXTCTRL_RXDMAENNEXT_Msk | I2S0_DMAENNEXTCTRL_TXDMAENNEXT_Msk;
            #else
            I2Sn(ui32Module)->TXDMAADDR   = pState->ui32TxBufferPing;
            I2Sn(ui32Module)->TXDMATOTCNT = pState->ui32TxBufferSizeBytes >> 2;
            I2Sn(ui32Module)->RXDMAADDR   = pState->ui32RxBufferPing;
            I2Sn(ui32Module)->RXDMATOTCNT = pState->ui32RxBufferSizeBytes >> 2;
            #endif

            I2Sn(ui32Module)->IPBIRPT = AM_HAL_I2S_INT_IPBIRPT_RXDMA | AM_HAL_I2S_INT_IPBIRPT_TXDMA;
            I2Sn(ui32Module)->INTEN   = AM_HAL_I2S_INT_TXDMACPL | AM_HAL_I2S_INT_RXDMACPL;
            break;
    }
    //
    // High Priority (service immediately)
    //
    I2Sn(ui32Module)->DMACFG_b.RXDMAPRI = 0x1;
    I2Sn(ui32Module)->DMACFG_b.TXDMAPRI = 0x1;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// am_hal_i2s_dma_get_buffer
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_get_buffer(void *pHandle, am_hal_i2s_xfer_dir_e xfer)
{
    uint32_t ui32BufferPtr;
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;

    //
    // Get the buffer on which the DMA isn't working.
    //
    if ( AM_HAL_I2S_XFER_RX == xfer )
    {
        // "Pong = 0xFFFFFFFF" means that only the ping buffer works, just return the ping buffer.
        if (pState->ui32RxBufferPong == 0xFFFFFFFF)
        {
            ui32BufferPtr = pState->ui32RxBufferPing;
        }
        else
        {
            #ifdef USE_I2S_TWO_STAGE_DMA
            ui32BufferPtr = pState->ui32RxBufferPtr;
            #else
            ui32BufferPtr = (pState->ui32RxBufferPtr == pState->ui32RxBufferPong)? pState->ui32RxBufferPing: pState->ui32RxBufferPong;
            #endif
        }
    }
    else
    {
        // "Pong = 0xFFFFFFFF" means that only the ping buffer works, just return the ping buffer.
        if (pState->ui32TxBufferPong == 0xFFFFFFFF)
        {
            ui32BufferPtr = pState->ui32TxBufferPing;
        }
        else
        {
            #ifdef USE_I2S_TWO_STAGE_DMA
            ui32BufferPtr = pState->ui32TxBufferPtr;
            #else
            ui32BufferPtr = (pState->ui32TxBufferPtr == pState->ui32TxBufferPong)? pState->ui32TxBufferPing: pState->ui32TxBufferPong;
            #endif
        }
    }

    return ui32BufferPtr;
}

//*****************************************************************************
//
// I2S enable function
//
//*****************************************************************************
uint32_t
am_hal_i2s_enable(void *pHandle)
{
    uint32_t ui32Status;
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    am_hal_clkmgr_user_id_e userID = (ui32Module == 0) ? AM_HAL_CLKMGR_USER_ID_I2S0 : AM_HAL_CLKMGR_USER_ID_I2S1;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    AM_HAL_I2S_CHK_HANDLE(pHandle);
#endif // AM_HAL_DISABLE_API_VALIDATION

    if (pState->prefix.s.bEnable)
    {
        return AM_HAL_STATUS_SUCCESS;
    }

    //
    // Request HFRC for DMA operations.
    //
    ui32Status = am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return ui32Status;
    }

    if (pState->bInternalMclkRequired)
    {
        //
        // Request I2S clock source.
        //
        am_hal_clkmgr_clock_id_e clockID = i2s_clksrc_get(pState->eClockSel);
        ui32Status = am_hal_clkmgr_clock_request(clockID, userID);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            //
            // Release HFRC if the previous request failed.
            //
            am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);
            return ui32Status;
        }

        i2s_clock_set(ui32Module, pState->eClockSel);

        //
        // MCLKEN can only control clocks from the non-NCO CLKGEN.
        //
        I2Sn(ui32Module)->CLKCFG_b.MCLKEN = 1;

        //
        // REFCLKEN can only control clocks from the NCO CLKGEN.
        //
        I2Sn(ui32Module)->CLKCFG_b.REFCLKEN = 1;
    }
    else
    {
        //
        // If the internal MCLK isn't required, gate it.
        //
        i2s_clock_set(ui32Module, eAM_HAL_I2S_CLKSEL_OFF);
        I2Sn(ui32Module)->CLKCFG_b.MCLKEN = 0;
        I2Sn(ui32Module)->CLKCFG_b.REFCLKEN = 0;
    }

    pState->prefix.s.bEnable = true;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// I2S disable function
//
//*****************************************************************************
uint32_t
am_hal_i2s_disable(void *pHandle)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t*)pHandle;
    uint32_t ui32Module = pState->ui32Module;
    am_hal_clkmgr_user_id_e userID = (ui32Module == 0) ? AM_HAL_CLKMGR_USER_ID_I2S0 : AM_HAL_CLKMGR_USER_ID_I2S1;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    AM_HAL_I2S_CHK_HANDLE(pHandle);
#endif // AM_HAL_DISABLE_API_VALIDATION

    if (!pState->prefix.s.bEnable)
    {
        return AM_HAL_STATUS_SUCCESS;
    }

    am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_HFRC, userID);

    if (pState->bInternalMclkRequired)
    {
        //
        // MCLKEN can only control clocks from the non-NCO CLKGEN.
        //
        I2Sn(ui32Module)->CLKCFG_b.MCLKEN = 0;

        //
        // REFCLKEN can only control clocks from the NCO CLKGEN.
        //
        I2Sn(ui32Module)->CLKCFG_b.REFCLKEN = 0;

        //
        // Set I2S clock source to CLK_OFF and release this clock.
        //
        i2s_clock_set(ui32Module, eAM_HAL_I2S_CLKSEL_OFF);

        am_hal_clkmgr_clock_id_e clockID = i2s_clksrc_get(pState->eClockSel);
        am_hal_clkmgr_clock_release(clockID, userID);
    }

    pState->prefix.s.bEnable = false;

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_i2s_disable()

//*****************************************************************************
//
// am_hal_i2s_dma_transfer_complete
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_transfer_complete(void *pHandle)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Once completed, software must first write the DMACFG register to 0,
    // prior to making any update
    //
    I2Sn(ui32Module)->DMACFG = 0x0;
    //
    // Clear dma status.
    //
    I2Sn(ui32Module)->RXDMASTAT = 0x0;
    I2Sn(ui32Module)->TXDMASTAT = 0x0;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// am_hal_i2s_tx_fifo_empty
//
//*****************************************************************************
bool
am_hal_i2s_tx_fifo_empty(void *pHandle)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // AM_HAL_I2S_INT_TXDMACPL is always triggered before the completion of FIFO TX,
    // So check the FIFOCNT to guarantee all datas transfered completely before next DMA transaction.
    //
    if ( I2Sn(ui32Module)->TXFIFOSTATUS_b.TXFIFOCNT == 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//*****************************************************************************
//
// am_hal_i2s_dma_error
//
//*****************************************************************************
uint32_t
am_hal_i2s_dma_error(void *pHandle, am_hal_i2s_xfer_dir_e xfer)
{
    am_hal_i2s_state_t *pState = (am_hal_i2s_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // If an error condition did occur during a DMA operation, the DMA must first be disabled
    //
    I2Sn(ui32Module)->DMACFG = 0x0;
    //
    // DMA status bits cleared.
    //
    if (xfer == AM_HAL_I2S_XFER_RX)
    {
        I2Sn(ui32Module)->RXDMASTAT = 0x0;
    }
    else if (xfer == AM_HAL_I2S_XFER_TX)
    {
        I2Sn(ui32Module)->TXDMASTAT = 0x0;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
