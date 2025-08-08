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
    uart_doorbell = true;
    int c;
    c = th_getchar();
    ee_serial_callback(c);
    uart_doorbell = false;
}

int16_t local_buf[512]__attribute__((aligned(32))) = {0};

extern "C" void am_dspi2s0_isr(void)
{
    uint32_t status;
    am_hal_i2s_interrupt_status_get(pI2SHandle, &status, true);
    am_hal_i2s_interrupt_clear(pI2SHandle, status);

    if (!(status & AM_HAL_I2S_INT_RXDMACPL))
        return;

    int16_t * dma_buf = (int16_t*)am_hal_i2s_dma_get_buffer(pI2SHandle, AM_HAL_I2S_XFER_RX);
    memcpy(local_buf, dma_buf, 512 * sizeof(int16_t));
    am_hal_i2s_interrupt_service(pI2SHandle, status, &g_sI2S0Config);
    switch (g_i2s_state)
    {
        case FileCapture:
            i2s_doorbell = false;
            process_chunk_and_cont_capture(local_buf);
            break;

        case Streaming:
            i2s_doorbell = false;
            process_chunk_and_cont_streaming(local_buf);
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
    i2s_doorbell = true;
}
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
        if(i2s_doorbell) {
            // switch (g_i2s_state)
            // {
            //     case FileCapture:
            //         i2s_doorbell = false;
            //         process_chunk_and_cont_capture(local_buf);
            //         break;

            //     case Streaming:
            //         i2s_doorbell = false;
            //         process_chunk_and_cont_streaming(local_buf);
            //         break;

            //     case Stopping:
            //         th_printf("Streaming stopped\r\n");
            //         g_i2s_state = Idle;
            //         i2s_doorbell = false;
            //         break;

            //     default:
            //         i2s_doorbell = false;
            //         break;
            // }
        }
        ns_deep_sleep();
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

void th_serialport_initialize(void) {
  NS_TRY(ns_uart_init(&uart_config, &uart_handle), 
          "Failed to initialize UART for serial port");
  ns_uart_register_callbacks(uart_handle, my_rx_cb, NULL);
  am_util_stdio_printf_init(uart_stdio_print);
}

static char th_getchar() { 
  char data;
  ns_uart_blocking_receive_data(&uart_config, &data, 1);
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
    // Send the entire null-terminated string over UART:
    size_t len = strlen(pcBuf);
    ns_uart_blocking_send_data(&uart_config, pcBuf, len);
    // We ignore the return count since the prototype is void.
}