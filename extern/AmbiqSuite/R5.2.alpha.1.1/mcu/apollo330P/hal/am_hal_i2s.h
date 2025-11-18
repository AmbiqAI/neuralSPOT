//*****************************************************************************
//
//! @file am_hal_i2s.h
//!
//! @brief HAL implementation for the I2S module.
//!
//! @addtogroup i2s4_ap510L I2S - Inter-IC Sound
//! @ingroup apollo330P_hal
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
#ifndef AM_HAL_I2S_H
#define AM_HAL_I2S_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! CMSIS-style macro for handling a variable I2S module number.
//
//*****************************************************************************
#define I2Sn(n) ((I2S0_Type*)(AM_REG_I2Sn(n)))

//*****************************************************************************
//
//! @name I2S Specific status codes
//! @{
//
//*****************************************************************************
#define AM_HAL_I2S_POWER_ON           AM_HAL_SYSCTRL_WAKE
#define AM_HAL_I2S_POWER_OFF          AM_HAL_SYSCTRL_NORMALSLEEP
//! @}

//
//! The FIFO size for both the TX and RX FIFOs is 64 samples.
//
#define AM_HAL_I2S_FIFO_SIZE          (64)

//
//! Mask and position of the mux selection in am_hal_i2s_clksel_e enumerator.
//
#define AM_HAL_I2S_CLK_CRM_MUX_POS      (0)
#define AM_HAL_I2S_CLK_CRM_MUX_MSK      (0x000000FF)
#define AM_HAL_I2S_CLK_NCO_MUX_POS      (8)
#define AM_HAL_I2S_CLK_NCO_MUX_MSK      (0x0000FF00)
#define AM_HAL_I2S_CLK_OFF              (0xFF000000)

//
//! A software flag to indicate if the clock is an NCO clock.
//
#define AM_HAL_I2S_CLK_NCO_ENABLE       (1)
#define AM_HAL_I2S_CLK_NCO_DISABLE      (0)

//
//! For I2S mono mode, we can define AM_HAL_I2S_CH_NUM_FOR_MONO as 2 to set 2
//! channels, one channel for mono audio data transfer, the other one is unused.
//! We can also define AM_HAL_I2S_CH_NUM_FOR_MONO as 1 to set only 1 channel
//! for the mono audio data transfer.
//
#ifndef AM_HAL_I2S_CH_NUM_FOR_MONO
#define AM_HAL_I2S_CH_NUM_FOR_MONO       (1)
#endif

