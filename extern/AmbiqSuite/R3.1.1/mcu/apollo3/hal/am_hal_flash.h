//*****************************************************************************
//
//! @file am_hal_flash.h
//!
//! @brief Functions for Performing Flash Operations.
//!
//! @addtogroup flash3 Flash - Flash Functionality
//! @ingroup apollo3_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_FLASH_H
#define AM_HAL_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//! Flash Program keys.
//
//*****************************************************************************
#define AM_HAL_FLASH_PROGRAM_KEY            0x12344321
#define AM_HAL_FLASH_INFO_KEY               0xD894E09E



//*****************************************************************************
//
//! Some helpful SRAM values and macros.
//
//*****************************************************************************
#define AM_HAL_FLASH_SRAM_ADDR                  SRAM_BASEADDR
#define AM_HAL_FLASH_SRAM_SIZE                  (384 * 1024)
#define AM_HAL_FLASH_SRAM_LARGEST_VALID_ADDR    (AM_HAL_FLASH_SRAM_ADDR + AM_HAL_FLASH_SRAM_SIZE - 1)
#define AM_HAL_FLASH_DTCM_START                 AM_HAL_FLASH_SRAM_ADDR
#define AM_HAL_FLASH_DTCM_END                   (AM_HAL_FLASH_SRAM_ADDR + (64 * 1024) - 1)

//*****************************************************************************
//
//! Some helpful flash values and macros.
//
//*****************************************************************************
#define AM_HAL_FLASH_ADDR                   0x00000000
#define AM_HAL_FLASH_INSTANCE_SIZE          ( 512 * 1024 )
#define AM_HAL_FLASH_NUM_INSTANCES          2
#define AM_HAL_FLASH_PAGE_SIZE              ( 8 * 1024 )
#define AM_HAL_FLASH_INFO_SIZE              AM_HAL_FLASH_PAGE_SIZE
#define AM_HAL_FLASH_INSTANCE_PAGES         ( AM_HAL_FLASH_INSTANCE_SIZE / AM_HAL_FLASH_PAGE_SIZE )
#define AM_HAL_FLASH_TOTAL_SIZE             ( AM_HAL_FLASH_INSTANCE_SIZE * AM_HAL_FLASH_NUM_INSTANCES )
#define AM_HAL_FLASH_LARGEST_VALID_ADDR     ( AM_HAL_FLASH_ADDR + AM_HAL_FLASH_TOTAL_SIZE - 1 )
#define AM_HAL_FLASH_APPL_ADDR              0xC000

//
//! Macros to determine whether a given address is a valid internal
//! flash or SRAM address.
//
#define ISADDRSRAM(x)       ((x >= AM_HAL_FLASH_SRAM_ADDR)  &&      \
                             (x <= (AM_HAL_FLASH_SRAM_LARGEST_VALID_ADDR & ~0x3)))
#if AM_HAL_FLASH_ADDR == 0x0
#define ISADDRFLASH(x)      (x <= (AM_HAL_FLASH_LARGEST_VALID_ADDR & ~0x3))
#else
#define ISADDRFLASH(x)      ((x >= AM_HAL_FLASH_ADDR)       &&      \
                             (x <= (AM_HAL_FLASH_LARGEST_VALID_ADDR & ~0x3)))
#endif

//
//! Macros to describe the flash ROW layout.
//
#define AM_HAL_FLASH_ROW_WIDTH_BYTES        (512)

//
//! Convert an absolute flash address to an instance
//
#define AM_HAL_FLASH_ADDR2INST(addr)        ( ( addr >> 19 ) & (AM_HAL_FLASH_NUM_INSTANCES - 1) )

//
//! Convert an absolute flash address to a page number relative to the instance
//
#define AM_HAL_FLASH_ADDR2PAGE(addr)        ( ( addr >> 13 ) & 0x3F )

//
//! Convert an absolute flash address to an absolute page number
//
#define AM_HAL_FLASH_ADDR2ABSPAGE(addr)     ( addr >> 13 )

