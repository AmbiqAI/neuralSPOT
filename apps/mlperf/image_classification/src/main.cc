/* Copyright 2020 The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
/// \file
/// \brief Main function to run benchmark on device.

/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "internally_implemented.h"
#include "submitter_implemented.h"

void ns_power_memory_config(const ns_power_config_t *pCfg) {
    // configure SRAM & other memories
    #define ALL_RETAIN 1
    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eROMMode       = AM_HAL_PWRCTRL_ROM_AUTO,
 
        #if defined(AM_PART_APOLLO5A)
        .bEnableCache   = true,
        .bRetainCache   = true,
        #if ALL_RETAIN
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
        #else
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
        #endif
        #elif defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510)
        #if ALL_RETAIN
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM128K_DTCM256K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
        #else
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
        #endif
        #endif
        #if defined(AM_PART_APOLLO5A)
                .bEnableNVM     = true,
        #else
        // .eNVMCfg        = AM_HAL_PWRCTRL_NVM0_AND_NVM1,
        .eNVMCfg        = AM_HAL_PWRCTRL_NVM0_ONLY,
        #endif
        .bKeepNVMOnInDeepSleep     = false
    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
 
#if defined(AM_PART_APOLLO5A)
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
#else
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0LPREN = 1;
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0SLPEN = 0;
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0PWRCTRL = 1;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1LPREN = 1;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1SLPEN = 0;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1PWRCTRL = 1;
#endif
    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
    // .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_3M,
    .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_NONE,
    // .eActiveWithMCU   = PWRCTRL_SSRAMPWREN_PWRENSSRAM_ALL,
    .eActiveWithMCU   = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithGFX   = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithDISP  = AM_HAL_PWRCTRL_SRAM_NONE,          
    // .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_3M,
    .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_NONE
    };       
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);
}


int main(int argc, char *argv[]) {
  ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
  NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
  NS_TRY(ns_power_config(&ns_mlperf_mode2), "Power Init Failed.\n");
  ns_power_memory_config(&ns_mlperf_mode2);
  ns_interrupt_master_enable();
  ee_benchmark_initialize();
  while (1) {
    ns_deep_sleep();
  }
  return 0;
}