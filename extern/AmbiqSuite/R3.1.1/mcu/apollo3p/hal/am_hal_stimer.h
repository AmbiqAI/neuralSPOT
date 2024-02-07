//*****************************************************************************
//
//! @file  am_hal_stimer.h
//!
//! @brief Functions for interfacing with the system timer (STIMER).
//!
//! @addtogroup stimer3p STIMER - System Timer
//! @ingroup apollo3p_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
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
#define AM_REG_STIMER_COMPARE(n, r)     (CTIMERADDRn(CTIMER, n, SCMPR0) +   \
                                         (r * (offsetof(CTIMER_Type, SCMPR1) - offsetof(CTIMER_Type, SCMPR0))))

//! Compute address of a given CAPTURE register. r should be 0-3.
#define AM_REG_STIMER_CAPTURE(n, r)     (CTIMERADDRn(CTIMER, n, SCAPT0) +   \
                                         (r * (offsetof(CTIMER_Type, SCAPT1) - offsetof(CTIMER_Type, SCAPT0))))

//! Compute address of a given NVRAM register. r should be 0-3.
#define AM_REG_STIMER_NVRAM(n, r)       (CTIMERADDRn(CTIMER, n, SNVR0) +    \
                                         (r * (offsetof(CTIMER_Type, SNVR1) - offsetof(CTIMER_Type, SNVR0))))


//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//! @{
//
//*****************************************************************************
#define AM_HAL_STIMER_INT_COMPAREA         CTIMER_STMINTSTAT_COMPAREA_Msk
#define AM_HAL_STIMER_INT_COMPAREB         CTIMER_STMINTSTAT_COMPAREB_Msk
#define AM_HAL_STIMER_INT_COMPAREC         CTIMER_STMINTSTAT_COMPAREC_Msk
#define AM_HAL_STIMER_INT_COMPARED         CTIMER_STMINTSTAT_COMPARED_Msk
#define AM_HAL_STIMER_INT_COMPAREE         CTIMER_STMINTSTAT_COMPAREE_Msk
#define AM_HAL_STIMER_INT_COMPAREF         CTIMER_STMINTSTAT_COMPAREF_Msk
#define AM_HAL_STIMER_INT_COMPAREG         CTIMER_STMINTSTAT_COMPAREG_Msk
#define AM_HAL_STIMER_INT_COMPAREH         CTIMER_STMINTSTAT_COMPAREH_Msk

#define AM_HAL_STIMER_INT_OVERFLOW         CTIMER_STMINTSTAT_OVERFLOW_Msk

#define AM_HAL_STIMER_INT_CAPTUREA         CTIMER_STMINTSTAT_CAPTUREA_Msk
#define AM_HAL_STIMER_INT_CAPTUREB         CTIMER_STMINTSTAT_CAPTUREB_Msk
#define AM_HAL_STIMER_INT_CAPTUREC         CTIMER_STMINTSTAT_CAPTUREC_Msk
#define AM_HAL_STIMER_INT_CAPTURED         CTIMER_STMINTSTAT_CAPTURED_Msk

//! @}

//*****************************************************************************
//
//! @name STimer Configuration Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//! @{
//
//*****************************************************************************
#define AM_HAL_STIMER_CFG_THAW              _VAL2FLD(CTIMER_STCFG_FREEZE,       CTIMER_STCFG_FREEZE_THAW)
#define AM_HAL_STIMER_CFG_FREEZE            _VAL2FLD(CTIMER_STCFG_FREEZE,       CTIMER_STCFG_FREEZE_FREEZE)
#define AM_HAL_STIMER_CFG_RUN               _VAL2FLD(CTIMER_STCFG_CLEAR,        CTIMER_STCFG_CLEAR_RUN)
#define AM_HAL_STIMER_CFG_CLEAR             _VAL2FLD(CTIMER_STCFG_CLEAR,        CTIMER_STCFG_CLEAR_CLEAR)
#define AM_HAL_STIMER_CFG_COMPARE_A_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_A_EN, CTIMER_STCFG_COMPARE_A_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_B_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_B_EN, CTIMER_STCFG_COMPARE_B_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_C_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_C_EN, CTIMER_STCFG_COMPARE_C_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_D_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_D_EN, CTIMER_STCFG_COMPARE_D_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_E_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_E_EN, CTIMER_STCFG_COMPARE_E_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_F_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_F_EN, CTIMER_STCFG_COMPARE_F_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_G_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_G_EN, CTIMER_STCFG_COMPARE_G_EN_ENABLE)
#define AM_HAL_STIMER_CFG_COMPARE_H_ENABLE  _VAL2FLD(CTIMER_STCFG_COMPARE_H_EN, CTIMER_STCFG_COMPARE_H_EN_ENABLE)