//*****************************************************************************
//
//! Given an integer number of microseconds, convert to a value representing
//! the number of am_hal_flash_delay() cycles that will provide that amount
//! of delay.  This macro is designed to take into account some of the call
//! overhead and latencies.
//!
//! e.g. To provide a 10us delay:
//!  am_hal_flash_delay( FLASH_CYCLES_US(10) );
//!
//! As of SDK 2.1, burst mode is accounted for in am_hal_flash_delay().
//!
//! The FLASH_CYCLES_US macro assumes:
//!  - Burst or normal mode operation.
//!  - If cache is not enabled, use FLASH_CYCLES_US_NOCACHE() instead.
//
//*****************************************************************************
#define CYCLESPERITER               (AM_HAL_CLKGEN_FREQ_MAX_MHZ / 3)
#define FLASH_CYCLES_US(n)          ((n * CYCLESPERITER) + 0)
#define FLASH_CYCLES_US_NOCACHE(n)  ( (n == 0) ? 0 : (n * CYCLESPERITER) - 5)

//
//! Backward compatibility
//
#define am_hal_flash_program_otp        am_hal_flash_program_info
#define am_hal_flash_program_otp_sram   am_hal_flash_program_info_sram

//*****************************************************************************
//
//! Structure of pointers to helper functions invoking flash operations.
//!
//! The functions we are pointing to here are in the Apollo 3
//! integrated BOOTROM.
//
//*****************************************************************************
typedef struct am_hal_flash_helper_struct
{
    //
    // The basics.
    //
    int  (*flash_mass_erase)(uint32_t, uint32_t);
    int  (*flash_page_erase)(uint32_t, uint32_t, uint32_t);
    int  (*flash_program_main)(uint32_t, uint32_t *, uint32_t *, uint32_t);
    int  (*flash_program_info_area)(uint32_t,  uint32_t, uint32_t *, uint32_t, uint32_t);

    //
    // Non-blocking variants, but be careful these are not interrupt safe so
    // mask interrupts while these very long operations proceed.
    //
    int  (*flash_mass_erase_nb)(uint32_t, uint32_t);
    int  (*flash_page_erase_nb)(uint32_t, uint32_t, uint32_t);
    int  (*flash_page_erase2_nb)( uint32_t value, uint32_t address);
    bool (*flash_nb_operation_complete)(void);

    //
    // Useful utilities.
    //
    uint32_t (*flash_util_read_word)( uint32_t *);
    void (*flash_util_write_word)( uint32_t *, uint32_t);
    void (*bootrom_delay_cycles)(uint32_t ui32Cycles);

    //
    // Essentially these are recovery options.
    //
    int  (*flash_info_erase)( uint32_t, uint32_t);
    int  (*flash_info_plus_main_erase)( uint32_t, uint32_t);
    int  (*flash_info_plus_main_erase_both)( uint32_t value);
    int  (*flash_recovery)( uint32_t value);

    //
    // The following functions pointers will generally never be called from
    // user programs. They are here primarily to document these entry points
    // which are usable from a debugger or debugger script.
    //
    void (*flash_program_main_from_sram)(void);
    void (*flash_program_info_area_from_sram)(void);
    void (*flash_erase_main_pages_from_sram)(void);
    void (*flash_mass_erase_from_sram)(void);
    void (*flash_info_erase_from_sram)(void);
    void (*flash_info_plus_main_erase_from_sram)(void);
    void (*flash_nb_operation_complete_from_sram)(void);
    void (*flash_page_erase2_nb_from_sram)(void);
    void (*flash_recovery_from_sram)(void);

} g_am_hal_flash_t;
extern const g_am_hal_flash_t g_am_hal_flash;

//*****************************************************************************
//
//! Define some FLASH INFO SPACE values and macros.
//
//*****************************************************************************
#define AM_HAL_FLASH_INFO_ADDR              0x50020000
#define AM_HAL_FLASH_INFO_SECURITY_O        0x10
#define AM_HAL_FLASH_INFO_WRITPROT_O        0x40
#define AM_HAL_FLASH_INFO_COPYPROT_O        0x50

#define AM_HAL_FLASH_INFO_SECURITY_ADDR     (AM_HAL_FLASH_INFO_ADDR + AM_HAL_FLASH_INFO_SECURITY_O)
#define AM_HAL_FLASH_INFO_WRITPROT_ADDR     (AM_HAL_FLASH_INFO_ADDR + AM_HAL_FLASH_INFO_WRITPROT_O)
#define AM_HAL_FLASH_INFO_COPYPROT_ADDR     (AM_HAL_FLASH_INFO_ADDR + AM_HAL_FLASH_INFO_COPYPROT_O)
#define AM_HAL_FLASH_INFO_CUST_TRIM_ADDR    (AM_HAL_FLASH_INFO_ADDR + 0x14)

