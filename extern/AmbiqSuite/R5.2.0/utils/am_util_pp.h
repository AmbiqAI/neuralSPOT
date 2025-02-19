//*****************************************************************************
//
//
//! @file am_bsp_pp.h
//!
//! @brief Functions to aid power profiling and debugging
//!
//! @addtogroup BSP Board Support Package (BSP), release date: TBD
//! @addtogroup pp Power Profiling Functionality
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

#ifndef AM_BSP_PP_H
#define AM_BSP_PP_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp_pins.h"

//These two files are for internal debugging only
#include "am_devices_led.h"
#include "am_devices_button.h"


#ifdef __cplusplus
extern "C"
{
#endif

//This debug flag is for pre-release debugging only
#define PP_DEBUG          1
#define MAX_STORAGE       1

/*****************************************************************************
5 major power control blocks
1.  PWRCTRL   P_
2.  MCU_CTRL  M_
3.  CLK_GEN   C_
4.  STIMER (system timer) ST_
5.  TIMER     T_
6.      VOS       V_
7.      Other     O_
****************************************************************************/

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

//Define the Power profiling data structure block by block
typedef struct
{
    bool     bSingle;   //single shot debug on/off
    uint32_t uSnapShot;
    uint32_t P_MCUPERFREQ;
    uint32_t P_DEVPWREN;
    uint32_t P_DEVPWRSTATUS;
    uint32_t P_AUDSSPWREN;
    uint32_t P_AUDSSPWRSTATUS;
    uint32_t P_MEMPWREN;
    uint32_t P_MEMPWRSTATUS;
    uint32_t P_MEMRETCFG;
    uint32_t P_SYSPWRSTATUS;
    uint32_t P_SSRAMPWREN;
    uint32_t P_SSRAMPWRST;
    uint32_t P_SSRAMRETCFG;
    uint32_t P_DEVPWREVENTEN;
    uint32_t P_MEMPWREVENTEN;
    uint32_t P_MMSOVERRIDE;
    uint32_t P_CPUPWRCTRL;
    uint32_t P_PWRCTRLMODESTATUS;
    uint32_t P_CPUPWRSTATUS;
    uint32_t P_PWRACKOVR;
    uint32_t P_PWRCNTDEFVAL;
    uint32_t P_GFXPERFREQ;
    uint32_t P_GFXPWRSWSEL;
    uint32_t P_EPURETCFG;
    uint32_t P_VRCTRL;
    uint32_t P_LEGACYVRLPOVR;
    uint32_t P_VRSTATUS;
    uint32_t P_SRAMCTRL;
    uint32_t P_ADCSTATUS;
    uint32_t P_AUDADCSTATUS;
    uint32_t P_TONCNTRCTRL;
    uint32_t P_LPOVRTHRESHVDDS;
    uint32_t P_LPOVRHYSTCNT;
    uint32_t P_LPOVRTHRESHVDDF;
    uint32_t P_LPOVRTHRESHVDDC;
    uint32_t P_LPOVRTHRESHVDDCLV;
    uint32_t P_LPOVRSTAT;
    uint32_t P_MRAMEXTCTRL;
    uint32_t P_EMONCTRL;
    uint32_t P_EMONCFG0;
    uint32_t P_EMONCFG1;
    uint32_t P_EMONCFG2;
    uint32_t P_EMONCFG3;
    uint32_t P_EMONCFG4;
    uint32_t P_EMONCFG5;
    uint32_t P_EMONCFG6;
    uint32_t P_EMONCFG7;
    uint32_t P_EMONCOUNT0;
    uint32_t P_EMONCOUNT1;
    uint32_t P_EMONCOUNT2;
    uint32_t P_EMONCOUNT3;
    uint32_t P_EMONCOUNT4;
    uint32_t P_EMONCOUNT5;
    uint32_t P_EMONCOUNT6;
    uint32_t P_EMONCOUNT7;
    uint32_t P_EMONSTATUS;
    uint32_t P_FPIOEN0;
    uint32_t P_FPIOEN1;
    uint32_t P_FPIOEN2;
}
am_util_pp_b1_t;

//Define the Power profiling data structure block by block
//Block 2: MCU Ctrl
typedef struct
{
    uint32_t M_CHIPPN;
    uint32_t M_CHIPID0;
    uint32_t M_CHIPID1;
    uint32_t M_CHIPREV;
    uint32_t M_VENDORID;
    uint32_t M_SKU;
    uint32_t M_SKUOVERRIDE;
    uint32_t M_DEBUGGER;
    uint32_t M_ACRG;
    uint32_t M_VREFGEN;
    uint32_t M_VREFGEN2;
    uint32_t M_VREFGEN3;
    uint32_t M_VREFGEN4;
    uint32_t M_VREFGEN5;
    uint32_t M_VREFBUF;
    uint32_t M_VRCTRL;
    uint32_t M_LDOREG1;
    uint32_t M_LDOREG2;
    uint32_t M_HFRC;
    uint32_t M_HFRC2;
    uint32_t M_LFRC;
    uint32_t M_BODCTRL;
    uint32_t M_ADCPWRCTRL;
    uint32_t M_ADCCAL;
    uint32_t M_ADCBATTLOAD;
    uint32_t M_XTALCTRL;
    uint32_t M_XTALGENCTRL;
    uint32_t M_XTALHSTRIMS;
    uint32_t M_XTALHSCTRL;
    uint32_t M_BGTLPCTRL;
    uint32_t M_MRAMCRYPTOPWRCTRL;
    uint32_t M_MISCPWRCTRL;
    uint32_t M_BODISABLE;
    uint32_t M_D2ASPARE;
    uint32_t M_BOOTLOADER;
    uint32_t M_SHADOWVALID;
    uint32_t M_SCRATCH0;
    uint32_t M_SCRATCH1;
    uint32_t M_DBGR1;
    uint32_t M_DBGR2;
    uint32_t M_WICCONTROL;
    uint32_t M_DBGCTRL;
    uint32_t M_OTAPOINTER;
    uint32_t M_APBDMACTRL;
    uint32_t M_FORCEAXICLKEN;
    uint32_t M_SRAMTRIMHP;
    uint32_t M_SRAMTRIM;
    uint32_t M_KEXTCLKSEL;
    uint32_t M_SIMOBUCK0;
    uint32_t M_SIMOBUCK1;
    uint32_t M_SIMOBUCK2;
    uint32_t M_SIMOBUCK3;
    uint32_t M_SIMOBUCK4;
    uint32_t M_SIMOBUCK5;
    uint32_t M_SIMOBUCK6;
    uint32_t M_SIMOBUCK7;
    uint32_t M_SIMOBUCK8;
    uint32_t M_SIMOBUCK9;
    uint32_t M_SIMOBUCK10;
    uint32_t M_SIMOBUCK11;
    uint32_t M_SIMOBUCK12;
    uint32_t M_SIMOBUCK13;
    uint32_t M_SIMOBUCK14;
    uint32_t M_SIMOBUCK15;
    uint32_t M_PWRSW0;
    uint32_t M_PWRSW1;
    uint32_t M_USBRSTCTRL;
    uint32_t M_FLASHWPROT0;
    uint32_t M_FLASHWPROT1;
    uint32_t M_FLASHWPROT2;
    uint32_t M_FLASHWPROT3;
    uint32_t M_FLASHRPROT0;
    uint32_t M_FLASHRPROT1;
    uint32_t M_FLASHRPROT2;
    uint32_t M_FLASHRPROT3;
    uint32_t M_SRAMWPROT0;
    uint32_t M_SRAMWPROT1;
    uint32_t M_SRAMWPROT2;
    uint32_t M_SRAMWPROT3;
    uint32_t M_SRAMWPROT4;
    uint32_t M_SRAMWPROT5;
    uint32_t M_SRAMRPROT0;
    uint32_t M_SRAMRPROT1;
    uint32_t M_SRAMRPROT2;
    uint32_t M_SRAMRPROT3;
    uint32_t M_SRAMRPROT4;
    uint32_t M_SRAMRPROT5;
    uint32_t M_CPUICACHETRIM;
    uint32_t M_CPUDCACHETRIM;
    uint32_t M_SSRAMTRIM;
    uint32_t M_AUDADCPWRCTRL;
    uint32_t M_AUDIO1;
    uint32_t M_PGAADCIFCTRL;
    uint32_t M_PGACTRL1;
    uint32_t M_PGACTRL2;
    uint32_t M_AUDADCPWRDLY;
    uint32_t M_SDIO0CTRL;
    uint32_t M_SDIO1CTRL;
    uint32_t M_PDMCTRL;
    uint32_t M_MMSMISCCTRL;
    uint32_t M_FLASHWPROT4;
    uint32_t M_FLASHWPROT5;
    uint32_t M_FLASHWPROT6;
    uint32_t M_FLASHWPROT7;
    uint32_t M_FLASHRPROT4;
    uint32_t M_FLASHRPROT5;
    uint32_t M_FLASHRPROT6;
    uint32_t M_FLASHRPROT7;
    uint32_t M_PWRSW2;
    uint32_t M_CPUCFG;
    uint32_t M_PLLCTL0;
    uint32_t M_PLLDIV0;
    uint32_t M_PLLDIV1;
    uint32_t M_PLLSTAT;
    uint32_t M_PLLMUXCTL;
}
am_util_pp_b2_t;

//Define the Power profiling data structure block by block
//Block 3: CLK_GEN ST and Timer
typedef struct
{
    uint32_t C_OCTRL;
    uint32_t C_CLKOUT;
    uint32_t C_STATUS;
    uint32_t C_HFADJ;
    uint32_t C_CLOCKENSTAT;
    uint32_t C_CLOCKEN2STAT;
    uint32_t C_CLOCKEN3STAT;
    uint32_t C_MISC;
    uint32_t C_HF2ADJ0;
    uint32_t C_HF2ADJ1;
    uint32_t C_HF2ADJ2;
    uint32_t C_HF2VAL;
    uint32_t C_LFRCCTRL;
    uint32_t C_DISPCLKCTRL;
    uint32_t C_CLKGENSPARES;
    uint32_t C_HFRCIDLECOUNTERS;
    uint32_t C_MSPIIOCLKCTRL;
    uint32_t C_CLKCTRL;

    uint32_t ST_STCFG;
    uint32_t ST_STMINTSTAT;

    uint32_t T_CTRL;
    uint32_t T_STATUS;
    uint32_t T_GLOBEN;
    uint32_t T_INTSTAT;
    uint32_t T_CTRL0;
    uint32_t T_CTRL1;
    uint32_t T_CTRL2;
    uint32_t T_CTRL3;
    uint32_t T_CTRL4;
    uint32_t T_CTRL5;
    uint32_t T_CTRL6;
    uint32_t T_CTRL7;
    uint32_t T_CTRL8;
    uint32_t T_CTRL9;
    uint32_t T_CTRL10;
    uint32_t T_CTRL11;
    uint32_t T_CTRL12;
    uint32_t T_CTRL13;
    uint32_t T_CTRL14;
    uint32_t T_CTRL15;

}
am_util_pp_b3_t;

//Block 4: Application specific registers, AUDADC
typedef struct
{
  uint32_t AU_CFG;
  uint32_t AU_STAT;
  uint32_t AU_SWT;
  uint32_t AU_SL0CFG;
  uint32_t AU_SL1CFG;
  uint32_t AU_SL2CFG;
  uint32_t AU_SL3CFG;
  uint32_t AU_SL4CFG;
  uint32_t AU_SL5CFG;
  uint32_t AU_SL6CFG;
  uint32_t AU_SL7CFG;
  uint32_t AU_WULIM;
  uint32_t AU_WLLIM;
  uint32_t AU_SCWLIM;
  uint32_t AU_FIFO;
  uint32_t AU_FIFOPR;
  uint32_t AU_FIFOSTAT;
  uint32_t AU_DATAOFFSET;
  uint32_t AU_ZXCFG;
  uint32_t AU_ZXLIM;
  uint32_t AU_GAINCFG;
  uint32_t AU_GAIN;
  uint32_t AU_SATCFG;
  uint32_t AU_SATLIM;
  uint32_t AU_SATMAX;
  uint32_t AU_SATCLR;
  uint32_t AU_IEREN;
  uint32_t AU_IERSTAT;
  uint32_t AU_IERCLR;
  uint32_t AU_IERSET;
  uint32_t AU_DMATRIGEN;
  uint32_t AU_DMATRIGSTAT;
  uint32_t AU_DMACFG;
  uint32_t AU_DMATOTCOUNT;
  uint32_t AU_DMATARGADDR;
  uint32_t AU_DMASTAT;
}
am_util_pp_b4_t;

//Block 5: Application specific registers, PDM
typedef struct
{
  uint32_t PDM_CTRL;
  uint32_t PDM_CORECFG0;
  uint32_t PDM_CORECFG1;
  uint32_t PDM_CORECTRL;
  uint32_t PDM_FIFOCNT;
  uint32_t PDM_FIFOREAD;
  uint32_t PDM_FIFOFLUSH;
  uint32_t PDM_FIFOTHR;
  uint32_t PDM_INTEN;
  uint32_t PDM_INTSTAT;
  uint32_t PDM_INTCLR;
  uint32_t PDM_INTSET;
  uint32_t PDM_DMATRIGEN;
  uint32_t PDM_DMATRIGSTAT;
  uint32_t PDM_DMACFG;
  uint32_t PDM_DMATARGADDR;
  uint32_t PDM_DMASTAT;
  uint32_t PDM_DMATARGADDRNEXT;
  uint32_t PDM_DMATOTCOUNTNEXT;
  uint32_t PDM_DMAENNEXTCTRL;
  uint32_t PDM_DMATOTCOUNT;
}
am_util_pp_b5_t;


void am_util_print_JSON(uint8_t uNumber);

//*****************************************************************************
//
// Main APIs
// Parameters--
// bDebug: a flag to indicate it's a debug or not
// uNumber: snapshot number
// bUseMemory: whether or not to store the JSON to memory
//*****************************************************************************
void am_util_pp_snapshot(bool bDebug, uint32_t uNumber, bool bStreamNow);

#ifdef __cplusplus
}
#endif

#endif // AM_BSP_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
