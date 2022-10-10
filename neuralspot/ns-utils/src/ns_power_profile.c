/**
 * @file ns_power_profile.c
 * @author Ambiq
 * @brief Collection of power profiling utilities
 * @version 0.1
 * @date 2022-10-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ns_power_profile.h"
#include "ns_ambiqsuite_harness.h"

//This is the data structure we need to fill up
am_bsp_pp_b1_t am_bsp_pp1; 
am_bsp_pp_b2_t am_bsp_pp2; 
am_bsp_pp_b3_t am_bsp_pp3; 
am_bsp_pp_b4_t am_bsp_pp4; 

void *g_pp_AUDADCHandle;

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
void
ns_print_JSON(void)
{
    //
    // Print the filled JSON file out
    //
char *pwrStr1 = "\n{\"PWRCTRL\": {\
\"Singleshot\": %u,\
\"SnapN\": %u,\
\"MCUPERFREQ\": %u,\
\"DEVPWRSTATUS\": %u,\
\"AUDSSPWRSTATUS\": %u,\
\"MEMPWRSTATUS\": %u,\
\"MEMRETCFG\": %u,\
\"SYSPWRSTATUS\": %u,\
 \"SSRAMPWRST\": %u,\
 \"SSRAMRETCFG\": %u,\
 \"DEVPWREVENTEN\": %u,\
 \"MEMPWREVENTEN\": %u,\
 \"MMSOVERRIDE\": %u, \
";

char *pwrStr2 = " \
 \"DSP0PWRCTRL\": %u,\
 \"DSP0PERFREQ\": %u,\
 \"DSP0MEMPWREN\": %u,\
 \"DSP0MEMPWRST\": %u,\
 \"DSP0MEMRETCFG\": %u,\
 \"DSP1PWRCTRL\": %u,\
 \"DSP1PERFREQ\": %u,\
 \"DSP1MEMPWREN\": %u,\
 \"DSP1MEMPWRST\": %u,\
 \"DSP1MEMRETCFG\": %u,\
 \"VRCTRL\": %u,\
 \"LEGACYVRLPOVR\": %u,\
 \"VRSTATUS\": %u, \
";

char *pwrStr3 = " \
 \"VRDEMOTIONTHR\": %u,\
 \"SRAMCTRL\": %u,\
 \"ADCSTATUS\": %u,\
 \"AUDADCSTATUS\": %u,\
 \"EMONCTRL\": %u,\
 \"EMONSTATUS\": %u, \
 \"FPIOEN0\": %u, \
 \"FPIOEN1\": %u, \
 \"FPIOEN2\": %u \
},";   

char *mcuCtrlStr = " \"MCUCTRL\": {\
\"ACRG\": %u, \
\"VRCTRL\": %u, \
\"LDOREG1\": %u, \
\"LDOREG2\": %u, \
\"LFRC\": %u, \
\"BODCTRL\": %u, \
\"ADCPWRDLY\": %u, \
\"ADCPWRCTRL\": %u, \
\"ADCCAL\": %u, \
\"ADCBATTLOAD\": %u, \
\"XTALCTRL\": %u, \
\"XTALGENCTRL\": %u, \
\"XTALHSTRIMS\": %u, \
\"XTALHSCTRL\": %u, \
\"BODISABLE\": %u, \
\"PMUENABLE\": %u, \
\"SIMOBUCK0\": %u, \
\"SIMOBUCK1\": %u, \
\"SIMOBUCK2\": %u, \
\"SIMOBUCK4\": %u, \
\"SIMOBUCK6\": %u, \
\"SIMOBUCK7\": %u, \
\"SIMOBUCK9\": %u, \
\"SIMOBUCK12\": %u, \
\"SIMOBUCK13\": %u, \
\"SIMOBUCK15\": %u, \
\"PWRSW0\": %u, \
\"PWRSW1\": %u, \
\"AUDADCPWRCTRL\": %u, \
\"AUDIO1\": %u, \
\"PGAADCIFCTRL\": %u, \
\"PGACTRL1\": %u, \
\"PGACTRL2\": %u, \
\"AUDADCPWRDLY\": %u, \
\"SDIOCTRL\": %u, \
\"PDMCTRL\": %u \
},";

char *audadcStr = " \"AUDADC\": {\
\"CFG\": %u, \
\"STAT\": %u, \
\"SWT\": %u, \
\"SL0CFG\": %u, \
\"SL1CFG\": %u, \
\"SL2CFG\": %u, \
\"SL3CFG\": %u, \
\"SL4CFG\": %u, \
\"SL5CFG\": %u, \
\"SL6CFG\": %u, \
\"SL7CFG\": %u, \
\"WULIM\": %u, \
\"WLLIM\": %u, \
\"SCWLIM\": %u, \
\"FIFO\": %u, \
\"FIFOPR\": %u, \
\"FIFOSTAT\": %u, \
\"DATAOFFSET\": %u, \
\"ZXCFG\": %u, \
\"ZXLIM\": %u, \
\"GAINCFG\": %u, \
\"GAIN\": %u, \
\"SATCFG\": %u, \
\"SATLIM\": %u, \
\"SATMAX\": %u, \
\"SATCLR\": %u, \
\"IEREN\": %u, \
\"IERSTAT\": %u, \
\"IERCLR\": %u, \
\"IERSET\": %u, \
\"DMATRIGEN\": %u, \
\"DMATRIGSTAT\": %u, \
\"DMACFG\": %u, \
\"DMATOTCOUNT\": %u, \
\"DMATARGADDR\": %u, \
\"DMASTAT\": %u \
},";

char *clkStr = " \"CLK\": {\
\"CALXT\": %u, \
\"CALRC\": %u, \
\"ACALCTR\": %u, \
\"OCTRL\": %u, \
\"CLKOUT\": %u, \
\"STATUS\": %u, \
\"HFADJ\": %u, \
\"HFVAL\": %u, \
\"CLOCKENSTAT\": %u, \
\"CLOCKEN2STAT\": %u, \
\"CLOCKEN3STAT\": %u, \
\"MISC\": %u, \
\"HF2ADJ0\": %u, \
\"HF2ADJ1\": %u, \
\"HF2ADJ2\": %u, \
\"HF2VAL\": %u, \
\"LFRCCTRL\": %u, \
\"DISPCLKCTRL\": %u \
},";

char *timerStr = " \"Timers\": {\
 \"STCFG\": %u, \
 \"STMINTSTAT\": %u, \
 \"CTRL\": %u, \
 \"STATUS\": %u, \
 \"GLOBEN\": %u, \
 \"INTSTAT\": %u, \
 \"CTRL0\": %u, \
 \"CTRL1\": %u, \
 \"CTRL2\": %u, \
 \"CTRL3\": %u, \
 \"CTRL4\": %u, \
 \"CTRL5\": %u, \
 \"CTRL6\": %u, \
 \"CTRL7\": %u, \
 \"CTRL8\": %u, \
 \"CTRL9\": %u, \
 \"CTRL10\": %u, \
 \"CTRL11\": %u, \
 \"CTRL12\": %u, \
 \"CTRL13\": %u, \
 \"CTRL14\": %u, \
 \"CTRL15\": %u \
}}\n";


    ns_printf(pwrStr1, am_bsp_pp1.bSingle, 
                          am_bsp_pp1.uSnapShot, \
                          am_bsp_pp1.P_MCUPERFREQ, \
                          am_bsp_pp1.P_DEVPWRSTATUS, \
                          am_bsp_pp1.P_AUDSSPWRSTATUS, \
                          am_bsp_pp1.P_MEMPWRSTATUS, \
                          am_bsp_pp1.P_MEMRETCFG, \
                          am_bsp_pp1.P_SYSPWRSTATUS, \
                          am_bsp_pp1.P_SSRAMPWRST, \
                          am_bsp_pp1.P_SSRAMRETCFG, \
                          am_bsp_pp1.P_DEVPWREVENTEN, \
                          am_bsp_pp1.P_MEMPWREVENTEN, \
                          am_bsp_pp1.P_MMSOVERRIDE);

    ns_printf(pwrStr2, am_bsp_pp1.P_DSP0PWRCTRL, \
                          am_bsp_pp1.P_DSP0PERFREQ, \
                          am_bsp_pp1.P_DSP0MEMPWREN, \
                          am_bsp_pp1.P_DSP0MEMPWRST, \
                          am_bsp_pp1.P_DSP0MEMRETCFG, \
                          am_bsp_pp1.P_DSP1PWRCTRL, \
                          am_bsp_pp1.P_DSP1PERFREQ, \
                          am_bsp_pp1.P_DSP1MEMPWREN, \
                          am_bsp_pp1.P_DSP1MEMPWRST, \
                          am_bsp_pp1.P_DSP1MEMRETCFG, \
                          am_bsp_pp1.P_VRCTRL, \
                          am_bsp_pp1.P_LEGACYVRLPOVR, \
                          am_bsp_pp1.P_VRSTATUS);
                            
    ns_printf(pwrStr3, am_bsp_pp1.P_VRDEMOTIONTHR, \
                          am_bsp_pp1.P_SRAMCTRL, \
                          am_bsp_pp1.P_ADCSTATUS, \
                          am_bsp_pp1.P_AUDADCSTATUS, \
                          am_bsp_pp1.P_EMONCTRL, \
                          am_bsp_pp1.P_EMONSTATUS, \
                          am_bsp_pp1.P_FPIOEN0, \
                          am_bsp_pp1.P_FPIOEN1, \
                          am_bsp_pp1.P_FPIOEN2);

    
    ns_printf(mcuCtrlStr, am_bsp_pp2.M_ACRG, \
                          am_bsp_pp2.M_VRCTRL, \
                          am_bsp_pp2.M_LDOREG1, \
                          am_bsp_pp2.M_LDOREG2, \
                          am_bsp_pp2.M_LFRC, \
                          am_bsp_pp2.M_BODCTRL, \
                          am_bsp_pp2.M_ADCPWRDLY, \
                          am_bsp_pp2.M_ADCPWRCTRL, \
                          am_bsp_pp2.M_ADCCAL, \
                          am_bsp_pp2.M_ADCBATTLOAD, \
                          am_bsp_pp2.M_XTALCTRL, \
                          am_bsp_pp2.M_XTALGENCTRL, \
                          am_bsp_pp2.M_XTALHSTRIMS, \
                          am_bsp_pp2.M_XTALHSCTRL, \
                          am_bsp_pp2.M_BODISABLE, \
                          am_bsp_pp2.M_PMUENABLE, \
                          am_bsp_pp2.M_SIMOBUCK0, \
                          am_bsp_pp2.M_SIMOBUCK1, \
                          am_bsp_pp2.M_SIMOBUCK2, \
                          am_bsp_pp2.M_SIMOBUCK4, \
                          am_bsp_pp2.M_SIMOBUCK6, \
                          am_bsp_pp2.M_SIMOBUCK7, \
                          am_bsp_pp2.M_SIMOBUCK9, \
                          am_bsp_pp2.M_SIMOBUCK12, \
                          am_bsp_pp2.M_SIMOBUCK13, \
                          am_bsp_pp2.M_SIMOBUCK15, \
                          am_bsp_pp2.M_PWRSW0, \
                          am_bsp_pp2.M_PWRSW1, \
                          am_bsp_pp2.M_AUDADCPWRCTRL, \
                          am_bsp_pp2.M_AUDIO1, \
                          am_bsp_pp2.M_PGAADCIFCTRL, \
                          am_bsp_pp2.M_PGACTRL1, \
                          am_bsp_pp2.M_PGACTRL2, \
                          am_bsp_pp2.M_AUDADCPWRDLY, \
                          am_bsp_pp2.M_SDIOCTRL, \
                          am_bsp_pp2.M_PDMCTRL);

    ns_printf(audadcStr, am_bsp_pp4.AU_CFG,   \
                          am_bsp_pp4.AU_STAT,   \
                          am_bsp_pp4.AU_SWT,   \
                          am_bsp_pp4.AU_SL0CFG,   \
                          am_bsp_pp4.AU_SL1CFG,   \
                          am_bsp_pp4.AU_SL2CFG,   \
                          am_bsp_pp4.AU_SL3CFG,   \
                          am_bsp_pp4.AU_SL4CFG,   \
                          am_bsp_pp4.AU_SL5CFG,   \
                          am_bsp_pp4.AU_SL6CFG,   \
                          am_bsp_pp4.AU_SL7CFG,   \
                          am_bsp_pp4.AU_WULIM,   \
                          am_bsp_pp4.AU_WLLIM,   \
                          am_bsp_pp4.AU_SCWLIM,   \
                          am_bsp_pp4.AU_FIFO,   \
                          am_bsp_pp4.AU_FIFOPR,   \
                          am_bsp_pp4.AU_FIFOSTAT,   \
                          am_bsp_pp4.AU_DATAOFFSET,   \
                          am_bsp_pp4.AU_ZXCFG,   \
                          am_bsp_pp4.AU_ZXLIM,   \
                          am_bsp_pp4.AU_GAINCFG,   \
                          am_bsp_pp4.AU_GAIN,   \
                          am_bsp_pp4.AU_SATCFG,   \
                          am_bsp_pp4.AU_SATLIM,   \
                          am_bsp_pp4.AU_SATMAX,   \
                          am_bsp_pp4.AU_SATCLR,   \
                          am_bsp_pp4.AU_IEREN,   \
                          am_bsp_pp4.AU_IERSTAT,   \
                          am_bsp_pp4.AU_IERCLR,   \
                          am_bsp_pp4.AU_IERSET,   \
                          am_bsp_pp4.AU_DMATRIGEN,   \
                          am_bsp_pp4.AU_DMATRIGSTAT,   \
                          am_bsp_pp4.AU_DMACFG,   \
                          am_bsp_pp4.AU_DMATOTCOUNT,   \
                          am_bsp_pp4.AU_DMATARGADDR,   \
                          am_bsp_pp4.AU_DMASTAT);
      
    ns_printf(clkStr, am_bsp_pp3.C_CALXT, \
                          am_bsp_pp3.C_CALRC, \
                          am_bsp_pp3.C_ACALCTR, \
                          am_bsp_pp3.C_OCTRL, \
                          am_bsp_pp3.C_CLKOUT, \
                          am_bsp_pp3.C_STATUS, \
                          am_bsp_pp3.C_HFADJ, \
                          am_bsp_pp3.C_HFVAL, \
                          am_bsp_pp3.C_CLOCKENSTAT, \
                          am_bsp_pp3.C_CLOCKEN2STAT, \
                          am_bsp_pp3.C_CLOCKEN3STAT, \
                          am_bsp_pp3.C_MISC, \
                          am_bsp_pp3.C_HF2ADJ0, \
                          am_bsp_pp3.C_HF2ADJ1, \
                          am_bsp_pp3.C_HF2ADJ2, \
                          am_bsp_pp3.C_HF2VAL, \
                          am_bsp_pp3.C_LFRCCTRL, \
                          am_bsp_pp3.C_DISPCLKCTRL);
    
    
   ns_printf(timerStr, am_bsp_pp3.ST_STCFG, \
                          am_bsp_pp3.ST_STMINTSTAT, \
                          am_bsp_pp3.T_CTRL, \
                          am_bsp_pp3.T_STATUS, \
                          am_bsp_pp3.T_GLOBEN, \
                          am_bsp_pp3.T_INTSTAT, \
                          am_bsp_pp3.T_CTRL0, \
                          am_bsp_pp3.T_CTRL1, \
                          am_bsp_pp3.T_CTRL2, \
                          am_bsp_pp3.T_CTRL3, \
                          am_bsp_pp3.T_CTRL4, \
                          am_bsp_pp3.T_CTRL5, \
                          am_bsp_pp3.T_CTRL6, \
                          am_bsp_pp3.T_CTRL7, \
                          am_bsp_pp3.T_CTRL8, \
                          am_bsp_pp3.T_CTRL9, \
                          am_bsp_pp3.T_CTRL10, \
                          am_bsp_pp3.T_CTRL11, \
                          am_bsp_pp3.T_CTRL12, \
                          am_bsp_pp3.T_CTRL13, \
                          am_bsp_pp3.T_CTRL14, \
                          am_bsp_pp3.T_CTRL15 );
    //Now, we can disable the UART to provide minimized impact to the system
    am_bsp_uart_printf_disable();    
    
}

void ns_pp_snapshot(bool bSingleShot, uint32_t uNumber)
{
    am_bsp_pp1.uSnapShot = uNumber;
    am_bsp_pp1.P_MCUPERFREQ = PWRCTRL-> MCUPERFREQ;
    
    am_bsp_pp1.P_DEVPWRSTATUS = PWRCTRL-> DEVPWRSTATUS;  //all peripheral power information
    am_bsp_pp1.P_DEVPWREN = PWRCTRL->DEVPWREN;
    
    //if there is an silicon bug, catch it here
    if (am_bsp_pp1.P_DEVPWRSTATUS != am_bsp_pp1.P_DEVPWREN) am_bsp_pp1.P_DEVPWRSTATUS = 0xFFFFFFFF;
    
    am_bsp_pp1.P_AUDSSPWRSTATUS = PWRCTRL-> AUDSSPWRSTATUS;
    am_bsp_pp1.P_MEMPWRSTATUS = PWRCTRL-> MEMPWRSTATUS;
    am_bsp_pp1.P_MEMRETCFG = PWRCTRL-> MEMRETCFG;
    am_bsp_pp1.P_SYSPWRSTATUS = PWRCTRL-> SYSPWRSTATUS;
    am_bsp_pp1.P_SSRAMPWRST = PWRCTRL-> SSRAMPWRST;
    am_bsp_pp1.P_SSRAMRETCFG = PWRCTRL-> SSRAMRETCFG;
    am_bsp_pp1.P_DEVPWREVENTEN = PWRCTRL-> DEVPWREVENTEN;
    am_bsp_pp1.P_MEMPWREVENTEN = PWRCTRL-> MEMPWREVENTEN;
    am_bsp_pp1.P_MMSOVERRIDE = PWRCTRL-> MMSOVERRIDE;
    am_bsp_pp1.P_DSP0PWRCTRL = PWRCTRL->  DSP0PWRCTRL;
    am_bsp_pp1.P_DSP0PERFREQ = PWRCTRL-> DSP0PERFREQ;
    am_bsp_pp1.P_DSP0MEMPWREN = PWRCTRL-> DSP0MEMPWREN;
    am_bsp_pp1.P_DSP0MEMPWRST = PWRCTRL-> DSP0MEMPWRST;
    am_bsp_pp1.P_DSP0MEMRETCFG = PWRCTRL-> DSP0MEMRETCFG;
    am_bsp_pp1.P_DSP1PWRCTRL = PWRCTRL-> DSP1PWRCTRL;
    am_bsp_pp1.P_DSP1PERFREQ = PWRCTRL-> DSP1PERFREQ;
    am_bsp_pp1.P_DSP1MEMPWREN = PWRCTRL-> DSP1MEMPWREN;
    am_bsp_pp1.P_DSP1MEMPWRST = PWRCTRL-> DSP1MEMPWRST;
    am_bsp_pp1.P_DSP1MEMRETCFG = PWRCTRL-> DSP1MEMRETCFG;
    am_bsp_pp1.P_VRCTRL = PWRCTRL-> VRCTRL;
    am_bsp_pp1.P_LEGACYVRLPOVR = PWRCTRL-> LEGACYVRLPOVR;
    am_bsp_pp1.P_VRSTATUS = PWRCTRL-> VRSTATUS;
    am_bsp_pp1.P_PWRWEIGHTULP0 = PWRCTRL-> PWRWEIGHTULP0;
    am_bsp_pp1.P_PWRWEIGHTULP1 = PWRCTRL-> PWRWEIGHTULP1;
    am_bsp_pp1.P_PWRWEIGHTULP2 = PWRCTRL-> PWRWEIGHTULP2;
    am_bsp_pp1.P_PWRWEIGHTULP3 = PWRCTRL-> PWRWEIGHTULP3;
    am_bsp_pp1.P_PWRWEIGHTULP4 = PWRCTRL-> PWRWEIGHTULP4;     
    am_bsp_pp1.P_PWRWEIGHTULP5 = PWRCTRL-> PWRWEIGHTULP5;
    am_bsp_pp1.P_PWRWEIGHTLP0 = PWRCTRL-> PWRWEIGHTLP0;
    am_bsp_pp1.P_PWRWEIGHTLP1 = PWRCTRL-> PWRWEIGHTLP1;
    am_bsp_pp1.P_PWRWEIGHTLP2 = PWRCTRL-> PWRWEIGHTLP2;
    am_bsp_pp1.P_PWRWEIGHTLP3 = PWRCTRL-> PWRWEIGHTLP3;
    am_bsp_pp1.P_PWRWEIGHTLP4 = PWRCTRL-> PWRWEIGHTLP4;
    am_bsp_pp1.P_PWRWEIGHTLP5 = PWRCTRL-> PWRWEIGHTLP5;
    am_bsp_pp1.P_PWRWEIGHTHP0 = PWRCTRL-> PWRWEIGHTHP0;
    am_bsp_pp1.P_PWRWEIGHTHP1 = PWRCTRL-> PWRWEIGHTHP1;
    am_bsp_pp1.P_PWRWEIGHTHP2 = PWRCTRL-> PWRWEIGHTHP2;
    am_bsp_pp1.P_PWRWEIGHTHP3 = PWRCTRL-> PWRWEIGHTHP3;
    am_bsp_pp1.P_PWRWEIGHTHP4 = PWRCTRL-> PWRWEIGHTHP4;
    am_bsp_pp1.P_PWRWEIGHTHP5 = PWRCTRL-> PWRWEIGHTHP5;
    am_bsp_pp1.P_PWRWEIGHTSLP = PWRCTRL-> PWRWEIGHTSLP;
    am_bsp_pp1.P_VRDEMOTIONTHR = PWRCTRL-> VRDEMOTIONTHR;
    am_bsp_pp1.P_SRAMCTRL = PWRCTRL-> SRAMCTRL;
    am_bsp_pp1.P_ADCSTATUS = PWRCTRL-> ADCSTATUS;
    am_bsp_pp1.P_AUDADCSTATUS = PWRCTRL-> AUDADCSTATUS;
    am_bsp_pp1.P_EMONCTRL = PWRCTRL-> EMONCTRL;
    am_bsp_pp1.P_EMONCFG0 = PWRCTRL-> EMONCFG0;
    am_bsp_pp1.P_EMONCFG1 = PWRCTRL-> EMONCFG1;
    am_bsp_pp1.P_EMONCFG2 = PWRCTRL-> EMONCFG2;
    am_bsp_pp1.P_EMONCFG3 = PWRCTRL-> EMONCFG3;
    am_bsp_pp1.P_EMONCFG4 = PWRCTRL-> EMONCFG4;
    am_bsp_pp1.P_EMONCFG5 = PWRCTRL-> EMONCFG5;
    am_bsp_pp1.P_EMONCFG6 = PWRCTRL-> EMONCFG6;
    am_bsp_pp1.P_EMONCFG7 = PWRCTRL-> EMONCFG7;
    am_bsp_pp1.P_EMONCOUNT0 = PWRCTRL-> EMONCOUNT0;
    am_bsp_pp1.P_EMONCOUNT1 = PWRCTRL-> EMONCOUNT1;
    am_bsp_pp1.P_EMONCOUNT2 = PWRCTRL-> EMONCOUNT2;
    am_bsp_pp1.P_EMONCOUNT3 = PWRCTRL-> EMONCOUNT3;
    am_bsp_pp1.P_EMONCOUNT4 = PWRCTRL-> EMONCOUNT4;
    am_bsp_pp1.P_EMONCOUNT5 = PWRCTRL-> EMONCOUNT5;
    am_bsp_pp1.P_EMONCOUNT6 = PWRCTRL-> EMONCOUNT6;
    am_bsp_pp1.P_EMONCOUNT7 = PWRCTRL-> EMONCOUNT7;
    am_bsp_pp1.P_EMONSTATUS = PWRCTRL-> EMONSTATUS;
    
    //append the FPIO here
    am_bsp_pp1.P_FPIOEN0 = FPIO-> EN0 ;
    am_bsp_pp1.P_FPIOEN1 = FPIO-> EN1;
    am_bsp_pp1.P_FPIOEN2 = FPIO-> EN2;

  
    //Function block 2: MCU_CTRL
    //Set AI keys 
     //WriteAIKeys();
    AI0 = 0x61EBBE3C;
    AI1 = 0xAA16A508;
    AI2 = 0x5C13A663;
    AI3 = 0x49B43703;
    GPIO->PADKEY = 0X73;
    
    am_bsp_pp2.M_ACRG= MCUCTRL->ACRG;
    am_bsp_pp2.M_VRCTRL= MCUCTRL->VRCTRL;
    am_bsp_pp2.M_LDOREG1= MCUCTRL->LDOREG1; 
    am_bsp_pp2.M_LDOREG2= MCUCTRL->LDOREG2;             
//    am_bsp_pp2.M_HFRC= MCUCTRL->HFRC;   
//    am_bsp_pp2.M_HFRC2= MCUCTRL->HFRC2;   
    am_bsp_pp2.M_LFRC= MCUCTRL->LFRC;   
    am_bsp_pp2.M_BODCTRL= MCUCTRL->BODCTRL;  
    am_bsp_pp2.M_ADCPWRDLY= MCUCTRL->ADCPWRDLY;  
    am_bsp_pp2.M_ADCPWRCTRL= MCUCTRL->ADCPWRCTRL; 
    am_bsp_pp2.M_ADCCAL= MCUCTRL->ADCCAL;   
    am_bsp_pp2.M_ADCBATTLOAD= MCUCTRL->ADCBATTLOAD; 
    am_bsp_pp2.M_XTALCTRL= MCUCTRL->XTALCTRL;
    am_bsp_pp2.M_XTALGENCTRL= MCUCTRL->XTALGENCTRL;
    am_bsp_pp2.M_XTALHSTRIMS= MCUCTRL->XTALHSTRIMS;
    am_bsp_pp2.M_XTALHSCTRL= MCUCTRL->XTALHSCTRL;       
    am_bsp_pp2.M_BODISABLE= MCUCTRL->BODISABLE;
    am_bsp_pp2.M_PMUENABLE= MCUCTRL->PMUENABLE;
    am_bsp_pp2.M_SIMOBUCK0= MCUCTRL->SIMOBUCK0;
    am_bsp_pp2.M_SIMOBUCK1= MCUCTRL->SIMOBUCK1;
    am_bsp_pp2.M_SIMOBUCK2= MCUCTRL->SIMOBUCK2;
    // am_bsp_pp2.M_SIMOBUCK4= MCUCTRL->SIMOBUCK4;
    am_bsp_pp2.M_SIMOBUCK6= MCUCTRL->SIMOBUCK6;
    am_bsp_pp2.M_SIMOBUCK7= MCUCTRL->SIMOBUCK7;
    am_bsp_pp2.M_SIMOBUCK9= MCUCTRL->SIMOBUCK9;
    am_bsp_pp2.M_SIMOBUCK12= MCUCTRL->SIMOBUCK12;
    am_bsp_pp2.M_SIMOBUCK13= MCUCTRL->SIMOBUCK13;
    am_bsp_pp2.M_SIMOBUCK15= MCUCTRL->SIMOBUCK15;
    am_bsp_pp2.M_PWRSW0= MCUCTRL->PWRSW0;
    am_bsp_pp2.M_PWRSW1= MCUCTRL->PWRSW1;
    am_bsp_pp2.M_AUDADCPWRCTRL= MCUCTRL->AUDADCPWRCTRL;
    am_bsp_pp2.M_AUDIO1= MCUCTRL->AUDIO1;
    am_bsp_pp2.M_PGAADCIFCTRL= MCUCTRL->PGAADCIFCTRL;
    am_bsp_pp2.M_PGACTRL1= MCUCTRL->PGACTRL1;
    am_bsp_pp2.M_PGACTRL2= MCUCTRL->PGACTRL2;  
    am_bsp_pp2.M_AUDADCPWRDLY= MCUCTRL->AUDADCPWRDLY;
    am_bsp_pp2.M_SDIOCTRL= MCUCTRL->SDIOCTRL;
    am_bsp_pp2.M_PDMCTRL= MCUCTRL->PDMCTRL;
    
    //Function Block 3: CLKGEN
    am_bsp_pp3.C_OCTRL	= CLKGEN->OCTRL;
    am_bsp_pp3.C_CLKOUT	= CLKGEN->CLKOUT;
    //am_bsp_pp3.C_STATUS = CLKGEN->STATUS;
    am_bsp_pp3.C_HFADJ	= CLKGEN->HFADJ;
    am_bsp_pp3.C_CLOCKENSTAT	= CLKGEN->CLOCKENSTAT;
    am_bsp_pp3.C_CLOCKEN2STAT	= CLKGEN->CLOCKEN2STAT;
    am_bsp_pp3.C_CLOCKEN3STAT       	= CLKGEN->CLOCKEN3STAT;       
    am_bsp_pp3.C_MISC	= CLKGEN->MISC;
    am_bsp_pp3.C_HF2ADJ0	= CLKGEN->HF2ADJ0;
    am_bsp_pp3.C_HF2ADJ1    	= CLKGEN->HF2ADJ1;    
    am_bsp_pp3.C_HF2ADJ2	= CLKGEN->HF2ADJ2;
    am_bsp_pp3.C_HF2VAL	= CLKGEN->HF2VAL;
    am_bsp_pp3.C_LFRCCTRL	= CLKGEN->LFRCCTRL;
    am_bsp_pp3.C_DISPCLKCTRL	= CLKGEN->DISPCLKCTRL;

    //Function Block 4: System timer
    am_bsp_pp3.ST_STCFG = STIMER->STCFG;  
    am_bsp_pp3.ST_STMINTSTAT = STIMER->STMINTSTAT;    
   
    //Function Block 5: Timer
    am_bsp_pp3.T_CTRL	= TIMER->CTRL;
    am_bsp_pp3.T_STATUS	= TIMER->STATUS;
    am_bsp_pp3.T_GLOBEN	= TIMER->GLOBEN;
    am_bsp_pp3.T_INTSTAT	= TIMER->INTSTAT;
    am_bsp_pp3.T_CTRL0	= TIMER->CTRL0;
    am_bsp_pp3.T_CTRL1	= TIMER->CTRL1;
    am_bsp_pp3.T_CTRL2	= TIMER->CTRL2;
    am_bsp_pp3.T_CTRL3	= TIMER->CTRL3;
    am_bsp_pp3.T_CTRL4	= TIMER->CTRL4;
    am_bsp_pp3.T_CTRL5	= TIMER->CTRL5;
    am_bsp_pp3.T_CTRL6	= TIMER->CTRL6;
    am_bsp_pp3.T_CTRL7	= TIMER->CTRL7;
    am_bsp_pp3.T_CTRL8	= TIMER->CTRL8;
    am_bsp_pp3.T_CTRL9	= TIMER->CTRL9;
    am_bsp_pp3.T_CTRL10	= TIMER->CTRL10;
    am_bsp_pp3.T_CTRL11	= TIMER->CTRL11;
    am_bsp_pp3.T_CTRL12	= TIMER->CTRL12;
    am_bsp_pp3.T_CTRL13	= TIMER->CTRL13;
    am_bsp_pp3.T_CTRL14	= TIMER->CTRL14;
    am_bsp_pp3.T_CTRL15	= TIMER->CTRL15;
    
    //Function Block 6: AUDADC
    if (AM_HAL_STATUS_SUCCESS == am_hal_audadc_power_control(g_pp_AUDADCHandle,
                                                          AM_HAL_SYSCTRL_WAKE,
                                                          false) )
    {
      am_bsp_pp4.AU_CFG  =	AUDADC->CFG;
      am_bsp_pp4.AU_STAT = 	AUDADC->STAT;
      am_bsp_pp4.AU_SWT = 	AUDADC->SWT;
          am_bsp_pp4.AU_SL0CFG = 	AUDADC->SL0CFG; 
      am_bsp_pp4.AU_SL1CFG = 	AUDADC->SL1CFG;
      am_bsp_pp4.AU_SL2CFG = 	AUDADC->SL2CFG;
      am_bsp_pp4.AU_SL3CFG = 	AUDADC->SL3CFG;
      am_bsp_pp4.AU_SL4CFG = 	AUDADC->SL4CFG;
      am_bsp_pp4.AU_SL5CFG = 	AUDADC->SL5CFG;
      am_bsp_pp4.AU_SL6CFG = 	AUDADC->SL6CFG;
      am_bsp_pp4.AU_SL7CFG = 	AUDADC->SL7CFG;
      am_bsp_pp4.AU_WULIM = 	AUDADC->WULIM;
      am_bsp_pp4.AU_WLLIM = 	AUDADC->WLLIM;
      am_bsp_pp4.AU_SCWLIM = 	AUDADC->SCWLIM;
      am_bsp_pp4.AU_FIFO = 	AUDADC->FIFO;
      am_bsp_pp4.AU_FIFOPR = 	AUDADC->FIFOPR;
      am_bsp_pp4.AU_FIFOSTAT = 	AUDADC->FIFOSTAT;
      am_bsp_pp4.AU_DATAOFFSET = 	AUDADC->DATAOFFSET;
      am_bsp_pp4.AU_ZXCFG = 	AUDADC->ZXCFG;
      am_bsp_pp4.AU_ZXLIM = 	AUDADC->ZXLIM;
      am_bsp_pp4.AU_GAINCFG = 	AUDADC->GAINCFG;
      am_bsp_pp4.AU_GAIN = 	AUDADC->GAIN;
      am_bsp_pp4.AU_SATCFG = 	AUDADC->SATCFG;

      am_bsp_pp4.AU_SATLIM = 	AUDADC->SATLIM;
      am_bsp_pp4.AU_SATMAX = 	AUDADC->SATMAX;
      am_bsp_pp4.AU_SATCLR = 	AUDADC->SATCLR;
      am_bsp_pp4.AU_IEREN = 	AUDADC->INTEN; 
      am_bsp_pp4.AU_IERSTAT = 	AUDADC->INTSTAT;
      am_bsp_pp4.AU_IERCLR = 	AUDADC->INTCLR;
      am_bsp_pp4.AU_IERSET = 	AUDADC->INTSET;
      am_bsp_pp4.AU_DMATRIGEN = 	AUDADC->DMATRIGEN;
      am_bsp_pp4.AU_DMATRIGSTAT = 	AUDADC->DMATRIGSTAT;
      am_bsp_pp4.AU_DMACFG = 	AUDADC->DMACFG;
      am_bsp_pp4.AU_DMATOTCOUNT = 	AUDADC->DMATOTCOUNT;
      am_bsp_pp4.AU_DMATARGADDR = 	AUDADC->DMATARGADDR;
      am_bsp_pp4.AU_DMASTAT = 	AUDADC->DMASTAT;
    }
    
  ns_print_JSON();
}