//
//! Define the customer info signature data (at AM_HAL_FLASH_INFO_ADDR).
//! These bits must exist in the customer info space in order for many of the
//! security and protection functions to work.
//
#define AM_HAL_FLASH_INFO_SIGNATURE0        0x48EAAD88
#define AM_HAL_FLASH_INFO_SIGNATURE1        0xC9705737
#define AM_HAL_FLASH_INFO_SIGNATURE2        0x0A6B8458
#define AM_HAL_FLASH_INFO_SIGNATURE3        0xE41A9D74

//
//! Define the customer security bits (at AM_HAL_FLASH_INFO_SECURITY_ADDR)
//
#define AM_HAL_FLASH_INFO_SECURITY_DEBUGGERPROT_S       0
#define AM_HAL_FLASH_INFO_SECURITY_SWOCTRL_S            1
#define AM_HAL_FLASH_INFO_SECURITY_SRAMWIPE_S           2
#define AM_HAL_FLASH_INFO_SECURITY_FLASHWIPE_S          3
#define AM_HAL_FLASH_INFO_SECURITY_ENINFOPRGM_S         4
#define AM_HAL_FLASH_INFO_SECURITY_ENINFOERASE_S        8
#define AM_HAL_FLASH_INFO_SECURITY_BOOTLOADERSPIN_S     9

#define AM_HAL_FLASH_INFO_SECURITY_DEBUGGERPROT_M       ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_DEBUGGERPROT_S))
#define AM_HAL_FLASH_INFO_SECURITY_SWOCTRL_M            ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_SWOCTRL_S))
#define AM_HAL_FLASH_INFO_SECURITY_SRAMWIPE_M           ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_SRAMWIPE_S))
#define AM_HAL_FLASH_INFO_SECURITY_FLASHWIPE_M          ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_FLASHWIPE_S))
#define AM_HAL_FLASH_INFO_SECURITY_ENINFOPRGM_M         ((uint32_t)(0xF << AM_HAL_FLASH_INFO_SECURITY_ENINFOPRGM_S))
#define AM_HAL_FLASH_INFO_SECURITY_ENINFOERASE_M        ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_ENINFOERASE_S))
#define AM_HAL_FLASH_INFO_SECURITY_BOOTLOADERSPIN_M     ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_BOOTLOADERSPIN_S))
#define AM_HAL_FLASH_INFO_SECURITY_DEEPSLEEP_M          ((uint32_t)(0x1 << AM_HAL_FLASH_INFO_SECURITY_BOOTLOADERSPIN_S))
#define AM_HAL_FLASH_INFO_SECURITY_DEEPSLEEP            ((uint32_t)(0x0 << AM_HAL_FLASH_INFO_SECURITY_BOOTLOADERSPIN_S))

//
//! Protection chunk macros
//! AM_HAL_FLASH_INFO_CHUNK2ADDR: Convert a chunk number to an address
//! AM_HAL_FLASH_INFO_CHUNK2INST: Convert a chunk number to an instance number
//! AM_HAL_FLASH_INFO_ADDR2CHUNK: Convert an address to a chunk number
//
#define AM_HAL_FLASH_INFO_CHUNKSIZE         (16*1024)

#define AM_HAL_FLASH_INFO_CHUNK2ADDR(n)     (AM_HAL_FLASH_ADDR + (n << 14))
#define AM_HAL_FLASH_INFO_CHUNK2INST(n)     ((n >> 5) & 1
#define AM_HAL_FLASH_INFO_ADDR2CHUNK(n)     ((n) >> 14)

