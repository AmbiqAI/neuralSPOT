#!/usr/bin/env python3

# *****************************************************************************
#
#    create_info0.py
#
#    Utility to create INFO0 for Apollo5
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2025, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
#
# *****************************************************************************

import argparse
import sys
import array
import os
import binascii
import importlib
from am_defines import *
import configparser
import mram_info0_regs as info0

#******************************************************************************
#
# Generate the info0 blob as per command line parameters
#
#******************************************************************************
def set_field_in_register(register, field_value, shift, mask):
    register &= ~(mask)
    register |= (field_value << shift) & mask
    return register

def process(defaults: dict):

    hdr_binarray = bytearray([0x00]*OTP_INFO0_SIZE_BYTES)

    # initialize signature
    valid = defaults['valid']
    if (valid == 1):
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE0_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED0)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE1_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED1)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE2_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED2)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE3_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED3)
    else:
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE0_O, FLASH_INVALID)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE1_O, FLASH_INVALID)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE2_O, FLASH_INVALID)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SIGNATURE3_O, FLASH_INVALID)

    am_print("info0 Signature...")
    am_print([hex(hdr_binarray[n]) for n in range(0, 16)])

    # Flash wipe is no longer supported
    if (valid == 1):

        blReset = defaults['BLRESET']
        blReset = set_field_in_register(blReset, defaults['BLRESET'], info0.AM_REG_INFO0_BLRESET_SPINORSLEEPn_S, info0.AM_REG_INFO0_BLRESET_SPINORSLEEPn_M)
        am_print("BLReset = ", hex(blReset))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_BLRESET_O, blReset)

        # Wired UART cfg
        u0 = defaults['SECURITY_WIRED_IFC_CFG0']
        u1 = defaults['SECURITY_WIRED_IFC_CFG1']
        u2 = defaults['SECURITY_WIRED_IFC_CFG2']
        u3 = defaults['SECURITY_WIRED_IFC_CFG3']
        u4 = defaults['SECURITY_WIRED_IFC_CFG4']
        u5 = defaults['SECURITY_WIRED_IFC_CFG5']

        am_print("UART Config ", hex(u0), hex(u1), hex(u2), hex(u3), hex(u4), hex(u5))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG0_O, u0)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG1_O, u1)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG2_O, u2)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG3_O, u3)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG4_O, u4)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_WIRED_IFC_CFG5_O, u5)

        # version
        version = defaults['SECURITY_VERSION_VERSION']
        am_print("Version = ", hex(version))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_VERSION_O, version)

        # SRAM Reservation
        sresv = defaults['SECURITY_SRAM_RESV_SRAM_RESV']
        am_print("SRAM Reservation = ", hex(sresv))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_SRAM_RESV_O, sresv)

        # RMA Override info
        rmaoverride = defaults['SECURITY_RMAOVERRIDE_OVERRIDE']
        am_print("RMA Override info = ", hex(rmaoverride))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SECURITY_RMAOVERRIDE_O, rmaoverride)

        # Wired timeout config
        wiredTimeout = defaults['WIRED_TIMEOUT_TIMEOUT']
        am_print("WiredTimeout = ", hex(wiredTimeout))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_WIRED_TIMEOUT_O, wiredTimeout)

        # SDCert ptr
        sdcert = defaults['SBR_SDCERT_ADDR_ICV']
        am_print("SDCert Ptr = ", hex(sdcert))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_SBR_SDCERT_ADDR_O, sdcert)

        # main ptr
        mainPtr = defaults['MAINPTR_ADDRESS']
        am_print("Main Ptr = ", hex(mainPtr))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_MAINPTR_O, mainPtr)

        # certChain ptr
        certChainPtr = defaults['CERTCHAINPTR_ADDRESS']
        am_print("Cert Chain Ptr = ", hex(certChainPtr))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_CERTCHAINPTR_O, certChainPtr)

        # MRAM Rcvy control
        mramRcvyCtrl = defaults['MRAM_RCVY_CTRL']
        if defaults['MRAM_RCVY_CTRL'] == 0x00000000:
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_MRAM_RCVY_EN'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_MRAM_RCVY_EN_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_MRAM_RCVY_EN_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_GPIO_RCVY_INPROGRESS'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_RCVY_INPROGRESS_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_RCVY_INPROGRESS_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_EMMC_PARTITION'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_EMMC_PARTITION_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_EMMC_PARTITION_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_WD_EN'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_WD_EN_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_WD_EN_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_GPIO_CTRL_POL'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_CTRL_POL_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_CTRL_POL_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_GPIO_CTRL_PIN'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_CTRL_PIN_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_GPIO_CTRL_PIN_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_APP_RCVY_REBOOT'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_APP_RCVY_REBOOT_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_APP_RCVY_REBOOT_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_NV_RCVY_TYPE'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_NV_RCVY_TYPE_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_NV_RCVY_TYPE_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_NV_MODULE_NUM'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_NV_MODULE_NUM_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_NV_MODULE_NUM_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_WIRED_RCVY_EN'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_WIRED_RCVY_EN_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_WIRED_RCVY_EN_M)
            mramRcvyCtrl = set_field_in_register(mramRcvyCtrl, defaults['MRAM_RCVY_CTRL_APP_RCVY_EN'], info0.AM_REG_INFO0_MRAM_RCVY_CTRL_APP_RCVY_EN_S, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_APP_RCVY_EN_M)

        am_print("MRAM Recovery = ", hex(mramRcvyCtrl))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_MRAM_RCVY_CTRL_O, mramRcvyCtrl)

        # MRAM Meta offset
        metaOffset = defaults['NV_METADATA_OFFSET_META_OFFSET']
        am_print("MRAM Offset = ", hex(metaOffset))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_METADATA_OFFSET_O, metaOffset)

        # MRAM Pwr reset
        pwrRstCfg = defaults['NV_PWR_RESET_CFG']
        if defaults['NV_PWR_RESET_CFG'] == 0x00000000:
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_JEDEC_RESET'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_JEDEC_RESET_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_JEDEC_RESET_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_RESET_POL'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_POL_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_POL_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_RESET_DLY_UNITS'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_DLY_UNITS_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_DLY_UNITS_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_RESET_DLY'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_DLY_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_RESET_DLY_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_PWR_POL'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_POL_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_POL_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_PWR_DLY_UNITS'], info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_DLY_UNITS_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_DLY_UNITS_M)
            pwrRstCfg = set_field_in_register(pwrRstCfg, defaults['NV_PWR_RESET_CFG_PWR_DLY'],info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_DLY_S, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_PWR_DLY_M)

        am_print("MRAM Pwr reset = ", hex(pwrRstCfg))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_PWR_RESET_CFG_O, pwrRstCfg)

        # NV pins
        nvPinNum = defaults['NV_PIN_NUMS']
        if defaults['NV_PIN_NUMS'] == 0x00000000:
            nvPinNum = set_field_in_register(nvPinNum, defaults['NV_PIN_NUMS_CE_PIN'], info0.AM_REG_INFO0_NV_PIN_NUMS_CE_PIN_S, info0.AM_REG_INFO0_NV_PIN_NUMS_CE_PIN_M)
            nvPinNum = set_field_in_register(nvPinNum, defaults['NV_PIN_NUMS_RESET_PIN'], info0.AM_REG_INFO0_NV_PIN_NUMS_RESET_PIN_S, info0.AM_REG_INFO0_NV_PIN_NUMS_RESET_PIN_M)
            nvPinNum = set_field_in_register(nvPinNum, defaults['NV_PIN_NUMS_PWR_PIN'], info0.AM_REG_INFO0_NV_PIN_NUMS_PWR_PIN_S, info0.AM_REG_INFO0_NV_PIN_NUMS_PWR_PIN_M)

        am_print("NV Pin nums = ", hex(nvPinNum))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_PIN_NUMS_O, nvPinNum)

        # CE/CMD pins
        cmdPinCfg = defaults['NV_CE_CMD_PINCFG_CE_CMD_PINCGF']
        am_print("CE Pins = ", hex(cmdPinCfg))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CE_CMD_PINCFG_O, cmdPinCfg)

        # NV Clock pins
        clkPinCfg = defaults['NV_CLK_PINCFG_CLK_PINCGF']
        am_print("NV Clock Pins = ", hex(clkPinCfg))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CLK_PINCFG_O, clkPinCfg)

        # NV Data pins
        dataPinCfg = defaults['NV_DATA_PINCFG_DATA_PINCFG']
        am_print("NV Data pins = ", hex(dataPinCfg))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_DATA_PINCFG_O, dataPinCfg)

        # NV DQS pins
        dqsPinCfg = defaults['NV_DQS_PINCFG_DQS_PINCGF']
        am_print("NV DQS pins = ", hex(dqsPinCfg))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_DQS_PINCFG_O, dqsPinCfg)

        # MRAM Recovery NV cfg
        nv0 = defaults['NV_CONFIG0_CONFIG0']
        nv1 = defaults['NV_CONFIG1_CONFIG1']
        nv2 = defaults['NV_CONFIG2_CONFIG2']
        nv3 = defaults['NV_CONFIG3_CONFIG3']

        am_print("NV Config = ", hex(nv0), hex(nv1), hex(nv2), hex(nv3))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CONFIG0_O, nv0)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CONFIG1_O, nv1)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CONFIG2_O, nv2)
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_CONFIG3_O, nv3)

        # NV Recovery options
        nvOpt = defaults['NV_OPTIONS']
        if defaults['NV_OPTIONS'] == 0x00000000:
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_EMMC_BUS_WIDTH'], info0.AM_REG_INFO0_NV_OPTIONS_EMMC_BUS_WIDTH_S, info0.AM_REG_INFO0_NV_OPTIONS_EMMC_BUS_WIDTH_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_EMMC_VOLTAGE'], info0.AM_REG_INFO0_NV_OPTIONS_EMMC_VOLTAGE_S, info0.AM_REG_INFO0_NV_OPTIONS_EMMC_VOLTAGE_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_MSPI_READ_CLKSEL'],info0.AM_REG_INFO0_NV_OPTIONS_MSPI_READ_CLKSEL_S, info0.AM_REG_INFO0_NV_OPTIONS_MSPI_READ_CLKSEL_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_MSPI_WIDTHS'], info0.AM_REG_INFO0_NV_OPTIONS_MSPI_WIDTHS_S, info0.AM_REG_INFO0_NV_OPTIONS_MSPI_WIDTHS_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_MSPI_PRECMD_CLKSEL'], info0.AM_REG_INFO0_NV_OPTIONS_MSPI_PRECMD_CLKSEL_S, info0.AM_REG_INFO0_NV_OPTIONS_MSPI_PRECMD_CLKSEL_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_MSPI_PRECMD_CTRL'], info0.AM_REG_INFO0_NV_OPTIONS_MSPI_PRECMD_CTRL_S, info0.AM_REG_INFO0_NV_OPTIONS_MSPI_PRECMD_CTRL_M)
            nvOpt = set_field_in_register(nvOpt, defaults['NV_OPTIONS_MSPI_READCMD'], info0.AM_REG_INFO0_NV_OPTIONS_MSPI_READCMD_S, info0.AM_REG_INFO0_NV_OPTIONS_MSPI_READCMD_M)

        am_print("Recovery Option = ", hex(nvOpt))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_OPTIONS_O, nvOpt)

        # MSPI pre-commands
        preCMD = defaults['NV_MSPI_PRECMDS']
        if defaults['NV_MSPI_PRECMDS'] == 0x00000000:
            preCMD = set_field_in_register(preCMD, defaults['NV_MSPI_PRECMDS_PCMD1'], info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD1_S, info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD1_M)
            preCMD = set_field_in_register(preCMD, defaults['NV_MSPI_PRECMDS_PCMD2'], info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD2_S, info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD2_M)
            preCMD = set_field_in_register(preCMD, defaults['NV_MSPI_PRECMDS_PCMD4'], info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD4_S, info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD4_M)
            preCMD = set_field_in_register(preCMD, defaults['NV_MSPI_PRECMDS_PCMD3'], info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD3_S, info0.AM_REG_INFO0_NV_MSPI_PRECMDS_PCMD3_M)

        am_print("MSPI Pre-Commands = ", hex(preCMD))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_NV_MSPI_PRECMDS_O, preCMD)

        # MRAM Recovery Max Retry
        maxRcvy = defaults['MRAM_RCV_RETRIES_TIMES']
        if defaults['MRAM_RCV_RETRIES_TIMES'] == 0x00000000:
            maxRcvy = set_field_in_register(maxRcvy, defaults['MRAM_RCV_RETRIES_TIMES_MAX_RETRIES'], info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MAX_RETRIES_S, info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MAX_RETRIES_M)
            maxRcvy = set_field_in_register(maxRcvy, defaults['MRAM_RCV_RETRIES_TIMES_MIN_RETRY_TIME'], info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MIN_RETRY_TIME_S, info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MIN_RETRY_TIME_M)
            maxRcvy = set_field_in_register(maxRcvy, defaults['MRAM_RCV_RETRIES_TIMES_MAX_RETRY_TIME'], info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MAX_RETRY_TIME_S, info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_MAX_RETRY_TIME_M)

        am_print("Max retries = ", hex(maxRcvy))
        fill_word(hdr_binarray, info0.AM_REG_INFO0_MRAM_RCV_RETRIES_TIMES_O, maxRcvy)

    # now output the binary array in the proper order
    output = defaults['output']

    filename, ext = os.path.splitext(output)
    if ext != '.bin':
        output = output + '.bin'

    with open(output, mode = 'wb') as out:
        am_print("Writing to file ", output)
        out.write(hdr_binarray)

