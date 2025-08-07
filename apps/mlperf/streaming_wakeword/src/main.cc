/* Copyright 2020 The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
/// \file
/// \brief Main function to run benchmark on device.

/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "submitter_implemented.h"

#include "am_hal_i2s.h"
#include "am_hal_gpio.h"
#include "ns_uart.h"
#include "sww_ref_util.h"
#define I2S_MODULE_0                (0)


void        *pI2SHandle;
#define     I2S_CHUNK_SAMPLES      2048
extern int16_t g_i2s_buffer0[I2S_CHUNK_SAMPLES];
extern int16_t g_i2s_buffer1[I2S_CHUNK_SAMPLES];
extern int16_t g_wav_block_buff[SWW_WINLEN_SAMPLES];

//*****************************************************************************
//
// I2S configurations:
//  - 2 channels
//  - Standard I2S format
//  - 32 bits word width
//  - 24 bits bit-depth
//  - sample rate:
//    * HFRC   15625Hz
//    * PLL    48000Hz
//    * NCO    48000Hz
//    * HF2ADJ 48000Hz
//
//*****************************************************************************
extern am_hal_i2s_io_signal_t g_sI2SIOConfig;

extern am_hal_i2s_data_format_t g_sI2SDataConfig;

extern am_hal_i2s_config_t g_sI2S0Config;

// Ping pong buffer settings.
extern am_hal_i2s_transfer_t sTransfer0;

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = true,
};

static void gpio_init();
static void i2s_init(void);
// void setup_i2s_buffers();
void th_timestamp();
int main(int argc, char *argv[]) {
    uint32_t uart_status;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_mlperf_mode3), "Power Init Failed.\n");
    NS_TRY(sww_model_init(), "Model init failed.\n");
    gpio_init();
    i2s_init();
    ns_interrupt_master_enable();
    th_serialport_initialize();
    ns_timer_init(&basic_tickTimer);
    th_timestamp();
    while (1) {
        int c;
        c = th_getchar();
        ee_serial_callback(c);
    }
    return 0;
}


static void gpio_init() {
    // timestamp pin
    am_hal_gpio_pinconfig(22, am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
    // processing pin
    am_hal_gpio_pinconfig(29, am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(29, AM_HAL_GPIO_OUTPUT_CLEAR);
    set_processing_pin_high();
    // wakeword detected pin
    am_hal_gpio_pincfg_t ww_det_out = am_hal_gpio_pincfg_output;
    am_hal_gpio_pinconfig(36, ww_det_out);
    am_hal_gpio_state_write(36, AM_HAL_GPIO_OUTPUT_CLEAR);
}

static void i2s_init(void)
{
    // Configure the necessary pins.
    am_bsp_i2s_pins_enable(I2S_MODULE_0, false);
    // Configure I2S0
    am_hal_i2s_initialize(I2S_MODULE_0, &pI2SHandle);

    am_hal_i2s_power_control(pI2SHandle, AM_HAL_I2S_POWER_ON, false);

    if (AM_HAL_STATUS_SUCCESS != am_hal_i2s_configure(pI2SHandle, &g_sI2S0Config))
    {
        th_printf("ERROR: Invalid I2S0 configuration.\nNote: For Apollo5 Rev.B0, I2S can only use PLL as the clock source.\n");
    }

    am_hal_i2s_enable(pI2SHandle);
    am_hal_i2s_dma_configure(pI2SHandle, &g_sI2S0Config, &sTransfer0);
    NVIC_EnableIRQ(I2S0_IRQn);
}


void th_timestamp(void) {
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_CLEAR);
    for (int i=0; i<100000; ++i) {
        __asm__ volatile("nop");
    }
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
}