//*****************************************************************************
//
// Function prototypes for the helper functions
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief This function performs a mass erase on a flash instance.
//!
//! @param ui32ProgramKey - The flash program key.
//! @param ui32FlashInst - The flash instance to erase.
//!
//! This function will erase the desired instance of flash.
//!
//! @note For Apollo3, each flash instance contains a maximum of 512KB.
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!      - 1   ui32ProgramKey is invalid.
//!      - 2   ui32FlashInst is invalid.
//!      - 3   Flash controller hardware timeout.
//
//*****************************************************************************
extern int am_hal_flash_mass_erase(uint32_t ui32ProgramKey, uint32_t ui32FlashInst);
//*****************************************************************************
//
//! @brief This function performs a page erase on a flash instance.
//!
//! @param ui32ProgramKey - The flash program key.
//! @param ui32FlashInst - The flash instance to reference the page number with.
//! @param ui32PageNum - The flash page relative to the specified instance.
//!
//! This function will erase the desired flash page in the desired instance of
//! flash.
//!
//! @note For Apollo3, each flash page is 8KB (or AM_HAL_FLASH_PAGE_SIZE).
//! Each flash instance contains a maximum of 64 pages (or
//! AM_HAL_FLASH_INSTANCE_PAGES).
//! @par
//! @note When given an absolute flash address, a couple of helpful macros can
//! be utilized when calling this function.
//! For example:
//!     am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
//!                             AM_HAL_FLASH_ADDR2INST(ui32Addr),
//!                             AM_HAL_FLASH_ADDR2PAGE(ui32Addr) );
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!      - 1   ui32ProgramKey is invalid.
//!      - 2   ui32FlashInst is invalid.
//!      - 3   ui32PageNum is invalid.
//!      - 4   Flash controller hardware timeout.
//
//*****************************************************************************
extern int am_hal_flash_page_erase(uint32_t ui32ProgramKey, uint32_t ui32FlashInst,
                                   uint32_t ui32PageNum);
//*****************************************************************************
//
//! @brief This programs up to N words of the Main array on one flash instance.
//!
//! @param ui32ProgramKey - The programming key, AM_HAL_FLASH_PROGRAM_KEY.
//! @param pui32Src - Pointer to word aligned array of data to program into
//! the flash instance.
//! @param pui32Dst - Pointer to the word aligned flash location where
//! programming of the flash instance is to begin.
//! @param ui32NumWords - The number of words to be programmed.
//!
//! This function will program multiple words in main flash.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!      - 1   ui32ProgramKey is invalid.
//!      - 2   pui32Dst is invalid.
//!      - 3   Flash addressing range would be exceeded.  That is, (pui32Dst +
//!         (ui32NumWords * 4)) is greater than the last valid address.
//!      - 4   pui32Src is invalid.
//!      - 5   pui32Src is invalid.
//!      - 6   Flash controller hardware timeout.
//
//*****************************************************************************
extern int am_hal_flash_program_main(
                    uint32_t ui32ProgramKey,
                    uint32_t *pui32Src,
                    uint32_t *pui32Dst,
                    uint32_t ui32NumWords);

//*****************************************************************************
// @name Recovery_Functions
// @{
// Recovery type functions for Customer INFO space.
//*****************************************************************************

//*****************************************************************************
//
//! @brief This function programs multiple words in the customer INFO space.
//!
//! @param ui32InfoKey - The customer INFO space key.
//! @param ui32InfoInst - The INFO space instance, 0 or 1.
//! @param *pui32Src - Pointer to word aligned array of data to program into
//! the customer INFO space.
//! @param ui32Offset - Word offset into customer INFO space (offset of 0 is
//! the first word, 1 is second word, etc.).
//! @param ui32NumWords - The number of words to be programmed, must not
//! exceed AM_HAL_FLASH_INFO_SIZE/4.
//!
//! This function will program multiple words in the customer INFO space.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!      - 1   ui32InfoKey is invalid.
//!      - 2   ui32InfoInst is invalid.
//!      - 3   ui32Offset is invalid.
//!      - 4   INFO addressing range would be exceeded.  That is, (ui32Offset +
//!         ui32NumWords) is greater than the last valid address.
//!      - 5   pui32Src is invalid.
//!      - 6   pui32Src is invalid.
//!      - 7   Hardware error.
//!      - 8   Flash controller hardware timeout.
//
//*****************************************************************************
extern int      am_hal_flash_program_info(uint32_t ui32InfoKey, uint32_t ui32InfoInst,
                                          uint32_t *pui32Src, uint32_t ui32Offset,
                                          uint32_t ui32NumWords);

//*****************************************************************************
//
//! @brief This function erases an instance of the customer INFO space.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!                      (AM_HAL_FLASH_INFO_KEY).
//! @param ui32Inst - The flash instance, either 0 or 1.
//!
//! This function will erase the the customer INFO space of the specified
//! instance.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!     - 1   ui32InfoKey is invalid.
//!     - 2   ui32Inst is invalid.
//!     - 3   Hardware error.
//!     - 4   Flash controller hardware timeout.
//
//*****************************************************************************
extern int      am_hal_flash_erase_info(uint32_t ui32InfoKey,
                                        uint32_t ui32Inst);
