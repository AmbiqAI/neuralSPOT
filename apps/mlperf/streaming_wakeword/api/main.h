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

// #include "am_mcu_apollo.h"
// #include "am_bsp.h"
// #include "am_util.h"
// #include "ns_uart.h"
// #include "ns_timer.h"
// #include "ns_ambiqsuite_harness.h"
// #include "submitter_implemented.h"
// #include "am_hal_i2s.h"
// #include "am_hal_gpio.h"
// #include "ns_uart.h"

// // #include <cstdarg>
// // #include <cstdio>
// // #include <cstdlib>
// // #include <cstring>

// #include "tensorflow/lite/micro/debug_log.h"
// #include "internally_implemented.h"

// static void uart_stdio_print(char *pcBuf)
// {
//     // Send the entire null-terminated string over UART:
//     size_t len = strlen(pcBuf);
//     ns_uart_blocking_send_data(&uart_config, pcBuf, len);
//     // We ignore the return count since the prototype is void.
// }

// void th_command_ready(char volatile *p_command) {
//   p_command = p_command;
//   ee_serial_command_parser_callback((char *)p_command);
// }

// char *th_strncpy(char *dest, const char *src, size_t n) {
//   return strncpy(dest, src, n);
// }

// size_t th_strnlen(const char *str, size_t maxlen) {
//   return strnlen(str, maxlen);
// }

// char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

// char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

// int th_atoi(const char *str) { return atoi(str); }

// void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

// void *th_memcpy(void *dst, const void *src, size_t n) {
//   return memcpy(dst, src, n);
// }

// int th_vprintf(const char *format, va_list ap) { return am_util_stdio_vprintf(format, ap); }
// void th_printf(const char *p_fmt, ...) {
//   va_list args;
//   va_start(args, p_fmt);
//   (void)th_vprintf(p_fmt, args); /* ignore return */
//   va_end(args);
// }

// void th_serialport_initialize(void) {
//   NS_TRY(ns_uart_init(&uart_config, &uart_handle), 
//           "Failed to initialize UART for serial port");
//   am_util_stdio_printf_init(uart_stdio_print);
// }