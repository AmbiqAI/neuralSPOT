// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

// This module is similar to a traditional assembly startup file paired with a
// linker script. It turns out that everything typically done in ARMv7-M
// assembly startup can be done straight from C code. This makes startup code
// easier to maintain, modify, and read.
//
// Core initialization is comprised of two primary parts:
//
// 1. Load boot information from ARMv7-M Vector Table: The ARMv7-M vector table
//    (See ARMv7-M Architecture Reference Manual DDI 0403E.b section B1.5)
//    dictates the starting Program Counter (PC) and Stack Pointer (SP) when the
//    SoC powers on. The vector table also contains a number of other vectors to
//    handle different exceptions. This module does not provide a vector table,
//    but it does account for it in the linker script.
//
// 2. Initialize static memory: When execution begins due to SoC power-on (or
//    the device is reset), static memory regions must be initialized to ensure
//    they contains the expected values when code begins to run. The SoC doesn't
//    inherently have a notion of how to do this, so before ANYTHING else the
//    memory must be initialized. This is done at the beginning of
//    pw_boot_Entry().
//
//
// The simple flow is as follows:
//   Power on -> PC and SP set (from vector_table by SoC) -> pw_boot_Entry()
//
// In pw_boot_Entry():
//   Initialize memory -> pw_PreMainInit() -> main()

#include <stdint.h>

#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

// The following extern symbols are provided by the linker script, and their
// values are accessible via the reference of the symbol.
//
// Example:
//   if (stack_pointer < &pw_boot_stack_low_addr) {
//     PW_LOG_ERROR("Main stack overflowed!")
//   }

// pw_boot_stack_[low/high]_addr indicate the range of the main stack. Note that
// this might not be the only stack in the system.
//
// The main stack pointer (sp_main) should be initialized to
// pw_boot_stack_high_addr. This can be done by inserting the address into index
// 0 of the ARMv7-M vector table. (See ARMv7-M Architecture Reference Manual DDI
// 0403E.b section B1.5.3)
extern uint8_t pw_boot_stack_low_addr;
extern uint8_t pw_boot_stack_high_addr;

// pw_boot_heap_[low/high]_addr indicate the address range reserved for the
// heap.
extern uint8_t pw_boot_heap_low_addr;
extern uint8_t pw_boot_heap_high_addr;

// The address that denotes the beginning of the .vector_table section. This
// can be used to set VTOR (vector table offset register) by the bootloader.
extern uint8_t pw_boot_vector_table_addr;

// Forward declaration of main. Pigweed applications are expected to implement
// this function. An implementation of main() is NOT provided by this module.
int main();

// Reset handler or boot entry point.
//
// For this module to work as expected, index 1 of the ARMv7-M vector table
// (which usually points to Reset_Handler) must be set to point to this
// function. This function is implemented by pw_boot_armv7m, and does early
// memory initialization.
PW_NO_PROLOGUE void pw_boot_Entry();

// pw_boot hook: Before static memory is initialized (user supplied)
//
// This is a hook function that users of pw_boot must supply. It is called
// immediately upon entry to pw_boot_Entry() and before zero initialization of
// RAM (.bss) and loading values into static memory (commonly labeled as the
// .data section in an ELF file).
// WARNING: Be EXTREMELY careful when in the context of this function as it
// violates the C spec in several ways as .bss has not yet been zero-initialized
// and static values have not yet been loaded into memory. This function is NOT
// implemented by pw_boot_armv7m.
void pw_boot_PreStaticMemoryInit();

// pw_boot hook: Before C++ static constructors are invoked (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called just
// after zero initialization of RAM and loading values into static memory
// (commonly labeled as the .data section in an ELF file). Per the naming, this
// function is called just before C++ static constructors are invoked. It is
// safe to run C code, but NOT safe to call out to any C++ code. This function
// is NOT implemented by pw_boot_armv7m.
void pw_boot_PreStaticConstructorInit();

// pw_boot hook: Before main is invoked (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called by
// pw_boot_Entry() after memory initialization but before main. This allows
// targets to have pre-main initialization of the device and seamlessly swap out
// the main() implementation. This function is NOT implemented by
// pw_boot_armv7m.
void pw_boot_PreMainInit();

// pw_boot hook: After main returned (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called by
// pw_boot_Entry() after main() has returned. This function must not return!
// This function is NOT implemented by pw_boot_armv7m.
PW_NO_RETURN void pw_boot_PostMain();

PW_EXTERN_C_END