//*****************************************************************************
//
//! @brief This function erases the main instance + the customer INFO space.
//!
//! @param ui32InfoKey - The customer INFO space key.
//! @param ui32Inst      - The flash instance, either 0 or 1.
//!
//! This function will erase the main flash + the customer INFO space of the
//! specified instance.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!     - 1   ui32InfoKey is invalid.
//!     - 2   ui32Inst is invalid.
//!     - 3   Hardware error.
//!     - 4   Flash controller hardware timeout.
//!     - 11  Internal error.
//!     - 12  Internal error.
//!     - 13  Flash controller hardware timeout.
//
//*****************************************************************************
extern int      am_hal_flash_erase_main_plus_info(uint32_t ui32InfoKey,
                                                  uint32_t ui32Inst);
//*****************************************************************************
//
//! @brief This function erases the main flash + the customer INFO space.
//!
//! @param ui32InfoKey - The customer INFO space key.
//!
//! This function will erase both instances the main flash + the
//! customer INFO space.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return 0 for success, non-zero for failure.
//!     Failing return code indicates:
//!     - 1   ui32InfoKey is invalid, instance 0.
//!     - 2   Internal error, instance 0.
//!     - 3   Hardware error, instance 0.
//!     - 4   Flash controller hardware timeout, instance 0.
//!     - 11   Internal error.
//!     - 12   Internal error.
//!     - 13   Flash controller hardware timeout.
//!     - 21   ui32InfoKey is invalid, instance 1.
//!     - 22   Internal error, instance 1.
//!     - 23   Hardware error, instance 1.
//!     - 24   Flash controller hardware timeout, instance 1.
//!     - 31   Internal error, instance 1.
//!     - 32   Internal error, instance 1.
//!     - 33   Flash controller hardware timeout, instance 1.
//
//*****************************************************************************
extern int      am_hal_flash_erase_main_plus_info_both_instances(
                                                  uint32_t ui32InfoKey);
//*****************************************************************************
//
//! @brief This function erases both main flash instances + both customer INFO
//! space instances.
//!
//! @param ui32RecoveryKey - The recovery key.
//!
//! This function erases both main instances and both customer INFOinstances
//! even if the customer INFO space is programmed to not be erasable. This
//! function completely erases the flash main and info instances and wipes the
//! SRAM. Upon completion of the erasure operations, it does a POI (power on
//! initialization) reset.
//!
//! @note The customer key lock is enforced by this function.  Therefore, the
//! customer key must be written prior to calling otherwise, the function will
//! fail.  Therefore, always check for a return code.  If the function returns,
//! a failure has occured.
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return Does not return if successful.  Returns failure code otherwise.
//!     - Failing return code indicates:
//!     - 0x00000001  ui32RecoveryKey is invalid.
//!     - 0x00000002  Customer key lock not set.
//!     - 0x00001001  Internal error.
//!     - 0x00001002  Internal error.
//!     - 0x00001003  Info erase, instance 0 - hardware error.
//!     - 0x00001004  Info erase, instance 0 - flash controller hardware timeout.
//!     - 0xi000ppee  Error erasing page in instance, pp=page number, ee=error code.
//!                 - i=2|3, instance 0.
//!                 - i=4|5, instance 1.
//!                 - ee=1|2|3    Internal or hardware error.
//!                 - ee=4        Flash controller hardware timeout.
//
//*****************************************************************************
extern int      am_hal_flash_recovery(uint32_t ui32RecoveryKey);


//*****************************************************************************
//
//! @brief Write a given uint32 value to a valid memory or peripheral location.
//!
//! @param pui32Address - The location to be written.
//! @param ui32Value    - Value to be written
//!
//! @note Use this function to safely store a value to peripheral or memory locations.
//!
//! This function calls a function that resides in BOOTROM or SRAM to do the
//! actual write, thus completely avoiding any conflict with flash or INFO.
//! This function safely writes to a peripheral or memory address while executing
//! from SRAM, thus avoiding any conflict with flash or INFO space.
//
//*****************************************************************************
extern void     am_hal_flash_store_ui32(uint32_t *pui32Address, uint32_t ui32Value);

//*****************************************************************************
// BOOTROM resident reader, writer and delay utility functions.
//*****************************************************************************

