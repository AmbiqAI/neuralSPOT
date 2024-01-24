//*****************************************************************************
//
//! @file am_hal_sysctrl.c
//!
//! @brief Functions for interfacing with the M4F system control registers
//!
//! @addtogroup sysctrl3p SYSCTRL - System Control
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

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
//
//! g_am_hal_sysctrl_sleep_count is a running total of the number of times the
//! MCU has gone to sleep. (Wraps around at uint32_t max)
//
uint32_t g_am_hal_sysctrl_sleep_count = 0;

#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
//
// Execute this deepsleep function in SRAM in order to implement a power
// saving algorithm.
// Function returns 0 if the power saving was successful, 1 otherwise.
//
uint32_t SRAM_sleep[20] =
{
    0xB401B538,         // B538         push {r3-r5,lr}
                        // B401         push {r0}
    0x1278F240,         // F2401278     movw r2, #0x0178
    0x0202F2C4,         // F2C40202     movt r2, #0x4002
    0xBF006813,         // 6813         ldr  r3, [r2, #0]
                        // BF00         nop
    0x51B0F646,         // F64651B0     movw   r1, #0x6DB0
    0x0106F2C0,         // F2C00106     movt   r1, #0x0006
    0x24006011,         // 6011         str    r1, [r2, #0]
                        // 2400         movs   r4, #0
    0x428D6815,         // 6815         ldr    r5, [r2, #0]
                        // 428D         cmp    r5, r1
    0x2401BF18,         // BF18         it     ne
                        // 2401         movne  r4, #1
    0x8F4FF3BF,         // F3BF8F4F     dsb
    0xBF002000,         // 2000         movs   r0, #0x0000
                        // BF00         nop
    0x70FFF6C5,         // F6C570FF     movt   r0, #0x5FFF
    0xBF306801,         // 6801         ldr    r1, [r0, #0]
                        // BF30         wfi
    0x8F6FF3BF,         // F3BF8F6F     isb
    0xBC016013,         // 6013         str      r3, [r2,#0]
                        // BC01         pop      {r0}
    0x019dF240,         // F240019D     movw    r1, #0x009d
    0x0100F6C0,         // F6C00100     movt    r1, #0x0800
    0x46204788,         // 4788         blx     r1
                        // 4620         mov     r0, r4
    0xBF00BD38,         // BD38         pop     {r3-r5,pc}
                        // BF00         nop
    0xBF00BF00
};

//
// If additional deepsleep power saving is desired, incurring the cost of
// a short delay on wake, the user can call am_hal_sysctrl_control() to
// enable the power savings. This variable holds that state.
//  Bit0: Workaround Enabled
//  Bit1: Good-to-go to use workaround
//  That is, only do the workaround if g_ui32DeepsleepMinPwr == 0x03.
//
#define DSLP_WORKAROUND_DIS     0x00
#define DSLP_WORKAROUND_EN      0x01
#define DSLP_WORKAROUND_GO      0x02
static uint32_t g_ui32DeepsleepMinPwr = DSLP_WORKAROUND_DIS;

//
// After sleep, this variable will be 0 if additional power saving
// was successful.
//
uint32_t g_ui32SleepReturnVal = 0;
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA


