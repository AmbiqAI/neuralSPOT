//*****************************************************************************
//
//! @file am_hal_stimer.h
//!
//! @brief Functions for interfacing with the system timer (STIMER).
//!
//! @addtogroup stimer4_4b STIMER - System Timer
//! @ingroup apollo4b_hal
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_STIMER_H
#define AM_HAL_STIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

//
//! Compute address of a given COMPARE register.
//! @note - The parameter n should be 0 (as only 1 stimer module exists).
//!         For Apollo3, the parameter r should be 0-7 (compare) or 0-3 (capture).
//
#define AM_HAL_STIMER_COMPARE_OFFSET (&STIMER->SCMPR1 - &STIMER->SCMPR0)
#define AM_REG_STIMER_COMPARE(n, r)     ((&STIMER->SCMPR0) +   \
                                         (r * AM_HAL_STIMER_COMPARE_OFFSET))

//! Compute address of a given CAPTURE register. r should be 0-3.
#define AM_HAL_STIMER_CAPTURE_OFFSET (&STIMER->SCAPT1 - &STIMER->SCAPT0)
#define AM_REG_STIMER_CAPTURE(n, r)     ((&STIMER->SCAPT0) +   \
                                         (r * AM_HAL_STIMER_CAPTURE_OFFSET))

//! Compute address of a given NVRAM register. r should be 0-3.
#define AM_HAL_STIMER_NVRAM_OFFSET (&STIMER->SNVR1 - &STIMER->SNVR0)
#define AM_REG_STIMER_NVRAM(n, r)       ((&STIMER->SNVR0) +    \
                                         (r * AM_HAL_STIMER_NVRAM_OFFSET))

//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @{
//! Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//
//*****************************************************************************
#define AM_HAL_STIMER_INT_COMPAREA         STIMER_STMINTSTAT_COMPAREA_Msk
#define AM_HAL_STIMER_INT_COMPAREB         STIMER_STMINTSTAT_COMPAREB_Msk
#define AM_HAL_STIMER_INT_COMPAREC         STIMER_STMINTSTAT_COMPAREC_Msk
#define AM_HAL_STIMER_INT_COMPARED         STIMER_STMINTSTAT_COMPARED_Msk
#define AM_HAL_STIMER_INT_COMPAREE         STIMER_STMINTSTAT_COMPAREE_Msk
#define AM_HAL_STIMER_INT_COMPAREF         STIMER_STMINTSTAT_COMPAREF_Msk
#define AM_HAL_STIMER_INT_COMPAREG         STIMER_STMINTSTAT_COMPAREG_Msk
#define AM_HAL_STIMER_INT_COMPAREH         STIMER_STMINTSTAT_COMPAREH_Msk

#define AM_HAL_STIMER_INT_OVERFLOW         STIMER_STMINTSTAT_OVERFLOW_Msk

#define AM_HAL_STIMER_INT_CAPTUREA         STIMER_STMINTSTAT_CAPTUREA_Msk
#define AM_HAL_STIMER_INT_CAPTUREB         STIMER_STMINTSTAT_CAPTUREB_Msk
#define AM_HAL_STIMER_INT_CAPTUREC         STIMER_STMINTSTAT_CAPTUREC_Msk
#define AM_HAL_STIMER_INT_CAPTURED         STIMER_STMINTSTAT_CAPTURED_Msk
//! @}

//*****************************************************************************
//
//! @name STimer Configuration Bits
//! @{
//! Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//
//*****************************************************************************
#define AM_HAL_STIMER_CFG_THAW              _VAL2FLD(STIMER_STCFG_FREEZE,       STIMER_STCFG_FREEZE_THAW)
#define AM_HAL_STIMER_CFG_FREEZE            _VAL2FLD(STIMER_STCFG_FREEZE,       STIMER_STCFG_FREEZE_FREEZE)
#define AM_HAL_STIMER_CFG_RUN               _VAL2FLD(STIMER_STCFG_CLEAR,        STIMER_STCFG_CLEAR_RUN)
#define AM_HAL_STIMER_CFG_CLEAR             _VAL2FLD(STIMER_STCFG_CLEAR,        STIMER_STCFG_CLEAR_CLEAR)
#define AM_HAL_STIMER_CFG_COMPARE_A_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREAEN, STIMER_STCFG_COMPAREAEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_B_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREBEN, STIMER_STCFG_COMPAREBEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_C_ENABLE  _VAL2FLD(STIMER_STCFG_COMPARECEN, STIMER_STCFG_COMPARECEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_D_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREDEN, STIMER_STCFG_COMPAREDEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_E_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREEEN, STIMER_STCFG_COMPAREEEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_F_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREFEN, STIMER_STCFG_COMPAREFEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_G_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREGEN, STIMER_STCFG_COMPAREGEN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_H_ENABLE  _VAL2FLD(STIMER_STCFG_COMPAREHEN, STIMER_STCFG_COMPAREHEN_ENABLE)
//! @}

