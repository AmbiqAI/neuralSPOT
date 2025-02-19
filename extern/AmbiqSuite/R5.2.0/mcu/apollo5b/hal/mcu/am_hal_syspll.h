//*****************************************************************************
//
//! @file am_hal_syspll.h
//!
//! @brief Functions for interfacing with the System PLL.
//!
//! @addtogroup syspll5b SYSPLL - System PLL
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
#ifndef AM_HAL_SYSPLL_H
#define AM_HAL_SYSPLL_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @brief CMSIS-style macro for handling a variable System PLL module number.
//
//*****************************************************************************
#define SYSPLLn(n) ((MCUCTRL_Type*)(MCUCTRL_BASE + (n * (MCUCTRL_BASE - MCUCTRL_BASE))))


//*****************************************************************************
//
// System PLL Limit values
//
//*****************************************************************************
#define AM_HAL_SYSPLL_MAX_REFDIV                (63U)
#define AM_HAL_SYSPLL_MIN_FBDIV_INT_MODE        (4U)
#define AM_HAL_SYSPLL_MAX_FBDIV_INT_MODE        (960U)
#define AM_HAL_SYSPLL_MIN_FBDIV_FRAC_MODE       (10U)
#define AM_HAL_SYSPLL_MAX_FBDIV_FRAC_MODE       (96U)
#define AM_HAL_SYSPLL_VCO_LO_MODE_FREQ_MIN      (60U)
#define AM_HAL_SYSPLL_VCO_LO_MODE_FREQ_MAX      (240U)
#define AM_HAL_SYSPLL_VCO_HI_MODE_FREQ_MIN      (240U)
#define AM_HAL_SYSPLL_VCO_HI_MODE_FREQ_MAX      (960U)
#define AM_HAL_SYSPLL_POST_DIV_MAX              (7U)
#define AM_HAL_SYSPLL_POST_DIV_TOTAL_MAX        (AM_HAL_SYSPLL_POST_DIV_MAX * AM_HAL_SYSPLL_POST_DIV_MAX)

//*****************************************************************************
//
//! @name SYSPLL power state
//! @{
//
//*****************************************************************************
#define AM_HAL_SYSPLL_POWER_ON           AM_HAL_SYSCTRL_WAKE
#define AM_HAL_SYSPLL_POWER_OFF          AM_HAL_SYSCTRL_NORMALSLEEP
//! @}

// ****************************************************************************
//
//! @enum am_hal_syspll_vcosel_e
//! @brief System PLL VCO select.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_VCOSEL_VCOLO = MCUCTRL_PLLCTL0_VCOSELECT_VCOLO,
    AM_HAL_SYSPLL_VCOSEL_VCOHI = MCUCTRL_PLLCTL0_VCOSELECT_VCOHI,
} am_hal_syspll_vcosel_e;

// ****************************************************************************
//
//! @enum am_hal_syspll_bypass_e
//! @brief System PLL bypass select.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_BYPASS_NORMAL = MCUCTRL_PLLCTL0_BYPASS_NORMAL,
    AM_HAL_SYSPLL_BYPASS_BYPASS = MCUCTRL_PLLCTL0_BYPASS_BYPASS,
} am_hal_syspll_bypass_e;

// ****************************************************************************
//
//! @enum am_hal_syspll_frefsel_e
//! @brief System PLL reference clock select.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_FREFSEL_XTAL32MHz = MCUCTRL_PLLCTL0_FREFSEL_XTAL32MHz,
    AM_HAL_SYSPLL_FREFSEL_EXTREFCLK = MCUCTRL_PLLCTL0_FREFSEL_EXTREFCLK,
} am_hal_syspll_frefsel_e;

// ****************************************************************************
//
//! @enum am_hal_syspll_fraction_mode_e
//! @brief System PLL delta-sigma modulator powerdown.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_FMODE_FRACTION = MCUCTRL_PLLCTL0_DSMPD_ACTIVE,
    AM_HAL_SYSPLL_FMODE_INTEGER  = MCUCTRL_PLLCTL0_DSMPD_POWERDOWN,
} am_hal_syspll_fraction_mode_e;

