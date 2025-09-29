/**
 * @file ns_psram.c
 * @author Ambiq
 * @brief Utility for configuring and accessing PSRAM
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_psram.h"

#ifdef apollo510_evb
#include "am_devices_mspi_psram_aps25616ba_1p2v.h"
#else
#include "am_devices_mspi_psram_aps25616n.h"
#endif

#define MSPI_PSRAM_OCTAL_CONFIG MSPI_PSRAM_OctalCE0MSPIConfig
#define MSPI_PSRAM_OCTAL_CONFIG MSPI_PSRAM_OctalCE0MSPIConfig

AM_SHARED_RW uint32_t DMATCBBuffer[2560];

void            *g_pDevHandle;
void            *g_pHandle;

// EVB Stuff

am_hal_mpu_region_config_t sMPUCfg =
{
    .ui32RegionNumber = 6,
    .ui32BaseAddress = (uint32_t)DMATCBBuffer,
    .eShareable = NON_SHARE,
    .eAccessPermission = RW_NONPRIV,
    .bExecuteNever = true,
    .ui32LimitAddress = (uint32_t)DMATCBBuffer + sizeof(DMATCBBuffer) - 1,
    .ui32AttrIndex = 0,
    .bEnable = true,
};
am_hal_mpu_attr_t sMPUAttr =
{
    .ui8AttrIndex = 0,
    .bNormalMem = true,
    .sOuterAttr = {
                    .bNonTransient = false,
                    .bWriteBack = true,
                    .bReadAllocate = false,
                    .bWriteAllocate = false
                  },
    .sInnerAttr = {
                    .bNonTransient = false,
                    .bWriteBack = true,
                    .bReadAllocate = false,
                    .bWriteAllocate = false
                  },
    .eDeviceAttr = 0,
};

// END EVB stuff


am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
#ifdef apollo510_evb
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
#else
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
#endif
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
#ifdef apollo510_evb
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
#else
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
#endif
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_HexCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_HEX_DDR_CE0,
#ifdef apollo510_evb
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
#else
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
#endif
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_HexCE1MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_HEX_DDR_CE1,
#ifdef apollo510_evb
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
#else
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
#endif
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
am_hal_mspi_cpu_read_burst_t sCPURQCFG =
{
    .eGQArbBais                         = AM_HAL_MSPI_GQARB_2_1_BAIS,
    .bCombineRID2CacheMissAccess        = true,
    .bCombineRID3CachePrefetchAccess    = true,
    .bCombineRID4ICacheAccess           = false,
    .bCPUReadQueueEnable                = true,
};
#endif

am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;

#ifdef apollo510_evb
#define MSPI_TEST_MODULE              0
#elif defined(apollo510L_eb)
#define MSPI_TEST_MODULE              1
#else
#define MSPI_TEST_MODULE              3
#endif

// Function calls are based on memory type
#ifdef apollo510_evb
#define init_psram_timing_check      am_devices_mspi_psram_aps25616ba_ddr_init_timing_check
#define init_psram                   am_devices_mspi_psram_aps25616ba_ddr_init
#define apply_psram_timing           am_devices_mspi_psram_aps25616ba_apply_ddr_timing
#define enable_psram_xip             am_devices_mspi_psram_aps25616ba_ddr_enable_xip
#else
#define init_psram_timing_check      am_devices_mspi_psram_aps25616n_ddr_init_timing_check
#define init_psram                   am_devices_mspi_psram_aps25616n_ddr_init
#define apply_psram_timing           am_devices_mspi_psram_aps25616n_apply_ddr_timing
#define enable_psram_xip             am_devices_mspi_psram_aps25616n_ddr_enable_xip
#endif


#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS MSPI0_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS MSPI1_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS MSPI2_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 3)
#define MSPI_XIP_BASE_ADDRESS MSPI3_APERTURE_START_ADDR
#endif // #if (MSPI_TEST_MODULE == 0)

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
#ifndef apollo510L_eb
    MSPI3_IRQn,
#endif
};

#define psram_mspi_isr                                                          \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

// MSPI ISRs.
void psram_mspi_isr(void)
{
    uint32_t      ui32Status;
    am_hal_mspi_interrupt_status_get(g_pHandle, &ui32Status, false);
    am_hal_mspi_interrupt_clear(g_pHandle, ui32Status);
    am_hal_mspi_interrupt_service(g_pHandle, ui32Status);
}

uint32_t ns_psram_platform_init(ns_psram_config_t *cfg) {
    uint32_t      ui32Status;
    am_devices_mspi_psram_config_t *psram_config;

#ifdef apollo510_evb
    // MPU Stuff
    am_hal_mpu_attr_configure(&sMPUAttr, 1);
    am_hal_mpu_region_clear();
    am_hal_mpu_region_configure(&sMPUCfg, 1);
    am_hal_cachectrl_dcache_invalidate(NULL, true);
    am_hal_mpu_enable(true, true);
#endif



    if (cfg->psram_type == PSRAM_TYPE_OCT) {
        psram_config = &MSPI_PSRAM_OctalCE0MSPIConfig;
    } else {
        psram_config = &MSPI_PSRAM_HexCE0MSPIConfig;
    }

    ui32Status = init_psram_timing_check(MSPI_TEST_MODULE, psram_config, &MSPIDdrTimingConfig);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        ns_lp_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
        ns_lp_printf("Must ensure Device is Connected and Properly Initialized!\n");
        return ui32Status;
    }

    ui32Status = init_psram(MSPI_TEST_MODULE, psram_config, &g_pDevHandle, &g_pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status) // return ui32Status;
    {
        ns_lp_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
        ns_lp_printf("Must ensure Device is Connected and Properly Initialized!\n");
        return ui32Status;
    }

    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);
    am_hal_interrupt_master_enable();

    apply_psram_timing(g_pDevHandle, &MSPIDdrTimingConfig);

    //
    // Enable XIP mode.
    //
    ui32Status = enable_psram_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        ns_lp_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    cfg->psram_base_address = MSPI_XIP_BASE_ADDRESS;
    return ui32Status;
}