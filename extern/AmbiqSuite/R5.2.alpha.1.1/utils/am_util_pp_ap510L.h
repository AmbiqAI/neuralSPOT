//*****************************************************************************
//
//! @file am_util_pp.h
//!
//! @brief Functions to aid power profiling and debugging
//!
//! @addtogroup ppf Power Profiling Functionality
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
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
6.  SYSCTRL   SC_
7.  PDM       PDM_
****************************************************************************/

#define MCUCTRL_HFRC            (*(volatile uint32_t*)0x4000A8C0)
#define MCUCTRL_VREFGEN         (*(volatile uint32_t*)0x4000A840)
#define MCUCTRL_VREFGEN3        (*(volatile uint32_t*)0x4000A848)
#define MCUCTRL_VREFGEN5        (*(volatile uint32_t*)0x4000A850)
#define MCUCTRL_MISCPWRCTRL     (*(volatile uint32_t*)0x4000A998)
#define MCUCTRL_SRAMTRIMHP      (*(volatile uint32_t*)0x4000AB30)
#define MCUCTRL_SRAMTRIM        (*(volatile uint32_t*)0x4000AB34)
#define MCUCTRL_CPUICACHETRIM   (*(volatile uint32_t*)0x4000AC24)
#define MCUCTRL_CPUDCACHETRIM   (*(volatile uint32_t*)0x4000AC28)
#define MCUCTRL_SSRAMTRIM       (*(volatile uint32_t*)0x4000AC2C)

#define SYSCTRL_ICTR            (*(volatile uint32_t*)0xE000E004)
#define SYSCTRL_ACTLR           (*(volatile uint32_t*)0xE000E008)
#define SYSCTRL_ICSR            (*(volatile uint32_t*)0xE000ED04)
#define SYSCTRL_VTOR            (*(volatile uint32_t*)0xE000ED08)
#define SYSCTRL_AIRCR           (*(volatile uint32_t*)0xE000ED0C)
#define SYSCTRL_SCR             (*(volatile uint32_t*)0xE000ED10)
#define SYSCTRL_CCR             (*(volatile uint32_t*)0xE000ED14)
#define SYSCTRL_SHPR1           (*(volatile uint32_t*)0xE000ED18)
#define SYSCTRL_SHPR2           (*(volatile uint32_t*)0xE000ED1C)
#define SYSCTRL_SHPR3           (*(volatile uint32_t*)0xE000ED20)
#define SYSCTRL_SHCSR           (*(volatile uint32_t*)0xE000ED24)
#define SYSCTRL_CFSR            (*(volatile uint32_t*)0xE000ED28)
#define SYSCTRL_HFSR            (*(volatile uint32_t*)0xE000ED2C)
#define SYSCTRL_MMFAR           (*(volatile uint32_t*)0xE000ED34)
#define SYSCTRL_BFAR            (*(volatile uint32_t*)0xE000ED38)
#define SYSCTRL_CPACR           (*(volatile uint32_t*)0xE000ED88)
#define SYSCTRL_DEMCR           (*(volatile uint32_t*)0xE000EDFC)
#define SYSCTRL_STIR            (*(volatile uint32_t*)0xE000EF00)
#define SYSCTRL_FPCCR           (*(volatile uint32_t*)0xE000EF34)
#define SYSCTRL_FPCAR           (*(volatile uint32_t*)0xE000EF38)
#define SYSCTRL_FPDSCR          (*(volatile uint32_t*)0xE000EF3C)

//Define the Power profiling data structure block by block
//Block 1: Power Control (P_)
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
    uint32_t P_CPUPWRSTATUS;
    uint32_t P_PWRACKOVR;
    uint32_t P_PWRCNTDEFVAL;
    uint32_t P_EPURETCFG;
    uint32_t P_CM4PWRCTRL;
    uint32_t P_CM4PWRSTATE;
    uint32_t P_VRCTRL;
    uint32_t P_LEGACYVRLPOVR;
    uint32_t P_VRSTATUS;
    uint32_t P_SRAMCTRL;
    uint32_t P_ADCSTATUS;
    uint32_t P_MRAMEXTCTRL;
    uint32_t P_I3CISOCTRL;
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

