//*****************************************************************************
//
//! @file am_hal_crm_private.h
//!
//! @brief Clock and Reset Management (CRM) functionality implementation.
//!
//! @addtogroup crm_ap510L Clock and Reset Management (CRM)
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
//! @cond CRM_HAL_PRIVATE_FUNC
#ifndef AM_HAL_CRM_PRIVATE_H
#define AM_HAL_CRM_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define am_hal_crm_adc_clksel_e CRM_ADCCRM_ADCCLKSEL_Enum

#define am_hal_crm_apbi2s_clksel_e CRM_APBI2SCRM_APBI2SCLKSEL_Enum

#define am_hal_crm_dispclk_clksel_e CRM_DISPCLKCRM_DISPCLKCLKSEL_Enum

#define am_hal_crm_dsipllrefclk_clksel_e CRM_DPHYPLLREFCRM_DPHYPLLREFCLKSEL_Enum

#define am_hal_crm_pdmclk_clksel_e CRM_DSPPDM0CRM_DSPPDM0CLKSEL_Enum

#define am_hal_crm_i2smclk_clksel_e CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_Enum

#define am_hal_crm_i2smclkout_clksel_e CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_Enum

#define am_hal_crm_i2srefclk_clksel_e CRM_I2S0REFCLKCRM_I2S0REFCLKCLKSEL_Enum

#define am_hal_crm_uart0hf_clksel_e CRM_UART0HFCRM_UART0HFCLKSEL_Enum

#define am_hal_crm_uart1hf_clksel_e CRM_UART1HFCRM_UART1HFCLKSEL_Enum

#define am_hal_crm_usbrefclk_clksel_e CRM_USBREFCLKCRM_USBREFCLKCLKSEL_Enum

//*****************************************************************************
//
//! @brief  Set the ADC peripheral clock
//!
//! @param  bEnable enable/disable the ADC peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_ADC_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the ADC peripheral clock
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_ADC(am_hal_crm_adc_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Reset the ADC module
//!
//! @param  bEnable enable/disable the reset of ADC module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_ADC_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the Display controller peripheral clock
//!
//! @param  bEnable enable/disable the Display controller peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DC_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Reset the DC module
//!
//! @param  bEnable enable/disable the reset of DC module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DC_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the DSI clock (DISPPHY)
//!
//! @param  bEnable enable/disable the DSI clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DSI_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Reset the DSI(DISPPHY) module
//!
//! @param  bEnable enable/disable the reset of DSI(DISPPHY) module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DSI_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the UART peripheral clock
//!
//! @param  bEnable enable/disable the Uart peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_UART0_CLOCK_SET(bool bEnable);
extern uint32_t am_hal_crm_control_UART1_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Reset the UART module
//!
//! @param  bEnable enable/disable reset of the UART module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_UART0_RESET(bool bEnable);
extern uint32_t am_hal_crm_control_UART1_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the I2S0 peripheral clock
//!
//! @param  bEnable enable/disable the I2S0 peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_I2S0_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the chosen I2S0 peripheral clock source
//!
//! @param  eClkSel choose the input clock source
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_I2S0(am_hal_crm_apbi2s_clksel_e eClkSel);

//*****************************************************************************
//
//! @brief  Reset the I2S0 module
//!
//! @param  bEnable enable/disable reset of the I2S0 module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_I2S0_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the PDM0 peripheral clock
//!
//! @param  bEnable enable/disable the PDM0 peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_PDM0_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Reset the PDM0 module
//!
//! @param  bEnable enable/disable reset of the PDM0 module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_PDM0_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the USB peripheral clock
//!
//! @param  bEnable enable/disable the USB peripheral clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_USB_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Reset the USB and USBPHY module
//!
//! @param  bEnable enable/disable reset of the USB and USBPHY module
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_USB_RESET(bool bEnable);

//*****************************************************************************
//
//! @brief  Set the Display clock
//!
//! @param  bEnable enable/disable the display clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DISPCLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the Display clock
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_DISPCLK(am_hal_crm_dispclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the DSI PLL REF clock
//!
//! @param  bEnable enable/disable the DPHY PLL REF clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_DSIPLLREFCLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the DSI PLL REF clock
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_DSIPLLREFCLK(am_hal_crm_dsipllrefclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the PDM master clock
//!
//! @param  bEnable enable/disable the PDM master clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_PDM0CLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the PDM master clock
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_PDM0CLK(am_hal_crm_pdmclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the I2S MCLK
//!
//! @param  bEnable enable/disable the I2S MCLK
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_I2S0MCLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the I2S MCLK
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_I2S0MCLK(am_hal_crm_i2smclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the I2S MCLK out
//!
//! @param  bEnable enable/disable the I2S MCLK out
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_I2S0MCLKOUT_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the I2S MCLK out
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_I2S0MCLKOUT(am_hal_crm_i2smclkout_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the I2S REFCLK
//!
//! @param  bEnable enable/disable the I2S REFCLK
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_I2S0REFCLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the I2S REFCLK
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_I2S0REFCLK(am_hal_crm_i2srefclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the UART0 HF
//!
//! @param  bEnable enable/disable the UART0 HF
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_UART0HF_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the UART0 HF
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_UART0HF(am_hal_crm_uart0hf_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the UART1 HF
//!
//! @param  bEnable enable/disable the UART1 HF
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_UART1HF_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the UART1 HF
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_UART1HF(am_hal_crm_uart1hf_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Set the USB REF clock
//!
//! @param  bEnable enable/disable the USB REF clock
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_control_USBREFCLK_CLOCK_SET(bool bEnable);

//*****************************************************************************
//
//! @brief  Configure the USB REF clock
//!
//! @param  eClkSel choose the input clock source
//! @param  ui32Divider choose the input clock source divider
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_clock_config_USBREFCLK(am_hal_crm_usbrefclk_clksel_e eClkSel, uint32_t ui32Divider);

//*****************************************************************************
//
//! @brief  Initialize the CRM
//!
//! @return Returns 0 on success
//
//*****************************************************************************
extern uint32_t am_hal_crm_initialize(void);

#define am_hal_crm_control(PID, REQUEST, bEnable) am_hal_crm_control_##PID##_##REQUEST(bEnable)

#define am_hal_crm_config(PID, eClkSel, ...) am_hal_crm_clock_config_##PID(eClkSel, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CRM_PRIVATE_H
//! @endcond CRM_HAL_PRIVATE_FUNC

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
