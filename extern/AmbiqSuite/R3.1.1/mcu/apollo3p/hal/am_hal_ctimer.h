//*****************************************************************************
//
//! @file am_hal_ctimer.h
//!
//! @brief Functions for Interfacing with the Counter/Timer Module.
//!
//! @addtogroup ctimer3p CTimer - Counter/Timer
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
#ifndef AM_HAL_CTIMER_H
#define AM_HAL_CTIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

//
// Designate this peripheral.
//
#define AM_APOLLO3_CTIMER   1

//*****************************************************************************
//
//! CTIMERADDRn()
//!  This is a specialized version of AM_REGADDRn().  It is necessary because
//!  the CTIMER does not work as a multi-module peripheral.  In typical
//!  multi-module peripherals, the base address is defined as MODULE0_BASE.
//!  For CTIMER it's CTIMER_BASE (there is no module 0 defined).
//!
//!  Usage:
//!  CTIMER_ADDRn(CTIMER, n, reg).
//!
//! periph:  Must always be CTIMER.
//! n:       The timer number specified as a macro, variable, etc.
//! reg:     The register name always ending in '0'.  E.g. TMR0, CTRL0, CMPRB0,
//!          etc (regardless of the timernum specified by 'n').
//
//*****************************************************************************
#define CTIMERADDRn(periph, n, reg) ( periph##_BASE                 +   \
                                      offsetof(periph##_Type, reg)  +   \
                                      (n * (offsetof(periph##_Type, TMR1) - offsetof(periph##_Type, TMR0))) )

//
//! Enumerations for the eOutputType argument of am_hal_ctimer_output_config().
//
typedef enum
{
    AM_HAL_CTIMER_OUTPUT_NORMAL         = 0x0,
    AM_HAL_CTIMER_OUTPUT_SECONDARY      = 0x1,
    AM_HAL_CTIMER_OUTPUT_FORCE0         = 0x2,
    AM_HAL_CTIMER_OUTPUT_FORCE1         = 0x3
} am_hal_ctimer_outputtype_e;


//*****************************************************************************
//
//! CMSIS-Style macro for handling a variable CTIMER module number.
//
//*****************************************************************************
#define CTIMERn(n) ((CTIMER_Type*)(CTIMER_BASE + (n * ((uint32_t)&CTIMER->TMR1 - (uint32_t)&CTIMER->TMR0))))

//*****************************************************************************
//
//! Number of timers
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMERS_NUM    8

//*****************************************************************************
//
//! Timer offset value
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMER_OFFSET  ((uint32_t)&CTIMER->TMR1 - (uint32_t)&CTIMER->TMR0)

//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_TIMERA0C0         CTIMER_INTEN_CTMRA0C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA0C1         CTIMER_INTEN_CTMRA0C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA1C0         CTIMER_INTEN_CTMRA1C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA1C1         CTIMER_INTEN_CTMRA1C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA2C0         CTIMER_INTEN_CTMRA2C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA2C1         CTIMER_INTEN_CTMRA2C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA3C0         CTIMER_INTEN_CTMRA3C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA3C1         CTIMER_INTEN_CTMRA3C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA4C0         CTIMER_INTEN_CTMRA4C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA4C1         CTIMER_INTEN_CTMRA4C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA5C0         CTIMER_INTEN_CTMRA5C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA5C1         CTIMER_INTEN_CTMRA5C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA6C0         CTIMER_INTEN_CTMRA6C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA6C1         CTIMER_INTEN_CTMRA6C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA7C0         CTIMER_INTEN_CTMRA7C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERA7C1         CTIMER_INTEN_CTMRA7C1INT_Msk

#define AM_HAL_CTIMER_INT_TIMERB0C0         CTIMER_INTEN_CTMRB0C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB0C1         CTIMER_INTEN_CTMRB0C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB1C0         CTIMER_INTEN_CTMRB1C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB1C1         CTIMER_INTEN_CTMRB1C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB2C0         CTIMER_INTEN_CTMRB2C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB2C1         CTIMER_INTEN_CTMRB2C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB3C0         CTIMER_INTEN_CTMRB3C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB3C1         CTIMER_INTEN_CTMRB3C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB4C0         CTIMER_INTEN_CTMRB4C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB4C1         CTIMER_INTEN_CTMRB4C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB5C0         CTIMER_INTEN_CTMRB5C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB5C1         CTIMER_INTEN_CTMRB5C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB6C0         CTIMER_INTEN_CTMRB6C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB6C1         CTIMER_INTEN_CTMRB6C1INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB7C0         CTIMER_INTEN_CTMRB7C0INT_Msk
#define AM_HAL_CTIMER_INT_TIMERB7C1         CTIMER_INTEN_CTMRB7C1INT_Msk
//! @}

//*****************************************************************************
//
//!  DEPRECATED Interrupt Status Bits
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_TIMERA0           AM_HAL_CTIMER_INT_TIMERA0C0
#define AM_HAL_CTIMER_INT_TIMERB0           AM_HAL_CTIMER_INT_TIMERB0C0
#define AM_HAL_CTIMER_INT_TIMERA1           AM_HAL_CTIMER_INT_TIMERA1C0
#define AM_HAL_CTIMER_INT_TIMERB1           AM_HAL_CTIMER_INT_TIMERB1C0
#define AM_HAL_CTIMER_INT_TIMERA2           AM_HAL_CTIMER_INT_TIMERA2C0
#define AM_HAL_CTIMER_INT_TIMERB2           AM_HAL_CTIMER_INT_TIMERB2C0
#define AM_HAL_CTIMER_INT_TIMERA3           AM_HAL_CTIMER_INT_TIMERA3C0
#define AM_HAL_CTIMER_INT_TIMERB3           AM_HAL_CTIMER_INT_TIMERB3C0

//*****************************************************************************
//
//! @name Configuration options
//! @brief Configuration options for \e am_hal_ctimer_config_t
//!
//! These options are to be used with the \e am_hal_ctimer_config_t structure
//! used by \e am_hal_ctimer_config
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_CLK_PIN               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x00)
#define AM_HAL_CTIMER_HFRC_12MHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x01)
#define AM_HAL_CTIMER_HFRC_3MHZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x02)
#define AM_HAL_CTIMER_HFRC_187_5KHZ         _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x03)
#define AM_HAL_CTIMER_HFRC_47KHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x04)
#define AM_HAL_CTIMER_HFRC_12KHZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x05)
#define AM_HAL_CTIMER_XT_32_768KHZ          _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x06)
#define AM_HAL_CTIMER_XT_16_384KHZ          _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x07)
#define AM_HAL_CTIMER_XT_2_048KHZ           _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x08)
#define AM_HAL_CTIMER_XT_256HZ              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x09)
#define AM_HAL_CTIMER_LFRC_512HZ            _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0A)
#define AM_HAL_CTIMER_LFRC_32HZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0B)
#define AM_HAL_CTIMER_LFRC_1HZ              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0C)
#define AM_HAL_CTIMER_LFRC_1_16HZ           _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0D)
#define AM_HAL_CTIMER_RTC_100HZ             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0E)
#define AM_HAL_CTIMER_HCLK_DIV4             _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x0F)
#define AM_HAL_CTIMER_XT_DIV4               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x10)
#define AM_HAL_CTIMER_XT_DIV8               _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x11)
#define AM_HAL_CTIMER_XT_DIV32              _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x12)
#define AM_HAL_CTIMER_RSVD                  _VAL2FLD(CTIMER_CTRL0_TMRA0CLK, 0x13)
//! @}

//*****************************************************************************
//
//! Timer function macros.
//!
//! @{
//
//*****************************************************************************
//! Single Count: Counts one time to the compare value, then the output
//! changes polarity and stays at that level, with an optional interrupt.
#define AM_HAL_CTIMER_FN_ONCE               _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 0)
//! Repeated Count: Periodic 1-clock-cycle wide pulses with optional interrupts.
#define AM_HAL_CTIMER_FN_REPEAT             _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 1)
//! Single Pulse (One Shot): A single pulse of programmed width, with an optional interrupt.
#define AM_HAL_CTIMER_FN_PWM_ONCE           _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 2)
//! Repeated Pulse: A rectangular (or square) waveform with programmed high and
//! low widths, and optional interrupts on each cycle.
#define AM_HAL_CTIMER_FN_PWM_REPEAT         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 3)
//! Single Pattern: one burst of bits specified by the CMPR0/1/2/3 registers.
#define AM_HAL_CTIMER_FN_PTN_ONCE           _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 4)
//! Repeated Pattern: repeated burst of bits specified by the CMPR0/1/2/3 registers.
#define AM_HAL_CTIMER_FN_PTN_REPEAT         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 5)
//! Continuous: Free running timer with a single level change on the output and
//! a single optional interrupt.
#define AM_HAL_CTIMER_FN_CONTINUOUS         _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 6)
//! Alternate Pulse: like Repeated Pulse but alternating between two different
//! pulse width/spacing settings.
#define AM_HAL_CTIMER_FN_PWM_ALTERNATE      _VAL2FLD(CTIMER_CTRL0_TMRA0FN, 7)
//! @}

//*****************************************************************************
//
//! Half-timer options.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_ENABLE            CTIMER_CTRL0_TMRA0IE0_Msk
//#define AM_HAL_CTIMER_PIN_ENABLE            CTIMER_CTRL0_TMRA0PE_Msk
#define AM_HAL_CTIMER_PIN_INVERT            CTIMER_CTRL0_TMRA0POL_Msk
#define AM_HAL_CTIMER_CLEAR                 CTIMER_CTRL0_TMRA0CLR_Msk
//! @}

//*****************************************************************************
//
//! Additional timer options.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_LINK                  CTIMER_CTRL0_CTLINK0_Msk
#define AM_HAL_CTIMER_ADC_TRIG              CTIMER_CTRL3_ADCEN_Msk
//! @}

//*****************************************************************************
//
//! Timer selection macros.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMERA                0x0000FFFF
#define AM_HAL_CTIMER_TIMERB                0xFFFF0000
#define AM_HAL_CTIMER_BOTH                  0xFFFFFFFF
//! @}

//*****************************************************************************
//
//! Timer trigger options for Apollo3 Blue (rev B0 and later) including
//! Apollo3 Blue Plus.
//!
//! Valid only for CTIMER4 and CTIMER5 when CTLINK==1 and TMRA4TRIG==1
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP0     CTIMER_AUX4_TMRB4TRIG_A7OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP1     CTIMER_AUX4_TMRB4TRIG_B7OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP2     CTIMER_AUX4_TMRB4TRIG_A1OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCAP3     CTIMER_AUX4_TMRB4TRIG_B1OUT
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP0     CTIMER_AUX4_TMRB4TRIG_B3OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP1     CTIMER_AUX4_TMRB4TRIG_A3OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP2     CTIMER_AUX4_TMRB4TRIG_A1OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP3     CTIMER_AUX4_TMRB4TRIG_B1OUT2
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP4     CTIMER_AUX4_TMRB4TRIG_A6OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP5     CTIMER_AUX4_TMRB4TRIG_A7OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP6     CTIMER_AUX4_TMRB4TRIG_B5OUT2DUAL
#define AM_HAL_CTIMER_AUX4_TMRB4TRIG_STIMERCMP7     CTIMER_AUX4_TMRB4TRIG_A5OUT2DUAL

#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP0     CTIMER_AUX5_TMRB5TRIG_A7OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP1     CTIMER_AUX5_TMRB5TRIG_B7OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP2     CTIMER_AUX5_TMRB5TRIG_A1OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCAP3     CTIMER_AUX5_TMRB5TRIG_B1OUT
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP0     CTIMER_AUX5_TMRB5TRIG_B3OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP1     CTIMER_AUX5_TMRB5TRIG_A3OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP2     CTIMER_AUX5_TMRB5TRIG_A1OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP3     CTIMER_AUX5_TMRB5TRIG_B1OUT2
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP4     CTIMER_AUX5_TMRB5TRIG_A6OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP5     CTIMER_AUX5_TMRB5TRIG_A7OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP6     CTIMER_AUX5_TMRB5TRIG_B5OUT2DUAL
#define AM_HAL_CTIMER_AUX5_TMRB5TRIG_STIMERCMP7     CTIMER_AUX5_TMRB5TRIG_A5OUT2DUAL
//! @}

//*****************************************************************************
//
//! @name All-In-One Configuration
//! @brief New API for multiple timer configuration.
//!
//! These options are to be used with the \e am_hal_ctimer_config_t structure
//! used by \e am_hal_ctimer_config
//! @{
//
//*****************************************************************************
//! CTimer AIO Compare Configuration.
typedef struct
{
    //
    //! Function Number.
    //
    uint32_t FN;
    //
    //! Timer Segment. Timer A, B, BOTH selector.
    //
    uint32_t AB;
    //
    //! Compare Register A0.
    //
    uint32_t A0;
    //
    //! Compare Register A1.
    //
    uint32_t A1;
    //
    //! Compare Register A2.
    //
    uint32_t A2;
    //
    //! Compare Register A3.
    //
    uint32_t A3;
    //
    //! Compare Register B0.
    //
    uint32_t B0;
    //
    //! Compare Register B1.
    //
    uint32_t B1;
    //
    //! Compare Register B2.
    //
    uint32_t B2;
    //
    //! Compare Register B3.
    //
    uint32_t B3;
    //
    //! LMT field values.
    //
    uint32_t LMT;
    //
    //! A "T" indicates that a 1 is loaded if the OUT2 output is used, otherwise a 0 is loaded.
    //
    uint32_t EN23;
    //
    //! TRIG: a single pattern will be triggered; TERM: a repeated pattern will be terminated.
    //
    uint32_t TRIG;
    //
    //! Select clock source: internal, external, a buck pulse, or output of another CTIMER.
    //
    uint32_t CLK;
    //
    //!  Enable the primary interrupt INT.
    //
    uint32_t IE0;
    //
    //!  Enable the secondary interrupt INT2.
    //
    uint32_t IE1;
    //
    //!  Select the polarity of the OUT output.
    //
    uint32_t POL;
    //
    //!  Select the polarity of the OUT2 output.
    //
    uint32_t POL23;
    //
    //! Select polarity of both OUT and OUT2 as a function of the trigger input.
    //
    uint32_t TINV;
    //
    //!  Disable clock synchronization on read.
    //
    uint32_t NOSYNC;
    //
    //! Enable the timer.\n
    //! This is ANDed with the global enable in GLOBEN, and allows the counter to begin counting.
    //
    uint32_t EN;
    //
    //!  Clear the timer. This will hold the timer at zero even if EN is asserted.\n
    //!  It is typically cleared at the end of a configuration and
    //!  is probably not included in the function structure.
    //
    //uint32_t CLR;

}
am_hal_ctimer_aio_config_t;

//! CTimer AIO Output Selection and Interconnect.
typedef struct
{
    //! Pad 0-9
    uint32_t OUTCFG0;
    //! Pad 10-19
    uint32_t OUTCFG1;
    //! Pad 20-29
    uint32_t OUTCFG2;
    //! Pad 30-31
    uint32_t OUTCFG3;
}
am_hal_ctimer_aio_connect_t;
//! @}

//*****************************************************************************
//
//! Timer configuration structure
//
//*****************************************************************************
typedef struct
{
    //
    //! Set to 1 to operate this timer as a 32-bit timer instead of two 16-bit
    //! timers.
    //
    uint32_t ui32Link;

    //
    //! Configuration options for TIMERA
    //
    uint32_t ui32TimerAConfig;

    //
    //! Configuration options for TIMERB
    //
    uint32_t ui32TimerBConfig;

}
am_hal_ctimer_config_t;

//*****************************************************************************
//
//! Function pointer type for CTimer interrupt handlers.
//
//*****************************************************************************
typedef void (*am_hal_ctimer_handler_t)(void);

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Set up the counter/timer.
//!
//! @param ui32ConfigVal - The value to set the global enable register.
//!
//! This function sets the global enable register inside a critical section.
//
//*****************************************************************************
extern void am_hal_ctimer_globen(uint32_t ui32ConfigVal);

//*****************************************************************************
//
//! @brief Set up the counter/timer.
//!
//! @param ui32TimerNumber - The number of the Timer that should be
//! configured.
//!
//! @param psConfig - A pointer to a structure that holds important settings
//! for the timer.
//!
//! This function should be used to perform the initial set-up of the
//! counter-timer.
//!
//! @note This function is deprecated and will eventually be replaced by
//! am_hal_ctimer_config_single(), which performs the same configuration
//! without requiring a structure and without assuming both timer halves
//! are being configured.
//! Please use am_hal_ctimer_config_single() for new development.
//! @par
//! @note In order to initialize the given timer into a known state, this
//! function asserts the CLR configuration bit. The CLR bit will be deasserted
//! with the write of the configuration register. The CLR bit is also
//! intentionally deasserted with a call to am_hal_ctimer_start().
//!
//
//*****************************************************************************
extern void am_hal_ctimer_config(uint32_t ui32TimerNumber,
                                 am_hal_ctimer_config_t *psConfig);

//*****************************************************************************
//
//! @brief Set up the counter/timer.
//!
//! @param ui32TimerNumber - The number of the Timer that should be
//! configured.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! enabled.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32ConfigVal - Specifies the configuration options for the selected
//! timer.
//! @parblock
//!     - The timer's clock source, mode, interrupt, and external pin behavior are
//!        all controlled through the \e ui32Configval parameter.\n The valid options
//!        for ui32ConfigVal include any ORed together combination of the following:
//!
//!     - Clock configuration macros:
//!         - AM_HAL_CTIMER_HFRC_24MHZ
//!         - AM_HAL_CTIMER_LFRC_512HZ
//!         - ... etc. (See am_hal_ctimer.h for the full set of options.)
//!
//!     - Mode selection macros:
//!         - AM_HAL_CTIMER_FN_ONCE
//!         - AM_HAL_CTIMER_FN_REPEAT
//!         - AM_HAL_CTIMER_FN_PWM_ONCE
//!         - AM_HAL_CTIMER_FN_PWM_REPEAT
//!         - AM_HAL_CTIMER_FN_CONTINUOUS
//!
//!     - Interrupt control:
//!         - AM_HAL_CTIMER_INT_ENABLE
//!
//!     - Pin control:
//!         - AM_HAL_CTIMER_PIN_ENABLE
//!         - AM_HAL_CTIMER_PIN_INVERT
//!
//!     - ADC trigger (Timer 3 only):
//!         - AM_HAL_CTIMER_ADC_TRIG
//! @endparblock
//!
//! This function should be used to perform the initial set-up of the
//! counter-timer. It can be used to configure either a 16-bit timer (A or B) or a
//! 32-bit timer using the BOTH option.
//!
//! @note In order to initialize the given timer into a known state, this
//! function asserts the CLR configuration bit. The CLR bit will be deasserted
//! with the write of the configuration register. The CLR bit is also
//! intentionally deasserted with a call to am_hal_ctimer_start().
//!
//
//*****************************************************************************
extern void am_hal_ctimer_config_single(uint32_t ui32TimerNumber,
                                        uint32_t ui32TimerSegment,
                                        uint32_t ui32ConfigVal);

//*****************************************************************************
//
//! @brief Set up the counter/timer trigger.
//!
//! @param ui32TimerNumber  - The number of the Timer that should be
//! configured.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! enabled.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!
//! @param ui32ConfigVal - Specifies the configuration options for the selected
//! timer trigger AUXn register.
//!
//! This function should be used to perform the configuration of the trigger
//! for the counter-timer (A or B).
//!
//! @note In order to initialize the given timer into a known state, this
//! function asserts the CLR configuration bit. The CLR bit will be deasserted
//! with the write of the configuration register. The CLR bit is also
//! intentionally deasserted with a call to am_hal_ctimer_start().
//!
//
//*****************************************************************************
extern void am_hal_ctimer_config_trigger(uint32_t ui32TimerNumber,
                                         uint32_t ui32TimerSegment,
                                         uint32_t ui32ConfigVal);

//*****************************************************************************
//
//! @brief Start a timer
//!
//! @param ui32TimerNumber  - The number of the timer to enable
//!     - The \e ui32TimerNumber parameter selects the timer that should be enabled.
//!     - For example, a 0 would target TIMER0.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! enabled.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!     - The \e ui32TimerSegment parameter allows the caller to individually
//!       select a segment within a timer to be enabled,\n
//!       such as TIMER0A, TIMER0B, or both.
//!
//! This function will enable a timer to begin incrementing.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_start(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

//*****************************************************************************
//
//! @brief Stop a timer
//!
//! @param ui32TimerNumber - The number of the timer to disable.
//!     - The \e ui32TimerNumber parameter selects the timer that should be enabled.
//!     - For example, a 0 would target TIMER0.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! disabled.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!     - The \e ui32TimerSegment parameter allows the caller to individually
//!       select a segment within a timer to be enabled,\n
//!       such as TIMER0A, TIMER0B, or both.
//!
//! This function will stop the selected timer from incrementing.
//!
//! @note This function will stop a counter/timer from counting, but does not return
//! the count value to 'zero'. If you would like to reset the counter back to
//! zero, try the am_hal_ctimer_clear() function instead.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_stop(uint32_t ui32TimerNumber,
                               uint32_t ui32TimerSegment);

//*****************************************************************************
//
//! @brief Stops a timer and resets its value back to zero.
//!
//! @param ui32TimerNumber - The number of the timer to clear.
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! cleared.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!     - The \e ui32TimerSegment parameter allows the caller to individually
//!          select a segment within, such as TIMER0A, TIMER0B, or both.
//!
//! This function will stop a free-running counter-timer, reset its value to
//! zero, and leave the timer disabled. When you would like to restart the
//! counter, you will need to call am_hal_ctimer_start().
//!
//! @note Setting the CLR bit is necessary for completing timer initialization
//! including after MCU resets.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_clear(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

//*****************************************************************************
//
//! @brief Returns the current free-running value of the selected timer.
//!
//! @param ui32TimerNumber - The number of the timer to read.
//! @param ui32TimerSegment - Specifies which segment of the timer should be
//! read.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! This function returns the current free-running value of the selected timer.
//!
//! @note When reading from a linked timer, be sure to use AM_HAL_CTIMER both
//! for the segment argument.
//!
//! @return Current timer value.
//
//*****************************************************************************
extern uint32_t am_hal_ctimer_read(uint32_t ui32TimerNumber,
                                   uint32_t ui32TimerSegment);

//*****************************************************************************
//
//! @brief Configure timer pin output.
//!
//! @param ui32TimerNumber   - The number of the timer to configure.
//!     - The timer number, 0-7.
//! @param ui32TimerSegment  - Specifies which segment of the timer to use.
//!     - AM_HAL_CTIMER_TIMERA
//!     - AM_HAL_CTIMER_TIMERB
//!     - AM_HAL_CTIMER_BOTH
//! @param ui32PadNum     - Pad number to be used for the output signal.
//! @param eOutputType    - Output Configuration options
//!     - AM_HAL_CTIMER_OUTPUT_NORMAL
//!     - AM_HAL_CTIMER_OUTPUT_SECONDARY
//!     - AM_HAL_CTIMER_OUTPUT_FORCE0
//!     - AM_HAL_CTIMER_OUTPUT_FORCE1
//! @param eDriveStrength - Drive strength
//!     - AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA   = 0x0,
//!     - AM_HAL_GPIO_PIN_DRIVESTRENGTH_4MA   = 0x1,
//!     - AM_HAL_GPIO_PIN_DRIVESTRENGTH_8MA   = 0x2,
//!     - AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA  = 0x3
//!
//! This function will configure the output pin for the selected timer.
//!
//! @return status (0 is Success).
//
//*****************************************************************************
extern uint32_t am_hal_ctimer_output_config(uint32_t ui32TimerNumber,
                                            uint32_t ui32TimerSegment,
                                            uint32_t ui32PadNum,
                                            uint32_t eOutputType,
                                            uint32_t eDriveStrength);

//*****************************************************************************
//
//! @brief Configure timer inputs.
//!
//! @param ui32TimerNumber - The number of the timer to configure.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer to use.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32TimerInputConfig Input Configuration options.
//!
//! This function will configure the input pin for the selected timer.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_input_config(uint32_t ui32TimerNumber,
                                       uint32_t ui32TimerSegment,
                                       uint32_t ui32TimerInputConfig);

//*****************************************************************************
//
//! @brief Set a compare register.
//!
//! @param ui32TimerNumber -The number of the timer to configure.
//!
//! @param ui32TimerSegment -Specifies which segment of the timer to use.
//!     - The \e ui32TimerSegment parameter allows the caller to individually
//!       select a segment within, such as TIMER0A, TIMER0B, or both.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32CompareReg - Specifies which compare register should be set
//! (either 0 or 1)
//!
//! @param ui32Value - The value that should be written to the compare
//! register.
//!
//! This function allows the caller to set the values in the compare registers
//! for a timer. These registers control the period and duty cycle of the
//! timers and their associated output pins. Please see the datasheet for
//! further information on the operation of the compare registers.
//!
//! @note For simple manipulations of period or duty cycle for timers and PWMs,
//! you may find it easier to use the am_hal_ctimer_period_set() function.
//
//*****************************************************************************
extern void am_hal_ctimer_compare_set(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment,
                                      uint32_t ui32CompareReg,
                                      uint32_t ui32Value);

//*****************************************************************************
//
//! @brief Set a compare register.
//!
//! @param ui32TimerNumber - The number of the timer to configure.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer to use.
//!     - The \e ui32TimerSegment parameter allows the caller to individually
//!       select a segment within, such as TIMER0A, TIMER0B, or both.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32CompareReg - Specifies which compare register should be set
//! (either 0 or 1)
//!
//! @param ui32Value - The value that should be written to the compare
//! register.
//!
//! This function allows the caller to set the values in the compare registers
//! for a timer. These registers control the period and duty cycle of the
//! timers and their associated output pins. Please see the datasheet for
//! further information on the operation of the compare registers.
//!
//! @note For simple manipulations of period or duty cycle for timers and PWMs,
//! you may find it easier to use the am_hal_ctimer_period_set() function.
//
//*****************************************************************************
extern void am_hal_ctimer_aux_compare_set(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment,
                                      uint32_t ui32CompareReg,
                                      uint32_t ui32Value);

//*****************************************************************************
//
//! @brief Set the period and duty cycle of a timer.
//!
//! @param ui32TimerNumber - The number of the timer to configure.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer to use.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32Period - Specifies the desired period.\n
//!     - This parameter effectively specifies the CTIMER CMPR field(s).
//!     - The CMPR fields are handled in hardware
//!       as (n+1) values, therefore ui32Period is actually specified as 1 less than
//!       the desired period.
//!     - Finally, as mentioned in the data sheet, the CMPR fields
//!       cannot be 0 (a value of 1), so neither can ui32Period be 0.
//!
//! @param ui32OnTime - Set the number of clocks where the output signal is high.
//!
//! @note This function should be used for simple manipulations of the period and
//! duty cycle of a counter/timer. To set the period and/or duty cycle of a
//! linked timer pair, use AM_HAL_CTIMER_BOTH as the timer segment argument. If
//! you would like to set the period and/or duty cycle for both TIMERA and
//! TIMERB you will need to call this function twice: once for TIMERA, and once
//! for TIMERB.
//! @par
//!
//! @note The ui32OnTime parameter will only work if the timer is currently
//! operating in one of the PWM modes.
//
//*****************************************************************************
extern void am_hal_ctimer_period_set(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     uint32_t ui32Period,
                                     uint32_t ui32OnTime);

//*****************************************************************************
//
//! @brief Set the period and duty cycle of a timer.
//!
//! @param ui32TimerNumber - The number of the timer to configure.
//!
//! @param ui32TimerSegment - Specifies which segment of the timer to use.
//!     - Valid values for ui32TimerSegment are:
//!         - AM_HAL_CTIMER_TIMERA
//!         - AM_HAL_CTIMER_TIMERB
//!         - AM_HAL_CTIMER_BOTH
//!
//! @param ui32Period - Specifies the desired period.\n
//!     - This parameter effectively specifies the CTIMER CMPR field(s).
//!     - The CMPR fields are handled in hardware
//!        as (n+1) values, therefore ui32Period is actually specified as 1 less than
//!        the desired period.
//!     - Finally, as mentioned in the data sheet, the CMPR fields
//!        cannot be 0 (a value of 1), so neither can ui32Period be 0.
//!
//! @param ui32OnTime - Set the number of clocks where the output signal is high.
//!
//! @note This function should be used for simple manipulations of the period and
//! duty cycle of a counter/timer. To set the period and/or duty cycle of a
//! linked timer pair, use AM_HAL_CTIMER_BOTH as the timer segment argument. If
//! you would like to set the period and/or duty cycle for both TIMERA and
//! TIMERB you will need to call this function twice: once for TIMERA, and once
//! for TIMERB.
//! @par
//!
//! @note The ui32OnTime parameter will only work if the timer is currently
//! operating in one of the PWM modes.
//
//*****************************************************************************
extern void am_hal_ctimer_aux_period_set(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     uint32_t ui32Period,
                                     uint32_t ui32OnTime);

//*****************************************************************************
//
//! @brief Enable the TIMERA3 ADC trigger
//!
//! This function enables the ADC trigger within TIMERA3.
//
//*****************************************************************************
extern void am_hal_ctimer_adc_trigger_enable(void);

//*****************************************************************************
//
//! @brief Disable the TIMERA3 ADC trigger
//!
//! This function disables the ADC trigger within TIMERA3.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_adc_trigger_disable(void);

//*****************************************************************************
//
//! @brief Enables the selected timer interrupt.
//!
//! @param ui32Interrupt - The interrupt to be used.
//!     - ui32Interrupt should be the logical OR of one or more of the following
//!       values:
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!
//! @note This function will enable the selected interrupts in the main CTIMER
//!       interrupt enable register.\n In order to receive an interrupt from a
//!       timer, you will need to enable the interrupt for that timer in this
//!       main register, as well as in the timer control register
//!       (accessible though am_hal_ctimer_config()), and in the NVIC.
//! @par
//!
//! @note The AM_HAL_CTIMER_INT_TIMER defines were re-definitions of
//!       AM_REG_CTIMER_INTEN_CTMRAxCxINT_M register defines. They are
//!       dropped in this release to go back to a single source definition.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_int_enable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Disables the selected timer interrupt.
//!
//! @param ui32Interrupt - The interrupt to be used.
//!     - ui32Interrupt should be the logical OR of one or more of the following
//!       values:
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!
//! This function will disable the selected interrupts in the main CTIMER
//! interrupt register.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_int_disable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Sets the selected timer interrupt.
//!
//! @param ui32Interrupt - The interrupt to be used.
//!     - ui32Interrupt should be the logical OR of one or more of the following
//!       values:
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!
//! This function will set the selected interrupts in the main CTIMER
//! interrupt register.
//!
//
//*****************************************************************************
extern void am_hal_ctimer_int_set(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Clears the selected timer interrupt.
//!
//! @param ui32Interrupt -The interrupt to be used.
//!     - ui32Interrupt should be the logical OR of one or more of the following
//!       values:
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!         - AM_HAL_CTIMER_INT_TIMERAxCx
//!
//! This function will clear the selected interrupts in the main CTIMER
//! interrupt register.
//
//*****************************************************************************
extern void am_hal_ctimer_int_clear(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Returns either the enabled or raw timer interrupt status.
//!
//! @param bEnabledOnly - If true this function returns the status of the
//!                       enabled interrupts only.
//!
//! This function will return the timer interrupt status.
//!
//! @return ui32RetVal either the timer interrupt status, or interrupt enabled.\n
//!     - The return value will be the logical OR of one or more of the
//!       following values:
//!          - AM_REG_CTIMER_INTEN_CTMRAxC0INT_M
//!          - AM_HAL_CTIMER_INT_TIMERAxC1
//
//*****************************************************************************
extern uint32_t am_hal_ctimer_int_status_get(bool bEnabledOnly);

//*****************************************************************************
//
//! @brief Register an interrupt handler for CTimer.
//!
//! @param ui32Interrupt - Interrupt number to assign this interrupt handler to.
//! @param pfnHandler - Function to call when this interrupt is received.
//!
//! This function allows the caller to specify a function that should be called
//! any time a Ctimer interrupt is received. Registering an
//! interrupt handler using this function adds the function pointer to an array
//! in SRAM. This interrupt handler will be called by am_hal_ctimer_int_service()
//! whenever the ui32Status parameter indicates that the corresponding interrupt.
//!
//! To remove an interrupt handler that has already been registered, the
//! pfnHandler parameter may be set to zero.
//!
//! @note This function will not have any effect unless the
//! am_hal_ctimer_int_service() function is being used.
//
//*****************************************************************************
extern void am_hal_ctimer_int_register(uint32_t ui32Interrupt,
                         am_hal_ctimer_handler_t pfnHandler);

//*****************************************************************************
//
//! @brief Convenience function for responding to CTimer interrupts.
//!
//! @param ui32Status - The interrupt status as returned by
//! am_hal_ctimer_int_status_get()
//!
//! This function may be called from am_ctimer_isr() to read the status of
//! the CTimer interrupts, determine which source caused the most recent
//! interrupt, and call an interrupt handler function to respond. The interrupt
//! handler to be called must be first registered with the
//! am_hal_ctimer_int_register() function.
//!
//! @note In the event that multiple sources are active, the corresponding
//! interrupt handlers will be called in numerical order based on interrupt def.
//
//*****************************************************************************
extern void am_hal_ctimer_int_service(uint32_t ui32Status);

//*****************************************************************************
//! @brief General function to do triple back-to-back reads.
//!
//! @param ui32TimerAddr
//! @param ui32Data
//*****************************************************************************
extern void am_hal_triple_read(uint32_t ui32TimerAddr, uint32_t ui32Data[]);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CTIMER_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