def parse_arguments(help_extended=False, formatter=None, suppress_required=False):
    def extended(description):
        return description if help_extended else argparse.SUPPRESS

    parser = argparse.ArgumentParser(description =
                     'Generate apollo330P Info0 Blob', formatter_class=formatter)

    if not suppress_required:
        parser.add_argument('output',
                        help='Output filename (without the extension)')

    parser.add_argument('--valid', dest = 'valid', type=auto_int, choices = [0,1,2], metavar='VALID',
                        help='INFO0 Valid. 0 = Uninitialized, 1 = Valid, 2 = Invalid (Default = 1)')

    parser.add_argument('--blrst', dest = 'BLRESET', type=auto_int, choices = [0,1], metavar='BLRESET',
                        help='This INFO0 register is required for RevB shadow loading (formerly known as SECURITY). (Default = 0x0)')

    parser.add_argument('--u0', dest = 'SECURITY_WIRED_IFC_CFG0', type=auto_int,
                        help='UART Config 0. (Default = 0x00000000)')

    parser.add_argument('--u1', dest = 'SECURITY_WIRED_IFC_CFG1', type=auto_int,
                        help='UART Config 1 (Default = 0x00000000)')

    parser.add_argument('--u2', dest = 'SECURITY_WIRED_IFC_CFG2', type=auto_int,
                        help='UART Config 2 (Default = 0x00000000)')

    parser.add_argument('--u3', dest = 'SECURITY_WIRED_IFC_CFG3', type=auto_int,
                        help='UART Config 3 (Default = 0x00000000)')

    parser.add_argument('--u4', dest = 'SECURITY_WIRED_IFC_CFG4', type=auto_int,
                        help='UART Config 4 (Default = 0x00000000)')

    parser.add_argument('--u5', dest = 'SECURITY_WIRED_IFC_CFG5', type=auto_int,
                        help='UART Config 5 (Default = 0x00000000)')

    parser.add_argument('--version', dest = 'SECURITY_VERSION_VERSION', type=auto_int,
                        help='Version ID. (Default = 0x0)')

    parser.add_argument('--sresv', dest='SECURITY_SRAM_RESV_SRAM_RESV', type=auto_int,
                        help='SRAM Reservation. (Default = 0x0)')

    parser.add_argument('--rma', dest = 'SECURITY_RMAOVERRIDE_OVERRIDE', type=auto_int,
                        help='RMA Override Config 2 = Enabled, 5 = Disabled (Default = 0x2)')

    parser.add_argument('--wTO', dest = 'WIRED_TIMEOUT_TIMEOUT', type=auto_int,
                        help='Wired interface timeout in mS. (Default = 20000)')

    parser.add_argument('--sdcert', dest = 'SBR_SDCERT_ADDR_ICV', type=auto_int,
                        help='Secure Debug Cert Address. (Default = 0x7ff400)')

    parser.add_argument('--main', dest = 'MAINPTR_ADDRESS', type=auto_int,
                        help='Main Firmware location (Default = ' + str(hex(AM_SECBOOT_DEFAULT_NONSECURE_MAIN)) + ')')

    parser.add_argument('--cchain', dest = 'CERTCHAINPTR_ADDRESS', type=auto_int,
                        help='Certificate Chain location (Default = 0x00000000)')

    parser.add_argument('--rcvyCtl', dest='MRAM_RCVY_CTRL', type=auto_int,
                        help=extended('MRAM recovery master enable and enables for recovery types and GPIO pins for status and initiation. (Default = 0x00000000)'))

    parser.add_argument('--rcvyEN', dest='MRAM_RCVY_CTRL_MRAM_RCVY_EN', type=auto_int,
                        help=extended('MRAM recovery master enable. (Default = 0x00000000)'))

    parser.add_argument('--progGPIO', dest='MRAM_RCVY_CTRL_GPIO_RCVY_INPROGRESS', type=auto_int,
                        help=extended('GPIO pin used to indicate MRAM recovery is in progress. (Default = 0x00000000)'))

    parser.add_argument('--emmcPart', dest='MRAM_RCVY_CTRL_EMMC_PARTITION', type=auto_int, choices=[0,1,2], metavar='MRAM_RCVY_CTRL_WD_EN',
                        help=extended('When configured for MRAM recovery from eMMC, this field specifies the partition where the recovery image is stored. 0 = User, 1 = BOOT1, 2 = BOOT2 (Default = 0x0)'))

    parser.add_argument('--wdtEn', dest='MRAM_RCVY_CTRL_WD_EN', type=auto_int,
                        help=extended('Watchdog Timer will be enabled with a timeout of 2 seconds prior to the handoff from the Bootrom/SBL to the OEM\'s Application or recovery image. (Default = 0x00000000)'))

    parser.add_argument('--rcvyPol', dest='MRAM_RCVY_CTRL_GPIO_CTRL_POL', type=auto_int,
                        help=extended('Polarity for the MRAM recovery control pin. (Default = 0x00000000)'))

    parser.add_argument('--rcvyPin', dest='MRAM_RCVY_CTRL_GPIO_CTRL_PIN', type=auto_int,
                        help=extended('GPIO pin used to initiate MRAM recovery. (Default = 0x00000000)'))

    parser.add_argument('--rcvyRbt', dest='MRAM_RCVY_CTRL_APP_RCVY_REBOOT', type=auto_int,
                        help=extended('If MRAM recovery fails, 1 = device will continue to boot, without having done MRAM recovery. 0 = device will lock, until a reset is issued. (Default = 0x0)'))

    parser.add_argument('--rcvyTyp', dest='MRAM_RCVY_CTRL_NV_RCVY_TYPE', type=auto_int,
                        help=extended('MRAM recovery NV device type. 0 = NV_OFF, 1 = MSPI, 2 = EMMC (Default = 0x0)'))

    parser.add_argument('--rcvyModNum', dest='MRAM_RCVY_CTRL_NV_MODULE_NUM', type=auto_int,
                        help=extended('Module number for the selected NV device. (Default = 0x00000000)'))

    parser.add_argument('--rcvyWire', dest='MRAM_RCVY_CTRL_WIRED_RCVY_EN', type=auto_int,
                        help=extended('Wired MRAM recovery enable. 0 = Disabled, 1 Enabled (Default = 0x0)'))

    parser.add_argument('--rcvyAppEN', dest='MRAM_RCVY_CTRL_APP_RCVY_EN', type=auto_int,
                        help=extended('Application initiated MRAM recovery enable. 0 = Disabled, 1 Enabled (Default = 0x00000000)'))

    parser.add_argument('--meta', dest='NV_METADATA_OFFSET_META_OFFSET', type=auto_int,
                        help=extended('Offset to the meta-data in the selected NV device. (Default = 0x00000000)'))

    parser.add_argument('--pwrRstCfg', dest='NV_PWR_RESET_CFG', type=auto_int,
                        help=extended('MRAM recovery NV device power and reset types, polarity and timing configurations. (Default = 0x00000000)'))

    parser.add_argument('--jedecRst', dest='NV_PWR_RESET_CFG_JEDEC_RESET', type=auto_int,
                        help=extended('NV device will be reset using the JEDEC standard for the NV device selected. (Default = 0x0)'))

    parser.add_argument('--pwrRstPol', dest='NV_PWR_RESET_CFG_RESET_POL', type=auto_int,
                        help=extended('Polarity of the RESET pin, if enabled. 0 = Reset is active low (Default = 0x0)'))

    parser.add_argument('--pwrRstDlyUnt', dest='NV_PWR_RESET_CFG_RESET_DLY_UNITS', type=auto_int, choices=[0, 1], metavar='NV_PWR_RESET_CFG_RESET_DLY_UNITS',
                        help=extended('The units of time that the RESET_DLY field represents. 0 = uS, 1 = mS (Default = 0x0)'))

    parser.add_argument('--pwrRstDly', dest='NV_PWR_RESET_CFG_RESET_DLY', type=auto_int,
                        help=extended('The delay following the release of reset to allow for the NV device to process the reset. (Default = 0x00000000)'))

    parser.add_argument('--pwrPol', dest='NV_PWR_RESET_CFG_PWR_POL', type=auto_int,
                        help=extended('Polarity of the Power Pin for the MRAM recovery NV device. 1 = set high to power on the device. (Default = 0x0)'))

    parser.add_argument('--pwrDlyUnt', dest='NV_PWR_RESET_CFG_PWR_DLY_UNITS', type=auto_int, choices=[0, 1], metavar='NV_PWR_RESET_CFG_PWR_DLY_UNITS',
                        help=extended('The units of time that the PWR_DLY field represents. 0 = uS, 1 = mS (Default = 0x0)'))

    parser.add_argument('--pwrDly', dest='NV_PWR_RESET_CFG_PWR_DLY', type=auto_int,
                        help=extended('The delay following powering ON the MRAM recovery NV device before preceding with reset - if enable. (Default = 0x00000000)'))

    parser.add_argument('--nvPin', dest='NV_PIN_NUMS', type=auto_int,
                        help=extended('MRAM recovery NV device pin numbers Power, Reset and CE pins. (Default = 0x00000000)'))

    parser.add_argument('--nvCEPin', dest='NV_PIN_NUMS_CE_PIN', type=auto_int,
                        help=extended('The pin number for the CE pin used to connect to external MSPI MRAM Recovery device. (Default = 0x00000000)'))

    parser.add_argument('--nvRstPin', dest='NV_PIN_NUMS_RESET_PIN', type=auto_int,
                        help=extended('Pin number for the MRAM recovery NV device\'s reset pin. (Default = 0x00000000)'))

    parser.add_argument('--nvPwrPin', dest='NV_PIN_NUMS_PWR_PIN', type=auto_int,
                        help=extended('Pin number for the MRAM recovery NV device\'s power control pin. (Default = 0x00000000)'))

    parser.add_argument('--cmdPinCfg', dest='NV_CE_CMD_PINCFG_CE_CMD_PINCGF', type=auto_int,
                        help=extended('Pin config for the CE pin (MSPI) or CMD and D0 pins (EMMC) for MRAM recovery device. For EMMC devices the FNCSELxx field is ignored and set automatically based on the EMMC module number specified. (Default = 0x00000000)'))

    parser.add_argument('--clkPinCfg', dest='NV_CLK_PINCFG_CLK_PINCGF', type=auto_int,
                        help=extended('The configuration to be written to the PINCFG register for the CLK pin for MRAM recovery. This applies to both MSPI and EMMC device types. For EMMC devices the FNCSELxx field is ignored and set automatically based on the EMMC module number specified. (Default = 0x00000000)'))

    parser.add_argument('--dataPinCfg', dest='NV_DATA_PINCFG_DATA_PINCFG', type=auto_int,
                        help=extended('The configuration to be written to the PINCFG register for the device\'s data pins. This applies to both MSPI and EMMC device types. For EMMC devices the FNCSELxx field is ignored and set automatically based on the EMMC module number specified. (Default = 0x00000000)'))

    parser.add_argument('--dqsPinCfg', dest='NV_DQS_PINCFG_DQS_PINCGF', type=auto_int,
                        help=extended('The configuration to be written to the PINCFG register for the DQS pin in the MRAM recovery device. For EMMC devices this is not used. (Default = 0x00000000)'))

    parser.add_argument('--nv0', dest='NV_CONFIG0_CONFIG0', type=auto_int,
                        help=extended('MRAM NV Config 0 (Default = 0x00000000)'))

    parser.add_argument('--nv1', dest='NV_CONFIG1_CONFIG1', type=auto_int,
                        help=extended('MRAM NV Config 1 (Default = 0x00000000)'))

    parser.add_argument('--nv2', dest='NV_CONFIG2_CONFIG2', type=auto_int,
                        help=extended('MRAM NV Config 2 (Default = 0x00000000)'))

    parser.add_argument('--nv3', dest='NV_CONFIG3_CONFIG3', type=auto_int,
                        help=extended('MRAM NV Config 3 (Default = 0x00000000)'))

    parser.add_argument('--nvOpt', dest='NV_OPTIONS', type=auto_int,
                        help=extended('Device specific options for the selected MRAM recovery device type [EMMC or MSPI]. (Default = 0x00000000)'))

    parser.add_argument('--emmcWidth', dest='NV_OPTIONS_EMMC_BUS_WIDTH', type=auto_int,
                        help=extended('Data width to be used when NV is configured for EMMC. (Default = 0x00000000)'))

    parser.add_argument('--emmcVolt', dest='NV_OPTIONS_EMMC_VOLTAGE', type=auto_int,
                        help=extended('I/O voltage to be used when NV is configured for EMMC. (Default = 0x00000000)'))

    parser.add_argument('--mspiRdClkSel', dest='NV_OPTIONS_MSPI_READ_CLKSEL', type=auto_int,
                        help=extended('Configures the CLKSEL field in the CLKCTRL register for the MSPI module to be used for the metadata and recovery image read operations. (Default = 0x00000000)'))

    parser.add_argument('--mspiWidth', dest='NV_OPTIONS_MSPI_WIDTHS', type=auto_int,
                        help=extended('Configures the address and data transfer widths . (Default = 0x00000000)'))

    parser.add_argument('--mspiPreClkSel', dest='NV_OPTIONS_MSPI_PRECMD_CLKSEL', type=auto_int,
                        help=extended('Configures the CLKSEL field in the CLKCTRL register for the MSPI module to be used for the precommands. (Default = 0x00000000)'))

    parser.add_argument('--mspiPreCtl', dest='NV_OPTIONS_MSPI_PRECMD_CTRL', type=auto_int,
                        help=extended('Controls what types of pre-commands are sent before the reading of the metadata and recovery images. (Default = 0x00000000)'))

    parser.add_argument('--mspiRdCmd', dest='NV_OPTIONS_MSPI_READCMD', type=auto_int,
                        help=extended('Read command to be sent to the device to read the Metadata and the recovery image. (Default = 0x00000000)'))

    parser.add_argument('--preCMDs', dest='NV_MSPI_PRECMDS', type=auto_int,
                        help=extended('Pre-Commands that loads the recovery image(s) for MSPI NV devices. (Default = 0x00000000)'))

    parser.add_argument('--preCMD1', dest='NV_MSPI_PRECMDS_PCMD1', type=auto_int,
                        help=extended('The first 8-bit pre-command to be sent, this will always be sent as CMD to the device. (Default = 0x00000000)'))

    parser.add_argument('--preCMD2', dest='NV_MSPI_PRECMDS_PCMD2', type=auto_int,
                        help=extended('The second 8-bit byte sent (as a pre-command or data). (Default = 0x00000000)'))

    parser.add_argument('--preCMD3', dest='NV_MSPI_PRECMDS_PCMD3', type=auto_int,
                        help=extended('The third 8-bit byte sent (as a pre-command or data). (Default = 0x00000000)'))

    parser.add_argument('--preCMD4', dest='NV_MSPI_PRECMDS_PCMD4', type=auto_int,
                        help=extended('The fourth 8-bit byte sent (as a pre-command or data). (Default = 0x00000000)'))

    parser.add_argument('--retryWD', dest='MRAM_RCV_RETRIES_TIMES', type=auto_int,
                        help=extended('The number of retries if MRAM recovery fails and the times between retries. (Default = 0x00000000)'))

    parser.add_argument('--maxRetry', dest='MRAM_RCV_RETRIES_TIMES_MAX_RETRIES', type=auto_int,
                        help=extended('The maximum number of retry attempts before stopping and requiring a reset. (Default = 0x00000000)'))

    parser.add_argument('--minRetryTm', dest='MRAM_RCV_RETRIES_TIMES_MIN_RETRY_TIME', type=auto_int,
                        help=extended('The minimum time between retries in seconds. (Default = 0x00000000)'))

    parser.add_argument('--maxRetryTm', dest='MRAM_RCV_RETRIES_TIMES_MAX_RETRY_TIME', type=auto_int,
                        help=extended('The maximum time between retries in minutes. (Default = 0x00000000)'))

    parser.add_argument('--info0Cfg', dest='info0Cfg', type=str,
                        help='Relative path to INFO0 configuration file')

    parser.add_argument('--loglevel', dest='loglevel', type=auto_int, default=AM_PRINT_LEVEL_INFO,
                        choices = range(AM_PRINT_LEVEL_MIN, AM_PRINT_LEVEL_MAX+1), metavar='LOG_LEVEL',
                        help=helpPrintLevel)

    parser.add_argument('-hx', '--help-extended', action='store_true',
                        help='show the full list of options')

    return parser