// ****************************************************************************
//
//! @enum am_hal_syspll_mux_select_e
//! @brief Last-layer clock mux selects for modules with PLL as one of its
//!        clock sources.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_LLMUX_PDM_SRC_PLL,
    AM_HAL_SYSPLL_LLMUX_PDM_SRC_HFRC,
    AM_HAL_SYSPLL_LLMUX_PDM_SRC_CLKGEN,
    AM_HAL_SYSPLL_LLMUX_AUDADC_SRC_PLL,
    AM_HAL_SYSPLL_LLMUX_AUDADC_SRC_HFRC,
    AM_HAL_SYSPLL_LLMUX_AUDADC_SRC_CLKGEN,
    AM_HAL_SYSPLL_LLMUX_USB_SRC_PLL,
    AM_HAL_SYSPLL_LLMUX_USB_SRC_CLKGEN,
    AM_HAL_SYSPLL_LLMUX_I2S0_SRC_PLL_FOUT4,
    AM_HAL_SYSPLL_LLMUX_I2S0_SRC_PLL_FOUT3,
    AM_HAL_SYSPLL_LLMUX_I2S0_SRC_CLKGEN,
    AM_HAL_SYSPLL_LLMUX_I2S1_SRC_PLL_FOUT4,
    AM_HAL_SYSPLL_LLMUX_I2S1_SRC_PLL_FOUT3,
    AM_HAL_SYSPLL_LLMUX_I2S1_SRC_CLKGEN,
    // AM_HAL_SYSPLL_LLMUX_SELECT_MAX should be the last entry
    AM_HAL_SYSPLL_LLMUX_SELECT_MAX
} am_hal_syspll_mux_select_e;

// ****************************************************************************
//
//! @enum am_hal_syspll_mux_e
//! @brief Last-layer clock mux outputs for modules with PLL as one of its
//!        clock sources.
//
// ****************************************************************************
typedef enum
{
    AM_HAL_SYSPLL_LLMUX_PDM,
    AM_HAL_SYSPLL_LLMUX_AUDADC,
    AM_HAL_SYSPLL_LLMUX_USB,
    AM_HAL_SYSPLL_LLMUX_I2S0,
    AM_HAL_SYSPLL_LLMUX_I2S1,
    // AM_HAL_SYSPLL_LLMUX_MAX should be the last entry
    AM_HAL_SYSPLL_LLMUX_MAX
} am_hal_syspll_mux_e;

//*****************************************************************************
//
//! @struct am_hal_syspll_config_t
//! @brief Configuration structure for the System PLL @n
//!     PLL Output Clock rates calculation:
//!     - FBDiv = ui16FBDivInt + (ui32FBDivFrac / (2^24))
//!     - PLL VCO Frequncy, Fvco = (Fref * FBDiv / ui8RefDiv)
//!     - PLL Clkout for PDM, AUDADC and USB, F_PLL_POSTDIV = (Fvco / ui8PostDiv1 / ui8PostDiv2)
//!     - PLL Clkout for I2S, F_PLL_I2S = (F_PLL_POSTDIV / 6)  and (F_PLL_POSTDIV / 8)
//
//*****************************************************************************
typedef struct
{
    //! Select System PLL reference clock.
    am_hal_syspll_frefsel_e         eFref;

    //! Select System PLL VCO Mode.
    am_hal_syspll_vcosel_e          eVCOSel;

    //! System PLL Fraction Mode Select
    am_hal_syspll_fraction_mode_e   eFractionMode;

    //! System PLL Reference clock divide value.
    uint8_t                         ui8RefDiv;

    //! System PLL post divide 1 value.
    uint8_t                         ui8PostDiv1;

    //! System PLL post divide 2 value.
    uint8_t                         ui8PostDiv2;

    //! System PLL feedback divide value integer part.
    uint16_t                        ui16FBDivInt;

    //! System PLL feedback divide value fractional part.
    uint32_t                        ui32FBDivFrac;
} am_hal_syspll_config_t;

//! Default system PLL settings for UART applications using 32MHz XTAL as PLL
//! reference clock. PLL Outputs for this setting:
//! - UART: 49.152MHz for 3Mbps baudrate
#define AM_HAL_SYSPLL_DEFAULT_CFG_UART \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOHI, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 5, \
        .ui8PostDiv2   = 1, \
        .ui16FBDivInt  = 15, \
        .ui32FBDivFrac = 6039798, \
    }

//! Default system PLL settings for USB applications using 32MHz XTAL as PLL
//! reference clock. PLL Outputs for this setting:
//! - USB: 24MHz
#define AM_HAL_SYSPLL_DEFAULT_CFG_USB \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_INTEGER, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 4, \
        .ui8PostDiv2   = 2, \
        .ui16FBDivInt  = 12, \
        .ui32FBDivFrac = 0, \
    }

//! Default system PLL settings for AUDADC using 32MHz XTAL as PLL reference
//! clock. PLL outputs for this setting:
//! - AUDADC: 24.576MHz
#define AM_HAL_SYSPLL_DEFAULT_CFG_AUDADC \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 3, \
        .ui8PostDiv2   = 3, \
        .ui16FBDivInt  = 13, \
        .ui32FBDivFrac = 13824426, \
    }

//! Default system PLL settings for telco applications at 16kHz,16-bit,1CH
//! PLL Outputs for this setting:
//! - PDM: 2.048MHz
//! - I2S: 0.256MHz(FOUT4)
#define AM_HAL_SYSPLL_DEFAULT_CFG_TELCO_16K_16BIT_1CH \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 5, \
        .ui8PostDiv1   = 6, \
        .ui8PostDiv2   = 6, \
        .ui16FBDivInt  = 11, \
        .ui32FBDivFrac = 8724152, \
    }