//! @}

//*****************************************************************************
//
//! @name Clock Configuration options
//! @brief STimer Configuration register options.
//!
//! These options are to be used with the am_hal_stimer_config() function.
//!  @{
//
//*****************************************************************************
#define AM_HAL_STIMER_NO_CLK            _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_NOCLK)
#define AM_HAL_STIMER_HFRC_3MHZ         _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_HFRC_DIV16)
#define AM_HAL_STIMER_HFRC_187_5KHZ     _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_HFRC_DIV256)
#define AM_HAL_STIMER_XTAL_32KHZ        _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_XTAL_DIV1)
#define AM_HAL_STIMER_XTAL_16KHZ        _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_XTAL_DIV2)
#define AM_HAL_STIMER_XTAL_1KHZ         _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_XTAL_DIV32)
#define AM_HAL_STIMER_LFRC_1KHZ         _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_LFRC_DIV1)
#define AM_HAL_STIMER_HFRC_CTIMER0A     _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_CTIMER0A)
#define AM_HAL_STIMER_HFRC_CTIMER0B     _VAL2FLD(CTIMER_STCFG_CLKSEL, CTIMER_STCFG_CLKSEL_CTIMER0B)
//! @}



//*****************************************************************************
//
//! @name Capture Control Register options.
//! @brief Configuration options for capture control register.
//!
//! These options are to be used with the am_hal_stimer_capture_control_set
//! function.
//!  @{
//
//*****************************************************************************
#define AM_HAL_STIMER_CAPTURE0_ENABLE   _VAL2FLD(CTIMER_CAPTURECONTROL_CAPTURE0, CTIMER_CAPTURECONTROL_CAPTURE0_ENABLE)
#define AM_HAL_STIMER_CAPTURE1_ENABLE   _VAL2FLD(CTIMER_CAPTURECONTROL_CAPTURE1, CTIMER_CAPTURECONTROL_CAPTURE1_ENABLE)
#define AM_HAL_STIMER_CAPTURE2_ENABLE   _VAL2FLD(CTIMER_CAPTURECONTROL_CAPTURE2, CTIMER_CAPTURECONTROL_CAPTURE2_ENABLE)
#define AM_HAL_STIMER_CAPTURE3_ENABLE   _VAL2FLD(CTIMER_CAPTURECONTROL_CAPTURE3, CTIMER_CAPTURECONTROL_CAPTURE3_ENABLE)

//! @}

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

//*****************************************************************************
//
//! @brief Set up the stimer.
//!
//! @param ui32STimerConfig - The value to load into the configuration reg.
//!
//! @note This function should be used to perform the initial set-up of the
//! stimer.
//!
//! @return The 32-bit current config of the STimer Config register
//
//*****************************************************************************
extern uint32_t am_hal_stimer_config(uint32_t ui32STimerConfig);

//*****************************************************************************
//
//! @brief Get the current stimer value.
//!
//! This function can be used to read, uninvasively, the value in the stimer.
//!
//! @return The 32-bit value from the STimer counter register.
//
//*****************************************************************************
extern uint32_t am_hal_stimer_counter_get(void);

//*****************************************************************************
//
//! @brief Clear the stimer counter.
//!
//! This function clears the STimer Counter and leaves the stimer running.
//
//*****************************************************************************
extern void     am_hal_stimer_counter_clear(void);

//*****************************************************************************
//
//! @brief Set the compare value.
//!
//! @param ui32CmprInstance - The compare register instance number (0-7).
//! @param ui32Delta - The value to add to the STimer counter and load into
//!        the comparator register.
//!
//! @note There is no way to set an absolute value into a comparator register.
//!       Only deltas added to the STimer counter can be written to the compare
//!       registers.
//!
//! @return 0 on success.
//
//*****************************************************************************
extern uint32_t am_hal_stimer_compare_delta_set(uint32_t ui32CmprInstance,
                                                uint32_t ui32Delta);
//*****************************************************************************
//
//! @brief Get the current stimer compare register value.
//!
//! @param ui32CmprInstance - The compare register instance number (0-7).
//!
//! This function can be used to read the value in an stimer compare register.
//
//*****************************************************************************
extern uint32_t am_hal_stimer_compare_get(uint32_t ui32CmprInstance);

