//*****************************************************************************
//
//! @file mspi_ddr_psram_loader.c
//!
//! @brief Example demonstrating how to load and run binary image to MSPI external DDR PSRAM.
//!
//! Purpose: The binary must be linked to run from MSPI PSRAM address range
//! (as specified by BIN_INSTALL_ADDR). The location and size of the binary
//! in internal flash are specified using BIN_ADDR_FLASH & BIN_SIZE
//!
//! This example has been enhanced to use the new 'binary patching' feature
//! This example will not build if proper startup/linker files are not used.
//!
//! Additional Information:
//! Prepare the example as follows:
//!     1. Generate hello_world example to load and execute at MSPI PSRAM XIP location 0x14000000.
//!         i. In the /examples/hello_world/iar directory modify the MRAM region as follows:
//!             //define region MCU_MRAM    = mem:[from 0x00018000 to 0x00200000];
//!             define region MCU_MRAM    = mem:[from 0x14000000 to 0x18000000];
//!         ii. Execute "make" in the /examples/hello_world/iar directory to rebuild the project.
//!     2. Copy /examples/hello_world/iar/bin/hello_world.bin into /boards/common4/examples/interfaces/mspi_ddr_psram_loader/
//!     3. Create the binary with mspi_ddr_psram_loader + external executable from Step #1.
//!         ./mspi_loader_binary_combiner.py --loaderbin iar/bin/mspi_ddr_psram_loader.bin --appbin hello_world.bin --install-address 0x14000000 --flags 0x2 --outbin loader_hello_world --loader-address 0x00018000
//!     4. Open the J-Link SWO Viewer to the target board.
//!     5. Open the J-Flash Lite program.  Select the /examples/interfaces/mspi_ddr_psram_loader/loader_hello_world.bin file and program at 0x00018000 offset.
//!
//! If Apollo4 EB board and DDR PSRAM daughter board are used, this example can work on:
//! Plugin DDR PSRAM daughter board to Apollo4 EB board, press SW2 button for 7 times to switch to the DDR PSRAM.
//!
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#ifdef APS25616N
#include "am_devices_mspi_psram_aps25616n.h"
#else
#include "am_devices_mspi_psram_aps12808l.h"
#endif
#include "am_util.h"

// Default Details of binary in internal flash
#define BIN_ADDR_FLASH          0x20000
#define BIN_SIZE                0x1E0000
#define BIN_INSTALL_ADDR        MSPI_XIP_BASE_ADDRESS
#define ENABLE_LOGGING

#define DEFAULT_TIMEOUT         10000
#define TEMP_BUFFER_SIZE        256
#ifdef ENABLE_LOGGING
#define DEBUG_PRINT am_util_stdio_printf
#else
#define DEBUG_PRINT(...)
#endif

void          *pDevHandle;
void          *pHandle;
uint32_t        g_TempBuf[TEMP_BUFFER_SIZE / 4];
uint32_t        DMATCBBuffer[2560];

// Patchable section of binary
extern uint32_t __Patchable[];

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
    .eClockFreq               = AM_HAL_MSPI_CLK_96MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig =
{
#ifdef APS25616N
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_HEX_DDR_CE1,
#else
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
#endif
    .eClockFreq               = AM_HAL_MSPI_CLK_96MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

#define MSPI_TEST_MODULE              0

#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
#define MSPI_XIP_BASE_ADDRESS 0x04000000
#elif defined (AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS 0x14000000
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS 0x18000000
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS 0x1C000000
#endif // #if (MSPI_TEST_MODULE == 0)
#endif // #if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
#if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    MSPI1_IRQn,
    MSPI2_IRQn,
#endif
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define psram_mspi_isr                                                          \
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
void psram_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(pHandle, &ui32Status, false);

    am_hal_mspi_interrupt_clear(pHandle, ui32Status);

    am_hal_mspi_interrupt_service(pHandle, ui32Status);
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

#ifndef APS25616N
am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;
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
#ifdef APS25616N
    am_devices_mspi_psram_config_t MspiCfg = MSPI_PSRAM_OctalCE1MSPIConfig;
#else
    am_devices_mspi_psram_config_t MspiCfg = MSPI_PSRAM_OctalCE0MSPIConfig;
#endif
    bool          bScramble = false;
    bool          bRun = false;
    uint32_t      ui32temp1 = 0;
    uint32_t      ui32temp2 = 0;
    uint32_t      ui32LeftByte = 0;

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();
#ifdef AM_PART_APOLLO4P
    ui32Status = am_hal_daxi_control(AM_HAL_DAXI_CONTROL_DISABLE, NULL);
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("Failed to configure DAXI!");
    }
#endif

#ifdef ENABLE_LOGGING
#if 0
    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();
#else
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();
#endif

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    DEBUG_PRINT("Apollo4 MSPI DDR PSRAM Loader Example\n\n");
#endif

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
    DEBUG_PRINT("Bin Install Address in external PSRAM = 0x%x\n", binInstallAddr);
    DEBUG_PRINT("Bin Size = 0x%x\n", binSize);
    DEBUG_PRINT("Scrambling is %s\n", bScramble ? "Enabled" : "Disabled");