// ****************************************************************************
//
// Apply various specific commands/controls on the SYSCTRL module.
//
// ****************************************************************************
uint32_t
am_hal_sysctrl_control(am_hal_sysctrl_control_e eControl, void *pArgs)
{
#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
    switch ( eControl )
    {
        case AM_HAL_SYSCTRL_CONTROL_DEEPSLEEP_MINPWR_DIS:
            g_ui32DeepsleepMinPwr = DSLP_WORKAROUND_DIS;
            break;

        case AM_HAL_SYSCTRL_CONTROL_DEEPSLEEP_MINPWR_EN:
            //
            // Activate the workaround that allows minimum power consumption
            // during deepsleep with the cost of a short delay after wake.
            // Before enabling, make sure the trim patch that allows the
            // workaround to function has been applied.
            //
            g_ui32DeepsleepMinPwr = AM_REGVAL(PATCHVER2) & 0x00000001 ?
                                    DSLP_WORKAROUND_DIS : DSLP_WORKAROUND_EN;
            break;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Return success status.
    //
    return AM_HAL_STATUS_SUCCESS;
#else // AM_HAL_SYSCTRL_DEEPSLEEP_WA
    switch ( eControl )
    {
        case AM_HAL_SYSCTRL_CONTROL_DEEPSLEEP_MINPWR_DIS:
            return AM_HAL_STATUS_INVALID_OPERATION;

        case AM_HAL_SYSCTRL_CONTROL_DEEPSLEEP_MINPWR_EN:
            return AM_HAL_STATUS_INVALID_OPERATION;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA
} // am_hal_sysctrl_control()

//*****************************************************************************
//
// Place the core into sleep or deepsleep.
//
// This function puts the MCU to sleep or deepsleep depending on bSleepDeep.
//
// Valid values for bSleepDeep are:
//     AM_HAL_SYSCTRL_SLEEP_NORMAL
//     AM_HAL_SYSCTRL_SLEEP_DEEP
//
//*****************************************************************************
void
am_hal_sysctrl_sleep(bool bSleepDeep)
{
    bool bBurstModeSleep;
    am_hal_burst_mode_e eBurstMode;

    //
    // Disable interrupts and save the previous interrupt state.
    //
    AM_CRITICAL_BEGIN

    g_am_hal_sysctrl_sleep_count++;

    //
    // If Apollo3 Blue Plus rev 0 and in burst mode, must exit burst mode
    // before going to sleep.
    //
    if ( am_hal_burst_mode_status() == AM_HAL_BURST_MODE )
    {
        bBurstModeSleep = true;

        if ( (am_hal_burst_mode_disable(&eBurstMode) != AM_HAL_STATUS_SUCCESS)  ||
             (eBurstMode != AM_HAL_NORMAL_MODE) )
        {
            while(1);
        }
#if AM_HAL_BURST_LDO_WORKAROUND
        else
        {
            am_hal_pwrctrl_wa_vddf_restore();
        }
#endif
   }
    else
    {
        bBurstModeSleep = false;
    }

    //
    // If the user selected DEEPSLEEP and the TPIU is off, attempt to enter
    // DEEP SLEEP.
    //
    if ( (bSleepDeep == AM_HAL_SYSCTRL_SLEEP_DEEP)    &&
         (MCUCTRL->TPIUCTRL_b.ENABLE == MCUCTRL_TPIUCTRL_ENABLE_DIS) )
    {
        //
        // Retrieve the reset generator status bits
        // This gets reset on Deep Sleep, so we take a snapshot here
        //
        if ( !gAmHalResetStatus )
        {
            gAmHalResetStatus = RSTGEN->STAT;
        }

        //
        // do not boost core and mem voltages
        //


        //
        // Prepare the core for deepsleep (write 1 to the DEEPSLEEP bit).
        //
        SCB->SCR |= _VAL2FLD(SCB_SCR_SLEEPDEEP, 1);
#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
        //
        // Set the workaround flag.
        //
        g_ui32DeepsleepMinPwr |= DSLP_WORKAROUND_GO;
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA
    }
    else
    {
        //
        // Prepare the core for normal sleep (write 0 to the DEEPSLEEP bit).
        //
        SCB->SCR &= ~_VAL2FLD(SCB_SCR_SLEEPDEEP, 1);
#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
        //
        // Clear the workaround flag.
        //
        g_ui32DeepsleepMinPwr &= ~DSLP_WORKAROUND_GO;
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA
    }

#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
    if ( g_ui32DeepsleepMinPwr == (DSLP_WORKAROUND_EN | DSLP_WORKAROUND_GO) )
    {
        //
        // Call the Apollo3p specific SRAM sleep routine that will minimize
        // deep sleep current consumption. On wake a short delay is incurred.
        //
        uint32_t SRAMCode = (uint32_t)SRAM_sleep | 0x1;
        uint32_t (*pFunc)(uint32_t) = (uint32_t (*)(uint32_t))SRAMCode;
        g_ui32SleepReturnVal = (*pFunc)(FLASH_CYCLES_US(5));
    }
    else
    {
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA
        //
        // Before executing WFI, flush any buffered core and peripheral writes.
        //
        __DSB();
        am_hal_sysctrl_bus_write_flush();

        //
        // Execute the sleep instruction.
        //
        __WFI();

        //
        // Upon wake, execute the Instruction Sync Barrier instruction.
        //
        __ISB();
#ifdef AM_HAL_SYSCTRL_DEEPSLEEP_WA
    }
#endif // AM_HAL_SYSCTRL_DEEPSLEEP_WA

    //
    // Restore burst mode?
    //
    if ( bBurstModeSleep )
    {
        bBurstModeSleep = false;

        if ( (am_hal_burst_mode_enable(&eBurstMode) != AM_HAL_STATUS_SUCCESS)  ||
             (eBurstMode != AM_HAL_BURST_MODE) )
        {
            //while(1);
        }
    }

    //
    // Restore the interrupt state.
    //
    AM_CRITICAL_END
}

//*****************************************************************************
//
// Enable the floating point module.
//
// Call this function to enable the ARM hardware floating point module.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_enable(void)
{
    //
    // Enable access to the FPU in both privileged and user modes.
    // NOTE: Write 0s to all reserved fields in this register.
    //
    SCB->CPACR = _VAL2FLD(SCB_CPACR_CP11, 0x3) |
                 _VAL2FLD(SCB_CPACR_CP10, 0x3);
}

//*****************************************************************************
//
// Disable the floating point module.
//
// Call this function to disable the ARM hardware floating point module.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_disable(void)
{
    //
    // Disable access to the FPU in both privileged and user modes.
    // NOTE: Write 0s to all reserved fields in this register.
    //
    SCB->CPACR = 0x00000000                         &
                 ~(_VAL2FLD(SCB_CPACR_CP11, 0x3) |
                   _VAL2FLD(SCB_CPACR_CP10, 0x3));
}

//*****************************************************************************
//
// Enable stacking of FPU registers on exception entry.
//
// @param bLazy - Set to "true" to enable "lazy stacking".
//
// This function allows the core to save floating-point information to the
// stack on exception entry. Setting the bLazy option enables "lazy stacking"
// for interrupt handlers.  Normally, mixing floating-point code and interrupt
// driven routines causes increased interrupt latency, because the core must
// save extra information to the stack upon exception entry. With the lazy
// stacking option enabled, the core will skip the saving of floating-point
// registers when possible, reducing average interrupt latency.
//
// @note At reset of the Cortex M4, the ASPEN and LSPEN bits are set to 1,
// enabling Lazy mode by default. Therefore this function will generally
// only have an affect when setting for full-context save (or when switching
// from full-context to lazy mode).
//
// @note See also:
// infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0298a/DAFGGBJD.html
//
// @note Three valid FPU context saving modes are possible.
// 1. Lazy           ASPEN=1 LSPEN=1 am_hal_sysctrl_fpu_stacking_enable(true)
//                                   and default.
// 2. Full-context   ASPEN=1 LSPEN=0 am_hal_sysctrl_fpu_stacking_enable(false)
// 3. No FPU state   ASPEN=0 LSPEN=0 am_hal_sysctrl_fpu_stacking_disable()
// 4. Invalid        ASPEN=0 LSPEN=1
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_stacking_enable(bool bLazy)
{
    uint32_t ui32fpccr;

    //
    // Set the requested FPU stacking mode in ISRs.
    //
    AM_CRITICAL_BEGIN
#define SYSCTRL_FPCCR_LAZY  (FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk)
    ui32fpccr  = FPU->FPCCR;
    ui32fpccr &= ~SYSCTRL_FPCCR_LAZY;
    ui32fpccr |= (bLazy ? SYSCTRL_FPCCR_LAZY : FPU_FPCCR_ASPEN_Msk);
    FPU->FPCCR = ui32fpccr;
    AM_CRITICAL_END
}

//*****************************************************************************
//
// Disable FPU register stacking on exception entry.
//
// This function disables all stacking of floating point registers for
// interrupt handlers.  This mode should only be used when it is absolutely
// known that no FPU instructions will be executed in an ISR.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_stacking_disable(void)
{
    //
    // Completely disable FPU context save on entry to ISRs.
    //
    AM_CRITICAL_BEGIN
    FPU->FPCCR &= ~SYSCTRL_FPCCR_LAZY;
    AM_CRITICAL_END
}

//*****************************************************************************
//
// Issue a system wide reset using the AIRCR bit in the M4 system ctrl.
//
// This function issues a system wide reset (Apollo POR level reset).
//
//*****************************************************************************
void
am_hal_sysctrl_aircr_reset(void)
{
    //
    // Set the system reset bit in the AIRCR register
    //
    __NVIC_SystemReset();
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