//*****************************************************************************
//
//! @brief Read a uint32 value from a valid memory or peripheral location.
//! @details BOOTROM resident reader, writer and delay utility functions.
//! This function calls a function that resides BOOTROM or SRAM to do the actual
//! read, thus completely avoiding any conflict with flash or INFO space.
//!
//! @param pui32Address - The location to be read.
//!
//! @note Use this function to safely read a value from peripheral or memory locations.
//!
//! @return The value read from the given address.
//
//*****************************************************************************
extern uint32_t am_hal_flash_load_ui32(uint32_t *pui32Address);
//*****************************************************************************
//
//! @brief Use the bootrom to implement a spin loop.
//!
//! @param ui32Iterations - Number of iterations to delay.
//!
//! Use this function to implement a CPU busy waiting spin loop without cache
//! or delay uncertainties.
//!
//! Notes for Apollo3:
//! - The ROM-based function executes at 3 cycles per iteration plus the normal
//!   function call, entry, and exit overhead and latencies.
//! - Cache settings affect call overhead.  However, the cache does not affect
//!   the time while inside the BOOTROM function.
//! - The function accounts for burst vs normal mode, along with some of the
//!   overhead encountered with executing the function itself (such as the
//!   check for burst mode).
//! - Use of the FLASH_CYCLES_US() or FLASH_CYCLES_US_NOCACHE() macros for the
//!   ui32Iterations parameter will result in approximate microsecond timing.
//! - The parameter ui32Iterations==0 is allowed but is still incurs a delay.
//!
//! Example:
//! - MCU operating at 48MHz -> 20.83 ns / cycle
//! - Therefore each iteration (once inside the bootrom function) will consume
//!   62.5ns (non-burst-mode).
//!
//! @note Interrupts are not disabled during execution of this function.
//!       Therefore, any interrupt taken will affect the delay timing.
//
//*****************************************************************************
extern void     am_hal_flash_delay(uint32_t ui32Iterations);
//*****************************************************************************
//
//! @brief Delays for a desired amount of cycles while also waiting for a
//! status to change a value.
//!
//! @param ui32usMaxDelay - Maximum number of ~1uS delay loops.
//! @param ui32Address    - Address of the register for the status change.
//! @param ui32Mask       - Mask for the status change.
//! @param ui32Value      - Target value for the status change.
//!
//! This function will delay for approximately the given number of microseconds
//! while checking for a status change, exiting when either the given time has
//! expired or the status change is detected.
//!
//! @returns
//!          - 0 = timeout.
//!          - 1 = status change detected.
//

//*****************************************************************************
extern uint32_t am_hal_flash_delay_status_change(
                                 uint32_t ui32usMaxDelay,
                                 uint32_t ui32Address,
                                 uint32_t ui32Mask,
                                 uint32_t ui32Value);
//*****************************************************************************
//
//! @brief Delays for a desired amount of cycles while also waiting for a
//! status to equal OR not-equal to a value.
//!
//! @param ui32usMaxDelay - Maximum number of ~1uS delay loops.
//! @param ui32Address    - Address of the register for the status change.
//! @param ui32Mask       - Mask for the status change.
//! @param ui32Value      - Target value for the status change.
//! @param bIsEqual       - Check for equal if true; not-equal if false.
//!
//! This function will delay for approximately the given number of microseconds
//! while checking for a status change, exiting when either the given time has
//! expired or the status change is detected.
//!
//! @returns
//!          - 0 = timeout.
//!          - 1 = status change detected.
//
//*****************************************************************************
extern uint32_t am_hal_flash_delay_status_check(
                        uint32_t ui32usMaxDelay,
                        uint32_t ui32Address,
                        uint32_t ui32Mask,
                        uint32_t ui32Value,
                        bool bIsEqual);

//*****************************************************************************
// @} // Recovery type functions for Customer INFO space
//*****************************************************************************

//*****************************************************************************
// @name Security_Protection_INFOBLK
// @{
// These functions update security/protection bits in the customer INFO blOCK.
//*****************************************************************************

//*****************************************************************************
//
//! @brief Check that the customer info bits are valid.
//!
//! Use this function to test the state of the 128 valid bits at the beginning
//! of customer info space. If these are not set correctly then the customer
//! protection bits in the INFO space will not be honored by the hardware.
//!
//! @return true if valid.
//
//*****************************************************************************
extern bool     am_hal_flash_customer_info_signature_check(void);