//*****************************************************************************
//
//! Clock source of I2S module clock
//
//*****************************************************************************
typedef enum
{
    //! HFRC 48 MHz, the actual frequency may change if HFADJ is enabled.
    AM_HAL_I2S_CLKSEL_HFRC_48MHz       = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_HFRC48,

    //! External crystal oscillator.
    AM_HAL_I2S_CLKSEL_XTHS             = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_RF_XTAL_48MHz,

    //! POSTDIV, an output of PLL.
    AM_HAL_I2S_CLKSEL_PLL_POSTDIV      = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_PLLPOSTDIV,

    //! FOUT3, an output of PLL, its frequency is equal to POSTDIV / 6.
    AM_HAL_I2S_CLKSEL_PLL_FOUT3        = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_PLLFOUT3,

    //! FOUT4, an output of PLL, its frequency is equal to POSTDIV / 8.
    AM_HAL_I2S_CLKSEL_PLL_FOUT4        = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_PLLFOUT4,

    //! External clock from GPIO 15.
    AM_HAL_I2S_CLKSEL_EXTREF           = (AM_HAL_I2S_CLK_NCO_DISABLE << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0MCLKCRM_I2S0MCLKCLKSEL_EXTREF_CLK,

    //! HFRC 96 MHz through NCO circuitry, the actual frequency may change if HFADJ is enabled.
    AM_HAL_I2S_CLKSEL_NCO_HFRC_96MHz   = (AM_HAL_I2S_CLK_NCO_ENABLE  << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0REFCLKCRM_I2S0REFCLKCLKSEL_HFRC_96MHz,

    //! External crystal oscillator through NCO circuitry.
    AM_HAL_I2S_CLKSEL_NCO_XTHS         = (AM_HAL_I2S_CLK_NCO_ENABLE  << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0REFCLKCRM_I2S0REFCLKCLKSEL_RF_XTAL_48MHz,

    //! PLL POSTDIV through NCO circuitry.
    AM_HAL_I2S_CLKSEL_NCO_PLL_POSTDIV  = (AM_HAL_I2S_CLK_NCO_ENABLE  << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0REFCLKCRM_I2S0REFCLKCLKSEL_PLLPOSTDIV,

    //! External clock from GPIO 15 through NCO circuitry.
    AM_HAL_I2S_CLKSEL_NCO_EXTREF       = (AM_HAL_I2S_CLK_NCO_ENABLE  << AM_HAL_I2S_CLK_NCO_MUX_POS) | CRM_I2S0REFCLKCRM_I2S0REFCLKCLKSEL_EXTREF_CLK,

    //! A virtual clock for internal use only, shouldn't be set by applications.
    AM_HAL_I2S_CLKSEL_OFF              = AM_HAL_I2S_CLK_OFF,
} am_hal_i2s_clksel_e;

//*****************************************************************************
//
//! Clock source of I2S MCLKOUT, which is used to drive external devices.
//
//*****************************************************************************
typedef enum
{
    //! HFRC 48 MHz, the actual frequency may change if HFADJ is enabled.
    AM_HAL_I2S_MCLKOUT_SEL_HFRC48     = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_HFRC48,

    //! External crystal oscillator.
    AM_HAL_I2S_MCLKOUT_SEL_XTAL       = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_RF_XTAL_48MHz,

    //! POSTDIV, an output of PLL.
    AM_HAL_I2S_MCLKOUT_SEL_PLLPOSTDIV = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_PLLPOSTDIV,

    //! FOUT3, an output of PLL, its frequency is equal to POSTDIV / 6.
    AM_HAL_I2S_MCLKOUT_SEL_PLLFOUT3   = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_PLLFOUT3,

    //! FOUT4, an output of PLL, its frequency is equal to POSTDIV / 8.
    AM_HAL_I2S_MCLKOUT_SEL_PLLFOUT4   = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_PLLFOUT4,

    //! External clock from GPIO 15.
    AM_HAL_I2S_MCLKOUT_SEL_EXTREF     = CRM_I2S0MCLKOUTCRM_I2S0MCLKOUTCLKSEL_EXTREF_CLK,

    //! A virtual clock for internal use only, shouldn't be set by applications.
    AM_HAL_I2S_MCLKOUT_SEL_OFF,
} am_hal_i2s_mclkout_sel_e;

//*****************************************************************************
//
//! @name I2S interrupts macros
//! @{
//
//*****************************************************************************
#define AM_HAL_I2S_INT_RXDMACPL         I2S0_INTSTAT_RXDMACPL_Msk
#define AM_HAL_I2S_INT_TXDMACPL         I2S0_INTSTAT_TXDMACPL_Msk
#define AM_HAL_I2S_INT_TXREQCNT         I2S0_INTSTAT_TXREQCNT_Msk
#define AM_HAL_I2S_INT_RXREQCNT         I2S0_INTSTAT_RXREQCNT_Msk
#define AM_HAL_I2S_INT_IPB              I2S0_INTSTAT_IPB_Msk

#define AM_HAL_I2S_STAT_DMA_TX_ERR      I2S0_TXDMASTAT_TXDMAERR_Msk
#define AM_HAL_I2S_STAT_DMA_RX_ERR      I2S0_TXDMASTAT_TXDMAERR_Msk

#define AM_HAL_I2S_INT_IPBIRPT_TXDMA    I2S0_IPBIRPT_TXDMAM_Msk
#define AM_HAL_I2S_INT_IPBIRPT_RXDMA    I2S0_IPBIRPT_RXDMAM_Msk
#define AM_HAL_I2S_INT_IPBIRPT_TXE      I2S0_IPBIRPT_TXEM_Msk
#define AM_HAL_I2S_INT_IPBIRPT_RXF      I2S0_IPBIRPT_RXFM_Msk
#define AM_HAL_I2S_INT_IPBIRPT_TXFF     I2S0_IPBIRPT_TXFFM_Msk
#define AM_HAL_I2S_INT_IPBIRPT_RXFF     I2S0_IPBIRPT_RXFFM_Msk
//
//! @}
//

//*****************************************************************************
//
// General defines
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief enumeration types for the I2S.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2S_REQ_INTSET               = 0,
    AM_HAL_I2S_REQ_INTCLR               = 1,
    AM_HAL_I2S_REQ_TXFIFOCNT            = 2,
    AM_HAL_I2S_REQ_READ_RXUPPERLIMIT    = 3,
    AM_HAL_I2S_REQ_READ_TXLOWERLIMIT    = 4,
    AM_HAL_I2S_REQ_WRITE_RXUPPERLIMIT   = 5,
    AM_HAL_I2S_REQ_WRITE_TXLOWERLIMIT   = 6,
    //! Overwrite the value AM_HAL_I2S_CH_NUM_FOR_MONO at runtime.
    AM_HAL_I2S_REQ_SET_CH_NUM_FOR_MONO  = 7,
    AM_HAL_I2S_REQ_MAX
} am_hal_i2s_request_e;

//*****************************************************************************
//
//! @brief enumeration types for the I2S clock selection.
//
//*****************************************************************************
typedef enum
{
    //! no change to the clock selected by FSEL.
    AM_HAL_I2S_CLKDIV_1 = 0,

    //! frequency divide-by-3 of the clock selected by FSEL.
    AM_HAL_I2S_CLKDIV_3 = 1
} am_hal_i2s_clk_div_e;

//*****************************************************************************
//
//! @name I2S Available word length/fsync period
//! @{
//
//*****************************************************************************
#define AM_HAL_I2S_WORD_8BITS                  (8)
#define AM_HAL_I2S_WORD_16BITS                 (16)
#define AM_HAL_I2S_WORD_24BITS                 (24)
#define AM_HAL_I2S_WORD_32BITS                 (32)
#define AM_HAL_I2S_WORD_RESERVED               (0)
//
//! @}
//

//*****************************************************************************
//
//! I2S Channel Length
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2S_FRAME_WDLEN_8BITS  = I2S0_I2SDATACFG_WDLEN1_8b,
    AM_HAL_I2S_FRAME_WDLEN_16BITS = I2S0_I2SDATACFG_WDLEN1_16b,
    AM_HAL_I2S_FRAME_WDLEN_24BITS = I2S0_I2SDATACFG_WDLEN1_24b,
    AM_HAL_I2S_FRAME_WDLEN_32BITS = I2S0_I2SDATACFG_WDLEN1_32b,
    AM_HAL_I2S_FRAME_WDLEN_MAX,
} am_hal_i2s_channel_length_e;

//*****************************************************************************
//
//! I2S Sample Length
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2S_SAMPLE_LENGTH_8BITS  = I2S0_I2SDATACFG_SSZ1_8b,
    AM_HAL_I2S_SAMPLE_LENGTH_16BITS = I2S0_I2SDATACFG_SSZ1_16b,
    AM_HAL_I2S_SAMPLE_LENGTH_24BITS = I2S0_I2SDATACFG_SSZ1_24b,
    AM_HAL_I2S_SAMPLE_LENGTH_32BITS = I2S0_I2SDATACFG_SSZ1_32b,
} am_hal_i2s_sample_length_e;

//
//! Available frame size.
//
static const uint32_t ui32I2sWordLength[AM_HAL_I2S_FRAME_WDLEN_MAX] =
{
    AM_HAL_I2S_WORD_8BITS,
    AM_HAL_I2S_WORD_RESERVED,
    AM_HAL_I2S_WORD_16BITS,
    AM_HAL_I2S_WORD_RESERVED,
    AM_HAL_I2S_WORD_24BITS,
    AM_HAL_I2S_WORD_32BITS,
};

//*****************************************************************************
//
//! Specifies the data format of I2S sub frames
//! I2SDATACFG @0x00000040
//!
//! Read Phase Bit. 0: Single Phase frame; 1: Dual-Phase frame.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2S_DATA_PHASE_SINGLE,
    AM_HAL_I2S_DATA_PHASE_DUAL,
} am_hal_i2s_data_phase_e;