//! Default system PLL settings for telco applications at 16kHz,16-bit,2CH
//! PLL Outputs for this setting:
//! - PDM: 4.096MHz
//! - I2S: 0.512MHz(FOUT4)
#define AM_HAL_SYSPLL_DEFAULT_CFG_TELCO_16K_16BIT_2CH \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 3, \
        .ui8PostDiv1   = 6, \
        .ui8PostDiv2   = 6, \
        .ui16FBDivInt  = 13, \
        .ui32FBDivFrac = 13824426, \
    }

//! Default system PLL settings for telco applications at 48kHz,16-bit,2CH
//! PLL Outputs for this setting:
//! - PDM: 12.288MHz
//! - I2S: 1.536MHz(FOUT4)
#define AM_HAL_SYSPLL_DEFAULT_CFG_TELCO_48K_16BIT_2CH \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 4, \
        .ui8PostDiv2   = 4, \
        .ui16FBDivInt  = 12, \
        .ui32FBDivFrac = 4831838, \
    }

//! Default system PLL settings for audio applications at 44.1kHz,32-bit,2CH
//! PLL Outputs for this setting:
//! - I2S: 2.8224MHz(FOUT4)
#define AM_HAL_SYSPLL_DEFAULT_CFG_AUDIO_44K1_32BIT_2CH \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 5, \
        .ui8PostDiv2   = 2, \
        .ui16FBDivInt  = 14, \
        .ui32FBDivFrac = 1879048, \
    }

//! Default system PLL settings for audio applications at 48kHz,32-bit,2CH
//! PLL Outputs for this setting:
//! - I2S: 3.072MHz(FOUT4)
#define AM_HAL_SYSPLL_DEFAULT_CFG_AUDIO_48K_32BIT_2CH \
    {\
        .eFref         = AM_HAL_SYSPLL_FREFSEL_XTAL32MHz, \
        .eVCOSel       = AM_HAL_SYSPLL_VCOSEL_VCOLO, \
        .eFractionMode = AM_HAL_SYSPLL_FMODE_FRACTION, \
        .ui8RefDiv     = 2, \
        .ui8PostDiv1   = 3, \
        .ui8PostDiv2   = 3, \
        .ui16FBDivInt  = 13, \
        .ui32FBDivFrac = 13824426, \
    }

//*****************************************************************************
//
//! @brief Generate system PLL configuration to achieve desired VCO Frequency @n
//! This function generates the following configurations for system PLL.
//! configuration not listed needs to be set in separately.
//! - eVCOSel
//! - eFractionMode
//! - ui8RefDiv
//! - ui16FBDivInt
//! - ui32FBDivFrac
//!
//! @param pConfig      - pointer to the system PLL Config structure.
//! @param fRefClkMHz   - system PLL reference clock frequency in MHz
//! @param fVCOClkMHz   - system PLL VCO output frequency desired in MHz
//!
//! @return status      - status whether valid configuration is generated:
//! - AM_HAL_STATUS_SUCCESS: configuration succesfully generated
//! - AM_HAL_STATUS_INVALID_ARG: invalid arguments received.
//! - AM_HAL_STATUS_OUT_OF_RANGE: fVCOClkMHz out of range.
//! - AM_HAL_STATUS_FAIL: No valid configuration found.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_config_generate(am_hal_syspll_config_t* pConfig, float fRefClkMHz, float fVCOClkMHz);

//*****************************************************************************
//
//  Generate system PLL configuration to achieve desired FOUTPOSTDIV
//! @brief Generate system PLL configuration to achieve desired FOUTPOSTIDV
//!        Frequency
//! @note This function is unable to generate eFref of the config. Please fill
//!       in the configuration separately
//!
//! @param pConfig              - pointer to the system PLL Config structure.
//! @param ui32RefClk_Hz        - system PLL reference clock frequency in Hz
//! @param ui32FoutPostDiv_Hz   - system PLL FOUTPOSTDIV clock frequency
//!                               desired in Hz
//!
//! @return status      - status whether valid configuration is generated:
//! - AM_HAL_STATUS_SUCCESS: configuration succesfully generated
//! - AM_HAL_STATUS_INVALID_ARG: invalid arguments received.
//! - AM_HAL_STATUS_OUT_OF_RANGE: fVCOClkMHz out of range.
//! - AM_HAL_STATUS_FAIL: No valid configuration found.
//
//*****************************************************************************
uint32_t
am_hal_syspll_config_generate_with_postdiv(am_hal_syspll_config_t* pConfig,
                                           uint32_t ui32RefClk_Hz,
                                           uint32_t ui32FoutPostDiv_Hz);

