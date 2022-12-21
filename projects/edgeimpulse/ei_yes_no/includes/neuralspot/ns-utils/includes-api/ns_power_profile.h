//*****************************************************************************
//
//  am_bsp_pp.h
//! @file
//!
//! @brief Functions to aid power profiling and debugging
//!
//! @addtogroup BSP Board Support Package (BSP), release date: TBD
//! @addtogroup apollo4 BSP for the Apollo4
//! @ingroup BSP
//! @{
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

#ifndef NS_POWER_PROFILE_H
#define NS_POWER_PROFILE_H

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
5 major power control blocks
1.	PWRCTRL   P_
2.	MCU_CTRL  M_
3.	CLK_GEN   C_
4.	STIMER (system timer) ST_
5.	TIMER     T_
6.      VOS       V_
7.      Other     O_
****************************************************************************/
#define AI0 (*(volatile uint32_t *)0x400401FC)
#define AI1 (*(volatile uint32_t *)0x400400F8)
#define AI2 (*(volatile uint32_t *)0x40040144)
#define AI3 (*(volatile uint32_t *)0x40040278)

// Define the Power profiling data structure block by block
typedef struct {
    bool bSingle; // single shot debug on/off
    uint32_t uSnapShot;
    uint32_t P_MCUPERFREQ;
    uint32_t P_DEVPWREN;
    uint32_t P_DEVPWRSTATUS;
    uint32_t P_AUDSSPWRSTATUS;
    uint32_t P_MEMPWRSTATUS;
    uint32_t P_MEMRETCFG;
    uint32_t P_SYSPWRSTATUS;
    uint32_t P_SSRAMPWRST;
    uint32_t P_SSRAMRETCFG;
    uint32_t P_DEVPWREVENTEN;
    uint32_t P_MEMPWREVENTEN;
    uint32_t P_MMSOVERRIDE;
    uint32_t P_DSP0PWRCTRL;
    uint32_t P_DSP0PERFREQ;
    uint32_t P_DSP0MEMPWREN;
    uint32_t P_DSP0MEMPWRST;
    uint32_t P_DSP0MEMRETCFG;
    uint32_t P_DSP1PWRCTRL;
    uint32_t P_DSP1PERFREQ;
    uint32_t P_DSP1MEMPWREN;
    uint32_t P_DSP1MEMPWRST;
    uint32_t P_DSP1MEMRETCFG;
    uint32_t P_VRCTRL;
    uint32_t P_LEGACYVRLPOVR;
    uint32_t P_VRSTATUS;
    uint32_t P_PWRWEIGHTULP0;
    uint32_t P_PWRWEIGHTULP1;
    uint32_t P_PWRWEIGHTULP2;
    uint32_t P_PWRWEIGHTULP3;
    uint32_t P_PWRWEIGHTULP4;
    uint32_t P_PWRWEIGHTULP5;
    uint32_t P_PWRWEIGHTLP0;
    uint32_t P_PWRWEIGHTLP1;
    uint32_t P_PWRWEIGHTLP2;
    uint32_t P_PWRWEIGHTLP3;
    uint32_t P_PWRWEIGHTLP4;
    uint32_t P_PWRWEIGHTLP5;
    uint32_t P_PWRWEIGHTHP0;
    uint32_t P_PWRWEIGHTHP1;
    uint32_t P_PWRWEIGHTHP2;
    uint32_t P_PWRWEIGHTHP3;
    uint32_t P_PWRWEIGHTHP4;
    uint32_t P_PWRWEIGHTHP5;
    uint32_t P_PWRWEIGHTSLP;
    uint32_t P_VRDEMOTIONTHR;
    uint32_t P_SRAMCTRL;
    uint32_t P_ADCSTATUS;
    uint32_t P_AUDADCSTATUS;
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
} am_bsp_pp_b1_t;

// Define the Power profiling data structure block by block
// Block 2: MCU Ctrl
typedef struct {
    uint32_t M_ACRG;
    uint32_t M_VREFGEN;
    uint32_t M_VREFGEN2;
    uint32_t M_VRCTRL;
    uint32_t M_LDOREG1;
    uint32_t M_LDOREG2;
    uint32_t M_UVRGCTRL;
    uint32_t M_UCRGCTRL;
    uint32_t M_HFRC;
    uint32_t M_HFRC2;
    uint32_t M_LFRC;
    uint32_t M_IOCTRL;
    uint32_t M_BODCTRL;
    uint32_t M_ADCPWRDLY;
    uint32_t M_ADCPWRCTRL;
    uint32_t M_ADCCAL;
    uint32_t M_ADCBATTLOAD;
    uint32_t M_XTALCTRL;
    uint32_t M_XTALGENCTRL;
    uint32_t M_XTALHSTRIMS;
    uint32_t M_XTALHSCTRL;
    uint32_t M_TEMPSCTRL;
    uint32_t M_BGTLPCTRL;
    uint32_t M_FLASHPWRDIS;
    uint32_t M_EXTCLKSEL;
    uint32_t M_OBSCTRL;
    uint32_t M_TESTMODECTRL;
    uint32_t M_MFGTEST3;
    uint32_t M_ANATEST;
    uint32_t M_MRAMPWRCTRL;
    uint32_t M_NVMCACHEPGCTRL;
    uint32_t M_SRAMPGCTRL;
    uint32_t M_SSRAMPGCTRL;
    uint32_t M_DSP0RAMPGCTRL;
    uint32_t M_DSP1RAMPGCTRL;
    uint32_t M_MISCPWRCTRL;
    uint32_t M_PWRONRSTDLY;
    uint32_t M_PWRSEQ1ANA;
    uint32_t M_PWRSEQ2ANA;
    uint32_t M_BODISABLE;
    uint32_t M_PMUENABLE;
    uint32_t M_MISCDEVST;
    uint32_t M_KEXTCLKSE;
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
    uint32_t M_PCM;
    uint32_t M_AUDADCPWRCTRL;
    uint32_t M_AUDIO1;
    uint32_t M_AUDIO2;
    uint32_t M_PGAADCIFCTRL;
    uint32_t M_PGACTRL1;
    uint32_t M_PGACTRL2;
    uint32_t M_AUDADCPWRDLY;
    uint32_t M_SDIOCTRL;
    uint32_t M_PDMCTRL;
} am_bsp_pp_b2_t;

// Define the Power profiling data structure block by block
// Block 3: CLK_GEN ST and Timer
typedef struct {

    uint32_t C_CALXT;
    uint32_t C_CALRC;
    uint32_t C_ACALCTR;
    uint32_t C_OCTRL;
    uint32_t C_CLKOUT;
    uint32_t C_STATUS;
    uint32_t C_HFADJ;
    uint32_t C_HFVAL;
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

} am_bsp_pp_b3_t;

// Block 4: Application specific registers, AUDADC
typedef struct {
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
} am_bsp_pp_b4_t;

void
ns_print_JSON();

//*****************************************************************************
//
// Main APIs
// Parameters--
// bDebug: a flag to indicate it's a debug or not
// uNumber: snapshot number
//*****************************************************************************
void
ns_pp_snapshot(bool bDebug, uint32_t uNumber);

#ifdef __cplusplus
}
#endif

#endif // NS_POWER_PROFILE_H
       //*****************************************************************************
       //
       // End Doxygen group.
       //! @}
       //
       //*****************************************************************************
