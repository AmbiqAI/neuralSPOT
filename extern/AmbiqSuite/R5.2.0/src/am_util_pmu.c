//*****************************************************************************
//
//! @file am_util_pmu.c
//!
//! @brief The Performance Monitors Extension describes an optional non-invasive
//! component that allows performance events to be identified and counterd.This
//! enables software to get information about events that are taking place in the
//! processor and can be used for performance analysis and system debug.
//!
//! The PMU supports eight 16-bit event counters and one 32-bit cycle counter.
//! The PMU also supports a chain function which allows the PMU to cascade two of
//! the 16-bit counters into one 32-bit counter.only odd event counter support the
//! chain feature.that means by chaining counters in pairs,the counter range can be
//! increased by halving the number of counters.
//!
//! @addtogroup pmu PMU Functionality
//! @ingroup utils
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
#include "am_mcu_apollo.h"
#include "am_util_pmu.h"
#include "am_util_stdio.h"

#if (__PMU_PRESENT == 1) && (__PMU_NUM_EVENTCNT == 8)

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
static uint32_t pui32CycOvfCnt[__PMU_NUM_EVENTCNT + 1];

//*****************************************************************************
//
// Local functions and ISRs
//
//*****************************************************************************
void DebugMon_Handler(void)
{
    uint32_t ui32CntrOvs = ARM_PMU_Get_CNTR_OVS();
    uint32_t i;
    for (i = 0; i < __PMU_NUM_EVENTCNT; i++)
    {
        if (ui32CntrOvs & (1UL << i))
        {
            pui32CycOvfCnt[i]++;
            ARM_PMU_Set_CNTR_OVS(1UL << i);
        }
    }
    if (ui32CntrOvs & (1UL << PMU_OVSSET_CYCCNT_STATUS_Pos))
    {
        pui32CycOvfCnt[__PMU_NUM_EVENTCNT]++;
        ARM_PMU_Set_CNTR_OVS(PMU_OVSCLR_CYCCNT_STATUS_Msk);
    }

    return;
} // DebugMon_Handler()

//*****************************************************************************
//
// Enable Cycle Counter and Event Counters
//
//*****************************************************************************
static inline uint32_t
cntr_enable(uint32_t ui32CountersEnable)
{
    if (ui32CountersEnable & (~VALID_PMU_COUNTERS))
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
    //
    // PMU counters increment if the appropriate bit in PMU_CNTENSET register is set.
    //
    ARM_PMU_CNTR_Enable(ui32CountersEnable);
    return AM_HAL_STATUS_SUCCESS;
} // cntr_enable()

//*****************************************************************************
//
// Disable Cycle Counter and Event Counters
//
//*****************************************************************************
static inline uint32_t
cntr_disable(uint32_t ui32CountersDisable)
{
    if (ui32CountersDisable & (~VALID_PMU_COUNTERS))
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    ARM_PMU_CNTR_Disable(ui32CountersDisable);
    return AM_HAL_STATUS_SUCCESS;
} // cntr_disable()

//*****************************************************************************
//
// Set event to count for PMU event type
//
//*****************************************************************************
static inline uint32_t
set_event_type(uint32_t ui32CounterNum, uint32_t ui32EventType)
{
    if (ui32CounterNum >= __PMU_NUM_EVENTCNT)
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
    //
    // It is possible to chain an odd-numbered counter with a preceding even-numbered counter to form a 32-bit counter.
    //
    if (ui32CounterNum % 2 == 0 && ui32EventType == ARM_PMU_CHAIN)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    ARM_PMU_Set_EVTYPER(ui32CounterNum, ui32EventType);
    return AM_HAL_STATUS_SUCCESS;
} // set_event_type()