//*****************************************************************************
//
//! Audio sample justification. 0: Left-justified, 1: Right-justified
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2S_DATA_JUSTIFIED_LEFT,
    AM_HAL_I2S_DATA_JUSTIFIED_RIGHT,
} am_hal_i2s_data_justified_e;

//*****************************************************************************
//
//! @name Specified polarity and clock configuration of the I2S IPB clocks and
//!   IO signals
//!   REGISTER OFFSET: 0x44
//!
//! @{
//
typedef enum
{
    AM_HAL_I2S_IO_MODE_SLAVE,
    AM_HAL_I2S_IO_MODE_MASTER,
} am_hal_i2s_io_mode_e;

typedef enum
{
    AM_HAL_I2S_IO_RX_CPOL_RISING,
    AM_HAL_I2S_IO_RX_CPOL_FALLING,
} am_hal_i2s_io_rx_cpol_e;

typedef enum
{
    AM_HAL_I2S_IO_TX_CPOL_FALLING,
    AM_HAL_I2S_IO_TX_CPOL_RISING,
} am_hal_i2s_io_tx_cpol_e;

typedef enum
{
    AM_HAL_I2S_IO_FSYNC_CPOL_HIGH,
    AM_HAL_I2S_IO_FSYNC_CPOL_LOW,
} am_hal_i2s_io_fsync_cpol_e;

