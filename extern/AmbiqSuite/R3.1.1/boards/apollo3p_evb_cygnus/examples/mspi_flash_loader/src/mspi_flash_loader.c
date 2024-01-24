//*****************************************************************************
//
//! @file mspi_flash_loader.c
//!
//! @brief MSPI External Flash Loading and Execution Example
//!
//! Purpose: This example demonstrates loading a binary image from internal
//! flash to MSPI external Octal flash, then executing the program using
//! XIP from the external flash.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! The binary must be linked to run from MSPI flash address range
//! (as specified by BIN_INSTALL_ADDR). The location and size of the binary
//! in internal flash are specified using BIN_ADDR_FLASH & BIN_SIZE
//!
//! This example has been enhanced to use the new 'binary patching' feature
//! This example will not build if proper startup/linker files are not used.
//!
//! Prepare the example as follows:
//!     1. Generate hello_world example to load and execute at MSPI Flash XIP location 0x04000000.
//!         i. In the /examples/hello_world/iar directory modify the FLASH region as follows:
//!             change "define region ROMEM = mem:[from 0x0000C000 to 0x000FC000];"
//!             to "define region ROMEM = mem:[from 0x04000000 to 0x040F0000];"
//!         ii. Execute "make" in the /examples/hello_world/iar directory to rebuild the project.
//!     2. Copy /examples/hello_world/iar/bin/hello_world.bin into /examples/mspi_flash_loader/
//!     3. Create the binary with mspi_flash_loader + external executable from Step #1.
//!         ./mspi_loader_binary_combiner.py --loaderbin iar/bin/mspi_flash_loader.bin --appbin hello_world.bin --install-address 0x04000000 --flags 0x2 --outbin loader_hello_world --loader-address 0x0000C000 --chipType apollo3p
//!     4. Open the J-Link SWO Viewer to the target board.
//!     5. Open the J-Flash Lite program.  Select the /examples/mspi_flash_loader/loader_hello_world.bin file and program at 0x0000C000 offset.
//!
//! And this example can work on:
//! Apollo3p_evb + Cygnus
//! Target hardware uses 1.8V power supply voltage.
//! Actual Octal flash on Cygnus board is ATXP128 (Device ID: 0x00A91F) instead of ATXP032
//!
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
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#if defined(ADESTO_ATXP032)
#include "am_devices_mspi_atxp032.h"
#define am_devices_mspi_flash_config_t am_devices_mspi_atxp032_config_t
#define AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS
#define AM_DEVICES_MSPI_FLASH_SECTOR_SIZE    AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE
#else
#error "Unknown FLASH Device"
#endif
#include "am_util.h"


// Location of the MSPI Flash in address space
#define MSPI_XIP_BASE_ADDRESS   0x04000000

// Default Details of binary in internal flash
#define BIN_ADDR_FLASH          (64*1024)
#define BIN_SIZE                (32*1024)
#define BIN_INSTALL_ADDR        MSPI_XIP_BASE_ADDRESS
#define ENABLE_LOGGING

#define MSPI_TEST_MODULE        1
#define MSPI_TEST_FREQ          AM_HAL_MSPI_CLK_24MHZ

#define DEFAULT_TIMEOUT         10000
#define TEMP_BUFFER_SIZE        2048
#ifdef ENABLE_LOGGING
#define DEBUG_PRINT am_util_stdio_printf
#else
#define DEBUG_PRINT(...)
#endif

void            *g_FlashHdl;
void            *g_MSPIHdl;
uint32_t        g_TempBuf[TEMP_BUFFER_SIZE / 4];
uint32_t        DMATCBBuffer[2560];

// Patchable section of binary
extern uint32_t __Patchable[];