//*****************************************************************************
//
// Enable performance monitoring unit
//
//*****************************************************************************
uint32_t
am_util_pmu_enable(void)
{
    am_util_stdio_printf("PMU Enable\n");
    ARM_PMU_Enable();
    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);
    for (uint32_t i = 0; i < __PMU_NUM_EVENTCNT + 1; i++)
    {
        pui32CycOvfCnt[i] = 0;
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_util_pmu_enable()

//*****************************************************************************
//
// Disable performance monitoring unit
//
//*****************************************************************************
uint32_t
am_util_pmu_disable(void)
{
    am_util_stdio_printf("PMU Disable\n\n");
    ARM_PMU_Disable();
    return AM_HAL_STATUS_SUCCESS;
} // am_util_pmu_disable()

//*****************************************************************************
//
// Initialize the PMU
//
//*****************************************************************************
uint32_t
am_util_pmu_init(am_util_pmu_config_t *pPMUConfig)
{
    uint32_t evtcnt;
    uint32_t ui32IntMsk = 0;

    if (pPMUConfig == NULL)
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    //
    // Configure for general debug i.e. DBG power, tracing, TPIU, etc.
    //
    am_hal_debug_enable();

    //
    // Enable Trace
    //
    DCB->DEMCR |= DCB_DEMCR_MON_EN_Msk;

    //
    // Enable Low Overhead Loops
    //
    SCB->CCR |= SCB_CCR_LOB_Msk;

    //
    // Configure Event Counters Registers
    //
    for (evtcnt = 0; evtcnt < __PMU_NUM_EVENTCNT; evtcnt++)
    {
        //
        // Set event type for expected event counters.
        //
        if (pPMUConfig->ui32Counters & (1U << evtcnt))
        {
            if (AM_HAL_STATUS_SUCCESS != set_event_type(evtcnt, pPMUConfig->ui32EventType[evtcnt]))
            {
                return AM_HAL_STATUS_INVALID_ARG;
            }
        }
    }

    //
    // Reset Cycle Counter and Event Counters
    //
    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();

    //
    // Clears overflow status of event counter register.
    //
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);
    for (evtcnt = 0; evtcnt < __PMU_NUM_EVENTCNT; evtcnt++)
    {
        if (evtcnt == __PMU_NUM_EVENTCNT - 1)
        {
            ui32IntMsk |= (1UL << evtcnt);
        }
        else
        {
            if ((pPMUConfig->ui32EventType[evtcnt] == ARM_PMU_CHAIN) || (pPMUConfig->ui32EventType[evtcnt + 1] != ARM_PMU_CHAIN))
            {
                ui32IntMsk |= (1UL << evtcnt);
            }
        }
    }
    ui32IntMsk |= PMU_INTENSET_CCYCNT_ENABLE_Msk;
    ARM_PMU_Set_CNTR_IRQ_Enable(ui32IntMsk);

    //
    // Enable Cycle Counter and Event Counters
    //
    cntr_enable(pPMUConfig->ui32Counters);

    return AM_HAL_STATUS_SUCCESS;
} // am_util_pmu_init()

//*****************************************************************************
//
// Deinit the PMU
//
//*****************************************************************************
uint32_t
am_util_pmu_deinit(void)
{
    uint32_t ui32Status;

    //
    // Deinitialize from general debug
    //
    ui32Status = am_hal_debug_disable();
    if ( ui32Status == AM_HAL_STATUS_IN_USE )
    {
        ui32Status = AM_HAL_STATUS_SUCCESS;
    }

    return ui32Status;
} // am_util_pmu_deinit()

