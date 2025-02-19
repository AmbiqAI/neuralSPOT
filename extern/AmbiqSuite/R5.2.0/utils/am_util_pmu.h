//*****************************************************************************
//
//! @file am_util_pmu.h
//!
//! @brief A few useful performance monitoring functions.
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

#ifndef AM_UTIL_PMU_H
#define AM_UTIL_PMU_H

#define VALID_PMU_COUNTERS  (PMU_CNTENSET_CNT0_ENABLE_Msk | \
                             PMU_CNTENSET_CNT1_ENABLE_Msk | \
                             PMU_CNTENSET_CNT2_ENABLE_Msk | \
                             PMU_CNTENSET_CNT3_ENABLE_Msk | \
                             PMU_CNTENSET_CNT4_ENABLE_Msk | \
                             PMU_CNTENSET_CNT5_ENABLE_Msk | \
                             PMU_CNTENSET_CNT6_ENABLE_Msk | \
                             PMU_CNTENSET_CNT7_ENABLE_Msk | \
                             PMU_CNTENSET_CCNTR_ENABLE_Msk)

#define VALID_PMU_OVSCLRS   (PMU_OVSCLR_CNT0_STATUS_Msk | \
                             PMU_OVSCLR_CNT1_STATUS_Msk | \
                             PMU_OVSCLR_CNT2_STATUS_Msk | \
                             PMU_OVSCLR_CNT3_STATUS_Msk | \
                             PMU_OVSCLR_CNT4_STATUS_Msk | \
                             PMU_OVSCLR_CNT5_STATUS_Msk | \
                             PMU_OVSCLR_CNT6_STATUS_Msk | \
                             PMU_OVSCLR_CNT7_STATUS_Msk | \
                             PMU_OVSCLR_CYCCNT_STATUS_Msk)

#define VALID_PMU_INTENSET  (PMU_INTENSET_CNT0_ENABLE_Msk | \
                             PMU_INTENSET_CNT1_ENABLE_Msk | \
                             PMU_INTENSET_CNT2_ENABLE_Msk | \
                             PMU_INTENSET_CNT3_ENABLE_Msk | \
                             PMU_INTENSET_CNT4_ENABLE_Msk | \
                             PMU_INTENSET_CNT5_ENABLE_Msk | \
                             PMU_INTENSET_CNT6_ENABLE_Msk | \
                             PMU_INTENSET_CNT7_ENABLE_Msk | \
                             PMU_INTENSET_CCYCNT_ENABLE_Msk)

//*****************************************************************************
//
//! @brief Structure used for configuring the PMU
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32EventType[__PMU_NUM_EVENTCNT];
    uint32_t ui32Counters;
}
am_util_pmu_config_t;

typedef struct
{
    bool        bChain;
    bool        bOverflow;
    bool        bCounterUsed;
    uint32_t    ui32EventType;
    uint32_t    ui32Counter;
    uint32_t    ui32CountValue;
}
am_util_pmu_status_t;

typedef struct
{
    am_util_pmu_status_t eventProfiling[__PMU_NUM_EVENTCNT];
    am_util_pmu_status_t cycleProfiling;
}
am_util_pmu_profiling_t;

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Global definitions
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief enable PMU
//!
//! This function can be used to enable performance monitoring unit.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_pmu_enable(void);

//*****************************************************************************
//
//! @brief Disable PMU
//!
//! This function can be used to disable performance monitoring unit.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_util_pmu_disable(void);

//*****************************************************************************
//
//! @brief initialize performance monitoring unit
//!
//! @param pPMUConfig  is the handle for the PMU events configurations.
//!
//! This function can be used to configure each performance event counter to count
//! any of the performance events that might be supported by an implementation. the
//! dedicated 32-bit cycle counter is hard-wared to count cycles. In case of counter
//! chain event, the architecture guarantee that the unsigned overflow of the lower
//! half of the counter and subsequent increment of the upper half of the counter
//! are counted within the same cycle.
//!
//! @return AM_HAL_STATUS_SUCCESS or others non-success status.
//
//*****************************************************************************
extern uint32_t am_util_pmu_init(am_util_pmu_config_t *pPMUConfig);

//*****************************************************************************
//
//! @brief De-initialize performance monitoring unit
//!
//! @param None
//!
//! This function de-initializes PMU usage.
//! It must be called for each corresponding call to am_util_pmu_init() after
//! PMU measurements have concluded.
//!
//! @return AM_HAL_STATUS_SUCCESS or others non-success status.
//
//*****************************************************************************
extern uint32_t am_util_pmu_deinit(void);

//*****************************************************************************
//
//! @brief Change the power state of the UART module.
//!
//! @param pPMUConfig  is the handle for the PMU events configurations.
//! @param pprofiling is the events profiling.
//!
//! This function can be used get information about events that are taking place in
//! the processor and can be used for performance analysis and system debug.
//!
//! @return AM_HAL_STATUS_SUCCESS or others non-success status.
//
//*****************************************************************************
extern uint32_t am_util_pmu_get_profiling(am_util_pmu_config_t *pPMUConfig, am_util_pmu_profiling_t *pprofiling);

#ifdef __cplusplus
}
#endif

#endif // AM_UTIL_PMU_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