typedef enum
{
    AM_HAL_I2S_XFER_RX,
    AM_HAL_I2S_XFER_TX,
    AM_HAL_I2S_XFER_RXTX,
} am_hal_i2s_xfer_dir_e;
//! @}

//*****************************************************************************
//
//! @brief Configuration structure for the I2S/TDM Data format.
//
//*****************************************************************************
typedef struct
{
    //
    //! PH: Read Phase Bit. 0: Single Phase frame; 1: Dual-Phase frame.
    //
    am_hal_i2s_data_phase_e ePhase;

    //
    //! FRLEN1: Number of channels in phase 1: N+1
    //
    uint32_t  ui32ChannelNumbersPhase1;

    //
    //! FRLEN2: Number of channels in phase 2: N+1
    //
    uint32_t  ui32ChannelNumbersPhase2;

    //
    //! WDLEN1: channel length in bits for phase 1. 0: 8b, 1: 12b, 2: 16b, 3: 20b, 4: 24b, 5: 32b.
    //
    am_hal_i2s_channel_length_e  eChannelLenPhase1;

    //
    //! WDLEN2: channel length in bits for phase 2
    //
    am_hal_i2s_channel_length_e  eChannelLenPhase2;

    //
    //! DATADLY: Receive data delay bit count. Valid values are 0-2
    //
    uint32_t  eDataDelay;

    //
    //! SSZ1: Receive audio sample length for phase 1. 0: 8b, 1: 12b, 2: 16b, 3: 20b, 4: 24b, 5: 32b.
    //
    am_hal_i2s_sample_length_e  eSampleLenPhase1;

    //
    //! SSZ2: Receive audio sample length for phase 2. 0: 8b, 1: 12b, 2: 16b, 3: 20b, 4: 24b, 5: 32b.
    //
    am_hal_i2s_sample_length_e  eSampleLenPhase2;

    //
    //! JUST: Audio sample justification. 0: Left-justified, 1: Right-justified
    //
    am_hal_i2s_data_justified_e eDataJust;
} am_hal_i2s_data_format_t;