//*****************************************************************************
//
//! @brief INFO signature set.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern bool     am_hal_flash_info_signature_set(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief Disable FLASH INFO space.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Then disable FLASH erasure.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_info_erase_disable(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief Check for Disabled FLASH INFO space.
//!
//! Use this function to determine whether FLASH INFO erasure is disabled.
//!
//! @return
//!     - true if FLASH INFO erase is disabled,
//!     - otherwise false.
//
//*****************************************************************************
extern bool     am_hal_flash_info_erase_disable_check(void);

//*****************************************************************************
//
//! @brief Mask off 1 to 4 quadrants of FLASH INFO space for programming.
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Then and the mask bits with the INFO
//! space programming disable bits.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! @param ui32Mask    - A mask of the 4 quadrants of info space where\n
//!      - bit0 = First quadrant (first 2KB).
//!      - bit1 = Second quadrant (second 2KB).
//!      - bit2 = Third quadrant (third 2KB).
//!      - bit3 = Fourth quadrant (fourth 2KB).
//!
//! @note This function disables only, any quadrant already disabled is not
//! re-enabled.  That is, any ui32Mask bits specified as 0 are essentially nops.
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_info_program_disable(uint32_t ui32InfoKey, uint32_t ui32Mask);

//*****************************************************************************
//
//! @brief Return a mask specifying which quadrants of customer INFO space have
//! been disabled for programming.
//!
//! Use this function to determine whether programming of customer INFO space
//! has been disabled.
//!
//! @return A 4-bit mask of the disabled quadrants.
//!      - 0xFFFFFFFF indicates an error.
//!      - 0x0  indicates all customer INFO space programming is enabled.
//!      - 0xF  indicates all customer INFO space programming is disabled.
//!      - bit0 indicates the first customer INFO space is disabled for programming.
//!      - bit1 indicates the second customer INFO space is disabled for programming.
//!      - bit2 indicates the third customer INFO space is disabled for programming.
//!      - bit3 indicates the fourth customer INFO space is disabled for programming.
//
//*****************************************************************************
extern uint32_t am_hal_flash_info_program_disable_get(void);

//*****************************************************************************
//
//! @brief Enable FLASH debugger protection (FLASH gets wiped if a debugger is
//! connected).
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Then set the FLASH wipe bit to zero.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_wipe_flash_enable(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief check for FLASH wipe protection enabled.
//!
//! Use this function to determine if FLASH wipe protection is enabled.
//!
//! @return
//!     - true if FLASH wipe protection is enabled,
//!     - otherwise false.
//
//*****************************************************************************
extern bool     am_hal_flash_wipe_flash_enable_check(void);

//*****************************************************************************
//
//! @brief Enable SRAM protection so SRAM gets wiped if a debgger is connected.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Then set the SRAM wipe bit to zero.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_wipe_sram_enable(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief check for SRAM protection enabled.
//!
//! Use this function to determine if SRAM protection is enabled.
//!
//! @return
//!     - true if SRAM wipe protection is enabled,
//!     - otherwise false.
//
//*****************************************************************************
extern bool     am_hal_flash_wipe_sram_enable_check(void);

//*****************************************************************************
//
//! @brief Disable Output from ITM/SWO.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Set the SWO disable bit to zero.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_swo_disable(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief check for SWO disabled.
//!
//! Use this function to determine if the SWO is disabled.
//!
//! @return true if the ITM/SWO is disabled, otherwise false.
//
//*****************************************************************************
extern bool     am_hal_flash_swo_disable_check(void);

//*****************************************************************************
//
//! @brief Disable Connections from a debugger on the SWD interface.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//!
//! Use this function to set the state of the 128 valid bits at the beginning
//! of customer info space, if needed. Set the debugger disable bit to zero.
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - Zero for success.
//!     - Non-Zero for errors.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_debugger_disable(uint32_t ui32InfoKey);

//*****************************************************************************
//
//! @brief check for debugger disabled.
//!
//! Use this function to determine if the debugger is disabled.
//!
//! @return
//!     - true if the debugger is disabled,
//!     - otherwise false.
//
//*****************************************************************************
extern bool     am_hal_flash_debugger_disable_check(void);

//*****************************************************************************
//
//! @brief This function sets copy protection for a range of flash chunks.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//! @param pui32StartAddress - Starting address in flash to begin protection.
//! @param pui32StopAddress - Ending address in flash to stop protection.
//!
//! This function will set copy protection bits for a range of flash chunks
//!
//! @note Each flash chunk contains 16KBytes and corresponds to one bit in
//! the protection register. Set the bit to zero to enable protection.
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - 0 for success.
//!     - 0x400000 if the protection bits were already programmed (mask the return
//!              value with 0x3FFFFF to ignore this case and treat as success).
//!     - Otherwise, non-zero for failure.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_copy_protect_set(uint32_t ui32InfoKey,
                                              uint32_t *pui32StartAddress,
                                              uint32_t *pui32StopAddress);