//*****************************************************************************
//
//! @brief Start capturing data with the specified capture register.
//!
//! @param ui32CaptureNum - The compare register instance number (0-7)
//! @param ui32GPIONumber - The compare register instance number (0-7)
//! @param bPolarity: false (0) = Capture on low to high transition.\n
//!                   true  (1) = Capture on high to low transition.
//!
//! Use this function to start capturing.
//
//*****************************************************************************
extern void     am_hal_stimer_capture_start(uint32_t ui32CaptureNum,
                                            uint32_t ui32GPIONumber,
                                            bool bPolarity);

//*****************************************************************************
//
//! @brief Start capturing data with the specified capture register.
//!
//! @param ui32CaptureNum - - The Capture Register Number to read.
//!
//! Use this function to start capturing.
//
//*****************************************************************************
extern void     am_hal_stimer_capture_stop(uint32_t ui32CaptureNum);

//*****************************************************************************
//
//! @brief Get the current stimer capture register value.
//!
//! @param ui32CaptureNum - The Capture Register Number to read.
//!
//! This function can be used to read the value in an stimer capture register.
//!
//! @return Stimer Capture Register Value.
//
//*****************************************************************************
extern uint32_t am_hal_stimer_capture_get(uint32_t ui32CaptureNum);

//*****************************************************************************
//
//! @brief Get the current stimer nvram register value.
//!
//! @param ui32NvramNum - The NVRAM Register Number.
//! @param ui32NvramVal - The value to write to NVRAM.
//!
//! This function can be used to read the value in an stimer NVRAM register.
//
//*****************************************************************************
extern void am_hal_stimer_nvram_set(uint32_t ui32NvramNum, uint32_t ui32NvramVal);

//*****************************************************************************
//
//! @brief Get the current stimer nvram register value.
//!
//! @param ui32NvramNum - The NVRAM Register Number to read.
//!
//! This function can be used to read the value in an stimer NVRAM register.
//!
//! @return NVRAM Register Value
//!
//
//*****************************************************************************
extern uint32_t am_hal_stimer_nvram_get(uint32_t ui32NvramNum);

//*****************************************************************************
//
//! @brief Enables the selected system timer interrupt.
//!
//! This function will enable the selected interrupts in the STIMER interrupt
//! enable register.
//!
//! @note In order to receive an interrupt from an stimer component,
//! you will need to enable the interrupt for that component in this main
//! register, as well as in the stimer configuration register (accessible though
//! am_hal_stimer_config()), and in the NVIC.
//!
//! @param ui32Interrupt - The interrupt to be used.
//! @parblock
//!
//! ui32Interrupt should be the logical OR of one or more of the following
//! values:
//!
//!     AM_HAL_STIMER_INT_COMPAREA
//!     AM_HAL_STIMER_INT_COMPAREB
//!     AM_HAL_STIMER_INT_COMPAREC
//!     AM_HAL_STIMER_INT_COMPARED
//!     AM_HAL_STIMER_INT_COMPAREE
//!     AM_HAL_STIMER_INT_COMPAREF
//!     AM_HAL_STIMER_INT_COMPAREG
//!     AM_HAL_STIMER_INT_COMPAREH
//!
//!     AM_HAL_STIMER_INT_OVERFLOW
//!
//!     AM_HAL_STIMER_INT_CAPTUREA
//!     AM_HAL_STIMER_INT_CAPTUREB
//!     AM_HAL_STIMER_INT_CAPTUREC
//!     AM_HAL_STIMER_INT_CAPTURED
//! @endparblock
//
//*****************************************************************************
extern void     am_hal_stimer_int_enable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Return the enabled stimer interrupts.
//!
//! This function will return all enabled interrupts in the STIMER
//! interrupt enable register.
//!
//! @return return enabled interrupts. This will be a logical or of:
//!
//!     AM_HAL_STIMER_INT_COMPAREA
//!     AM_HAL_STIMER_INT_COMPAREB
//!     AM_HAL_STIMER_INT_COMPAREC
//!     AM_HAL_STIMER_INT_COMPARED
//!     AM_HAL_STIMER_INT_COMPAREE
//!     AM_HAL_STIMER_INT_COMPAREF
//!     AM_HAL_STIMER_INT_COMPAREG
//!     AM_HAL_STIMER_INT_COMPAREH
//!     AM_HAL_STIMER_INT_OVERFLOW
//!     AM_HAL_STIMER_INT_CAPTUREA
//!     AM_HAL_STIMER_INT_CAPTUREB
//!     AM_HAL_STIMER_INT_CAPTUREC
//!     AM_HAL_STIMER_INT_CAPTURED
//
//*****************************************************************************
extern uint32_t am_hal_stimer_int_enable_get(void);