//*****************************************************************************
//
//! @brief The pulse type of the frame sync signal.
//
//*****************************************************************************
typedef enum
{
    //! The pulse width is euqal to the word width of the first channel.
    AM_HAL_I2S_FSYNC_PULSE_ONE_SUBFRAME,

    //! The pulse width is euqal to one bit clock.
    AM_HAL_I2S_FSYNC_PULSE_ONE_BIT_CLOCK,

    //! The pulse width is euqal to 50% of the frame period.
    AM_HAL_I2S_FSYNC_PULSE_HALF_FRAME_PERIOD,

    //! The pulse width is defined by the user.
    AM_HAL_I2S_FSYNC_PULSE_CUSTOM,
} am_hal_i2s_fsync_pulse_type_e;

//*****************************************************************************
//
//! @brief Structure to configure the pulse of the frame sync signal.
//!
//! If the frame sync signal is active high, the pulse width of the frame sync
//! signal is equal to the duration of its active high level.
//! If the frame sync signal is active low, the pulse width of the frame sync
//! signal is equal to the duration of its active low level.
//
//*****************************************************************************
typedef struct
{
    //! The pulse type of the frame sync signal.
    am_hal_i2s_fsync_pulse_type_e eFsyncPulseType;

    //! The pulse width of the frame sync signal in units of sclks.
    //! If the pulse type isn't AM_HAL_I2S_FSYNC_PULSE_CUSTOM, this value won't be used.
    uint32_t ui32FsyncPulseWidth;
} am_hal_i2s_fsync_pulse_t;

//*****************************************************************************
//
//! @brief Configuration structure for the I2S IPB clocks and IO signals.
//
//*****************************************************************************
typedef struct
{
    //
    //! The pulse type/width of the frame sync signal.
    //
    am_hal_i2s_fsync_pulse_t sFsyncPulseCfg;

    //
    //! FSP: Polarity of fsync/lr_clk signal. 0: Active high. 1: Active low
    //
    am_hal_i2s_io_fsync_cpol_e eFyncCpol;

    //
    //! PRx/CLKP: Receive clock edge polarity bit. 0: rising edge. 1: falling edge.
    //
    am_hal_i2s_io_rx_cpol_e eRxCpol;

    //
    //! PRTX: Transmit clock edge polarity bit. 0: sdata starting from the falling edge. 1: sdata starting from the rising edge.
    //
    am_hal_i2s_io_tx_cpol_e eTxCpol;
} am_hal_i2s_io_signal_t;

//*****************************************************************************
//
//! DMA transfer structure
//!
//! @note If the data cache is enabled, ensure that the starting address of the
//! DMA buffer is aligned to 32 bytes to meet the requirements of cache operations.
//!
//! @note If the data cache is enabled, the flushing of the TX DMA buffer should
//! be handled by the user in their application code, the invalidation of the RX
//! DMA buffer is handled by the HAL in the am_hal_i2s_dma_get_buffer() function.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32RxTargetAddr;
    uint32_t ui32RxTargetAddrReverse;
    uint32_t ui32RxTotalCount;

    uint32_t ui32TxTargetAddr;
    uint32_t ui32TxTargetAddrReverse;
    uint32_t ui32TxTotalCount;
} am_hal_i2s_transfer_t;

//*****************************************************************************
//
//! @brief Configuration structure for the I2S.
//
//*****************************************************************************
typedef struct
{
    //
    //! User setting.
    //
    am_hal_i2s_io_mode_e         eMode;
    am_hal_i2s_xfer_dir_e        eXfer;

    am_hal_i2s_clksel_e          eClock;

    //! To determine whether to enable the DIV3 divider between the clock source and the MCLK.
    //! Only works when eClock selects a non-NCO clock.
    uint32_t                     eDiv3;

    //! The divider coefficient between the clock source and the MCLK.
    //! Only works when eClock selects an NCO clock.
    //! @note If the fractional part of f64NcoDiv isn't zero, the jitter of nco_divider output will increase.
    double                       f64NcoDiv;
    uint32_t                     eASRC;

    //
    //! I2S data format.
    //
    am_hal_i2s_data_format_t*    eData;

    //
    //! I2S signal io.
    //
    am_hal_i2s_io_signal_t*      eIO;

    //
    //! DMA transfer.
    //
    am_hal_i2s_transfer_t*      eTransfer;

    uint32_t ui32ClockDivideRatio;

    //
    //! MCLKOUT selection.
    //
    am_hal_i2s_mclkout_sel_e eMclkout;
    uint32_t ui32MclkoutDiv;
} am_hal_i2s_config_t;