//*****************************************************************************
//
//! @brief This function checks copy protection for a range of flash chunks.
//!
//! @param pui32StartAddress - Starting address in flash.
//! @param pui32StopAddress - Ending address in flash.
//!
//! This function will check copy protection bits for a range of flash chunks
//! it expects all chunks in the range to be protected.
//!
//! @note Each flash chunk contains 16KBytes and corresponds to one bit in
//! the protection register. Set the bit to zero to enable protection.
//!
//! @return
//!         - false for at least one chunk in the covered range is not protected,
//!         - true if all chunks in the covered range are protected.
//!
//
//*****************************************************************************
extern bool     am_hal_flash_copy_protect_check(uint32_t *pui32StartAddress,
                                                uint32_t *pui32StopAddress);

//*****************************************************************************
//
//! @brief This function sets write protection for a range of flash chunks.
//!
//! @param ui32InfoKey - The customer INFO space programming key
//! @param pui32StartAddress - Starting address in flash to begin protection.
//! @param pui32StopAddress - Ending address in flash to stop protection.
//!
//! This function will set write protection bits for a range of flash chunks
//!
//! @note Each flash chunk contains 16KBytes and corresponds to one bit in
//! the protection register. Set the bit to zero to enable protection.
//! @par
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//!
//! @return
//!     - 0 for success.
//!     - 0x400000 if the protection bits were already programmed (mask the return
//!              value with 0x3FFFFF to ignore this case and treat as success).
//!     - Otherwise, non-zero for failure.
//!
//! Note: See am_hal_flash_program_info() for further details on return codes.
//
//*****************************************************************************
extern int32_t  am_hal_flash_write_protect_set(uint32_t ui32InfoKey,
                                               uint32_t *pui32StartAddress,
                                               uint32_t *pui32StopAddress);

//*****************************************************************************
//
//! @brief This function checks write protection for a range of flash chunks.
//!
//! @param pui32StartAddress - Starting address in flash.
//! @param pui32StopAddress - Ending address in flash.
//!
//! This function will check write protection bits for a range of flash chunks
//! it expects all chunks in the range to be protected.
//!
//! @note Each flash chunk contains 16KBytes and corresponds to one bit in
//! the protection register. Set the bit to zero to enable protection.
//!
//! @return
//!         - false for at least one chunk in the covered range is not protected,
//!         - true if all chunks in the covered range are protected.
//!
//
//*****************************************************************************
extern bool     am_hal_flash_write_protect_check(uint32_t *pui32StartAddress,
                                                 uint32_t *pui32StopAddress);

//*****************************************************************************
//
//! @brief This clears the specified bits in the addressed flash word
//!
//! @param ui32ProgramKey - The programming key, AM_HAL_FLASH_PROGRAM_KEY.
//! @param pui32Addr - Pointer to word aligned flash word to program into
//! @param ui32BitMask - The bits to be cleared
//!
//! This function will clear one of more bits in a word in main flash.
//! This function is mainly used when the same word is to be written multiple times
//! few bits at a time, between erase cycle
//!
//! @note Interrupts are active during execution of this function. Any interrupt
//! taken could cause execution errors. Please see the IMPORTANT note under
//! Detailed Description above for more details.
//! @par
//! @note We can reprogram a bit in flash to 0 only once. This function takes
//! care of not re-clearing bits if they are already programmed as 0
//!
//! @return 0 for success, non-zero for failure.
//!
//! Note: See am_hal_flash_program_main() for further details on return codes.
//
//*****************************************************************************
extern int      am_hal_flash_clear_bits(uint32_t ui32ProgramKey,
                                        uint32_t *pui32Addr,
                                        uint32_t ui32BitMask);


//*****************************************************************************
// @}   Security_Protection_INFOBLK end
//*****************************************************************************


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_FLASH_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