//*****************************************************************************
//
//! @name Clock Configuration options
//! @{
//! STimer Configuration register options.
//!
//! These options are to be used with the am_hal_stimer_config() function.
//
//*****************************************************************************
#define AM_HAL_STIMER_NO_CLK            _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_NOCLK)
#if defined(AM_PART_APOLLO4)
#define AM_HAL_STIMER_HFRC_3MHZ         _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_HFRC_3MHZ)
#define AM_HAL_STIMER_HFRC_187_5KHZ     _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_HFRC_187KHZ)
#elif defined(AM_PART_APOLLO4B)
#define AM_HAL_STIMER_HFRC_6MHZ         _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_HFRC_6MHZ)
#define AM_HAL_STIMER_HFRC_375KHZ       _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_HFRC_375KHZ)
#endif
#define AM_HAL_STIMER_XTAL_32KHZ        _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_XTAL_32KHZ)
#define AM_HAL_STIMER_XTAL_16KHZ        _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_XTAL_16KHZ)
#define AM_HAL_STIMER_XTAL_1KHZ         _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_XTAL_1KHZ)
#define AM_HAL_STIMER_LFRC_1KHZ         _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_LFRC_1KHZ)
#define AM_HAL_STIMER_HFRC_CTIMER0A     _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_CTIMER0)
#define AM_HAL_STIMER_HFRC_CTIMER0B     _VAL2FLD(STIMER_STCFG_CLKSEL, STIMER_STCFG_CLKSEL_CTIMER1)
//! @}

//*****************************************************************************
//
//! @name Minimum delta value that can be set for Stimer
//! @{
//! STimer Limit options.
//!
//! This constant is relevant to the am_hal_stimer_compare_delta_set() function.
//
//*****************************************************************************
#define AM_HAL_STIMER_MIN_DELTA         2
//! @}

//*****************************************************************************
//
//! @name Capture Control Register options.
//! @{
//! Configuration options for capture control register.
//!
//! These options are to be used with the am_hal_stimer_capture_control_set
//! function.
//
//*****************************************************************************
#define AM_HAL_STIMER_CAPTURE0_ENABLE   _VAL2FLD(STIMER_CAPTURECONTROL_CAPTURE0, STIMER_CAPTURECONTROL_CAPTURE0_ENABLE)
#define AM_HAL_STIMER_CAPTURE1_ENABLE   _VAL2FLD(STIMER_CAPTURECONTROL_CAPTURE1, STIMER_CAPTURECONTROL_CAPTURE1_ENABLE)
#define AM_HAL_STIMER_CAPTURE2_ENABLE   _VAL2FLD(STIMER_CAPTURECONTROL_CAPTURE2, STIMER_CAPTURECONTROL_CAPTURE2_ENABLE)
#define AM_HAL_STIMER_CAPTURE3_ENABLE   _VAL2FLD(STIMER_CAPTURECONTROL_CAPTURE3, STIMER_CAPTURECONTROL_CAPTURE3_ENABLE)
//! @}

//*****************************************************************************
//
//! @enum STIMER_Err
//!
//! @brief STIMER Specific status codes
//
//*****************************************************************************
typedef enum
{
    //
    //! Delta too small for setting Compare
    //
    AM_HAL_STIMER_DELTA_TOO_SMALL = AM_HAL_STATUS_MODULE_SPECIFIC_START
} am_hal_stimer_err_e;

//*****************************************************************************
//
//! Stimer configuration structure
//
//*****************************************************************************
typedef struct
{
    //
    //! Configuration options for the STIMER
    //
    uint32_t ui32STimerConfig;
}
am_hal_stimer_config_t;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern uint32_t am_hal_stimer_config(uint32_t ui32STimerConfig);
extern bool am_hal_stimer_is_running(void);
extern void am_hal_stimer_reset_config(void);

extern uint32_t am_hal_stimer_counter_get(void);
extern void     am_hal_stimer_counter_clear(void);
extern uint32_t am_hal_stimer_compare_delta_set(uint32_t ui32CmprInstance,
                                                uint32_t ui32Delta);
extern uint32_t am_hal_stimer_compare_get(uint32_t ui32CmprInstance);
extern void     am_hal_stimer_capture_start(uint32_t ui32CaptureNum,
                                            uint32_t ui32GPIONumber,
                                            bool bPolarity);
extern void     am_hal_stimer_capture_stop(uint32_t ui32CaptureNum);
extern uint32_t am_hal_stimer_capture_get(uint32_t ui32CaptureNum);
extern void am_hal_stimer_nvram_set(uint32_t ui32NvramNum, uint32_t ui32NvramVal);
extern uint32_t am_hal_stimer_nvram_get(uint32_t ui32NvramNum);
extern void     am_hal_stimer_int_enable(uint32_t ui32Interrupt);
extern uint32_t am_hal_stimer_int_enable_get(void);
extern void     am_hal_stimer_int_disable(uint32_t ui32Interrupt);
extern void     am_hal_stimer_int_set(uint32_t ui32Interrupt);
extern void     am_hal_stimer_int_clear(uint32_t ui32Interrupt);
extern uint32_t am_hal_stimer_int_status_get(bool bEnabledOnly);
extern bool     am_hal_stimer_check_compare_delta_set(uint32_t ui32CmprInstance);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_STIMER_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