//*****************************************************************************
//
// Functions
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief initialize the I2S device controller
//!
//! @param ui32Module - the index to the I2S
//! @param pHandle    - the handle of initialized I2S instance
//!
//! This function should be called firstly before we use any other I2S HAL driver
//! functions.
//!
//! @return status    - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_initialize(uint32_t ui32Module, void **ppHandle);

//*****************************************************************************
//
//! @brief Uninitialize the I2S device controller
//!
//! @param pHandle - the handle of initialized I2S instance
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief I2S Power control function. function
//!
//! @param pHandle      - handle for the I2S.
//! @param ePowerState  - power state requested
//! @param bRetainState - boolean on whether to retain state
//!
//! This function allows advanced settings
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_power_control(void *pHandle, am_hal_sysctrl_power_state_e ePowerState, bool bRetainState);

//*****************************************************************************
//
//! @brief I2S configuration function
//!
//! @param pHandle  - handle for the module instance.
//! @param psConfig - pointer to the configuration structure.
//!
//! This function configures the I2S for operation.
//!
//! @return status  - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_configure(void *pHandle, am_hal_i2s_config_t *psConfig);

//*****************************************************************************
//
//! @brief I2S control function
//!
//! @param pHandle  - handle for the module instance.
//! @param eReq     - request type.
//! @param pArgs    - pointer to the request arguments.
//!
//! @return status  - generic or interface specific status.
//!
//*****************************************************************************
extern uint32_t am_hal_i2s_control(void *pHandle, am_hal_i2s_request_e eReq, void *pArgs);

//*****************************************************************************
//
//! @brief I2S enable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function enables the I2S operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************

extern uint32_t am_hal_i2s_enable(void *pHandle);

//*****************************************************************************
//
//! @brief I2S disable function
//!
//! @param pHandle - handle for the module instance.
//!
//! This function disables the I2S operation.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_disable(void *pHandle);

//*****************************************************************************
//
//! @brief I2S Interrupt Service Routine
//!
//! @param pHandle     - handle for the module instance.
//! @param ui32IntMask - uint32_t for interrupts to clear
//! @param psConfig    - Pointer to the I2S Config
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_interrupt_service(void *pHandle, uint32_t ui32IntMask, am_hal_i2s_config_t* psConfig);

//*****************************************************************************
//
//! @brief I2S IPB Interrupt Service Routine
//!
//! This function is currently called internal to am_hal_i2s_interrupt_service
//! IRB -> full-duplex I2S (stereo TX + stereo RX) using shared CLK & FS
//!
//! @param pHandle     - handle for the module instance.
//! @param ui32IntMask - uint32_t for interrupts to clear
//! @param psConfig    - Pointer to the I2S Config
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_ipb_interrupt_service(void *pHandle);

//*****************************************************************************
//
//! @brief I2S interrupt status function
//!
//! @param pHandle      - handle for the interface.
//! @param pui32Status  - pointer to status
//! @param bEnabledOnly - if I2S enabled
//!
//! This function returns the specific indicated interrupt status.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_interrupt_status_get(void *pHandle, uint32_t *pui32Status, bool bEnabledOnly);