//*****************************************************************************
//
//! @brief Disables the selected stimer interrupt.
//!
//! This function will disable the selected interrupts in the STIMER
//! interrupt register.
//!
//! @param ui32Interrupt - The interrupt to be used.
//! @parblock
//!
//! ui32Interrupt should be the logical OR of one or more of the following
//! values:
//!
//!     AM_HAL_STIMER_INT_COMPAREA
//!     AM_HAL_STIMER_INT_COMPAREB
//!     AM_HAL_STIMER_INT_COMPAREC
//!     AM_HAL_STIMER_INT_COMPARED
//!     AM_HAL_STIMER_INT_COMPAREE
//!     AM_HAL_STIMER_INT_COMPAREF
//!     AM_HAL_STIMER_INT_COMPAREG
//!     AM_HAL_STIMER_INT_COMPAREH
//!
//!     AM_HAL_STIMER_INT_OVERFLOW
//!
//!     AM_HAL_STIMER_INT_CAPTUREA
//!     AM_HAL_STIMER_INT_CAPTUREB
//!     AM_HAL_STIMER_INT_CAPTUREC
//!     AM_HAL_STIMER_INT_CAPTURED
//! @endparblock
//
//*****************************************************************************
extern void     am_hal_stimer_int_disable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Sets the selected stimer interrupt.
//!
//! This function will set the selected interrupts in the STIMER
//! interrupt register.
//!
//! @param ui32Interrupt - The interrupt to be used.
//! @parblock
//!
//! ui32Interrupt should be the logical OR of one or more of the following
//! values:
//!
//!     AM_HAL_STIMER_INT_COMPAREA
//!     AM_HAL_STIMER_INT_COMPAREB
//!     AM_HAL_STIMER_INT_COMPAREC
//!     AM_HAL_STIMER_INT_COMPARED
//!     AM_HAL_STIMER_INT_COMPAREE
//!     AM_HAL_STIMER_INT_COMPAREF
//!     AM_HAL_STIMER_INT_COMPAREG
//!     AM_HAL_STIMER_INT_COMPAREH
//!
//!     AM_HAL_STIMER_INT_OVERFLOW
//!
//!     AM_HAL_STIMER_INT_CAPTUREA
//!     AM_HAL_STIMER_INT_CAPTUREB
//!     AM_HAL_STIMER_INT_CAPTUREC
//!     AM_HAL_STIMER_INT_CAPTURED
//! @endparblock
//
//*****************************************************************************
extern void     am_hal_stimer_int_set(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Clears the selected stimer interrupt.
//!
//! This function will clear the selected interrupts in the STIMER
//! interrupt register.
//!
//! @param ui32Interrupt - The interrupt to be used.
//! @parblock
//!
//! ui32Interrupt should be the logical OR of one or more of the following
//! values:
//!
//!     AM_HAL_STIMER_INT_COMPAREA
//!     AM_HAL_STIMER_INT_COMPAREB
//!     AM_HAL_STIMER_INT_COMPAREC
//!     AM_HAL_STIMER_INT_COMPARED
//!     AM_HAL_STIMER_INT_COMPAREE
//!     AM_HAL_STIMER_INT_COMPAREF
//!     AM_HAL_STIMER_INT_COMPAREG
//!     AM_HAL_STIMER_INT_COMPAREH
//!
//!     AM_HAL_STIMER_INT_OVERFLOW
//!
//!     AM_HAL_STIMER_INT_CAPTUREA
//!     AM_HAL_STIMER_INT_CAPTUREB
//!     AM_HAL_STIMER_INT_CAPTUREC
//!     AM_HAL_STIMER_INT_CAPTURED
//! @endparblock
//
//*****************************************************************************
extern void     am_hal_stimer_int_clear(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Returns either the enabled or raw stimer interrupt status.
//!
//! This function will return the stimer interrupt status.
//!
//! @param bEnabledOnly if true returns the status of the enabled interrupts
//! only.
//!
//! The return value will be the logical OR of one or more of the following
//! values:
//!
//! @return Returns the stimer interrupt status.
//
//*****************************************************************************
extern uint32_t am_hal_stimer_int_status_get(bool bEnabledOnly);


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