const am_devices_mspi_flash_config_t MSPI_Flash_Config =
{
//    .eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0,
    .eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_CE0,
    .eClockFreq = MSPI_TEST_FREQ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

//
// Typedef - to encapsulate device driver functions
//
typedef struct
{
    uint8_t  devName[20];
    uint32_t (*flash_init)(uint32_t ui32Module, am_devices_mspi_flash_config_t *pDevConfig, void **ppHandle, void **ppMspiHandle);
    uint32_t (*flash_term)(void *pHandle);

    uint32_t (*flash_read_id)(void *pHandle);

    uint32_t (*flash_write)(void *pHandle, uint8_t *pui8TxBuffer,
                            uint32_t ui32WriteAddress,
                            uint32_t ui32NumBytes,
                            bool bWaitForCompletion);

    uint32_t (*flash_read)(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           bool bWaitForCompletion);

    uint32_t (*flash_mass_erase)(void *pHandle);
    uint32_t (*flash_sector_erase)(void *pHandle, uint32_t ui32SectorAddress);

    uint32_t (*flash_enable_xip)(void *pHandle);
    uint32_t (*flash_disable_xip)(void *pHandle);
    uint32_t (*flash_enable_scrambling)(void *pHandle);
    uint32_t (*flash_disable_scrambling)(void *pHandle);

} flash_device_func_t;

flash_device_func_t device_func =
{
#if defined(ADESTO_ATXP032)
    // cygnus installed MSPI FLASH device
    .devName = "MSPI FLASH ATXP032",
    .flash_init = am_devices_mspi_atxp032_init,
    .flash_term = am_devices_mspi_atxp032_deinit,
    .flash_read_id = am_devices_mspi_atxp032_id,
    .flash_write = am_devices_mspi_atxp032_write,
    .flash_read = am_devices_mspi_atxp032_read,
    .flash_mass_erase = am_devices_mspi_atxp032_mass_erase,
    .flash_sector_erase = am_devices_mspi_atxp032_sector_erase,
    .flash_enable_xip = am_devices_mspi_atxp032_enable_xip,
    .flash_disable_xip = am_devices_mspi_atxp032_disable_xip,
    .flash_enable_scrambling = am_devices_mspi_atxp032_enable_scrambling,
    .flash_disable_scrambling = am_devices_mspi_atxp032_disable_scrambling,
#else
#error "Unknown FLASH Device"
#endif
};

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
#if defined(AM_PART_APOLLO3P)
    MSPI1_IRQn,
    MSPI2_IRQn,
#endif
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define flash_mspi_isr                                                          \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************
void flash_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIHdl, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_MSPIHdl, ui32Status);

    am_hal_mspi_interrupt_service(g_MSPIHdl, ui32Status);
}


// This function intializes the VTOR, SP and jumps the the Reset Vector of the image provided
#if defined(__GNUC_STDC_INLINE__)
__attribute__((naked))
static void
run_new_image(uint32_t *vtor)
{
    __asm
    (
        "   movw    r3, #0xED08\n\t"    // Store the vector table pointer of the new image into VTOR.
        "   movt    r3, #0xE000\n\t"
        "   str     r0, [r3, #0]\n\t"
        "   ldr     r3, [r0, #0]\n\t"   // Load the new stack pointer into R3 and the new reset vector into R2.
        "   ldr     r2, [r0, #4]\n\t"
        "   mov     sp, r3\n\t"         // Set the stack pointer for the new image.
        "   bx      r2\n\t"            // Jump to the new reset vector.
    );
}