//Block 2: MCU Control (M_)
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
    uint32_t M_LFRC;
    uint32_t M_BODCTRL;
    uint32_t M_ADCPWRCTRL;
    uint32_t M_ADCCAL;
    uint32_t M_ADCBATTLOAD;
    uint32_t M_XTALCTRL;
    uint32_t M_XTALGENCTRL;
    uint32_t M_XTALHSCTRL;
    uint32_t M_MRAMCRYPTOPWRCTRL;
    uint32_t M_D2ASPARE;
    uint32_t M_BODISABLE;
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
    uint32_t M_KEXTCLKSEL;
    uint32_t M_SIMOBUCK0;
    uint32_t M_SIMOBUCK1;
    uint32_t M_SIMOBUCK2;
    uint32_t M_SIMOBUCK3;
    uint32_t M_SIMOBUCK4;
    uint32_t M_SIMOBUCK6;
    uint32_t M_SIMOBUCK7;
    uint32_t M_SIMOBUCK8;
    uint32_t M_SIMOBUCK9;
    uint32_t M_SIMOBUCK10;
    uint32_t M_SIMOBUCK11;
    uint32_t M_D2ASPARE2;
    uint32_t M_I3CPHYCTRL;
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
    uint32_t M_SRAMRPROT0;
    uint32_t M_SRAMRPROT1;
    uint32_t M_SRAMRPROT2;
    uint32_t M_SRAMRPROT3;
    uint32_t M_SDIO0CTRL;
    uint32_t M_SDIO1CTRL;
    uint32_t M_PDMCTRL;
    uint32_t M_SSRAMMISCCTRL;
    uint32_t M_DISPSTATUS;
    uint32_t M_CPUCFG;
    uint32_t M_PLLCTL0;
    uint32_t M_PLLDIV0;
    uint32_t M_PLLDIV1;
    uint32_t M_PLLSTAT;
    uint32_t M_PLLMUXCTL;
    uint32_t M_CM4CODEBASE;
    uint32_t M_RADIOFINECNT;
    uint32_t M_RADIOCLKNCNT;
}
am_util_pp_b2_t;

//Block 3: Clock Generator (C_) System Timer (ST_) and Timer (T_)
typedef struct
{
    uint32_t C_OCTRL;
    uint32_t C_CLKOUT;
    uint32_t C_HFADJ;
    uint32_t C_CLOCKENSTAT;
    uint32_t C_CLOCKEN2STAT;
    uint32_t C_CLOCKEN3STAT;
    uint32_t C_MISC;
    uint32_t C_LFRCCTRL;
    uint32_t C_CLKGENSPARES;
    uint32_t C_HFRCIDLECOUNTERS;
    uint32_t C_MSPIIOCLKCTRL;
    uint32_t C_CLKCTRL;

    uint32_t ST_STCFG;
    uint32_t ST_STTMR;
    uint32_t ST_SCAPCTRL0;
    uint32_t ST_SCAPCTRL1;
    uint32_t ST_SCAPCTRL2;
    uint32_t ST_SCAPCTRL3;
    uint32_t ST_SCMPR0;
    uint32_t ST_SCMPR1;
    uint32_t ST_SCMPR2;
    uint32_t ST_SCMPR3;
    uint32_t ST_SCMPR4;
    uint32_t ST_SCMPR5;
    uint32_t ST_SCMPR6;
    uint32_t ST_SCMPR7;
    uint32_t ST_SCAPT0;
    uint32_t ST_SCAPT1;
    uint32_t ST_SCAPT2;
    uint32_t ST_SCAPT3;
    uint32_t ST_SNVR0;
    uint32_t ST_SNVR1;
    uint32_t ST_HALSTATES;
    uint32_t ST_STMINTEN;
    uint32_t ST_STMINTSTAT;
    uint32_t ST_STMINTCLR;
    uint32_t ST_STMINTSET;

    uint32_t T_CTRL;
    uint32_t T_STATUS;
    uint32_t T_GLOBEN;
    uint32_t T_INTEN;
    uint32_t T_INTSTAT;
    uint32_t T_INTCLR;
    uint32_t T_INTSET;
    uint32_t T_CTRL0;
    uint32_t T_TIMER0;
    uint32_t T_TMR0CMP0;
    uint32_t T_TMR0CMP1;
    uint32_t T_MODE0;
    uint32_t T_TMR0LMTVAL;
    uint32_t T_CTRL1;
    uint32_t T_TIMER1;
    uint32_t T_TMR1CMP0;
    uint32_t T_TMR1CMP1;
    uint32_t T_MODE1;
    uint32_t T_TMR1LMTVAL;
    uint32_t T_CTRL2;
    uint32_t T_TIMER2;
    uint32_t T_TMR2CMP0;
    uint32_t T_TMR2CMP1;
    uint32_t T_MODE2;
    uint32_t T_TMR2LMTVAL;
    uint32_t T_CTRL3;
    uint32_t T_TIMER3;
    uint32_t T_TMR3CMP0;
    uint32_t T_TMR3CMP1;
    uint32_t T_MODE3;
    uint32_t T_TMR3LMTVAL;
    uint32_t T_CTRL4;
    uint32_t T_TIMER4;
    uint32_t T_TMR4CMP0;
    uint32_t T_TMR4CMP1;
    uint32_t T_MODE4;
    uint32_t T_TMR4LMTVAL;
    uint32_t T_CTRL5;
    uint32_t T_TIMER5;
    uint32_t T_TMR5CMP0;
    uint32_t T_TMR5CMP1;
    uint32_t T_MODE5;
    uint32_t T_TMR5LMTVAL;
    uint32_t T_CTRL6;
    uint32_t T_TIMER6;
    uint32_t T_TMR6CMP0;
    uint32_t T_TMR6CMP1;
    uint32_t T_MODE6;
    uint32_t T_TMR6LMTVAL;
    uint32_t T_CTRL7;
    uint32_t T_TIMER7;
    uint32_t T_TMR7CMP0;
    uint32_t T_TMR7CMP1;
    uint32_t T_MODE7;
    uint32_t T_TMR7LMTVAL;
    uint32_t T_CTRL8;
    uint32_t T_TIMER8;
    uint32_t T_TMR8CMP0;
    uint32_t T_TMR8CMP1;
    uint32_t T_MODE8;
    uint32_t T_TMR8LMTVAL;
    uint32_t T_CTRL9;
    uint32_t T_TIMER9;
    uint32_t T_TMR9CMP0;
    uint32_t T_TMR9CMP1;
    uint32_t T_MODE9;
    uint32_t T_TMR9LMTVAL;
    uint32_t T_CTRL10;
    uint32_t T_TIMER10;
    uint32_t T_TMR10CMP0;
    uint32_t T_TMR10CMP1;
    uint32_t T_MODE10;
    uint32_t T_TMR10LMTVAL;
    uint32_t T_CTRL11;
    uint32_t T_TIMER11;
    uint32_t T_TMR11CMP0;
    uint32_t T_TMR11CMP1;
    uint32_t T_MODE11;
    uint32_t T_TMR11LMTVAL;
    uint32_t T_CTRL12;
    uint32_t T_TIMER12;
    uint32_t T_TMR12CMP0;
    uint32_t T_TMR12CMP1;
    uint32_t T_MODE12;
    uint32_t T_TMR12LMTVAL;
    uint32_t T_CTRL13;
    uint32_t T_TIMER13;
    uint32_t T_TMR13CMP0;
    uint32_t T_TMR13CMP1;
    uint32_t T_MODE13;
    uint32_t T_TMR13LMTVAL;
    uint32_t T_CTRL14;
    uint32_t T_TIMER14;
    uint32_t T_TMR14CMP0;
    uint32_t T_TMR14CMP1;
    uint32_t T_MODE14;
    uint32_t T_TMR14LMTVAL;
    uint32_t T_CTRL15;
    uint32_t T_TIMER15;
    uint32_t T_TMR15CMP0;
    uint32_t T_TMR15CMP1;
    uint32_t T_MODE15;
    uint32_t T_TMR15LMTVAL;
    uint32_t T_TIMERSPARES;
}
am_util_pp_b3_t;

