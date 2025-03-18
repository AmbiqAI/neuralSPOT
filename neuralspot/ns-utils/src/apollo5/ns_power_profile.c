//*****************************************************************************
//
//! @file am_util_pp.c for Apollo4
//!
//! @brief This is the Ambiq Power Profiling plugin module
//!
//! Functions for Ambiq power profiling.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro
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
// This is part of revision 4.0.0 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_power_profile.h"
// #include "ns_pp.h"
#include "am_util_debug.h"
#include "am_bsp.h"
#include "ns_ambiqsuite_harness.h"

// #include "am_vos_board_setup.h"

#if USE_AMIC_AUDADC
#define AM_HAL_MAGIC_AUDADC                0xAFAFAF
#define AM_HAL_AUDADC_CHK_HANDLE(h)        ((h) && ((am_hal_handle_prefix_t *)(h))->s.bInit && (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_AUDADC))
#endif

#if USE_DMIC_PDM
#define AM_HAL_MAGIC_PDM                0xF956E2
#define AM_HAL_PDM_HANDLE_VALID(h)                                            \
    ((h) &&                                                                   \
    (((am_hal_handle_prefix_t *)(h))->s.bInit) &&                             \
    (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_PDM))
#endif

/****************************************************************************
 * Example: regmap.c
 *
 * This file demonstrates:
 *   1) A const array `regmap[]` listing all registers-of-interest.
 *   2) A snapshot buffer for storing up to 5 snapshots of those registers.
 *   3) Two functions:
 *       - capture_snapshot(...)  (captures the register values)
 *       - print_snapshot(...)    (prints in JSON or CSV)
 *
 * Include this file in your project, along with the proper CMSIS and board
 * support headers that define PWRCTRL, MCUCTRL, FPIO, etc. 
 ****************************************************************************/

 
 //*****************************************************************************
 // Our regmap structure as requested.
 //*****************************************************************************
 typedef struct {
     // We store the "address" in a uintptr_t for clarity. 
     // Alternatively, you could store as `volatile uint32_t *addr;`.
     uintptr_t          addr; 
     const char         regname[64];
     const char         description[128];
 } regmap_t;
 