//*****************************************************************************
//
//! @brief I2S interrupt clear
//!
//! @param pHandle     - handle for the interface.
//! @param ui32IntMask - uint32_t for interrupts to clear
//!
//! This function clears the interrupts for the given peripheral.
//!
//! @return status     - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_interrupt_clear(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I2S enable interrupts function
//!
//! @param pHandle       - pointer to the handle for the interface.
//! @param ui32IntMask   - I2S interrupt mask.
//!
//! This function enables the specific indicated interrupts.
//!
//! @return status       - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_interrupt_enable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I2S disable interrupts function
//!
//! @param pHandle       - pointer to the handle for the interface.
//! @param ui32IntMask   - I2S interrupt mask.
//!
//! This function disables the specific indicated interrupts.
//!
//! @return status       - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_interrupt_disable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I2S DMA Complete
//!
//! Gets the DMA status(TXMDASTAT/RXDMASTAT)
//!
//! @param pHandle     - handle for the interface.
//! @param pui32Status - Pointer to the DMA Status
//! @param xfer        - Type of DMA Transfer
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_status_get(void *pHandle, uint32_t *pui32Status, am_hal_i2s_xfer_dir_e rxtx);

//*****************************************************************************
//
//! @brief I2S DMA Transaction Configuration
//!
//! Gets the DMA status(TXMDASTAT/RXDMASTAT)
//!
//! @param pHandle      - handle for the interface.
//! @param psConfig     - Pointer to the I2S Config
//! @param pTransferCfg - Pointer to the I2S Transaction Config
//!
//! @note If the data cache is enabled, ensure that the starting address of the
//! DMA buffer is aligned to 32 bytes to meet the requirements of cache operations.
//!
//! @note If the data cache is enabled, the flushing of the TX DMA buffer should
//! be handled by the user in their application code, the invalidation of the RX
//! DMA buffer is handled by the HAL in the am_hal_i2s_dma_get_buffer() function.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_configure(void *pHandle, am_hal_i2s_config_t* psConfig, am_hal_i2s_transfer_t *pTransferCfg);

//*****************************************************************************
//
//! @brief I2S DMA NonBlocking Transfer Start
//!
//! Gets the DMA status(TXMDASTAT/RXDMASTAT)
//!
//! @param pHandle - handle for the interface.
//! @param pConfig - Pointer to the I2S Config
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_transfer_start(void *pHandle, am_hal_i2s_config_t *pConfig);

//*****************************************************************************
//
//! @brief I2S DMA NonBlocking Transfer Continue
//!
//! Gets the DMA status(TXMDASTAT/RXDMASTAT)
//!
//! @param pHandle - handle for the interface.
//! @param psConfig - Pointer to the I2S Config
//! @param pTransferCfg - Pointer to the I2S Transaction Config
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_transfer_continue(void *pHandle, am_hal_i2s_config_t* psConfig, am_hal_i2s_transfer_t *pTransferCfg);

//*****************************************************************************
//
//! @brief I2S DMA Complete
//!
//! Complete DMA Transfer by writing the DMACFG register to 0 and clearing the
//! DMA TX/RX status register.
//!
//! @param pHandle - handle for the interface.
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_transfer_complete(void *pHandle);

//*****************************************************************************
//
//! @brief I2S DMA Get Buffer
//!
//! @param pHandle - handle for the interface.
//! @param xfer    - Type of DMA Transfer
//!
//! @return Pointer to the DMA Buffer
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_get_buffer(void *pHandle, am_hal_i2s_xfer_dir_e xfer);

//*****************************************************************************
//
//! @brief I2S DMA Error Check
//!
//! @param pHandle - handle for the interface.
//! @param xfer    - Type of DMA Transfer
//!
//! @return AM_HAL_STATUS_SUCCESS
//
//*****************************************************************************
extern uint32_t am_hal_i2s_dma_error(void *pHandle, am_hal_i2s_xfer_dir_e xfer);

//*****************************************************************************
//
//! @brief I2S DMA TX Buffer Empty Check
//!
//! @param pHandle - handle for the interface.
//!
//! @return true if empty, false is not empty
//
//*****************************************************************************
extern bool am_hal_i2s_tx_fifo_empty(void *pHandle);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_I2S_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