//Block 4: System Control (SC_)
typedef struct
{
    uint32_t SC_ICTR;
    uint32_t SC_ACTLR;
    uint32_t SC_ICSR;
    uint32_t SC_VTOR;
    uint32_t SC_AIRCR;
    uint32_t SC_SCR;
    uint32_t SC_CCR;
    uint32_t SC_SHPR1;
    uint32_t SC_SHPR2;
    uint32_t SC_SHPR3;
    uint32_t SC_SHCSR;
    uint32_t SC_CFSR;
    uint32_t SC_HFSR;
    uint32_t SC_MMFAR;
    uint32_t SC_BFAR;
    uint32_t SC_CPACR;
    uint32_t SC_DEMCR;
    uint32_t SC_STIR;
    uint32_t SC_FPCCR;
    uint32_t SC_FPCAR;
    uint32_t SC_FPDSCR;
}
am_util_pp_b4_t;

//Block 5: Application specific registers, PDM (PDM_)
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

//*****************************************************************************
//
//! @brief Prints the filled up JSON to serial port.
//!
//! @param void
//!
//! This function will print filled JSON fields to serial port.
//!
//! @note - before use this function, make sure the UART or SWO is enabled
//!
//!
//! @returns None
//
//*****************************************************************************
extern void am_util_print_JSON(uint8_t uNumber);

//*****************************************************************************
//
//! @brief Iinvoke the snapshot
//!
//! @param bDebug - a flag to indicate it's a debug or not
//! @param uNumber - snapshot number
//! @param bUseMemory - whether or not to store the JSON to memory
//!
//! This is the customer facing API function calls to invoke the snapshot
//!
//! @returns None
//
//*****************************************************************************
extern void am_util_pp_snapshot(bool bDebug, uint32_t uNumber, bool bStreamNow);

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