#define MCUCTRL_HFRC            (*(volatile uint32_t*)0x400200C0)
#define MCUCTRL_HFRC2           (*(volatile uint32_t*)0x400200C4)
#define MCUCTRL_VREFGEN         (*(volatile uint32_t*)0x40020040)
#define MCUCTRL_VREFGEN3        (*(volatile uint32_t*)0x40020048)
#define MCUCTRL_VREFGEN5        (*(volatile uint32_t*)0x40020050)
#define MCUCTRL_MISCPWRCTRL     (*(volatile uint32_t*)0x40020198)
#define MCUCTRL_SRAMTRIMHP     (*(volatile uint32_t*)0x40020330)
#define MCUCTRL_SRAMTRIM       (*(volatile uint32_t*)0x40020334)
#define MCUCTRL_CPUICACHETRIM   (*(volatile uint32_t*)0x40020424)
#define MCUCTRL_CPUDCACHETRIM   (*(volatile uint32_t*)0x40020428)
#define MCUCTRL_SSRAMTRIM       (*(volatile uint32_t*)0x4002042C)
 //*****************************************************************************
 // The master list of registers-of-interest.
 // In real code, fill in the 'description' from apollo5b.h or other docs.
 //*****************************************************************************
 static const regmap_t g_regmap[] =
 {
     // Example PWRCTRL registers
     { (uintptr_t)&PWRCTRL->MCUPERFREQ,      "PWRCTRL->MCUPERFREQ",      "MCU performance frequency configuration." },
     { (uintptr_t)&PWRCTRL->DEVPWREN,        "PWRCTRL->DEVPWREN",        "Device power enable." },
     { (uintptr_t)&PWRCTRL->DEVPWRSTATUS,    "PWRCTRL->DEVPWRSTATUS",    "Device power status." },
     { (uintptr_t)&PWRCTRL->AUDSSPWREN,      "PWRCTRL->AUDSSPWREN",      "Audio subsystem power enable." },
     { (uintptr_t)&PWRCTRL->AUDSSPWRSTATUS,  "PWRCTRL->AUDSSPWRSTATUS",  "Audio subsystem power status." },
     { (uintptr_t)&PWRCTRL->MEMPWREN,        "PWRCTRL->MEMPWREN",        "Memory power enable." },
     { (uintptr_t)&PWRCTRL->MEMPWRSTATUS,    "PWRCTRL->MEMPWRSTATUS",    "Memory power status." },
     { (uintptr_t)&PWRCTRL->MEMRETCFG,       "PWRCTRL->MEMRETCFG",       "Memory retention configuration." },
     { (uintptr_t)&PWRCTRL->SYSPWRSTATUS,    "PWRCTRL->SYSPWRSTATUS",    "System power status." },
     { (uintptr_t)&PWRCTRL->SSRAMPWREN,      "PWRCTRL->SSRAMPWREN",      "SSRAMP power enable bits." },
     { (uintptr_t)&PWRCTRL->SSRAMPWRST,      "PWRCTRL->SSRAMPWRST",      "SSRAMP power status." },
     { (uintptr_t)&PWRCTRL->SSRAMRETCFG,     "PWRCTRL->SSRAMRETCFG",     "SSRAMP retention configuration." },
     { (uintptr_t)&PWRCTRL->DEVPWREVENTEN,   "PWRCTRL->DEVPWREVENTEN",   "Device power event enable." },
     { (uintptr_t)&PWRCTRL->MEMPWREVENTEN,   "PWRCTRL->MEMPWREVENTEN",   "Memory power event enable." },
     { (uintptr_t)&PWRCTRL->MMSOVERRIDE,     "PWRCTRL->MMSOVERRIDE",     "Misc. override bits." },
     { (uintptr_t)&PWRCTRL->CPUPWRCTRL,      "PWRCTRL->CPUPWRCTRL",      "CPU power control." },
     { (uintptr_t)&PWRCTRL->PWRCTRLMODESTATUS,"PWRCTRL->PWRCTRLMODESTATUS","PWRCTRL mode status." },
     { (uintptr_t)&PWRCTRL->CPUPWRSTATUS,    "PWRCTRL->CPUPWRSTATUS",    "CPU power status." },
     { (uintptr_t)&PWRCTRL->PWRACKOVR,       "PWRCTRL->PWRACKOVR",       "Power-ack override." },
     { (uintptr_t)&PWRCTRL->PWRCNTDEFVAL,    "PWRCTRL->PWRCNTDEFVAL",    "Power count default value." },
     { (uintptr_t)&PWRCTRL->GFXPERFREQ,      "PWRCTRL->GFXPERFREQ",      "Graphics performance frequency configuration." },
     { (uintptr_t)&PWRCTRL->GFXPWRSWSEL,     "PWRCTRL->GFXPWRSWSEL",     "Graphics power switch select." },
     { (uintptr_t)&PWRCTRL->EPURETCFG,       "PWRCTRL->EPURETCFG",       "EPU retention configuration." },
     { (uintptr_t)&PWRCTRL->VRCTRL,          "PWRCTRL->VRCTRL",          "Voltage regulator control." },
     { (uintptr_t)&PWRCTRL->LEGACYVRLPOVR,   "PWRCTRL->LEGACYVRLPOVR",   "Legacy VR low-power override." },
     { (uintptr_t)&PWRCTRL->VRSTATUS,        "PWRCTRL->VRSTATUS",        "Voltage regulator status." },
     { (uintptr_t)&PWRCTRL->SRAMCTRL,        "PWRCTRL->SRAMCTRL",        "SRAM control." },
     { (uintptr_t)&PWRCTRL->ADCSTATUS,       "PWRCTRL->ADCSTATUS",       "ADC power status." },
     { (uintptr_t)&PWRCTRL->AUDADCSTATUS,    "PWRCTRL->AUDADCSTATUS",    "Audio ADC status." },
     { (uintptr_t)&PWRCTRL->TONCNTRCTRL,     "PWRCTRL->TONCNTRCTRL",     "TON counter control." },
     { (uintptr_t)&PWRCTRL->LPOVRTHRESHVDDS, "PWRCTRL->LPOVRTHRESHVDDS", "LPO override threshold VDDS." },
     { (uintptr_t)&PWRCTRL->LPOVRHYSTCNT,    "PWRCTRL->LPOVRHYSTCNT",    "LPO override hysteresis count." },
     { (uintptr_t)&PWRCTRL->LPOVRTHRESHVDDF, "PWRCTRL->LPOVRTHRESHVDDF", "LPO override threshold VDDF." },
     { (uintptr_t)&PWRCTRL->LPOVRTHRESHVDDC, "PWRCTRL->LPOVRTHRESHVDDC", "LPO override threshold VDDC." },
     { (uintptr_t)&PWRCTRL->LPOVRTHRESHVDDCLV,"PWRCTRL->LPOVRTHRESHVDDCLV","LPO override threshold VDDCLV." },
     { (uintptr_t)&PWRCTRL->LPOVRSTAT,       "PWRCTRL->LPOVRSTAT",       "LPO override status." },
     { (uintptr_t)&PWRCTRL->MRAMEXTCTRL,     "PWRCTRL->MRAMEXTCTRL",     "MRAM external control." },
     { (uintptr_t)&PWRCTRL->EMONCTRL,        "PWRCTRL->EMONCTRL",        "Energy monitor control." },
     { (uintptr_t)&PWRCTRL->EMONCFG0,        "PWRCTRL->EMONCFG0",        "Energy monitor config 0." },
     { (uintptr_t)&PWRCTRL->EMONCFG1,        "PWRCTRL->EMONCFG1",        "Energy monitor config 1." },
     { (uintptr_t)&PWRCTRL->EMONCFG2,        "PWRCTRL->EMONCFG2",        "Energy monitor config 2." },
     { (uintptr_t)&PWRCTRL->EMONCFG3,        "PWRCTRL->EMONCFG3",        "Energy monitor config 3." },
     { (uintptr_t)&PWRCTRL->EMONCFG4,        "PWRCTRL->EMONCFG4",        "Energy monitor config 4." },
     { (uintptr_t)&PWRCTRL->EMONCFG5,        "PWRCTRL->EMONCFG5",        "Energy monitor config 5." },
     { (uintptr_t)&PWRCTRL->EMONCFG6,        "PWRCTRL->EMONCFG6",        "Energy monitor config 6." },
     { (uintptr_t)&PWRCTRL->EMONCFG7,        "PWRCTRL->EMONCFG7",        "Energy monitor config 7." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT0,      "PWRCTRL->EMONCOUNT0",      "Energy monitor count 0." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT1,      "PWRCTRL->EMONCOUNT1",      "Energy monitor count 1." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT2,      "PWRCTRL->EMONCOUNT2",      "Energy monitor count 2." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT3,      "PWRCTRL->EMONCOUNT3",      "Energy monitor count 3." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT4,      "PWRCTRL->EMONCOUNT4",      "Energy monitor count 4." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT5,      "PWRCTRL->EMONCOUNT5",      "Energy monitor count 5." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT6,      "PWRCTRL->EMONCOUNT6",      "Energy monitor count 6." },
     { (uintptr_t)&PWRCTRL->EMONCOUNT7,      "PWRCTRL->EMONCOUNT7",      "Energy monitor count 7." },
     { (uintptr_t)&PWRCTRL->EMONSTATUS,      "PWRCTRL->EMONSTATUS",      "Energy monitor status." },
 
     // FPIO
     { (uintptr_t)&FPIO->EN0,                "FPIO->EN0",                "FPIO enable0." },
     { (uintptr_t)&FPIO->EN1,                "FPIO->EN1",                "FPIO enable1." },
     { (uintptr_t)&FPIO->EN2,                "FPIO->EN2",                "FPIO enable2." },
 
     // MCUCTRL
     { (uintptr_t)&MCUCTRL->CHIPPN,          "MCUCTRL->CHIPPN",          "Chip part number." },
     { (uintptr_t)&MCUCTRL->CHIPID0,         "MCUCTRL->CHIPID0",         "Chip ID0." },
     { (uintptr_t)&MCUCTRL->CHIPID1,         "MCUCTRL->CHIPID1",         "Chip ID1." },
     { (uintptr_t)&MCUCTRL->CHIPREV,         "MCUCTRL->CHIPREV",         "Chip revision." },
     { (uintptr_t)&MCUCTRL->VENDORID,        "MCUCTRL->VENDORID",        "Vendor ID." },
     { (uintptr_t)&MCUCTRL->SKU,             "MCUCTRL->SKU",             "SKU bits." },
     { (uintptr_t)&MCUCTRL->SKUOVERRIDE,     "MCUCTRL->SKUOVERRIDE",     "Override SKU." },
     { (uintptr_t)&MCUCTRL->DEBUGGER,        "MCUCTRL->DEBUGGER",        "Debugger control." },
     { (uintptr_t)&MCUCTRL->ACRG,            "MCUCTRL->ACRG",            "ACRG config." },
     { (uintptr_t)&MCUCTRL_VREFGEN,         "MCUCTRL_VREFGEN",         "VREF generator control." },
     { (uintptr_t)&MCUCTRL->VREFGEN2,        "MCUCTRL->VREFGEN2",        "VREF generator control 2." },
     { (uintptr_t)&MCUCTRL_VREFGEN3,        "MCUCTRL->VREFGEN3",        "VREF generator control 3." },
     { (uintptr_t)&MCUCTRL->VREFGEN4,        "MCUCTRL->VREFGEN4",        "VREF generator control 4." },
     { (uintptr_t)&MCUCTRL_VREFGEN5,        "MCUCTRL->VREFGEN5",        "VREF generator control 5." },
     { (uintptr_t)&MCUCTRL->VREFBUF,         "MCUCTRL->VREFBUF",         "VREF buffer config." },
     { (uintptr_t)&MCUCTRL->VRCTRL,          "MCUCTRL->VRCTRL",          "Voltage regulator control." },
     { (uintptr_t)&MCUCTRL->LDOREG1,         "MCUCTRL->LDOREG1",         "LDO regulator1." },
     { (uintptr_t)&MCUCTRL->LDOREG2,         "MCUCTRL->LDOREG2",         "LDO regulator2." },
     { (uintptr_t)&MCUCTRL_HFRC,            "MCUCTRL->HFRC",            "HFRC trim." },
     { (uintptr_t)&MCUCTRL_HFRC2,           "MCUCTRL->HFRC2",           "HFRC2 trim." },
     { (uintptr_t)&MCUCTRL->LFRC,            "MCUCTRL->LFRC",            "LFRC control." },
     { (uintptr_t)&MCUCTRL->BODCTRL,         "MCUCTRL->BODCTRL",         "Brownout detect control." },
     { (uintptr_t)&MCUCTRL->ADCPWRCTRL,      "MCUCTRL->ADCPWRCTRL",      "ADC power control." },
     { (uintptr_t)&MCUCTRL->ADCCAL,          "MCUCTRL->ADCCAL",          "ADC calibration." },
     { (uintptr_t)&MCUCTRL->ADCBATTLOAD,     "MCUCTRL->ADCBATTLOAD",     "ADC battery load." },
     { (uintptr_t)&MCUCTRL->XTALCTRL,        "MCUCTRL->XTALCTRL",        "XTAL control." },
     { (uintptr_t)&MCUCTRL->XTALGENCTRL,     "MCUCTRL->XTALGENCTRL",     "XTAL gen control." },
     { (uintptr_t)&MCUCTRL->XTALHSTRIMS,     "MCUCTRL->XTALHSTRIMS",     "XTAL HS trims." },
     { (uintptr_t)&MCUCTRL->XTALHSCTRL,      "MCUCTRL->XTALHSCTRL",      "XTAL HS control." },
     { (uintptr_t)&MCUCTRL->BGTLPCTRL,       "MCUCTRL->BGTLPCTRL",       "Bandgap LP control." },
     { (uintptr_t)&MCUCTRL->MRAMCRYPTOPWRCTRL,"MCUCTRL->MRAMCRYPTOPWRCTRL","MRAM crypto pwr ctrl." },
     { (uintptr_t)&MCUCTRL_MISCPWRCTRL,     "MCUCTRL->MISCPWRCTRL",     "Misc power control." },
     { (uintptr_t)&MCUCTRL->BODISABLE,       "MCUCTRL->BODISABLE",       "BOD disable." },
     { (uintptr_t)&MCUCTRL->D2ASPARE,        "MCUCTRL->D2ASPARE",        "Spare bits." },
     { (uintptr_t)&MCUCTRL->BOOTLOADER,      "MCUCTRL->BOOTLOADER",      "Bootloader config." },
     { (uintptr_t)&MCUCTRL->SHADOWVALID,     "MCUCTRL->SHADOWVALID",     "Shadow registers valid." },
     { (uintptr_t)&MCUCTRL->SCRATCH0,        "MCUCTRL->SCRATCH0",        "Scratch register 0." },
     { (uintptr_t)&MCUCTRL->SCRATCH1,        "MCUCTRL->SCRATCH1",        "Scratch register 1." },
     { (uintptr_t)&MCUCTRL->DBGR1,           "MCUCTRL->DBGR1",           "Debug register 1." },
     { (uintptr_t)&MCUCTRL->DBGR2,           "MCUCTRL->DBGR2",           "Debug register 2." },
     { (uintptr_t)&MCUCTRL->WICCONTROL,      "MCUCTRL->WICCONTROL",      "WIC control." },
     { (uintptr_t)&MCUCTRL->DBGCTRL,         "MCUCTRL->DBGCTRL",         "Debug control." },
     { (uintptr_t)&MCUCTRL->OTAPOINTER,      "MCUCTRL->OTAPOINTER",      "OTA pointer." },
     { (uintptr_t)&MCUCTRL->APBDMACTRL,      "MCUCTRL->APBDMACTRL",      "APBDMA control." },
     { (uintptr_t)&MCUCTRL->FORCEAXICLKEN,   "MCUCTRL->FORCEAXICLKEN",   "Force AXI clock enable." },
     { (uintptr_t)&MCUCTRL_SRAMTRIMHP,      "MCUCTRL->SRAMTRIMHP",      "SRAM HP trim." },
     { (uintptr_t)&MCUCTRL_SRAMTRIM,        "MCUCTRL->SRAMTRIM",        "SRAM trim." },
     { (uintptr_t)&MCUCTRL->KEXTCLKSEL,      "MCUCTRL->KEXTCLKSEL",      "Kext clock select." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK0,       "MCUCTRL->SIMOBUCK0",       "SIMO buck config 0." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK1,       "MCUCTRL->SIMOBUCK1",       "SIMO buck config 1." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK2,       "MCUCTRL->SIMOBUCK2",       "SIMO buck config 2." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK3,       "MCUCTRL->SIMOBUCK3",       "SIMO buck config 3." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK4,       "MCUCTRL->SIMOBUCK4",       "SIMO buck config 4." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK5,       "MCUCTRL->SIMOBUCK5",       "SIMO buck config 5." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK6,       "MCUCTRL->SIMOBUCK6",       "SIMO buck config 6." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK7,       "MCUCTRL->SIMOBUCK7",       "SIMO buck config 7." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK8,       "MCUCTRL->SIMOBUCK8",       "SIMO buck config 8." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK9,       "MCUCTRL->SIMOBUCK9",       "SIMO buck config 9." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK10,      "MCUCTRL->SIMOBUCK10",      "SIMO buck config 10." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK11,      "MCUCTRL->SIMOBUCK11",      "SIMO buck config 11." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK12,      "MCUCTRL->SIMOBUCK12",      "SIMO buck config 12." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK13,      "MCUCTRL->SIMOBUCK13",      "SIMO buck config 13." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK14,      "MCUCTRL->SIMOBUCK14",      "SIMO buck config 14." },
     { (uintptr_t)&MCUCTRL->SIMOBUCK15,      "MCUCTRL->SIMOBUCK15",      "SIMO buck config 15." },
     { (uintptr_t)&MCUCTRL->PWRSW0,          "MCUCTRL->PWRSW0",          "Power switch 0." },
     { (uintptr_t)&MCUCTRL->PWRSW1,          "MCUCTRL->PWRSW1",          "Power switch 1." },
     { (uintptr_t)&MCUCTRL->USBRSTCTRL,      "MCUCTRL->USBRSTCTRL",      "USB reset ctrl." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT0,     "MCUCTRL->FLASHWPROT0",     "Flash write protect 0." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT1,     "MCUCTRL->FLASHWPROT1",     "Flash write protect 1." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT2,     "MCUCTRL->FLASHWPROT2",     "Flash write protect 2." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT3,     "MCUCTRL->FLASHWPROT3",     "Flash write protect 3." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT0,     "MCUCTRL->FLASHRPROT0",     "Flash read protect 0." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT1,     "MCUCTRL->FLASHRPROT1",     "Flash read protect 1." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT2,     "MCUCTRL->FLASHRPROT2",     "Flash read protect 2." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT3,     "MCUCTRL->FLASHRPROT3",     "Flash read protect 3." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT0,      "MCUCTRL->SRAMWPROT0",      "SRAM write protect 0." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT1,      "MCUCTRL->SRAMWPROT1",      "SRAM write protect 1." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT2,      "MCUCTRL->SRAMWPROT2",      "SRAM write protect 2." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT3,      "MCUCTRL->SRAMWPROT3",      "SRAM write protect 3." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT4,      "MCUCTRL->SRAMWPROT4",      "SRAM write protect 4." },
     { (uintptr_t)&MCUCTRL->SRAMWPROT5,      "MCUCTRL->SRAMWPROT5",      "SRAM write protect 5." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT0,      "MCUCTRL->SRAMRPROT0",      "SRAM read protect 0." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT1,      "MCUCTRL->SRAMRPROT1",      "SRAM read protect 1." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT2,      "MCUCTRL->SRAMRPROT2",      "SRAM read protect 2." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT3,      "MCUCTRL->SRAMRPROT3",      "SRAM read protect 3." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT4,      "MCUCTRL->SRAMRPROT4",      "SRAM read protect 4." },
     { (uintptr_t)&MCUCTRL->SRAMRPROT5,      "MCUCTRL->SRAMRPROT5",      "SRAM read protect 5." },
     { (uintptr_t)&MCUCTRL_CPUICACHETRIM,   "MCUCTRL->CPUICACHETRIM",   "CPU Icache trim." },
     { (uintptr_t)&MCUCTRL_CPUDCACHETRIM,   "MCUCTRL->CPUDCACHETRIM",   "CPU Dcache trim." },
     { (uintptr_t)&MCUCTRL_SSRAMTRIM,       "MCUCTRL->SSRAMTRIM",       "SSRAM trim." },
     { (uintptr_t)&MCUCTRL->AUDADCPWRCTRL,   "MCUCTRL->AUDADCPWRCTRL",   "Audio ADC power ctrl." },
     { (uintptr_t)&MCUCTRL->AUDIO1,          "MCUCTRL->AUDIO1",          "Audio config register 1." },
     { (uintptr_t)&MCUCTRL->PGAADCIFCTRL,    "MCUCTRL->PGAADCIFCTRL",    "PGA ADC IF ctrl." },
     { (uintptr_t)&MCUCTRL->PGACTRL1,        "MCUCTRL->PGACTRL1",        "PGA control1." },
     { (uintptr_t)&MCUCTRL->PGACTRL2,        "MCUCTRL->PGACTRL2",        "PGA control2." },
     { (uintptr_t)&MCUCTRL->AUDADCPWRDLY,    "MCUCTRL->AUDADCPWRDLY",    "Audio ADC power delay." },
     { (uintptr_t)&MCUCTRL->SDIO0CTRL,       "MCUCTRL->SDIO0CTRL",       "SDIO 0 control." },
     { (uintptr_t)&MCUCTRL->SDIO1CTRL,       "MCUCTRL->SDIO1CTRL",       "SDIO 1 control." },
     { (uintptr_t)&MCUCTRL->PDMCTRL,         "MCUCTRL->PDMCTRL",         "PDM control." },
     { (uintptr_t)&MCUCTRL->MMSMISCCTRL,     "MCUCTRL->MMSMISCCTRL",     "MMS misc control." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT4,     "MCUCTRL->FLASHWPROT4",     "Flash write protect 4." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT5,     "MCUCTRL->FLASHWPROT5",     "Flash write protect 5." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT6,     "MCUCTRL->FLASHWPROT6",     "Flash write protect 6." },
     { (uintptr_t)&MCUCTRL->FLASHWPROT7,     "MCUCTRL->FLASHWPROT7",     "Flash write protect 7." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT4,     "MCUCTRL->FLASHRPROT4",     "Flash read protect 4." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT5,     "MCUCTRL->FLASHRPROT5",     "Flash read protect 5." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT6,     "MCUCTRL->FLASHRPROT6",     "Flash read protect 6." },
     { (uintptr_t)&MCUCTRL->FLASHRPROT7,     "MCUCTRL->FLASHRPROT7",     "Flash read protect 7." },
     { (uintptr_t)&MCUCTRL->PWRSW2,          "MCUCTRL->PWRSW2",          "Power switch 2." },
     { (uintptr_t)&MCUCTRL->CPUCFG,          "MCUCTRL->CPUCFG",          "CPU config." },
     { (uintptr_t)&MCUCTRL->PLLCTL0,         "MCUCTRL->PLLCTL0",         "PLL ctrl0." },
     { (uintptr_t)&MCUCTRL->PLLDIV0,         "MCUCTRL->PLLDIV0",         "PLL div0." },
     { (uintptr_t)&MCUCTRL->PLLDIV1,         "MCUCTRL->PLLDIV1",         "PLL div1." },
     { (uintptr_t)&MCUCTRL->PLLSTAT,         "MCUCTRL->PLLSTAT",         "PLL status." },
     { (uintptr_t)&MCUCTRL->PLLMUXCTL,       "MCUCTRL->PLLMUXCTL",       "PLL mux ctrl." },
 
     // CLKGEN
     { (uintptr_t)&CLKGEN->OCTRL,            "CLKGEN->OCTRL",            "Oscillator control." },
     { (uintptr_t)&CLKGEN->CLKOUT,           "CLKGEN->CLKOUT",           "Clock output config." },
     { (uintptr_t)&CLKGEN->HFADJ,            "CLKGEN->HFADJ",            "HFRC adjust." },
     { (uintptr_t)&CLKGEN->CLOCKENSTAT,      "CLKGEN->CLOCKENSTAT",      "Clock enable status." },
     { (uintptr_t)&CLKGEN->CLOCKEN2STAT,     "CLKGEN->CLOCKEN2STAT",     "Clock enable2 status." },
     { (uintptr_t)&CLKGEN->CLOCKEN3STAT,     "CLKGEN->CLOCKEN3STAT",     "Clock enable3 status." },
     { (uintptr_t)&CLKGEN->MISC,             "CLKGEN->MISC",             "Misc clockgen register." },
     { (uintptr_t)&CLKGEN->HF2ADJ0,          "CLKGEN->HF2ADJ0",          "HF2 adjust0." },
     { (uintptr_t)&CLKGEN->HF2ADJ1,          "CLKGEN->HF2ADJ1",          "HF2 adjust1." },
     { (uintptr_t)&CLKGEN->HF2ADJ2,          "CLKGEN->HF2ADJ2",          "HF2 adjust2." },
     { (uintptr_t)&CLKGEN->HF2VAL,           "CLKGEN->HF2VAL",           "HF2 value." },
     { (uintptr_t)&CLKGEN->LFRCCTRL,         "CLKGEN->LFRCCTRL",         "LFRC control." },
     { (uintptr_t)&CLKGEN->DISPCLKCTRL,      "CLKGEN->DISPCLKCTRL",      "Display clock ctrl." },
     { (uintptr_t)&CLKGEN->CLKGENSPARES,     "CLKGEN->CLKGENSPARES",     "Clockgen spares." },
     { (uintptr_t)&CLKGEN->HFRCIDLECOUNTERS, "CLKGEN->HFRCIDLECOUNTERS", "HFRC idle counters." },
     { (uintptr_t)&CLKGEN->MSPIIOCLKCTRL,    "CLKGEN->MSPIIOCLKCTRL",    "MSPI IO clock ctrl." },
     { (uintptr_t)&CLKGEN->CLKCTRL,          "CLKGEN->CLKCTRL",          "Clock control." },
 
     // STIMER
     { (uintptr_t)&STIMER->STCFG,            "STIMER->STCFG",            "System timer config." },
     { (uintptr_t)&STIMER->STMINTSTAT,       "STIMER->STMINTSTAT",       "System timer interrupt status." },
 
     // TIMER
     { (uintptr_t)&TIMER->CTRL,              "TIMER->CTRL",              "Timer control." },
     { (uintptr_t)&TIMER->STATUS,            "TIMER->STATUS",            "Timer status." },
     { (uintptr_t)&TIMER->GLOBEN,            "TIMER->GLOBEN",            "Timer global enable." },
     { (uintptr_t)&TIMER->INTSTAT,           "TIMER->INTSTAT",           "Timer interrupt status." },
     { (uintptr_t)&TIMER->CTRL0,             "TIMER->CTRL0",             "Timer channel0 ctrl." },
     { (uintptr_t)&TIMER->CTRL1,             "TIMER->CTRL1",             "Timer channel1 ctrl." },
     { (uintptr_t)&TIMER->CTRL2,             "TIMER->CTRL2",             "Timer channel2 ctrl." },
     { (uintptr_t)&TIMER->CTRL3,             "TIMER->CTRL3",             "Timer channel3 ctrl." },
     { (uintptr_t)&TIMER->CTRL4,             "TIMER->CTRL4",             "Timer channel4 ctrl." },
     { (uintptr_t)&TIMER->CTRL5,             "TIMER->CTRL5",             "Timer channel5 ctrl." },
     { (uintptr_t)&TIMER->CTRL6,             "TIMER->CTRL6",             "Timer channel6 ctrl." },
     { (uintptr_t)&TIMER->CTRL7,             "TIMER->CTRL7",             "Timer channel7 ctrl." },
     { (uintptr_t)&TIMER->CTRL8,             "TIMER->CTRL8",             "Timer channel8 ctrl." },
     { (uintptr_t)&TIMER->CTRL9,             "TIMER->CTRL9",             "Timer channel9 ctrl." },
     { (uintptr_t)&TIMER->CTRL10,            "TIMER->CTRL10",            "Timer channel10 ctrl." },
     { (uintptr_t)&TIMER->CTRL11,            "TIMER->CTRL11",            "Timer channel11 ctrl." },
     { (uintptr_t)&TIMER->CTRL12,            "TIMER->CTRL12",            "Timer channel12 ctrl." },
     { (uintptr_t)&TIMER->CTRL13,            "TIMER->CTRL13",            "Timer channel13 ctrl." },
     { (uintptr_t)&TIMER->CTRL14,            "TIMER->CTRL14",            "Timer channel14 ctrl." },
     { (uintptr_t)&TIMER->CTRL15,            "TIMER->CTRL15",            "Timer channel15 ctrl." },
 
     // AUDADC
     /*
     { (uintptr_t)&AUDADC->CFG,              "AUDADC->CFG",              "Audio ADC config." },
     { (uintptr_t)&AUDADC->STAT,             "AUDADC->STAT",             "Audio ADC status." },
     { (uintptr_t)&AUDADC->SWT,              "AUDADC->SWT",              "Software trigger." },
     { (uintptr_t)&AUDADC->SL0CFG,           "AUDADC->SL0CFG",           "Slot0 config." },
     { (uintptr_t)&AUDADC->SL1CFG,           "AUDADC->SL1CFG",           "Slot1 config." },
     { (uintptr_t)&AUDADC->SL2CFG,           "AUDADC->SL2CFG",           "Slot2 config." },
     { (uintptr_t)&AUDADC->SL3CFG,           "AUDADC->SL3CFG",           "Slot3 config." },
     { (uintptr_t)&AUDADC->SL4CFG,           "AUDADC->SL4CFG",           "Slot4 config." },
     { (uintptr_t)&AUDADC->SL5CFG,           "AUDADC->SL5CFG",           "Slot5 config." },
     { (uintptr_t)&AUDADC->SL6CFG,           "AUDADC->SL6CFG",           "Slot6 config." },
     { (uintptr_t)&AUDADC->SL7CFG,           "AUDADC->SL7CFG",           "Slot7 config." },
     { (uintptr_t)&AUDADC->WULIM,            "AUDADC->WULIM",            "Window upper limit." },
     { (uintptr_t)&AUDADC->WLLIM,            "AUDADC->WLLIM",            "Window lower limit." },
     { (uintptr_t)&AUDADC->SCWLIM,           "AUDADC->SCWLIM",           "Short-circuit window limit." },
     { (uintptr_t)&AUDADC->FIFO,             "AUDADC->FIFO",             "ADC FIFO data." },
     { (uintptr_t)&AUDADC->FIFOPR,           "AUDADC->FIFOPR",           "ADC FIFO peek." },
     { (uintptr_t)&AUDADC->FIFOSTAT,         "AUDADC->FIFOSTAT",         "ADC FIFO status." },
     { (uintptr_t)&AUDADC->DATAOFFSET,       "AUDADC->DATAOFFSET",       "Data offset correction." },
     { (uintptr_t)&AUDADC->ZXCFG,            "AUDADC->ZXCFG",            "Zero crossing config." },
     { (uintptr_t)&AUDADC->ZXLIM,            "AUDADC->ZXLIM",            "Zero crossing limit." },
     { (uintptr_t)&AUDADC->GAINCFG,          "AUDADC->GAINCFG",          "Gain config." },
     { (uintptr_t)&AUDADC->GAIN,             "AUDADC->GAIN",             "Gain register." },
     { (uintptr_t)&AUDADC->SATCFG,           "AUDADC->SATCFG",           "Saturation config." },
     { (uintptr_t)&AUDADC->SATLIM,           "AUDADC->SATLIM",           "Saturation limit." },
     { (uintptr_t)&AUDADC->SATMAX,           "AUDADC->SATMAX",           "Saturation max." },
     { (uintptr_t)&AUDADC->SATCLR,           "AUDADC->SATCLR",           "Saturation clear." },
     { (uintptr_t)&AUDADC->INTEN,            "AUDADC->INTEN",            "ADC Interrupt enable." },
     { (uintptr_t)&AUDADC->INTSTAT,          "AUDADC->INTSTAT",          "ADC Interrupt status." },
     { (uintptr_t)&AUDADC->INTCLR,           "AUDADC->INTCLR",           "ADC Interrupt clear." },
     { (uintptr_t)&AUDADC->INTSET,           "AUDADC->INTSET",           "ADC Interrupt set." },
     { (uintptr_t)&AUDADC->DMATRIGEN,        "AUDADC->DMATRIGEN",        "ADC DMA trigger enable." },
     { (uintptr_t)&AUDADC->DMATRIGSTAT,      "AUDADC->DMATRIGSTAT",      "ADC DMA trigger status." },
     { (uintptr_t)&AUDADC->DMACFG,           "AUDADC->DMACFG",           "ADC DMA config." },
     { (uintptr_t)&AUDADC->DMATOTCOUNT,      "AUDADC->DMATOTCOUNT",      "ADC DMA total count." },
     { (uintptr_t)&AUDADC->DMATARGADDR,      "AUDADC->DMATARGADDR",      "ADC DMA target addr." },
     { (uintptr_t)&AUDADC->DMASTAT,          "AUDADC->DMASTAT",          "ADC DMA status." },
 
     // PDM (example for PDMn(0))
     { (uintptr_t)&(PDMn(0)->CTRL),              "PDMn(0)->CTRL",              "PDM ctrl." },
     { (uintptr_t)&(PDMn(0)->CORECFG0),          "PDMn(0)->CORECFG0",          "PDM core cfg0." },
     { (uintptr_t)&(PDMn(0)->CORECFG1),          "PDMn(0)->CORECFG1",          "PDM core cfg1." },
     { (uintptr_t)&(PDMn(0)->CORECTRL),          "PDMn(0)->CORECTRL",          "PDM core ctrl." },
     { (uintptr_t)&(PDMn(0)->FIFOCNT),           "PDMn(0)->FIFOCNT",           "PDM FIFO count." },
     { (uintptr_t)&(PDMn(0)->FIFOREAD),          "PDMn(0)->FIFOREAD",          "PDM FIFO read." },
     { (uintptr_t)&(PDMn(0)->FIFOFLUSH),         "PDMn(0)->FIFOFLUSH",         "PDM FIFO flush." },
     { (uintptr_t)&(PDMn(0)->FIFOTHR),           "PDMn(0)->FIFOTHR",           "PDM FIFO threshold." },
     { (uintptr_t)&(PDMn(0)->INTEN),             "PDMn(0)->INTEN",             "PDM interrupt enable." },
     { (uintptr_t)&(PDMn(0)->INTSTAT),           "PDMn(0)->INTSTAT",           "PDM interrupt status." },
     { (uintptr_t)&(PDMn(0)->INTCLR),            "PDMn(0)->INTCLR",            "PDM interrupt clear." },
     { (uintptr_t)&(PDMn(0)->INTSET),            "PDMn(0)->INTSET",            "PDM interrupt set." },
     { (uintptr_t)&(PDMn(0)->DMATRIGEN),         "PDMn(0)->DMATRIGEN",         "PDM DMA trigger enable." },
     { (uintptr_t)&(PDMn(0)->DMATRIGSTAT),       "PDMn(0)->DMATRIGSTAT",       "PDM DMA trigger status." },
     { (uintptr_t)&(PDMn(0)->DMACFG),            "PDMn(0)->DMACFG",            "PDM DMA config." },
     { (uintptr_t)&(PDMn(0)->DMATARGADDR),       "PDMn(0)->DMATARGADDR",       "PDM DMA target addr." },
     { (uintptr_t)&(PDMn(0)->DMASTAT),           "PDMn(0)->DMASTAT",           "PDM DMA status." },
     { (uintptr_t)&(PDMn(0)->DMATARGADDRNEXT),   "PDMn(0)->DMATARGADDRNEXT",   "PDM DMA target addr next." },
     { (uintptr_t)&(PDMn(0)->DMATOTCOUNTNEXT),   "PDMn(0)->DMATOTCOUNTNEXT",   "PDM DMA total count next." },
     { (uintptr_t)&(PDMn(0)->DMAENNEXTCTRL),     "PDMn(0)->DMAENNEXTCTRL",     "PDM DMA enable next ctrl." },
     { (uintptr_t)&(PDMn(0)->DMATOTCOUNT),       "PDMn(0)->DMATOTCOUNT",       "PDM DMA total count." },
      */
 };
 
 // Determine how many registers are in our map:
 #define REGMAP_SIZE  (sizeof(g_regmap)/sizeof(g_regmap[0]))
 
 // We will store up to 5 snapshots.  Each snapshot is simply an array of 
 // the register values read at the time of capture.
 static uint32_t g_snapshots[5][REGMAP_SIZE];
 
 // A simple boolean to track which snapshot slots are valid (captured).
 static bool     g_snapshotValid[5] = {false, false, false, false, false};
 
 /******************************************************************************
  * capture_snapshot
  *
  * Captures the current values of all registers-of-interest into snapshotIndex.
  * snapshotIndex must be in [0..4].
  ******************************************************************************/
 void capture_snapshot(int snapshotIndex)
 {
     if (snapshotIndex < 0 || snapshotIndex >= 5)
     {
         // Out-of-bounds check; handle error as desired.
         printf("ERROR: capture_snapshot called with invalid snapshot index %d\n", snapshotIndex);
         return;
     }
 
     // Iterate over each register in our table and read it
     for (size_t i = 0; i < REGMAP_SIZE; i++)
     {
         // Use volatile read from the address:
         volatile uint32_t *regPtr = (volatile uint32_t *) (g_regmap[i].addr);
         // Print out the register name and value
            // ns_lp_printf("Capturing %d %s: 0x%08lX\n", i, g_regmap[i].regname, *regPtr);
         g_snapshots[snapshotIndex][i] = *regPtr;
     }
 
     g_snapshotValid[snapshotIndex] = true;
 }
 
 /******************************************************************************
  * print_snapshot
  *
  * Prints the contents of the specified snapshot either in JSON or in CSV.
  * snapshotIndex must be in [0..4].
  * If bJson == true, output JSON style. Otherwise, CSV style.
  ******************************************************************************/
 void print_snapshot(int snapshotIndex, bool bJson)
 {
     if (snapshotIndex < 0 || snapshotIndex >= 5)
     {
      ns_lp_printf("ERROR: print_snapshot called with invalid snapshot index %d\n", snapshotIndex);
         return;
     }
     if (!g_snapshotValid[snapshotIndex])
     {
      ns_lp_printf("Snapshot %d is not valid (never captured).\n", snapshotIndex);
         return;
     }
 
     if (bJson)
     {
         // Print as JSON
         // For a large number of registers, we typically start with '{' or '['
         // and carefully format each entry.  Below is a simple demonstration:
         ns_lp_printf("{\n");
         ns_lp_printf("  \"snapshotIndex\": %d,\n", snapshotIndex);
         ns_lp_printf("  \"registers\": [\n");
         for (size_t i = 0; i < REGMAP_SIZE; i++)
         {
             // For cleanliness, add a comma only if not the last item
             const char *comma = (i < (REGMAP_SIZE - 1)) ? "," : "";
             ns_lp_printf("    {\n");
             ns_lp_printf("      \"regname\": \"%s\",\n", g_regmap[i].regname);
             ns_lp_printf("      \"value\": \"%08X\",\n", g_snapshots[snapshotIndex][i]);
             ns_lp_printf("      \"description\": \"%s\"\n", g_regmap[i].description);
             ns_lp_printf("    }%s\n", comma);
         }
         ns_lp_printf("  ]\n");
         ns_lp_printf("}\n");
     }
     else
     {
        // Print as CSV
        // There is one row per register. Columns are regname, then one value per snapshot.
        // Print a header row: regname, snapshot0, snapshot1, ..., reg description
        // Only valid snapshots are printed.
        ns_lp_printf("REGNAME");
        for (int i = 0; i < 5; i++)
        {
            if (g_snapshotValid[i])
            {
                ns_lp_printf(",SNAPSHOT%d", i);
            }
        }
        ns_lp_printf(",DESCRIPTION\n");

        // Print the contents for each row
        for (size_t i = 0; i < REGMAP_SIZE; i++)
        {
            ns_lp_printf("%s", g_regmap[i].regname);
            ns_delay_us(1000); 
            for (int j = 0; j < 5; j++)
            {
                if (g_snapshotValid[j])
                {
                    ns_lp_printf(",0x%08lX", g_snapshots[j][i]);
                    ns_delay_us(1000); 

                }
            }
            ns_lp_printf(",%s\n", g_regmap[i].description);
            ns_delay_us(1000); 

        }


        // // For CSV, we might do one line per register, with e.g.:
        // //   snapshotIndex, regname, 0xVALUE, description
        // ns_lp_printf("snapshot_index,regname,value,description\n");
        // for (size_t i = 0; i < REGMAP_SIZE; i++)
        // {
        // ns_lp_printf("%d,\"%s\",0x%08X,\"%s\"\n",
        //     snapshotIndex,
        //     g_regmap[i].regname,
        //     g_snapshots[snapshotIndex][i],
        //     g_regmap[i].description);
        // }
     }
 }
 
/* Mostly generated by ChatGPT
Prompt: 
am_util_pp.c defines a function called am_util_pp_snapshot which reads a series of ‘mcu registers of interest’, captures them in data structures, and prints them out the next time it is invoked. The description of each register can be found as comments in apollo5b.h.

Create a C program that defines a const map of each register-of-interest’s address (PWCTRL->MCUPERFREQ is the address of the register), a short description of the register (a string containing the register name, can be “PWRCTRL->MCUPERFREQ” in this example), and a string containing a long string based on the comments in apollo5b.h. In C, the address of the register is as described above - no need to #define something new, just use the address directly.

The map should be an array of a struct such as:

typedef struct  {
    uint32_t addr;
    const char regname[50];
    const char description[120];
} regmap_t;

An example entry in the map would be something like:
const regmap_t regmap[] = {
{PWRCTRL->MCUPERFREQ, “PWRCTRL->MCUPERFREQ”, “This register provides the performance mode knobs for MCU.”},
etc…
}

Create two functions: one that captures a snapshot of all the registers and places them in a snapshot buffer (there can be up to 5 snapshots), and another that prints out either a JSON or CSV representation of the snapshots.

Implement the code fully, not example code, and include all registers-of-interest

*/
 