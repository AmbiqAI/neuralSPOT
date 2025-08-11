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
#include "sww_ref_util.h"
#include "main.h"

void my_rx_cb(ns_uart_transaction_t *t)
{
    int c;
    c = th_getchar();
    th_printf("1\n"); // debug print
    ee_serial_callback(c);
}

int16_t local_buf[512] __attribute__((aligned(32))) = {0};

extern "C" void am_dspi2s0_isr(void)
{
    th_printf("-");
    uint32_t status;
    am_hal_i2s_interrupt_status_get(pI2SHandle, &status, true);
    am_hal_i2s_interrupt_clear(pI2SHandle, status);

    if (!(status & AM_HAL_I2S_INT_RXDMACPL))
        return;

    int16_t * dma_buf = (int16_t*)am_hal_i2s_dma_get_buffer(pI2SHandle, AM_HAL_I2S_XFER_RX);
    memcpy(local_buf, dma_buf, 512 * sizeof(int16_t));
    am_hal_i2s_interrupt_service(pI2SHandle, status, &g_sI2S0Config);
    i2s_doorbell = true;
}

int main(int argc, char *argv[]) {
    uint32_t uart_status;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_mlperf_mode3), "Power Init Failed.\n");
    // am_bsp_low_power_init();
    // am_hal_cachectrl_icache_enable();
    // am_hal_cachectrl_dcache_enable(true);

    NS_TRY(sww_model_init(), "Model init failed.\n");
    gpio_init();
    th_serialport_initialize();
    i2s_init();
    ns_timer_init(&basic_tickTimer);
    th_timestamp();
    ns_interrupt_master_enable();
    while (1) {
        if(i2s_doorbell) {
            switch (g_i2s_state)
            {
                case FileCapture:
                    process_chunk_and_cont_capture(local_buf);
                    i2s_doorbell = false;
                    break;

                case Streaming:
                    process_chunk_and_cont_streaming(local_buf);
                    i2s_doorbell = false;
                    break;

                case Stopping:
                    th_printf("Streaming stopped\r\n");
                    g_i2s_state = Idle;
                    i2s_doorbell = false;
                    break;

                default:
                    i2s_doorbell = false;
                    break;
            }
        th_printf("+");
        }
        ns_deep_sleep();
        // helper_sleep();
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
    am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_SYSPLL, 24576000, NULL);
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
    NVIC_SetPriority(I2S0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(I2S0_IRQn);

    am_hal_i2s_dma_configure(pI2SHandle, &g_sI2S0Config, &sTransfer0);

}


void th_timestamp(void) {
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_CLEAR);
    for (int i=0; i<100000; ++i) {
        __asm__ volatile("nop");
    }
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
}

void th_serialport_initialize(void) {
  NS_TRY(ns_uart_init(&uart_config, &uart_handle), 
          "Failed to initialize UART for serial port");
  ns_uart_register_callbacks(uart_handle, my_rx_cb, NULL);
  am_util_stdio_printf_init(uart_stdio_print);
}

static char th_getchar() { 
  char data;
  ns_uart_nonblocking_receive_data(&uart_config, &data, 1);
  return data; 
}

void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

static void uart_stdio_print(char *pcBuf)
{
    size_t len = strlen(pcBuf);
    ns_uart_blocking_send_data(&uart_config, pcBuf, len);
}

// void helper_sleep(void)
// {
//     // #if (USE_SLEEP_MODE > USE_SLEEP_MODE_NONE)
//     #if defined(AM_PART_APOLLO5A)
//     PWRCTRL->PWRCNTDEFVAL_b.PWRDEFVALDEVSTMC = 12;
//     #endif

//     // Disable Interrupt and check whether all expected interrupt has been
//     // received. Only enter sleep if we are still expecting interrupt. This is
//     // to prevent entering sleep after all expected interrupt has been received.
//     uint32_t ui32Critical = am_hal_interrupt_master_disable();
//     uint32_t status;
//     am_hal_i2s_interrupt_status_get(pI2SHandle, &status, true);
//     // Check for all expected interrupt
//     if (status & (AM_HAL_I2S_INT_RXDMACPL | AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT))
//     {
//         // Recover master interrupt
//         am_hal_interrupt_master_set(ui32Critical);

//         // All expected interrupt is received. Return without sleep
//         return;
//     }

//     // //Wait for ITM to be idle and turn it off
//     // while (!am_hal_itm_print_not_busy() || !am_hal_itm_not_busy());
//     // am_bsp_itm_printf_disable();

//     //Power Off ROM and OTP so that MCU can enter Deep Sleep
//     am_hal_pwrctrl_rom_disable();
//     am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);

//     // Enter deep sleep
//     am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

//     // MCU Wakeup: re-enable itm printf
//     // am_bsp_itm_printf_enable();


//     // Recover master interrupt
//     am_hal_interrupt_master_set(ui32Critical);

//     // #endif //(USE_SLEEP_MODE > USE_SLEEP_MODE_NONE)
// }