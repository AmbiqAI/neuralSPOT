//*****************************************************************************
//
//! @file am_hal_card.h
//!
//! @brief Functions for interfacing with the ambiq card host.
//!
//! @addtogroup card Card Functionality for SD/MMC/eMMC/SDIO
//! @ingroup apollo5b_hal
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
#ifndef AM_HAL_CARD_H
#define AM_HAL_CARD_H

#ifdef __cplusplus
extern "C"
{
#endif

//
// SD/MMC(eMMC)/SDIO protocol HAL driver APIs
//

#define MMC_CMD_GO_IDLE_STATE           0
#define MMC_CMD_SEND_OP_COND            1
#define MMC_CMD_ALL_SEND_CID            2
#define MMC_CMD_SET_RELATIVE_ADDR       3
#define MMC_CMD_SET_DSR                 4
#define MMC_CMD_SLEEP_AWAKE             5
#define MMC_CMD_SWITCH                  6
#define MMC_CMD_SELECT_CARD             7
#define MMC_CMD_SEND_EXT_CSD            8
#define MMC_CMD_SEND_CSD                9
#define MMC_CMD_SEND_CID                10
#define MMC_CMD_STOP_TRANSMISSION       12
#define MMC_CMD_SEND_STATUS             13
#define MMC_CMD_READ_TUNING_BLOCK       14
#define MMC_CMD_SET_BLOCKLEN            16
#define MMC_CMD_READ_SINGLE_BLOCK       17
#define MMC_CMD_READ_MULTIPLE_BLOCK     18
#define MMC_CMD_SEND_TUNING_BLOCK       19
#define MMC_CMD_SEND_TUNING_BLOCK_HS200 21
#define MMC_CMD_SET_BLOCK_COUNT         23
#define MMC_CMD_WRITE_SINGLE_BLOCK      24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK    25
#define MMC_CMD_ERASE_GROUP_START       35
#define MMC_CMD_ERASE_GROUP_END         36
#define MMC_CMD_ERASE                   38
#define MMC_CMD_APP_CMD                 55
#define MMC_CMD_CMD56                   56
#define MMC_CMD_SPI_READ_OCR            58
#define MMC_CMD_SPI_CRC_ON_OFF          59
#define MMC_CMD_RES_MAN                 62

#define SDIO_CMD_IO_SEND_OP_COND        5
#define SDIO_CMD_SEND_IF_COND           8
#define SDIO_CMD_VOLTAGE_SWITCH         11
#define SDIO_CMD_IO_RW_DIRECT           52
#define SDIO_CMD_IO_RW_EXTENDED         53

#define SD_CMD_ERASE_WR_BLK_START       32
#define SD_CMD_ERASE_WR_BLK_END         33
#define SD_CMD_SEND_OP_COND             41

#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1) /* 136 bit response */
#define MMC_RSP_CRC     (1 << 2) /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3) /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4) /* response contains opcode */

#define MMC_RSP_NONE (0)
#define MMC_RSP_R1   (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R1b  (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE | MMC_RSP_BUSY)
#define MMC_RSP_R2   (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC)
#define MMC_RSP_R3   (MMC_RSP_PRESENT)
#define MMC_RSP_R4   (MMC_RSP_PRESENT)
#define MMC_RSP_R5   (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R6   (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R7   (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)

//
// defines for EXT_CSD register in MMC HIGH SPEED cards
//
// Valid POWER_OFF_NOTIFICATION values
#define MMC_EXT_CSD_NO_POWER_NOTIFICATION   (0x00)
#define MMC_EXT_CSD_POWERED_ON              (0x01)
#define MMC_EXT_CSD_POWER_OFF_SHORT         (0x02)
#define MMC_EXT_CSD_POWER_OFF_LONG          (0x03)
#define MMC_EXT_CSD_SLEEP_NOTIFICATION      (0x04)

//
// Supported Bus Width Types
//
#define SD_BUS_WIDTH1     0
#define SD_BUS_WIDTH4     1
#define SD_BUS_WIDTH8     2
#define SD_BUS_WIDTH4_DDR 5
#define SD_BUS_WIDTH8_DDR 6

#define MMC_EXT_MODE_COMMAND_SET (0 << 24)
#define MMC_EXT_MODE_SET_BIT     (1 << 24)
#define MMC_EXT_MODE_CLEAR_BITS  (2 << 24)
#define MMC_EXT_MODE_WRITE_BYTE (3 << 24)

// Added for MMC4.3
#define eMMC_BGA_TYPE 0x01

#define MMC_EXT_REGS_CMD_SET             (191 << 16)
#define MMC_EXT_REGS_POWER_CLASS         (187 << 16)
#define MMC_EXT_REGS_HIGH_SPEED          (185 << 16)
#define MMC_EXT_REGS_BUS_WIDTH           (183 << 16)
#define MMC_EXT_REGS_BOOT_CONFIG         (179 << 16)
#define MMC_EXT_REGS_BOOT_GONFIGPROT     (178 << 16)
#define MMC_EXT_REGS_BOOT_BUSWIDTH       (177 << 16)
#define MMC_EXT_REGS_ERASEGRP_DEFN       (175 << 16)
#define MMC_EXT_REGS_BOOT_WP             (173 << 16)
#define MMC_EXT_REGS_USR_WP              (171 << 16)
#define MMC_EXT_REGS_FW_CONFIG           (169 << 16)
#define MMC_EXT_REGS_SANITIZE_START      165
#define MMC_EXT_REGS_BKOPS_START         164
#define MMC_EXT_REGS_BKOPS_ENABLE        163
#define MMC_EXT_REGS_RST_FUNC            (162 << 16)
#define MMC_EXT_REGS_HPI_MGMT            161
#define MMC_EXT_REGS_PART_ATTRB          (156 << 16)
#define MMC_EXT_REGS_PART_COMPLETE       (155 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_4_2    (154 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_4_1    (153 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_4_0    (152 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_3_2    (151 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_3_1    (150 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_3_0    (149 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_2_2    (148 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_2_1    (147 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_2_0    (146 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_1_2    (145 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_1_1    (144 << 16)
#define MMC_EXT_REGS_GP_SIZE_MULT_1_0    (143 << 16)
#define MMC_EXT_REGS_ENH_SIZE_MULT2      (142 << 16)
#define MMC_EXT_REGS_ENH_SIZE_MULT1      (141 << 16)
#define MMC_EXT_REGS_ENH_SIZE_MULT0      (140 << 16)
#define MMC_EXT_REGS_ENH_START_ADDR      (136 << 16)
#define MMC_EXT_REGS_EXCEPTION_EVTS_CTRL (56 << 16)
#define MMC_EXT_REGS_CONTEXT_CONF        37
#define MMC_EXT_REGS_PWR_OFF_NOTIFY      34
#define MMC_EXT_REGS_CACHE_CTRL          33
#define MMC_EXT_REGS_FLUSH_CACHE         32
#define MMC_EXT_REGS_HS_TIMING           185
#define MMC_EXT_REGS_PARTITON_CONFIG     179
#define MMC_EXT_REGS_RPMB_SIZE_MULT      168
#define MMC_EXT_REGS_BOOT_SIZE_MULT      226

#define MMC_EXP_PACKED_EVENT_EN  0x8
#define MMC_EXP_SYSPOOL_EVENT_EN 0x4
#define MMC_EXP_DYN_CAP_EVENT_EN 0x2
#define MMC_EXT_SEC_SANITIZE     (0x1 << 6)

#define MMC_EXT_SET_BUS_WIDTH1     (SD_BUS_WIDTH1 << 8)
#define MMC_EXT_SET_BUS_WIDTH4     (SD_BUS_WIDTH4 << 8)
#define MMC_EXT_SET_BUS_WIDTH8     (SD_BUS_WIDTH8 << 8)
#define MMC_EXT_SET_BUS_WIDTH4_DDR (SD_BUS_WIDTH4_DDR << 8)
#define MMC_EXT_SET_BUS_WIDTH8_DDR (SD_BUS_WIDTH8_DDR << 8)

#define MMC_EXT_SET_HIGH_SPEED (1 << 8)
#define MMC_EXT_SET_CMD_ATA    (1 << 4)

// Added for MMC4.3

// CARD TYPE FLAGS
#define HS_26MHZ           0x1
#define HS_52MHZ           0x2
#define HS_DDR_52MHZ_18_3V 0x4
#define HS_DDR_52MHZ_12V   0x4
#define HS200_200MHZ_18V   0x10
#define HS200_200MHZ_12V   0x20
#define HS200_TIMING       0x02

// BOOT ACK FLAGS
#define eMMC_BOOT_ACK_EN 0x01
#define eMMC_NO_BOOT_ACK 0x00

// BOOT PARTITION ENABLE FLAGS
#define eMMC_NO_BOOT_EN    0x00
#define eMMC_BOOT_PART1_EN 0x01
#define eMMC_BOOT_PART2_EN 0x02
#define eMMC_BOOT_USER_EN  0x07

// BOOT PARTITION ACCESS FLAGS
#define eMMC_NO_BOOT_ACCESS 0x00
#define eMMC_BOOT1_ACCESS   0x01
#define eMMC_BOOT2_ACCESS   0x02
#define eMMC_RPMB_ACCESS    0x03
#define eMMC_GPP1_ACCESS    0x04
#define eMMC_GPP2_ACCESS    0x05
#define eMMC_GPP3_ACCESS    0x06
#define eMMC_GPP4_ACCESS    0x07
#define eMMC_PARTITION_ACCESS_MASK 0x07

// BOOT BUS WIDTH FLAGS
#define eMMC_RESET_BUSWDITH  0x00
#define eMMC_RETAIN_BUSWDITH 0x01

// BOOT BUS WIDTH MODES
#define SDR_BCKWARD_COMP_BOOT_MOD 0x00
#define SDR_HS_TIMING_BOOT_MODE   0x01
#define DDR_BOOT_MODE             0x02

// BOOT BUS WIDTHS
#define eMMC_BOOT_BUSWIDTH1 0x00
#define eMMC_BOOT_BUSWIDTH4 0x01
#define eMMC_BOOT_BUSWIDTH8 0x02

// Boot Config Protection Flags
#define PERM_BOOT_CONFIG_PROT_EN  0x01
#define PERM_BOOT_CONFIG_PROT_DIS 0x00
#define PWR_BOOT_CONFIG_PROT_EN   0x01
#define PWR_BOOT_CONFIG_PROT_DIS  0x00

// Boot Write Protection Flags
#define BOOT_PWR_WP_DIS  0x01
#define BOOT_PWR_WP_USE  0x00
#define BOOT_PERM_WP_DIS 0x01
#define BOOT_PERM_WP_USE 0x00
#define BOOT_PERM_WP_EN  0x01
#define BOOT_PERM_NO_WP  0x00
#define BOOT_PWR_WP_EN   0x01
#define BOOT_PWR_NO_WP   0x00

// User Write Protection Flags
#define PERM_PSWD_DIS           0x1
#define PERM_PSWD_EN            0x0
#define CD_PERM_WP_DIS          0x1
#define CD_PERM_WP_EN           0x0
#define US_PERM_WP_GRP_DIS      0x1
#define US_PERM_WP_GRP_EN       0x0
#define US_PWR_WP_GRP_DIS       0x1
#define US_PWR_WP_GRP_EN        0x0
#define US_PERM_WP_CMD28GRP_EN  0x1
#define US_PERM_WP_CMD28GRP_DIS 0x0
#define US_PWR_WP_CMD28GRP_EN   0x1
#define US_PWR_WP_CMD28GRP_DIS  0x0

// FW update Flags
#define FW_UPDATE_EN  0x0
#define FW_UPDATE_DIS 0x1

// Reset Function Enable
#define RST_N_TEMP_DIS 0x0
#define RST_N_PERM_EN  0x1
#define RST_N_PERM_DIS 0x2

// Partition Support Flags
#define ENH_ATTRIBUTE_EN  0x1
#define ENH_ATTRIBUTE_DIS 0x0
#define PARTITIONING_EN   0x1
#define PARTITIONING_DIS  0x0

// Partition Atrribute Features
#define SET_ENH_USR            0x1
#define SET_ENH_GPP1           0x2
#define SET_ENH_GPP2           0x4
#define SET_ENH_GPP3           0x8
#define SET_ENH_GPP4           0x10
#define SET_ALL_PART_ATTRB     0x1F
#define SET_ALL_GPP_PART_ATTRB 0x1E

// Partition Completion Flags
#define PARTITION_COMPLETE 0x1

// Ext CSD Read Flags
#define HS_BOOT_MODE_SUPPORT  0x1
#define DDR_BOOT_MODE_SUPPORT (1 << 1)
#define ALT_BOOT_MODE_SUPPORT (1 << 2)
#define SEC_ER_EN             0x1
#define SEC_GB_CL_EN          0x10

// eMMC44 RPMB Flags
#define AUTH_KEY_PGM_REQ 0x0001
#define WR_CNTR_VAL_REQ  0x0002
#define AUTH_DAT_WR_REQ  0x0003
#define AUTH_DAT_RD_REQ  0x0004
#define RESULT_RD_REQ    0x0005

#define AUTH_KEY_PGM_RESP 0x0100
#define WR_CNTR_VAL_RESP  0x0200
#define AUTH_DAT_WR_RESP  0x0300
#define AUTH_DAT_RD_RESP  0x0400

#define OPR_OK                    0x00
#define WRCNT_EXP_OPR_OK          (0x80)
#define GEN_FAILURE               0x01
#define WRCNT_EXP_GEN_FAILURE     (0x81)
#define AUTH_FAILURE              0x02
#define WRCNT_EXP_AUTH_FAILURE    (0x82)
#define COUNTER_FAILURE           0x03
#define WRCNT_EXP_COUNTER_FAILURE (0x83)
#define ADDR_FAILURE              0x04
#define WRCNT_EXP_ADDR_FAILURE    (0x84)
#define WRITE_FAILURE             0x05
#define WRCNT_EXP_WRITE_FAILURE   (0x85)
#define READ_FAILURE              0x06
#define WRCNT_EXP_READ_FAILURE    (0x86)
#define AUTH_KEY_NOT_PGMMED       0x07

#define RPMB_MAC_KEY_LENGTH        32
#define RPMB_NONCE_LENGTH          16
#define RPMB_DATA_LENGTH           256
#define RPMB_STUFF_LENGTH          196
#define RPMB_COUNTER_LENGTH        4
#define RPMB_ADDR_LENGTH           2
#define RPMB_BLKCNT_LENGTH         2
#define RPMB_RESULT_LENGTH         2
#define RPMB_RSPREQ_LENGTH         2
#define TOTAL_MAC_LEN_PER_FRAME    284
#define MACLEN_EXCL_DATA_PER_FRAME 28
#define RPMB_BLKCNT_MAX(x)         ((x)*128*1024/256)

// EMMC ERROR CODES
#define EMMC_INVALID_BOOT_ACK          -26
#define EMMC_INVALID_BOOT_PART         -27
#define EMMC_INVALID_BOOT_ACCESS       -28
#define EMMC_INVALID_BOOT_BUSWIDTH     -29
#define EMMC_INVALID_BOOT_BUSWIDTHFLAG -30
#define EMMC_INVALID_BOOT_BUSMODE      -31
#define EMMC_INVALID_BOOT_CONFIGPROT   -32
#define EMMC_INVALID_BOOT_WPENABLE     -33
#define EMMC_INVALID_USR_WPENABLE      -34
#define EMMC_INVALID_ERASEGRPDEFN      -35
#define EMMC_INVALID_GPP_NO            -36

// Added for eMMC4.51
#define EMMC_CMD38_ERASE        0x00000000
#define EMMC_CMD38_SECURE_ERASE 0x80000000
#define EMMC_CMD38_SECURE_TRIM1 0x80000001
#define EMMC_CMD38_SECURE_TRIM2 0x80008000
#define EMMC_CMD38_TRIM         0x00000001
#define EMMC_CMD38_DISCARD      0x00000003

// SD Card HCS operation define
#define SD_CARD_OCR_MSK_BUSY             0x80000000 // Busy flag
#define SD_CARD_OCR_MSK_HC               0x40000000 // High Capacity flag
#define SD_CARD_OCR_MSK_VOLTAGE_ALL      0x00FF8000 // All Voltage flag
#define SD_CARD_SWITCH_1P8V              0x1000000  // 1 << 24
#define SD_CARD_DEFAULT_OCR              (SD_CARD_OCR_MSK_VOLTAGE_ALL | SD_CARD_OCR_MSK_HC | SD_CARD_SWITCH_1P8V)
#define SD_CARD_1P8V_MSK                 (1 << 24)

// SD Card switch function define
#define SD_CARD_CHECK_FUNCTION           (0 << 31)
#define SD_CARD_SWITCH_FUNCTION          (1 << 31)
#define SD_CARD_SWITCH_ARGUMENT          ((0xF << 4) | (0xFF << 8) | (0xFF << 16))

// SD Card function group bit define
#define SD_CARD_ACCESS_MODE_GROUP         0
#define SD_CARD_COMMAND_GROUP             4
#define SD_CARD_DRIVER_STRENGTH_GROUP     8
#define SD_CARD_POWER_GROUP               12

// SD Card access mode bit define
#define SD_CARD_CHECK_ALL_SPEED            0
#define SD_CARD_DEFAULT_SPEED             (0 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_HIGH_SPEED                (1 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_SDR12                     (0 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_SDR25                     (1 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_SDR50                     (2 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_SDR104                    (3 << SD_CARD_ACCESS_MODE_GROUP)
#define SD_CARD_DDR50                     (4 << SD_CARD_ACCESS_MODE_GROUP)

// SD Card max capacity define
#define SDSC_MAX_BLK_CNT                  0x400000      // SDSC max capacity is 2GB , so max block is 2*1024*1024*1024/512 = 0x400000
#define SDHC_MAX_BLK_CNT                  0x4000000     // SDHC max capacity is 32GB , so max block is 32*1024*1024*1024/512 = 0x4000000
#define SDXC_MAX_BLK_CNT                  0x100000000   // SDXC max capacity is 2TB , so max block is 2*1024*1024*1024*1024/512 = 0x100000000

// SD Card voltage check operation define
#define SD_CARD_CHECK_PATTERN               (0xAA << 0)
#define SD_CARD_2P7_3P6_VOLTAGE_SUPPLY      (1 << 8)
#define SD_CARD_LOW_VOLTAGE_RESERVE         (2 << 8)

#define DEFAULT_CMD6_TIMEOUT_MS  500
#define DEFAULT_GET_STATUS_TIMEOUT_MS  20

#define MMC_STATUS_MASK         (~0x0206BF7F)
#define MMC_STATUS_SWITCH_ERROR (1 << 7)
#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE   (0xf << 9)
#define MMC_STATUS_ERROR        (1 << 19)

// Card voltage setting
#define MMC_VDD_17_18           0x00000020
#define MMC_VDD_18_19           0x00000040
#define MMC_VDD_19_20           0x00000080

// SDIO CCCR Registers
#define SDIO_CCCR_CCCR_REV          0x00
#define SDIO_CCCR_SD_REV            0x01
#define SDIO_CCCR_IO_ENABLE         0x02
#define SDIO_CCCR_IO_RDY            0x03
#define SDIO_CCCR_INT_ENABLE        0x04
#define SDIO_CCCR_INT_RDY           0x05
#define SDIO_CCCR_IO_ABORT          0x06
#define SDIO_CCCR_BUS_CNTRL         0x07
#define SDIO_CCCR_CARD_CAPAB        0x08
#define SDIO_CCCR_CIS_PTR1          0x09
#define SDIO_CCCR_CIS_PTR2          0x0A
#define SDIO_CCCR_CIS_PTR3          0x0B
#define SDIO_CCCR_BUS_SUSPEND       0x0C
#define SDIO_CCCR_FN_SELECT         0x0D
#define SDIO_CCCR_EXEC_FLAGS        0x0E
#define SDIO_CCCR_RDY_FLAGS         0x0F
#define SDIO_CCCR_FN_BLK_SIZE1      0x10
#define SDIO_CCCR_FN_BLK_SIZE2      0x11
#define SDIO_CCCR_PWR_CNTRL         0x12
#define SDIO_CCCR_BUS_SPEED_SELECT  0x13
#define SDIO_CCCR_UHS_SUPPORT       0x14
#define SDIO_CCCR_DRIVER_STRENGTH   0x15

// SDIO Card Capability Register Bits
#define  SDIO_CCCR_CAP_SDC          0x01    /* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB          0x02    /* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW          0x04    /* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS          0x08    /* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI         0x10    /* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI         0x20    /* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC          0x40    /* low speed card */
#define  SDIO_CCCR_CAP_4BLS         0x80    /* 4 bit low speed card */

// SDIO Card Power Control Register Bits
#define  SDIO_POWER_SMPC            0x01    /* Supports Master Power Control */
#define  SDIO_POWER_EMPC            0x02    /* Enable Master Power Control */

// SDIO Card Bus Speed Select Register Bits
#define  SDIO_SPEED_SHS             0x01    /* Supports High-Speed mode */

// SDIO Card Tuple Definitions
#define SDIO_CIS_TPL_NULL           0x00
#define SDIO_CIS_TPL_MANFID         0x20
#define SDIO_CIS_TPL_FUNID          0x21
#define SDIO_CIS_TPL_FUNCE          0x22
#define SDIO_CIS_TPL_END            0xFF

#define SDIO_CARD_MISMATCH_CNT_MAX  0x02

// SDIO Card Speed Setting
#define SDIO_HIGHSPEED_SUPPORT     0x1
#define SDIO_HIGHSPEED_ENABLED     0x3
#define SDIO_FULLSPEED_ENABLED     0x0
#define SDIO_CARD_MAX_SPEED_LIMIT  96000000
#define SDIO_CARD_FULL_SPEED_LIMIT 25000000

// EMMC Speed
#define MMC_LEGACY_HS 26000000
#define MMC_SDR_HS    52000000
#define MMC_DDR_HS    52000000
#define MMC_HS200    200000000

//
// apollo4 SDHC speed limitation settings
//
#define MMC_HS200_MAX_SPEED_LIMIT   96000000
#define MMC_HS_MAX_SPEED_LIMIT      48000000

//
// Internal macros
//
#define SDIO_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH  2 // Acceptable length should be determined based on system level test.
#define AM_HAL_CARD_DEBUG(fmt, ...) am_util_debug_printf("[CARD] line %04d - " fmt, __LINE__, ##__VA_ARGS__)

#define SECTOR_MODE       0x4
#define CARD_RDY_BIT      ((uint32_t)0x1 << 31)

#define SDIO_SCAN_TXDELAY_MAX    16
#define SDIO_SCAN_RXDELAY_MAX    32

//
// SDHC auto cmd23 enable
//
#define ENABLE_SDHC_AUTO_CMD23_FEATURE

static inline uint32_t am_hal_unstuff_bits(uint32_t *resp, uint32_t start, uint32_t size)
{
    const int __size = size;
    const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;
    const int __off = 3 - ((start) / 32);
    const int __shft = (start)&31;
    uint32_t __res;

    __res = resp[__off] >> __shft;
    if (__size + __shft > 32)
    {
        __res |= resp[__off - 1] << ((32 - __shft) % 32);
    }
    return __res &__mask;
}

static inline uint32_t am_hal_unstuff_bytes(uint32_t *ext_csd, uint32_t start, uint32_t size)
{
    uint32_t i;
    const uint32_t __size = size;
    uint32_t __res = 0x0;
    const uint8_t *__p = (const uint8_t *)ext_csd;
    for (i = 0; i < __size; i++)
    {
        __res |= __p[start + i] << 8 * i;
    }
    return __res;
}


typedef enum
{
    AM_HAL_ERASE        =          0x00000000,  // Erase
    AM_HAL_DISCARD      =          0x00000003,  // Discard
    AM_HAL_SECURE_ERASE = (int32_t)0x80000000,  // Secure Erase
    AM_HAL_TRIM         =          0x00000001,  // Trim
    AM_HAL_SECURE_TRIM1 = (int32_t)0x80000001,  // Secure Trim Step 1
    AM_HAL_SECURE_TRIM2 = (int32_t)0x80008000,  // Secure Trim Step 2
} am_hal_card_erase_type_t;

//
// SDHC card types
//
typedef enum
{
    AM_HAL_CARD_TYPE_UNKNOWN,
    AM_HAL_CARD_TYPE_MMC,
    AM_HAL_CARD_TYPE_EMMC,
    AM_HAL_CARD_TYPE_SDSC,
    AM_HAL_CARD_TYPE_SDHC,
    AM_HAL_CARD_TYPE_SDXC,
    AM_HAL_CARD_TYPE_SDIO,
    AM_HAL_CARD_TYPE_COMBO,
} am_hal_card_type_e;

typedef enum
{
    AM_HAL_CARD_STATE_NOT_PRESENT,
    AM_HAL_CARD_STATE_PRESENT,
    AM_HAL_CARD_STATE_IDLE,
    AM_HAL_CARD_STATE_READY,
    AM_HAL_CARD_STATE_IDENT,
    AM_HAL_CARD_STATE_STDY,
    AM_HAL_CARD_STATE_TRANS,
    AM_HAL_CARD_STATE_DATA,
    AM_HAL_CARD_STATE_RCV,
    AM_HAL_CARD_STATE_PRG,
    AM_HAL_CARD_STATE_DIS,
    AM_HAL_CARD_STATE_BTST,
    AM_HAL_CARD_STATE_SLP,
    AM_HAL_CARD_STATE_ERROR,
    AM_HAL_CARD_STATE_PWROFF,
    AM_HAL_CARD_STATE_PWRON,
} am_hal_card_state_e;

typedef struct
{
    am_hal_card_type_e  eType;        // Card type
    uint16_t ui16CmdClass;            // Card command classes
    uint32_t ui32RCA;                 // Relative card address
    uint32_t ui32MaxBlks;             // Card capacity in blocks
    uint32_t ui32BlkSize;             // Block size in bytes
    uint32_t ui32LogiMaxBlks;         // Card logical capacity in blocks
    uint32_t ui32LogiBlkSize;         // Logical block size in bytes
} am_hal_card_info_t;

typedef enum
{
    AM_HAL_CARD_PWR_ON,
    AM_HAL_CARD_PWR_OFF,
    AM_HAL_CARD_PWR_CYCLE,
    AM_HAL_CARD_PWR_SWITCH,
} am_hal_card_pwr_e;

typedef enum
{
    AM_HAL_SD_CARD_3_3_V,
    AM_HAL_SD_CARD_1_8_V,
} am_hal_sd_card_voltage_e;

typedef uint32_t (*am_hal_card_pwr_ctrl_func)(am_hal_card_pwr_e eCardPwr);

typedef uint32_t (*am_hal_sdio_card_reset_func)(am_hal_host_inst_index_e eIndex);

typedef enum
{
    AM_HAL_CARD_PWR_CTRL_NONE,
    AM_HAL_CARD_PWR_CTRL_SDHC_OFF,
    AM_HAL_CARD_PWR_CTRL_SDHC_OFF_AND_CARD_SLEEP,
    AM_HAL_CARD_PWR_CTRL_SDHC_AND_CARD_OFF,
} am_hal_card_pwr_ctrl_policy_e;

typedef enum
{
    AM_HAL_EMMC_USER_PARTITION,
    AM_HAL_EMMC_BOOT1_PARTITION,
    AM_HAL_EMMC_BOOT2_PARTITION,
    AM_HAL_EMMC_RPMB_PARTITION,
} am_hal_card_emmc_partition_e;

typedef struct
{
    uint8_t ui8CccrRev;
    uint8_t ui8SdRev;
    uint8_t bMultiBlock:1,
            bFullSpeed:1,
            bWideBus:1,
            bHighPower:1,
            bHighSpeed:1;
}am_hal_sdio_card_cccr_t;

typedef struct
{
    uint32_t  ui32Vendor;
    uint32_t  ui32Device;
    uint32_t  ui32BlkSize;
    uint32_t  ui32MaxSpeed;
}am_hal_sdio_card_cis_t;

typedef struct
{
    bool        bHighSpeed;
    bool        bWrProtect;
    bool        bSupportLowVolt;
    bool        bSupportDefaultSpeed;
    bool        bSupportHighSpeed;
    bool        bSupportSDR12;
    bool        bSupportSDR25;
    bool        bSupportSDR50;
    bool        bSupportSDR104;
    bool        bSupportDDR50;
    uint32_t    ui32CsdVersion;
    uint32_t    ui32CSize;
    uint32_t    ui32CSizeMult;
    uint32_t    ui32RdBlockLen;
    uint32_t    ui32CurrentVolt;
}am_hal_sd_card_info_t;

//
// SDHC card
//
typedef struct
{
    am_hal_card_type_e  eType;
    am_hal_card_state_e eState;
    uint32_t bCidValid:1;
    uint32_t bCsdValid:1;
    uint32_t bExtCsdValid:1;
    uint32_t ui32OCR;
    uint32_t ui32RCA;
    uint32_t ui32FuncNum;
    uint32_t ui32CID[4];
    uint32_t ui32CSD[4];
    uint32_t ui32ExtCSD[128];
    uint8_t  ui8SpecVer;
    uint8_t  ui8ExtCSDRev;
    uint8_t  ui8PowerOffNotification;
    bool     bHighCapcity;
    uint32_t ui32CacheSize;
    uint32_t ui32SleepNotificationTimeout;
    uint32_t ui32PowerOffNotificationLongTimeout;
    uint32_t ui32GenericCmd6Timeout;
    uint32_t ui32MaxBlks;
    uint32_t ui32Capacity;
    uint8_t  ui8SecureErase;
    uint8_t  ui8DeviceType;
    uint32_t ui32BlkNum;
    bool     bUseBlkEmulation;
    uint32_t ui32NativeBlkSize;
    uint32_t ui32BlkSize;
    uint32_t ui32RpmbSizeMult;
    uint32_t ui32BootMaxBlks;
    uint16_t ui16CmdClass;
    bool     bCardTypeDetect;
    am_hal_card_emmc_partition_e eCurrentPartition;
    am_hal_card_pwr_ctrl_func pCardPwrCtrlFunc;
    am_hal_card_pwr_ctrl_policy_e eCardPwrCtrlPolicy;
    am_hal_card_cfg_t cfg;
    am_hal_card_host_t *pHost;
    am_hal_sdio_card_cccr_t cccr;
    am_hal_sdio_card_cis_t cis;
    am_hal_sd_card_info_t sdcard;
}  am_hal_card_t;

//*****************************************************************************
//
//! @brief Power off the SDHC or eMMC CARD
//!
//! @param pCard        - pointer to the card instance.
//!
//! This function is used to power off the SDHC host controller or eMMC card according
//! to the power policy setting in pCard->eCardPwrCtrlPolicy
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_card_pwrctrl_sleep(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief Power on the SDHC or eMMC CARD
//!
//! @param pCard        - pointer to the card instance.
//!
//! This function is used to power on the SDHC host controller or eMMC card according
//! to the power policy setting in pCard->eCardPwrCtrlPolicy
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_card_pwrctrl_wakeup(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief Get the card instance function
//!
//! @param pHost        - pointer to the card host instance.
//!
//! @param pCard        - pointer to the card instance.
//!
//! This function finds a card and trying to initialize it. if
//! card's initialization succeeds, the card will be 'Trans' state and can do
//! block read, write and erase operations.
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_host_find_card(am_hal_card_host_t *pHost, am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief Set the card operation configurations
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param eType        - card type like eMMC, MMC, SDIO, .etc
//!
//! @param eBusWidth    - card bus width like 1, 4, 8 bit bus width
//!
//! @param ui32Clock    - card bus clock speed
//!
//! @param eIoVoltage   - card bus operation voltage
//!
//! @param eUHSMode     - card UHS mode
//!
//! This function sets the card's type, bus width, clock speed, bus operation voltage and UHS mode
//! but these settings will take effect immediately.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_cfg_set(am_hal_card_t *pCard, am_hal_card_type_e eType,
    am_hal_host_bus_width_e eBusWidth, uint32_t ui32Clock, am_hal_host_bus_voltage_e eIoVoltage,
    am_hal_host_uhs_mode_e eUHSMode);

//*****************************************************************************
//
//! @brief Initialize the card instance function
//!
//! @param pCard              - pointer to the card instance.
//! @param eType              - card type like eMMC, MMC, SDIO, .etc
//! @param pCardPwrCtrlFunc   - pointer to the User defined Card Pwr Ctrl Func
//! @param eCardPwrCtrlPolicy - Power Control Policy can be:
//!     AM_HAL_CARD_PWR_CTRL_NONE
//!     AM_HAL_CARD_PWR_CTRL_SDHC_OFF
//!     AM_HAL_CARD_PWR_CTRL_SDHC_OFF_AND_CARD_SLEEP
//!     AM_HAL_CARD_PWR_CTRL_SDHC_AND_CARD_OFF
//!
//! This function sends a sequence of commands to initalize the card and let the card
//! enter into the 'Trans' state.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_init(am_hal_card_t *pCard, am_hal_card_type_e eType,
                                 am_hal_card_pwr_ctrl_func pCardPwrCtrlFunc,
                                 am_hal_card_pwr_ctrl_policy_e eCardPwrCtrlPolicy);

//*****************************************************************************
//
//! @brief De-Initialize the card instance function
//!
//! @param pCard        - pointer to the card instance.
//!
//! This function resets the card and cleans the card related setting and information.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_deinit(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief synchronous block-oriented read function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - read block count
//!
//! @param pui8Buf      - read buffer
//!
//! This function reads the 'ui32BlkCnt' blocks starting from 'ui32Blk' block and
//! saves the data in the 'pui8Buf' read buffer. The caller will be blocked until all
//! data has been received or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_block_read_sync(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief synchronous block-oriented write function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param pui8Buf      - write buffer
//!
//! This function writes 'ui32BlkCnt' blocks in the 'pui8Buf' write buffer to the card blocks
//! starting from 'ui32Blk' block. The caller will be blocked until all data has been sent
//! out or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_block_write_sync(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief asynchronous block-oriented read function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - read block count
//!
//! @param pui8Buf      - read buffer
//!
//! This function reads the 'ui32BlkCnt' blocks starting from 'ui32Blk' block and
//! saves the data in the 'pui8Buf' read buffer. The caller will not be blocked,
//! Data is ready in the buffer will be notified by the register callback function.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_block_read_async(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief asynchronous block-oriented write function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param pui8Buf      - write buffer
//!
//! This function writes 'ui32BlkCnt' blocks in the 'pui8Buf' write buffer to the card blocks
//! starting from 'ui32Blk' block. The caller will not be blocked, Completion of data transfer
//! will be notified by the register callback function.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_block_write_async(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief emmc scatter write synchronous function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param pIoVec       - pointer to the write vector.
//!
//! @param ui8IovCnt    - write vector count
//!
//! This function writes 'ui8IovCnt' write vector to emmc card and starting from 'ui32Blk'
//! block. The caller will be blocked until all data has been sent out or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_emmc_card_scatter_write_sync(am_hal_card_t *pCard, uint32_t ui32Blk, am_hal_card_iovec_t *pIoVec, uint8_t ui8IovCnt);

//*****************************************************************************
//
//! @brief emmc scatter read synchronous function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param pIoVec       - pointer to the read vector.
//!
//! @param ui8IovCnt    - read vector count
//!
//! This function reads 'ui8IovCnt' read vector starting from 'ui32Blk', and saves the data in 'pIoVec'.
//! The caller will be blocked until all data has been received or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_emmc_card_scatter_read_sync(am_hal_card_t *pCard, uint32_t ui32Blk, am_hal_card_iovec_t *pIoVec, uint8_t ui8IovCnt);

//*****************************************************************************
//
//! @brief emmc scatter write asynchronous function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param pIoVec       - pointer to the write vector.
//!
//! @param ui8IovCnt    - write vector count
//!
//! This function writes 'ui8IovCnt' write vector to emmc card and starting from 'ui32Blk'
//! block. The caller will not be blocked, Completion of data transfer will be notified by
//! the register callback function.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_emmc_card_scatter_write_async(am_hal_card_t *pCard, uint32_t ui32Blk, am_hal_card_iovec_t *pIoVec, uint8_t ui8IovCnt);

//*****************************************************************************
//
//! @brief emmc scatter read asynchronous function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param pIoVec       - pointer to the read vector.
//!
//! @param ui8IovCnt    - read vector count
//!
//! This function reads 'ui8IovCnt' read vector starting from 'ui32Blk', and saves the data in 'pIoVec'.
//! The caller will not be blocked, Completion of data transfer will be notified by
//! the register callback function.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_emmc_card_scatter_read_async(am_hal_card_t *pCard, uint32_t ui32Blk, am_hal_card_iovec_t *pIoVec, uint8_t ui8IovCnt);

//*****************************************************************************
//
//! @brief eMMC erase function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param erasetype    - eMMC erase type
//!         AM_HAL_ERASE: Eraseable unit is Erase Group which defined by ERASE_GROUP_DEF in CSD
//!                       and EXT_CSD, this erase operation remove the mapped device address range to
//!                       unmapped device address range. Controller can perform actual erase at a convenient time.
//!  AM_HAL_SECURE_ERASE: Perform a secure purge according to Secure Removal Type in EXT_CSD. Secure erase to
//!                       avoid unintentional data loss, eraseable uint is Erase Group, erase data on the memory array.
//!                       Host need to wait until the operation is complete before moving to the next device operation.
//!          AM_HAL_TRIM: Perform a secure purge according to Secure Removal Type in EXT_CSD. Similar to
//!                       the AM_HAL_ERASE, but the eraseable unit is write blocks instead of
//!                       erase group, erase block count is ui32BlkCnt.
//!  AM_HAL_SECURE_TRIM1: Mark the write blocks for secure erase, erase block count is ui32BlkCnt.
//!  AM_HAL_SECURE_TRIM2: Perform a secure purge on all write blocks according to Secure Removal Type,
//!                       as well as any copies of those blocks, that were marked during AM_HAL_SECURE_TRIM1, and
//!                       complete the secure trim operation.
//!       AM_HAL_DISCARD: Discard the write blocks, controller can perform partial or full the actual erase
//!                       at a convenient time. A read to a region that was discarded may be return some or
//!                       all of the original data, erase block count is ui32BlkCnt.
//!
//! @param ui32TimeoutMS - erase timeout value in millisecond
//!
//! This function erases 'ui32BlkCnt' blocks staring from 'ui32Blk' block. The caller will be blocked
//! until the erase is done or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_block_erase(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, am_hal_card_erase_type_t erasetype, uint32_t ui32TimeoutMS);

//*****************************************************************************
//
//! @brief Get the card status function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param pui32Status  - the card status
//!
//! This function gets the card status by sending CMD13.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_status(am_hal_card_t *pCard, uint32_t *pui32Status);

//*****************************************************************************
//
//! @brief Get the card state function
//!
//! @param pCard        - pointer to the card instance.
//!
//! This function gets the card state by sending CMD13.
//!
//! @return status      - emmc card state.
//
//*****************************************************************************
extern am_hal_card_state_e am_hal_card_state(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief register the card event call back function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param pfunCallback - function pointer to the call back function.
//!
//! This function registers a card event callback function for async block-oriented
//! read and write functions.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_register_evt_callback(am_hal_card_t *pCard, am_hal_host_event_cb_t pfunCallback);

//*****************************************************************************
//
//! @brief card cid information parse function
//!
//! @param pCard        - pointer to the card instance.
//! @param ui16Offset  - bit start position in the CID data structure.
//! @param ui8Size     - bit length in the CID data structrure.
//!
//! This function parses the specified CID field in the CID data structure and returns
//! as 32bit length integer.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_get_cid_field(am_hal_card_t *pCard, uint16_t ui16Offset, uint8_t ui8Size);

//*****************************************************************************
//
//! @brief card csd information parse function
//!
//! @param pCard        - pointer to the card instance.
//! @param ui16Offset  - bit start position in the CSD data structure.
//! @param ui8Size     - bit length in the CSD data structrure.
//!
//! This function parses the specified CSD field in the CSD data structure and returns
//! as 32bit length integer.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_get_csd_field(am_hal_card_t *pCard, uint16_t ui16Offset, uint8_t ui8Size);

//*****************************************************************************
//
//! @brief card ext csd information parse function
//!
//! @param pCard        - pointer to the card instance.
//! @param ui16Offset  - byte start position in the EXT CSD data structure.
//! @param ui8Size     - byte length in the EXT CSD data structrure.
//!
//! This function parses the specified EXT CSD field in the EXT CSD data structure and returns
//! as 32bit length integer.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_get_ext_csd_field(am_hal_card_t *pCard, uint16_t ui16Offset, uint8_t ui8Size);

//*****************************************************************************
//
//! @brief Get the card information function
//!
//! @param pCard        - pointer to the card instance.
//! @param pCardInfo    - pointer to am_hal_card_info_t structure that contains card info.
//!
//! This function gets the general eMMC card info into am_hal_card_info_t structure.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_get_info(am_hal_card_t *pCard, am_hal_card_info_t *pCardInfo);

//*****************************************************************************
//
//! @brief Set the card mode function
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Mode     - the card mode
//! @param ui32Timeout  - mode switch timeout
//!
//! This function sets the card mode by sending CMD6.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_mode_switch(am_hal_card_t *pCard, uint32_t ui32Mode, uint32_t ui32Timeout);

//*****************************************************************************
//
//! @brief Get the TX/RX delay setting by calibration
//!
//! @param eIndex       - index to the underlying card host instance.
//! @param eUHSMode     - card UHS mode
//! @param ui32Clock    - card bus clock speed
//! @param eBusWidth    - SDIO bus width.
//! @param ui8CalibBuf  - data buffer used to do the calibration
//! @param ui32StartBlk - eMMC start block used to do the calibration
//! @param ui32BlkCnt   - eMMC block number used to do the calibration
//! @param ui8TxRxDelays- an averaged the TX/RX delay values
//!
//! This function get the tx rx delay setting by finding all workable TX/RX delay
//! settings, then an average TX/RX values are returned.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_emmc_calibrate(am_hal_host_inst_index_e eIndex,
                                           am_hal_host_uhs_mode_e eUHSMode,
                                           uint32_t ui32Clock,
                                           am_hal_host_bus_width_e eBusWidth,
                                           uint8_t *ui8CalibBuf,
                                           uint32_t ui32StartBlk,
                                           uint32_t ui32BlkCnt,
                                           uint8_t ui8TxRxDelays[2]);

//*****************************************************************************
//
//! @brief Get the TX/RX delay setting by calibration
//!
//! @param eUHSMode     - card UHS mode
//! @param ui32Clock    - card bus clock speed
//! @param eBusWidth    - SDIO bus width.
//! @param ui8CalibBuf  - data buffer used to do the calibration
//! @param ui32StartBlk - sd card start block used to do the calibration
//! @param ui32BlkCnt   - sd card block number used to do the calibration
//! @param ui8TxRxDelays- an averaged the TX/RX delay values
//! @param pCardPwrCtrlFunc - pointer to the User defined Card Pwr Ctrl Func
//!
//! This function get the tx rx delay setting by finding all workable TX/RX delay
//! settings, then an average TX/RX values are returned.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_calibrate(am_hal_host_inst_index_e eIndex,
                                         am_hal_host_uhs_mode_e eUHSMode,
                                         uint32_t ui32Clock,
                                         am_hal_host_bus_width_e eBusWidth,
                                         uint8_t *ui8CalibBuf,
                                         uint32_t ui32StartBlk,
                                         uint32_t ui32BlkCnt,
                                         uint8_t ui8TxRxDelays[2],
                                         am_hal_card_pwr_ctrl_func pCardPwrCtrlFunc);

//*****************************************************************************
//
//! @brief Read blocks of data from the card (GEN_CMD) asynchronously
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Arg  - command arguments
//! @param pui8Buf  - read buffer
//!
//! @return status  - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_cmd56_read_async(am_hal_card_t *pCard, uint32_t ui32Arg, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief Read blocks of data from the card (GEN_CMD) synchronously
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Arg - command arguments
//! @param pui8Buf  - read buffer
//!
//! @return status  - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_card_cmd56_read_sync(am_hal_card_t *pCard, uint32_t ui32Arg, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief Enable SDIO card function
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Func - SDIO card function number.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_func_enable(am_hal_card_t *pCard, uint32_t ui32Func);

//*****************************************************************************
//
//! @brief Disable SDIO card function
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Func - SDIO card function number.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_func_disable(am_hal_card_t *pCard, uint32_t ui32Func);

//*****************************************************************************
//
//! @brief Enable SDIO card function's interrupt
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Func - SDIO card function number.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_func_interrupt_enable(am_hal_card_t *pCard, uint32_t ui32Func);

//*****************************************************************************
//
//! @brief Disable SDIO card function's interrupt
//!
//! @param pCard    - pointer to the card instance.
//! @param ui32Func - SDIO card function number.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_func_interrupt_disable(am_hal_card_t *pCard, uint32_t ui32Func);

//*****************************************************************************
//
//! @brief Get SDIO card function block size
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param pui32BlkSize - pointer to SDIO card block size.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_get_block_size(am_hal_card_t *pCard, uint32_t ui32Func, uint32_t *pui32BlkSize);

//*****************************************************************************
//
//! @brief Set SDIO card function block size
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32BlkSize  - SDIO card block size.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_set_block_size(am_hal_card_t *pCard, uint32_t ui32Func, uint32_t ui32BlkSize);

//*****************************************************************************
//
//! @brief Read single byte from SDIO card
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - read address
//! @param pui8Data     - read data
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_byte_read(am_hal_card_t *pCard, uint32_t ui32Func, uint32_t ui32Addr, uint8_t *pui8Data);

//*****************************************************************************
//
//! @brief Write single byte to SDIO card
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - write address
//! @param ui8Data      - write data
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_byte_write(am_hal_card_t *pCard, uint32_t ui32Func, uint32_t ui32Addr, uint8_t ui8Data);

//*****************************************************************************
//
//! @brief Read multiple bytes/blocks from SDIO card in sync/blocking mode
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - read address
//! @param pui8Buf      - read buffer
//! @param ui32BlkCnt   - block or byte count
//! @param ui32BlkSize  - block size
//! @param bIncrAddr    - Selection for incrementing/fixed address
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_multi_bytes_read_sync(am_hal_card_t *pCard,
                                                            uint32_t ui32Func,
                                                            uint32_t ui32Addr,
                                                            uint8_t *pui8Buf,
                                                            uint32_t ui32BlkCnt,
                                                            uint32_t ui32BlkSize,
                                                            bool     bIncrAddr);

//*****************************************************************************
//
//! @brief Write multiple bytes/blocks to SDIO card in sync/blocking mode
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - write address
//! @param pui8Buf      - write buffer
//! @param ui32BlkCnt   - block or byte count
//! @param ui32BlkSize  - block size
//! @param bIncrAddr    - Selection for incrementing/fixed address
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_multi_bytes_write_sync(am_hal_card_t *pCard,
                                                             uint32_t ui32Func,
                                                             uint32_t ui32Addr,
                                                             uint8_t *pui8Buf,
                                                             uint32_t ui32BlkCnt,
                                                             uint32_t ui32BlkSize,
                                                             bool     bIncrAddr);

//*****************************************************************************
//
//! @brief Read multiple bytes/blocks from SDIO card in async/non-blocking mode
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - read address
//! @param pui8Buf      - read buffer
//! @param ui32BlkCnt   - block or byte count
//! @param ui32BlkSize  - block size
//! @param bIncrAddr    - Selection for incrementing/fixed address
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_multi_bytes_read_async(am_hal_card_t *pCard,
                                                             uint32_t ui32Func,
                                                             uint32_t ui32Addr,
                                                             uint8_t *pui8Buf,
                                                             uint32_t ui32BlkCnt,
                                                             uint32_t ui32BlkSize,
                                                             bool     bIncrAddr);

//*****************************************************************************
//
//! @brief Write multiple bytes/blocks to SDIO card in async/non-blocking mode
//!
//! @param pCard        - pointer to the card instance.
//! @param ui32Func     - SDIO card function number.
//! @param ui32Addr     - write address
//! @param pui8Buf      - write buffer
//! @param ui32BlkCnt   - block or byte count
//! @param ui32BlkSize  - block size
//! @param bIncrAddr    - Selection for incrementing/fixed address
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_multi_bytes_write_async(am_hal_card_t *pCard,
                                                              uint32_t ui32Func,
                                                              uint32_t ui32Addr,
                                                              uint8_t *pui8Buf,
                                                              uint32_t ui32BlkCnt,
                                                              uint32_t ui32BlkSize,
                                                              bool     bIncrAddr);

//*****************************************************************************
//
//! @brief emmc rpmb read write function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param pui8Buf      - write/read buffer
//!
//! @param bRead        - eMMC rpmb write/read direction
//!
//! @param bRelWrite    - eMMC rpmb is reliable write type.
//!
//! This function only support for eMMC rpmb operation.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t
am_hal_card_block_rpmb_rw(am_hal_card_t *pCard, uint8_t *pui8Buf, bool bRead, bool bRelWrite);

//*****************************************************************************
//
//! @brief eMMC partition switch
//! @param pCard       - pointer to the card instance.
//! @param ePartition  - eMMC partition.
//!
//! This function is used to switch eMMC partition according to the ePartition
//!
//! @return status     - generic or interface specific status.
//
//*****************************************************************************
uint32_t
am_hal_card_emmc_partition_switch(am_hal_card_t *pCard, am_hal_card_emmc_partition_e ePartition);

//*****************************************************************************
//
//! @brief Enable SD Card detect function
//!
//! @param pCard         - pointer to the card instance.
//!
//! @param pfunCallback  - function pointer to the call back function.
//!
//! This function detect the card insert or removal in the slot
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_enable_card_detect(am_hal_card_t *pCard, am_hal_host_event_cb_t pfunCallback);

//*****************************************************************************
//
//! @brief Disable SD Card detect function
//!
//! @param pCard         - pointer to the card instance.
//!
//! This function only support for sd card operation.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_disable_card_detect(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief SD Card write protect detect function
//!
//! @param pCard         - pointer to the card instance.
//!
//! This function detect the card write is protected or enabled in the slot.
//! must config SD card's WP pin in bsp before write protect detection.
//! return 0: write enable, 1:write protect
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_write_protect_detect(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief SD Card get block count function
//!
//! @param pCard         - pointer to the card instance.
//!
//! This function get sd card block count.
//!
//! @return ui32MaxBlks      - sd card block count.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_get_block_count(am_hal_card_t *pCard);

//*****************************************************************************
//
//! @brief SD card synchronous block-oriented read function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - read block count
//!
//! @param pui8Buf      - read buffer
//!
//! This function reads the 'ui32BlkCnt' blocks starting from 'ui32Blk' block and
//! saves the data in the 'pui8Buf' read buffer. The caller will be blocked until all
//! data has been received or failed.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_block_read_sync(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief SD card synchronous block-oriented write function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param pui8Buf      - write buffer
//!
//! This function writes 'ui32BlkCnt' blocks in the 'pui8Buf' write buffer to the card blocks
//! starting from 'ui32Blk' block. The caller will be blocked until all data has been sent
//! out or failed.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_block_write_sync(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief SD card asynchronous block-oriented read function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - read block count
//!
//! @param pui8Buf      - read buffer
//!
//! This function reads the 'ui32BlkCnt' blocks starting from 'ui32Blk' block and
//! saves the data in the 'pui8Buf' read buffer. The caller will not be blocked,
//! Data is ready in the buffer will be notified by the register callback function.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_block_read_async(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief SD card asynchronous block-oriented write function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param pui8Buf      - write buffer
//!
//! This function writes 'ui32BlkCnt' blocks in the 'pui8Buf' write buffer to the card blocks
//! starting from 'ui32Blk' block. The caller will not be blocked, Completion of data transfer
//! will be notified by the register callback function.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_block_write_async(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint8_t *pui8Buf);

//*****************************************************************************
//
//! @brief SD card block-oriented erase function
//!
//! @param pCard        - pointer to the card instance.
//!
//! @param ui32Blk      - start block number
//!
//! @param ui32BlkCnt   - write block count
//!
//! @param ui32TimeoutMS - erase timeout value in millisecond
//!
//! This function erases 'ui32BlkCnt' blocks staring from 'ui32Blk' block. The caller will be blocked
//! until the erase is done or failed.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_sd_card_block_erase(am_hal_card_t *pCard, uint32_t ui32Blk, uint32_t ui32BlkCnt, uint32_t ui32TimeoutMS);

//*****************************************************************************
//
//! @brief Get the TX/RX delay setting by calibration
//!
//! @param eIndex         - index to the underlying card host instance.s
//!
//! @param eUHSMode       - card UHS mode
//!
//! @param ui32Clock      - card bus clock speed
//!
//! @param eBusWidth      - SDIO bus width.
//!
//! @param ui8CalibBuf    - data buffer used to do the calibration
//!
//! @param ui32StartAddr  - SDIO card start address used to do the calibration
//!
//! @param ui32BlockCnt   - SDIO card block number used to do the calibration
//!
//! @param eIoVoltage     - card bus operation voltage
//!
//! @param ui8TxRxDelays  - an averaged the TX/RX delay values
//!
//! @param pSdioCardReset - pointer to the User reset sdio card
//!
//! This function get the tx rx delay setting by finding all workable TX/RX delay
//! settings, then an average TX/RX values are returned.
//!
//! @return status      - generic or interface specific status..
//
//*****************************************************************************
extern uint32_t am_hal_sdio_card_calibrate(am_hal_host_inst_index_e eIndex,
                                    am_hal_host_uhs_mode_e eUHSMode,
                                    uint32_t ui32Clock,
                                    am_hal_host_bus_width_e eBusWidth,
                                    uint8_t *ui8CalibBuf,
                                    uint32_t ui32StartAddr,
                                    uint32_t ui32BlockCnt,
                                    am_hal_host_bus_voltage_e eIoVoltage,
                                    uint8_t ui8TxRxDelays[2],
                                    am_hal_sdio_card_reset_func pSdioCardReset);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CARD_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

