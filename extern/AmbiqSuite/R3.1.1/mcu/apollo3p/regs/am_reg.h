//*****************************************************************************
//
//  am_reg.h
//! @file
//!
//! @brief Apollo4 register macros
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
#ifndef AM_REG_H
#define AM_REG_H

//*****************************************************************************
//
//! @brief ADC
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_ADC_NUM_MODULES                       1
#define AM_REG_ADCn(n) \
    (REG_ADC_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief APBDMA
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_APBDMA_NUM_MODULES                    1
#define AM_REG_APBDMAn(n) \
    (REG_APBDMA_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief BLEIF
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_BLEIF_NUM_MODULES                     1
#define AM_REG_BLEIFn(n) \
    (REG_BLEIF_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief CACHECTRL
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_CACHECTRL_NUM_MODULES                 1
#define AM_REG_CACHECTRLn(n) \
    (REG_CACHECTRL_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief CLKGEN
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_CLKGEN_NUM_MODULES                    1
#define AM_REG_CLKGENn(n) \
    (REG_CLKGEN_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief CTIMER
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_CTIMER_NUM_MODULES                    1
#define AM_REG_CTIMERn(n) \
    (REG_CTIMER_BASEADDR + 0x00000020 * n)

//*****************************************************************************
//
//! @brief FLASHCTRL
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_FLASHCTRL_NUM_MODULES                 1
#define AM_REG_FLASHCTRLn(n) \
    (REG_FLASHCTRL_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief GPIO
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_GPIO_NUM_MODULES                      1
#define AM_REG_GPIOn(n) \
    (REG_GPIO_BASEADDR + 0x00000004 * n)

//*****************************************************************************
//
//! @brief IOM
//! Instance finder. (6 instance(s) available)
//
//*****************************************************************************
#define AM_REG_IOM_NUM_MODULES                       6
#define AM_REG_IOMn(n) \
    (REG_IOM_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief IOSLAVE
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_IOSLAVE_NUM_MODULES                   1
#define AM_REG_IOSLAVEn(n) \
    (REG_IOSLAVE_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief MCUCTRL
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_MCUCTRL_NUM_MODULES                   1
#define AM_REG_MCUCTRLn(n) \
    (REG_MCUCTRL_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief MSPI
//! Instance finder. (3 instance(s) available)
//
//*****************************************************************************
#define AM_REG_MSPI_NUM_MODULES                      3
#define AM_REG_MSPIn(n) \
    (REG_MSPI_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief PDM
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_PDM_NUM_MODULES                       1
#define AM_REG_PDMn(n) \
    (REG_PDM_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief PWRCTRL
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_PWRCTRL_NUM_MODULES                   1
#define AM_REG_PWRCTRLn(n) \
    (REG_PWRCTRL_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief RSTGEN
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_RSTGEN_NUM_MODULES                    1
#define AM_REG_RSTGENn(n) \
    (REG_RSTGEN_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief RTC
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_RTC_NUM_MODULES                       1
#define AM_REG_RTCn(n) \
    (REG_RTC_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief SCARD
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_SCARD_NUM_MODULES                     1
#define AM_REG_SCARDn(n) \
    (REG_SCARD_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief SECURITY
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_SECURITY_NUM_MODULES                  1
#define AM_REG_SECURITYn(n) \
    (REG_SECURITY_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief UART
//! Instance finder. (2 instance(s) available)
//
//*****************************************************************************
#define AM_REG_UART_NUM_MODULES                      2
#define AM_REG_UARTn(n) \
    (REG_UART_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
//! @brief VCOMP
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_VCOMP_NUM_MODULES                     1
#define AM_REG_VCOMPn(n) \
    (REG_VCOMP_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
//! @brief WDT
//! Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_WDT_NUM_MODULES                       1
#define AM_REG_WDTn(n) \
    (REG_WDT_BASEADDR + 0x00000000 * n)

#endif // AM_REG_H