#elif (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION <  6000000)
static __asm void
run_new_image(uint32_t *vtor)
{
    //
    // Store the vector table pointer of the new image into VTOR.
    //
    movw    r3, #0xED08
    movt    r3, #0xE000
    str     r0, [r3, #0]

    //
    // Load the new stack pointer into R1 and the new reset vector into R2.
    //
    ldr     r3, [r0, #0]
    ldr     r2, [r0, #4]

    //
    // Set the stack pointer for the new image.
    //
    mov     sp, r3

    //
    // Jump to the new reset vector.
    //
    bx      r2
}
#elif defined(__IAR_SYSTEMS_ICC__)
__stackless static inline void
run_new_image(uint32_t *vtor)
{
    __asm volatile (
          "    movw    r3, #0xED08\n"    // Store the vector table pointer of the new image into VTOR.
          "    movt    r3, #0xE000\n"
          "    str     r0, [r3, #0]\n"
          "    ldr     r3, [r0, #0]\n"   // Load the new stack pointer into R1 and the new reset vector into R2.
          "    ldr     r2, [r0, #4]\n"
          "    mov     sp, r3\n"         // Set the stack pointer for the new image.
          "    bx      r2\n"            // Jump to the new reset vector.
          );
}
#else
#error "IDE not supported"
#endif

//*****************************************************************************
//
// MSPI Example Main.
//
//*****************************************************************************
int
main(void)
{
    uint32_t      ui32Status;
    uint32_t      u32InstallOffset;

    am_devices_mspi_flash_config_t MspiCfg = MSPI_Flash_Config;
    bool          bScramble = false;
    bool          bRun = false;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#ifdef ENABLE_LOGGING
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();
#endif

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    DEBUG_PRINT("Apollo3p MSPI Flash Loader Example\n\n");


    uint32_t binAddr, binInstallAddr, binSize;

    // some of the parameters are controllable through binary patching
    if (__Patchable[0])
    {
        DEBUG_PRINT("Valid Patched information found\n");
        binAddr = __Patchable[0];
        binInstallAddr = __Patchable[1];
        binSize = __Patchable[2];
        u32InstallOffset = (binInstallAddr - MSPI_XIP_BASE_ADDRESS);
        if (__Patchable[3] & 0x1)
        {
            // Enable scrambling
            MspiCfg.ui32ScramblingStartAddr = u32InstallOffset;
            MspiCfg.ui32ScramblingEndAddr = u32InstallOffset + binSize - 1;
            bScramble = true;
        }
        if (__Patchable[3] & 0x2)
        {
            // Jump to program after installing
            bRun = true;
        }
    }
    else
    {
        binAddr = BIN_ADDR_FLASH;
        binInstallAddr = BIN_INSTALL_ADDR;
        u32InstallOffset = (BIN_INSTALL_ADDR - MSPI_XIP_BASE_ADDRESS);
        binSize = BIN_SIZE;
    }

    DEBUG_PRINT("Bin Address in internal flash = 0x%x\n", binAddr);
    DEBUG_PRINT("Bin Install Address in external flash = 0x%x\n", binInstallAddr);
    DEBUG_PRINT("Bin Size = 0x%x\n", binSize);
    DEBUG_PRINT("Scrambling is %s\n", bScramble ? "Enabled" : "Disabled");

    //
    // Configure the MSPI and Flash Device.
    //
    ui32Status = device_func.flash_init(MSPI_TEST_MODULE, (void*)&MspiCfg, &g_FlashHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to configure the MSPI and Flash Device correctly!\n");
        return -1;
    }
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();

    //
    // Read the MSPI Device ID.
    //
#if !defined (ADESTO_ATXP032)
    ui32Status = device_func.flash_read_id(g_FlashHdl);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to read Flash Device ID!\n");
        return -1;
    }
#endif

#if !defined (ADESTO_ATXP032)
    // Mass Erase
    DEBUG_PRINT("Initiating mass erase Flash Device!\n");
    DEBUG_PRINT("This takes about 40 seconds!\n");
    ui32Status = device_func.flash_mass_erase(g_FlashHdl);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to mass erase Flash Device!\n");
        return -1;
    }
    DEBUG_PRINT("mass erase Flash Device Done!\n");
#else
    // Erase the sectors we need to program
    DEBUG_PRINT("Initiating erase of required sectors of Flash Device!\n");
    for (uint32_t address = u32InstallOffset; address < (u32InstallOffset + binSize); address += AM_DEVICES_MSPI_FLASH_SECTOR_SIZE)
    {
        uint32_t sector = address / AM_DEVICES_MSPI_FLASH_SECTOR_SIZE;
        //
        // Erase the target sector.
        //
        DEBUG_PRINT("Erasing Sector %d\n", sector);
        ui32Status = device_func.flash_sector_erase(g_FlashHdl, sector << 16);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to erase Flash Device sector!\n");
            return -1;
        }
    }
#endif
    DEBUG_PRINT("Erase Done!\n");

    //
    // Enable XIP mode.
    //
    ui32Status = device_func.flash_enable_xip(g_FlashHdl);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    //
    // Check to see that the sector is actually erased.
    //
    uint32_t    *pui32Address = (uint32_t *)(MSPI_XIP_BASE_ADDRESS + u32InstallOffset);
    for (uint32_t i = 0; i < binSize / 4; i++)
    {
        if ( *(pui32Address + i) != 0xFFFFFFFF )
        {
            am_util_stdio_printf("Failed to erase Flash Device sector!\n");
            return -1;
        }
    }

    //
    // Make sure we aren't in XIP mode.
    //
    ui32Status = device_func.flash_disable_xip(g_FlashHdl);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to disable XIP mode in the MSPI!\n");
        return -1;
    }

    if (bScramble)
    {
        //
        // Turn on scrambling operation.
        //
        DEBUG_PRINT("Putting the MSPI into Scrambling mode\n");
        ui32Status = device_func.flash_enable_scrambling(g_FlashHdl);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to enable MSPI scrambling!\n");
        }
    }

    //
    // Write the executable binary into MSPI flash
    //
    DEBUG_PRINT("Writing image to External Flash Device!\n");
    ui32Status = device_func.flash_write(g_FlashHdl, (uint8_t *)binAddr, u32InstallOffset, binSize, true);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to write buffer to Flash Device!\n");
        return -1;
    }
    am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_FLASH_CACHE_INVALIDATE, 0);
    // Confirm that the flash now has the correct data
    DEBUG_PRINT("Verifying image in External Flash Device!\n");
    for (uint32_t address = u32InstallOffset; address < (u32InstallOffset + binSize); address += TEMP_BUFFER_SIZE)
    {
        //
        // Read the data back into the RX buffer.
        //
        ui32Status = device_func.flash_read(g_FlashHdl, (uint8_t *)g_TempBuf, address, TEMP_BUFFER_SIZE, true);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to read buffer to Flash Device!\n");
            return -1;
        }

        //
        // Compare the buffers
        //
        for (uint32_t i = 0; (i < TEMP_BUFFER_SIZE / 4) && ((address + i * 4) < binSize); i++)
        {
            if (g_TempBuf[i] != *((uint32_t *)(binAddr + address + i*4 - u32InstallOffset)))
            {
                DEBUG_PRINT("TX and RX buffers failed to compare at offset 0x%x - Expected 0x%x Actual 0x%x!\n",
                                address + i*4,
                                *((uint32_t *)(binAddr + address + i*4 - u32InstallOffset)),
                                g_TempBuf[i]);
                return -1;
            }
        }
    }

    if (bRun)
    {
        //
        // Set up for XIP operation.
        //
        DEBUG_PRINT("Putting the MSPI and External Flash into XIP mode\n");
        ui32Status = device_func.flash_enable_xip(g_FlashHdl);
        if ( bScramble )
        {
            ui32Status = device_func.flash_enable_scrambling(g_FlashHdl);
        }
        else
        {
            ui32Status = device_func.flash_disable_scrambling(g_FlashHdl);
        }
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to put the MSPI into XIP mode!\n");
            return -1;
        }

        uint32_t vtor = binInstallAddr;

        DEBUG_PRINT("Jumping to relocated image in MSPI Flash\n");
        // Add delay
        am_util_delay_ms(100);
        // Run binary from MSPI
        run_new_image((uint32_t *)vtor);

        // Will not return!!
    }
    else
    {
        DEBUG_PRINT("MSPI Flash Loaded\n");
        while(1);
    }
}