//*****************************************************************************
//
// Get event types and cycle profilings
//
//*****************************************************************************
uint32_t
am_util_pmu_get_profiling(am_util_pmu_config_t *pPMUConfig, am_util_pmu_profiling_t *pprofiling)
{
    double DCache_Reads = 0.0;
    double DCache_Misses = 0.0;
    double ICache_Access = 0.0;
    double ICache_Refills = 0.0;

    if (pPMUConfig == NULL || pprofiling == NULL)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Disable Cycle Counter and Event Counters
    //
    cntr_disable(pPMUConfig->ui32Counters);

    //
    // Iterate all event counters.
    //
    for (uint32_t evtcnt = 0; evtcnt < __PMU_NUM_EVENTCNT; evtcnt++)
    {
        pprofiling->eventProfiling[evtcnt].ui32Counter     = evtcnt;
        pprofiling->eventProfiling[evtcnt].ui32EventType   = pPMUConfig->ui32EventType[evtcnt];
        //
        // Check the active event counters
        //
        if (pPMUConfig->ui32Counters & (1U << evtcnt))
        {
            pprofiling->eventProfiling[evtcnt].bCounterUsed = true;

            if (pprofiling->eventProfiling[evtcnt].ui32EventType == ARM_PMU_CHAIN)
            {
                pprofiling->eventProfiling[evtcnt].bChain       = true;
            }
            else
            {
                pprofiling->eventProfiling[evtcnt].bChain       = false;
            }

            pprofiling->eventProfiling[evtcnt].ui32CountValue  = ARM_PMU_Get_EVCNTR(evtcnt);
        }
        else
        {
            pprofiling->eventProfiling[evtcnt].bChain          = false;
            pprofiling->eventProfiling[evtcnt].bOverflow       = false;
            pprofiling->eventProfiling[evtcnt].bCounterUsed    = false;
            pprofiling->eventProfiling[evtcnt].ui32CountValue  = 0;
        }
    }
    for (uint32_t evtcnt = 0; evtcnt < __PMU_NUM_EVENTCNT; evtcnt++)
    {
        //
        // print events profiling
        //
        if (pprofiling->eventProfiling[evtcnt].bCounterUsed && !(pprofiling->eventProfiling[evtcnt].bChain))
        {
            if (evtcnt >= (__PMU_NUM_EVENTCNT - 1) || !(pprofiling->eventProfiling[evtcnt + 1].bChain))
            {
                am_util_stdio_printf("Event Counter %u Event Type: 0x%04X, Counter Value: %llu.\n",
                                    pprofiling->eventProfiling[evtcnt].ui32Counter,
                                    pprofiling->eventProfiling[evtcnt].ui32EventType,
                                    pprofiling->eventProfiling[evtcnt].ui32CountValue + (1LLU << 16) * pui32CycOvfCnt[evtcnt]);
            }
            else
            {
                am_util_stdio_printf("Event Counter %u Event Type: 0x%04X, Counter Value: %llu.\n",
                                    pprofiling->eventProfiling[evtcnt].ui32Counter,
                                    pprofiling->eventProfiling[evtcnt].ui32EventType,
                                    (pprofiling->eventProfiling[evtcnt].ui32CountValue | (pprofiling->eventProfiling[evtcnt + 1].ui32CountValue << 16)) + (1LLU << 32) * pui32CycOvfCnt[evtcnt + 1]);

                if (pprofiling->eventProfiling[evtcnt].ui32EventType == ARM_PMU_L1D_CACHE_RD)
                {
                    DCache_Reads = (double)(pprofiling->eventProfiling[ evtcnt ].ui32CountValue | (pprofiling->eventProfiling[ evtcnt + 1 ].ui32CountValue << 16)) + (1LLU << 32) * pui32CycOvfCnt[ evtcnt + 1 ];
                }
                if (pprofiling->eventProfiling[evtcnt].ui32EventType == ARM_PMU_L1D_CACHE_MISS_RD)
                {
                    DCache_Misses = ( double )(pprofiling->eventProfiling[ evtcnt ].ui32CountValue | (pprofiling->eventProfiling[ evtcnt + 1 ].ui32CountValue << 16)) + (1LLU << 32) * pui32CycOvfCnt[ evtcnt + 1 ];
                }
                if (pprofiling->eventProfiling[evtcnt].ui32EventType == ARM_PMU_L1I_CACHE)
                {
                    ICache_Access = ( double )(pprofiling->eventProfiling[ evtcnt ].ui32CountValue | (pprofiling->eventProfiling[ evtcnt + 1 ].ui32CountValue << 16)) + (1LLU << 32) * pui32CycOvfCnt[ evtcnt + 1 ];
                }
                if (pprofiling->eventProfiling[evtcnt].ui32EventType == ARM_PMU_L1I_CACHE_REFILL)
                {
                    ICache_Refills = ( double )(pprofiling->eventProfiling[ evtcnt ].ui32CountValue | (pprofiling->eventProfiling[ evtcnt + 1 ].ui32CountValue << 16)) + (1LLU << 32) * pui32CycOvfCnt[ evtcnt + 1 ];
                }
            }
        }
        else
        {
            am_util_stdio_printf("Event Counter %u is Unused or Set as Chain Counter.\n", pprofiling->eventProfiling[evtcnt].ui32Counter);
        }
    }

    if ( ( DCache_Reads > 0 ) && ( DCache_Misses > 0 ) )
    {
        am_util_stdio_printf( "--> DCache miss percent: %f%%\n", ( DCache_Misses / DCache_Reads * 100.0 ) );
    }
    else
    {
        am_util_stdio_printf( "--> %f %f\n", DCache_Misses, DCache_Reads );
    }
    if (( ICache_Access > 0 ) && ( ICache_Refills > 0 ))
    {
        am_util_stdio_printf( "--> ICache refill percent: %f%%\n", ( ICache_Refills / ICache_Access * 100.0 ) );
    }
    else
    {
        am_util_stdio_printf( "--> %f %f\n", DCache_Misses, DCache_Reads );
    }

    pprofiling->cycleProfiling.ui32Counter     = 0;     //invalid field
    pprofiling->cycleProfiling.ui32EventType   = 0;     //invalid field
    pprofiling->cycleProfiling.bChain          = false; //invalid field
    //
    // Cycle counter
    //
    if (pPMUConfig->ui32Counters & PMU_CNTENSET_CCNTR_ENABLE_Msk)
    {
        //
        // For each counter n,if the lower 8 bits of that counter overflows,
        // the associated OVn bit of the event counter packet is set. If multiple
        // counters overflow during the same period,multiple bits might be set.
        //
        if (ARM_PMU_Get_CNTR_OVS() & PMU_OVSSET_CYCCNT_STATUS_Msk)
        {
            pprofiling->cycleProfiling.bOverflow    = true;
        }
        else
        {
            pprofiling->cycleProfiling.bOverflow    = false;
        }

        pprofiling->cycleProfiling.bCounterUsed    = true;
        pprofiling->cycleProfiling.ui32CountValue  = ARM_PMU_Get_CCNTR();
    }
    else
    {
        pprofiling->cycleProfiling.bOverflow       = false;
        pprofiling->cycleProfiling.bCounterUsed    = false;
        pprofiling->cycleProfiling.ui32CountValue  = 0;
    }

    //
    // print cycle profiling
    //
    if (pprofiling->cycleProfiling.bCounterUsed)
    {
        am_util_stdio_printf("Cycle Counter Value: %llu.\n", pprofiling->cycleProfiling.ui32CountValue + (1LLU << 32) * pui32CycOvfCnt[__PMU_NUM_EVENTCNT]);
    }
    else
    {
        am_util_stdio_printf("Cycle Counter Unused.\n");
    }

    //
    // Reset Cycle Counter and Event Counters
    //
    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();

    //
    // Clears overflow status of event counter register.
    //
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);

    //
    // Enable Cycle Counter and Event Counters
    //
    cntr_enable(pPMUConfig->ui32Counters);

    return AM_HAL_STATUS_SUCCESS;
} // am_util_pmu_get_profiling()

#endif

//*****************************************************************************
//
//! @}
// End Doxygen group.
//
//*****************************************************************************
