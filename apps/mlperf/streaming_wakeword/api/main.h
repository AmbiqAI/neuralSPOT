// /*
// Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file reflects a modified version of th_lib from EEMBC. The reporting logic
// in th_results is copied from the original in EEMBC.
// ==============================================================================*/
// /// \file
// /// \brief C++ implementations of submitter_implemented.h
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"

#include "am_hal_i2s.h"
#include "am_hal_gpio.h"
#include "ns_uart.h"
#define I2S_MODULE_0 0
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

void        *pI2SHandle;
int16_t AM_SHARED_RW g_i2s_buffer0[1024/sizeof(int16_t)] __attribute__((aligned(32)));
int16_t AM_SHARED_RW g_i2s_buffer1[1024/sizeof(int16_t)] __attribute__((aligned(32)));
volatile i2s_state_t g_i2s_state = Idle;

am_hal_i2s_io_signal_t g_sI2SIOConfig =
{
    .sFsyncPulseCfg =
    {
        .eFsyncPulseType = AM_HAL_I2S_FSYNC_PULSE_ONE_SUBFRAME,
    },
    .eFyncCpol = AM_HAL_I2S_IO_FSYNC_CPOL_LOW,
    .eRxCpol   = AM_HAL_I2S_IO_RX_CPOL_RISING,
};

am_hal_i2s_data_format_t g_sI2SDataConfig =
{
    .ePhase                   = AM_HAL_I2S_DATA_PHASE_SINGLE,
    .ui32ChannelNumbersPhase1 = 2,
	.eChannelLenPhase1        = AM_HAL_I2S_FRAME_WDLEN_32BITS,
    .eDataDelay               = 0x1,
    .eSampleLenPhase1         = AM_HAL_I2S_SAMPLE_LENGTH_16BITS,
    .eDataJust                = AM_HAL_I2S_DATA_JUSTIFIED_LEFT,
};

// Ping pong buffer settings.
am_hal_i2s_transfer_t sTransfer0 = {
    .ui32RxTargetAddr        = (uint32_t)&g_i2s_buffer0[0],
    .ui32RxTargetAddrReverse = (uint32_t)&g_i2s_buffer1[0],
	.ui32RxTotalCount        = 256,
    .ui32TxTargetAddr        = 0,
    .ui32TxTargetAddrReverse = 0,
	.ui32TxTotalCount        = 0,
};

am_hal_i2s_config_t g_sI2S0Config =
{

    .eMode  = AM_HAL_I2S_IO_MODE_SLAVE,
    .eXfer  = AM_HAL_I2S_XFER_RX,
    .eClock = eAM_HAL_I2S_CLKSEL_PLL_FOUT4,
    .eDiv3  = 0,
    .eASRC  = 0,
    .eData  = &g_sI2SDataConfig,
    .eIO    = &g_sI2SIOConfig,
	.eTransfer = &sTransfer0
};

volatile  bool uart_doorbell = false;
volatile bool i2s_doorbell = false;
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = true,
};
am_hal_uart_config_t am_uart_config =
{
    // Standard UART settings: 115200-8-N-1
#if EE_CFG_ENERGY_MODE == 1
    .ui32BaudRate = 9600,
#else
    .ui32BaudRate = 115200,
#endif
    .eDataBits = AM_HAL_UART_DATA_BITS_8,
    .eParity = AM_HAL_UART_PARITY_NONE,
    .eStopBits = AM_HAL_UART_ONE_STOP_BIT,
    .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
    .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
    .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
};

ns_uart_config_t uart_config = {
    .api=&ns_uart_V0_0_1,
    .uart_config = &am_uart_config,
    .tx_blocking = true,
    .rx_blocking = false,
};
static ns_uart_handle_t uart_handle = NULL;


static void uart_stdio_print(char *pcBuf);
int th_vprintf(const char *format, va_list ap) { return am_util_stdio_vprintf(format, ap); }
static void gpio_init();
static void th_serialport_initialize(void);
static void i2s_init(void);
void th_timestamp();
static char th_getchar();
void th_printf(const char *p_fmt, ...);
void helper_sleep(void);