//*****************************************************************************
//
//! @brief System PLL initialization function
//!
//! @param ui32Module   - module instance.
//! @param handle       - pointer to handle pointer for the module instance.
//!
//! This function accepts a module instance, allocates the interface and then
//! returns a handle to be used by the remaining interface functions.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL initialized succesfully
//!         - AM_HAL_STATUS_OUT_OF_RANGE: Invalid ui32Module.
//!         - AM_HAL_STATUS_INVALID_ARG: Invalid ppHandle.
//!         - AM_HAL_STATUS_INVALID_OPERATION: Module already initialized.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_initialize(uint32_t ui32Module, void **ppHandle);

//*****************************************************************************
//
//! @brief System PLL deinitialization function
//!
//! @param pHandle       - pointer to handle pointer for the module instance.
//!
//! This function accepts a handle to an instance and de-initializes the
//! interface.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL deinitialized succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief System PLL enable function
//!
//! @param pHandle   - pointer to handle for the module instance.
//!
//! This function enables the system PLL operation.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL enabled succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//!         - AM_HAL_STATUS_INVALID_OPERATION: SIMOBUCK force no in place. Make
//!             sure am_bsp_low_power_init() is invoked before enabling system
//!             PLL.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_enable(void *pHandle);

//*****************************************************************************
//
//! @brief System PLL disable function
//!
//! @param pHandle   - pointer to handle for the module instance.
//!
//! This function disables the system PLL operation.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL disabled succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_disable(void *pHandle);

//*****************************************************************************
//
//! @brief System PLL configuration function
//!
//! @param pHandle   - pointer to handle for the module instance.
//! @param psConfig  - pointer to the configuration structure.
//!
//! This function configures the system PLL for operation.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL configured succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//!         - AM_HAL_STATUS_INVALID_OPERATION: Invalid Operation. Make sure
//!             that system PLL is not enabled.
//!         - AM_HAL_STATUS_INVALID_ARG: Invalid system PLL configuration. Make
//!             sure that config is valid.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_configure(void *pHandle, am_hal_syspll_config_t *psConfig);

//*****************************************************************************
//
//! @brief System PLL Get PLL Lock function
//!
//! @param pHandle   - pointer to handle for the module instance.
//! @param pLock     - pointer to boolean of PLL lock status.
//!
//! This function writes true to pLock when PLL clock is ready, and writes
//! false otherwise.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL lock status read succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_lock_read(void *pHandle, bool *pll_ready);

//*****************************************************************************
//
//! @brief System PLL Wait PLL Lock function
//!
//! @param pHandle   - pointer to handle for the module instance.
//!
//! This function waits for PLL lock according to System PLL settings
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL locked.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//!         - AM_HAL_STATUS_INVALID_OPERATION: System PLL not enabled
//!         - AM_HAL_STATUS_TIMEOUT: System PLL lock wait timed out
//
//*****************************************************************************
extern uint32_t am_hal_syspll_lock_wait(void *pHandle);

//*****************************************************************************
//
//! @brief System PLL refClk bypass function
//!
//! @param pHandle   - handle for the module instance.
//! @param bBypass   - boolean setting to bypass refClk.
//!
//! This function sets bypass setting to bypass PLL reference clock to its
//! output. This function is for debugging purposes only.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL bypass setting set succesfully.
//!         - AM_HAL_STATUS_INVALID_HANDLE: Invalid pHandle.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_bypass_set(void *pHandle, bool bBypass);

//*****************************************************************************
//
//! @brief System PLL Mux select
//!
//! @param eMuxSel   - am_hal_syspll_mux_select_e value for mux selection.
//! @param ui32MuxSwitchDelay - Time to wait for mux switch complete in us
//!
//! This function set last level mux select for modules using PLL clock as one
//! of its clock source.
//! Note: Do not call this function directly. Use respective module's HAL to
//!       select the clock. Calling this directly might cause clock malfunction.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL MUX selected succesfully.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_mux_select(am_hal_syspll_mux_select_e eMuxSel, uint32_t ui32MuxSwitchDelay);

//*****************************************************************************
//
//! @brief System PLL Mux Get
//!
//! @param eMuxOut   - am_hal_syspll_mux_e value for mux to query
//! @param pEMuxSel  - pointer to am_hal_syspll_mux_select_e variable to return
//!                    query result.
//!
//!
//! This function get current setting of last level mux select for modules
//! using PLL clock as one of its clock source.
//!
//! @return status      - generic or interface specific status.
//!         - AM_HAL_STATUS_SUCCESS: System PLL MUX setting get successfully.
//
//*****************************************************************************
extern uint32_t am_hal_syspll_mux_get(am_hal_syspll_mux_e eMuxOut, am_hal_syspll_mux_select_e *pEMuxSel);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_SYSPLL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************