#ifndef APS25616N
    am_util_debug_printf("Starting MSPI DDR Timing Scan: \n");
    if ( AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS == am_devices_mspi_psram_aps12808l_ddr_init_timing_check(MSPI_TEST_MODULE, &MspiCfg, &MSPIDdrTimingConfig) )
    {
        am_util_debug_printf("==== Scan Result: RXDQSDELAY0 = %d \n", MSPIDdrTimingConfig.ui32Rxdqsdelay);
    }
    else
    {
        am_util_debug_printf("==== Scan Result: Failed, no valid setting.  \n");
    }
#endif
    //
    // Configure the MSPI and PSRAM Device.
    //
#ifdef APS25616N
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_init(MSPI_TEST_MODULE, &MspiCfg, &pDevHandle, &pHandle);
#else
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_init(MSPI_TEST_MODULE, &MspiCfg, &pDevHandle, &pHandle);
#endif
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to configure the MSPI and PSRAM Device correctly!\n");
        return -1;
    }
    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();
#ifndef APS25616N
    //
    //  Set the DDR timing from previous scan.
    //
    am_devices_mspi_psram_aps12808l_apply_ddr_timing(pDevHandle, &MSPIDdrTimingConfig);
#endif
    //
    // Make sure we aren't in XIP mode.
    //
#ifdef APS25616N
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_disable_xip(pDevHandle);
#else
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_disable_xip(pDevHandle);
#endif
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
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
#ifdef APS25616N
        ui32Status = am_devices_mspi_psram_aps25616n_ddr_enable_scrambling(pDevHandle);
#else
        ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_scrambling(pDevHandle);
#endif
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to enable MSPI scrambling!\n");
        }
    }

    //
    // Write the executable binary into MSPI PSRAM
    //
    DEBUG_PRINT("Writing image to External PSRAM Device!\n");
#ifdef APS25616N
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_write(pDevHandle, (uint8_t *)binAddr, u32InstallOffset, binSize, true);
#else
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_write(pDevHandle, (uint8_t *)binAddr, u32InstallOffset, binSize, true);
#endif
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to write buffer to PSRAM Device!\n");
        return -1;
    }
    // Confirm that the PSRAM now has the correct data
    DEBUG_PRINT("Verifying image in External PSRAM Device!\n");
    for (uint32_t address = u32InstallOffset; address < (u32InstallOffset + binSize); address += TEMP_BUFFER_SIZE)
    {
        //
        // Read the data back into the RX buffer.
        //
#ifdef APS25616N
        ui32Status = am_devices_mspi_psram_aps25616n_ddr_read(pDevHandle, (uint8_t *)g_TempBuf, address, TEMP_BUFFER_SIZE, true);
//        ui32Status = am_devices_mspi_psram_aps25616n_ddr_read(pDevHandle, (uint8_t *)g_TempBuf, address, TEMP_BUFFER_SIZE, true);
//        ui32Status = am_devices_mspi_psram_aps25616n_ddr_enable_xip(pDevHandle);
#else
        ui32Status = am_devices_mspi_psram_aps12808l_ddr_read(pDevHandle, (uint8_t *)g_TempBuf, address, TEMP_BUFFER_SIZE, true);
#endif
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to read buffer to PSRAM Device!\n");
            return -1;
        }

        //
        // Compare the buffers
        //
        for (uint32_t i = 0; (i < TEMP_BUFFER_SIZE / 4) && ((address + i * 4) < binSize); i++)
        {
            if ((address + (i + 1) * 4) > binSize)
            {
                ui32LeftByte = binSize - (address + i * 4);
                ui32temp1 = g_TempBuf[i] & (0xffffffff >> (4 - ui32LeftByte) * 8);
                ui32temp2 = (*((uint32_t *)(binAddr + address + i*4 - u32InstallOffset))) & (0xffffffff >> (4 - ui32LeftByte) * 8);
                if (ui32temp1 != ui32temp2)
                {
                    DEBUG_PRINT("TX and RX buffers failed to compare at offset 0x%x - Expected 0x%x Actual 0x%x!\n",
                                    address + i*4, ui32temp2, ui32temp1);
                    return -1;
                }
            }
            else if (g_TempBuf[i] != *((uint32_t *)(binAddr + address + i*4 - u32InstallOffset)))
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
        DEBUG_PRINT("Putting the MSPI and External PSRAM into XIP mode\n");
#ifdef APS25616N
        ui32Status = am_devices_mspi_psram_aps25616n_ddr_enable_xip(pDevHandle);
#else
        ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_xip(pDevHandle);
#endif
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to put the MSPI into XIP mode!\n");
            return -1;
        }

        uint32_t vtor = binInstallAddr;

        DEBUG_PRINT("Jumping to relocated image in MSPI PSRAM\n");
        // Add delay
        am_util_delay_ms(100);
        // Run binary from MSPI
        run_new_image((uint32_t *)vtor);

        // Will not return!!
    }
    else
    {
        DEBUG_PRINT("MSPI DDR PSRAM Loaded\n");
        while(1);
    }
}