def parse_config(configFile: str, defaults: dict):
    config = configparser.ConfigParser()

    # Preserve capitalization in config file
    config.optionxform = str

    if os.path.exists(configFile):
        config.read(configFile)
    else:
        raise FileNotFoundError(f"File '{configFile}' not found.")

    for key in config['DEFAULT']:
        if key in defaults:
            defaults[key] = auto_int(config['DEFAULT'][key])
        else:
            raise KeyError(f"'{key}' is not a valid info0 field. Please refer to info0_defaults.")

def validate_config_choices(args, defaults: dict):
    for key, value in defaults.items():
        # Not all arguments have restrictions for their entries
        choices = None
        try:
            choices = args._option_string_actions[f"--{key}"].choices
        except:
            continue

        if not args._option_string_actions[f"--{key}"].dest:
            raise KeyError(f"{key} was found in info0 config, but argument has not been added.")

        if choices and (value not in choices):
            raise ValueError(f"Invalid value given '{value}' for '{key}'. "
                                f"Valid choices for '{key}' are {choices}")

def initialize_defaults():
    info0_defaults = {
        'output': 'info0',
        'valid': 1,
        'BLRESET': 0x00000000,
        'SECURITY_WIRED_IFC_CFG0': 0x00000000,
        'SECURITY_WIRED_IFC_CFG1': 0x00000000,
        'SECURITY_WIRED_IFC_CFG2': 0x00000000,
        'SECURITY_WIRED_IFC_CFG3': 0x00000000,
        'SECURITY_WIRED_IFC_CFG4':  0x00000000,
        'SECURITY_WIRED_IFC_CFG5': 0x00000000,
        'SECURITY_VERSION_VERSION': 0,
        'SECURITY_SRAM_RESV_SRAM_RESV': 0x0,
        'SECURITY_RMAOVERRIDE_OVERRIDE': 0x2,
        'WIRED_TIMEOUT_TIMEOUT': 2000,
        'SBR_SDCERT_ADDR_ICV': 0x500000,
        'MAINPTR_ADDRESS': AM_SECBOOT_DEFAULT_NONSECURE_MAIN,
        'CERTCHAINPTR_ADDRESS': 0x00000000,
        'MRAM_RCVY_CTRL': 0x00000000,
        'MRAM_RCVY_CTRL_MRAM_RCVY_EN': 0x00000000,
        'MRAM_RCVY_CTRL_GPIO_RCVY_INPROGRESS': 0x00000000,
        'MRAM_RCVY_CTRL_EMMC_PARTITION': 0x00000000,
        'MRAM_RCVY_CTRL_WD_EN': 0x00000000,
        'MRAM_RCVY_CTRL_GPIO_CTRL_POL': 0x00000000,
        'MRAM_RCVY_CTRL_GPIO_CTRL_PIN': 0x00000000,
        'MRAM_RCVY_CTRL_APP_RCVY_REBOOT': 0x00000000,
        'MRAM_RCVY_CTRL_NV_RCVY_TYPE': 0x00000000,
        'MRAM_RCVY_CTRL_NV_MODULE_NUM': 0x00000000,
        'MRAM_RCVY_CTRL_WIRED_RCVY_EN': 0x00000000,
        'MRAM_RCVY_CTRL_APP_RCVY_EN': 0x00000000,
        'NV_METADATA_OFFSET_META_OFFSET': 0x00000000,
        'NV_PWR_RESET_CFG': 0x00000000,
        'NV_PWR_RESET_CFG_JEDEC_RESET': 0x00000000,
        'NV_PWR_RESET_CFG_RESET_POL': 0x00000000,
        'NV_PWR_RESET_CFG_RESET_DLY_UNITS': 0x00000000,
        'NV_PWR_RESET_CFG_RESET_DLY': 0x00000000,
        'NV_PWR_RESET_CFG_PWR_POL': 0x00000000,
        'NV_PWR_RESET_CFG_PWR_DLY_UNITS': 0x00000000,
        'NV_PWR_RESET_CFG_PWR_DLY': 0x00000000,
        'NV_PIN_NUMS': 0x00000000,
        'NV_PIN_NUMS_CE_PIN': 0x00000000,
        'NV_PIN_NUMS_RESET_PIN': 0x00000000,
        'NV_PIN_NUMS_PWR_PIN': 0x00000000,
        'NV_CE_CMD_PINCFG_CE_CMD_PINCGF': 0x00000000,
        'NV_CLK_PINCFG_CLK_PINCGF': 0x00000000,
        'NV_DATA_PINCFG_DATA_PINCFG': 0x00000000,
        'NV_DQS_PINCFG_DQS_PINCGF': 0x00000000,
        'NV_CONFIG0_CONFIG0': 0x00000000,
        'NV_CONFIG1_CONFIG1': 0x00000000,
        'NV_CONFIG2_CONFIG2': 0x00000000,
        'NV_CONFIG3_CONFIG3': 0x00000000,
        'NV_OPTIONS': 0x00000000,
        'NV_OPTIONS_EMMC_BUS_WIDTH': 0x00000000,
        'NV_OPTIONS_EMMC_VOLTAGE': 0x00000000,
        'NV_OPTIONS_MSPI_READ_CLKSEL': 0x00000000,
        'NV_OPTIONS_MSPI_WIDTHS': 0x00000000,
        'NV_OPTIONS_MSPI_PRECMD_CLKSEL': 0x00000000,
        'NV_OPTIONS_MSPI_PRECMD_CTRL': 0x00000000,
        'NV_OPTIONS_MSPI_READCMD': 0x00000000,
        'NV_MSPI_PRECMDS': 0x00000000,
        'NV_MSPI_PRECMDS_PCMD1': 0x00000000,
        'NV_MSPI_PRECMDS_PCMD2': 0x00000000,
        'NV_MSPI_PRECMDS_PCMD3': 0x00000000,
        'NV_MSPI_PRECMDS_PCMD4': 0x00000000,
        'MRAM_RCV_RETRIES_TIMES': 0x00000000,
        'MRAM_RCV_RETRIES_TIMES_MAX_RETRIES': 0x00000000,
        'MRAM_RCV_RETRIES_TIMES_MIN_RETRY_TIME': 0x00000000,
        'MRAM_RCV_RETRIES_TIMES_MAX_RETRY_TIME': 0x00000000,
        'info0Cfg': None,
    }

    return info0_defaults

#******************************************************************************
#
# Main function.
#
#******************************************************************************
def main():
    # format help message to extend the maximum space limit for long metavar
    formatter = lambda prog: argparse.HelpFormatter(prog,max_help_position=60)

    # Preparser to check for --help-extended
    preParser = parse_arguments(formatter=formatter, suppress_required=True)
    preArgs, remainingArgs = preParser.parse_known_args()

    if preArgs.help_extended:
        parse_arguments(help_extended=True, formatter=formatter).print_help()
        raise SystemExit

    # Read the arguments.
    parser = parse_arguments(formatter=formatter)
    args = parser.parse_args()
    args.parser = parser

    am_set_print_level(args.loglevel)

    info0_defaults = initialize_defaults()

    # Read config fields and update the values within the dictionary
    if args.info0Cfg:
        parse_config(args.info0Cfg, info0_defaults)

        # Ensure config values match the value restrictions used in argument parser
        validate_config_choices(args.parser, info0_defaults)

    # Giving highest precedence to cmd line args, update the values within the dictionary
    for key,value in vars(args).items():
        if value == None:
            continue
        info0_defaults[key] = value

    process(info0_defaults)

if __name__ == '__main__':
    main()
