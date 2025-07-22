/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"
#include "ns_timer.h"
#include "ns_ambiqsuite_harness.h"
#include "submitter_implemented.h"

// #include <cstdarg>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>

#include "tensorflow/lite/micro/debug_log.h"


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
    .rx_blocking = true,
};
static ns_uart_handle_t uart_handle = NULL;
static char txbuffer[256];

static void uart_stdio_print(char *pcBuf)
{
    // Send the entire null-terminated string over UART:
    size_t len = strlen(pcBuf);
    ns_uart_blocking_send_data(&uart_config, pcBuf, len);
    // We ignore the return count since the prototype is void.
}

// void th_command_ready(char volatile *p_command) {
//   p_command = p_command;
//   ee_serial_command_parser_callback((char *)p_command);
// }

char *th_strncpy(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}

size_t th_strnlen(const char *str, size_t maxlen) {
  return strnlen(str, maxlen);
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}

int th_vprintf(const char *format, va_list ap) { return am_util_stdio_vprintf(format, ap); }
void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

char th_getchar() { 
  char data;
  ns_uart_blocking_receive_data(&uart_config, &data, 1);
  return data; 
}

void th_serialport_initialize(void) {
  NS_TRY(ns_uart_init(&uart_config, &uart_handle), 
          "Failed to initialize UART for serial port");
  am_util_stdio_printf_init(uart_stdio_print);
}

// void th_timestamp(void) {
//   am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_CLEAR);
//   for (int i=0; i<100'000; ++i) {
//     asm("nop");
//   }
//   am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
 
// }

// extern "C" void DebugLog(const char* s) {
// #ifndef TF_LITE_STRIP_ERROR_STRINGS
//   th_printf("%s", s);
// #endif
// }