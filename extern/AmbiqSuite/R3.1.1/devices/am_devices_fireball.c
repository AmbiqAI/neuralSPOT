//*****************************************************************************
//
//! @file am_devices_fireball.c
//!
//! @brief Control the Ambiq FIREBALL board.
//!
//! This module contains functions for controlling the Ambiq internal FIREBALL
//! board.
//!
//! @addtogroup fireball FIREBALL Board Driver
//! @ingroup devices
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
#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_devices_fireball.h"

//*****************************************************************************
//
// Macros.
//
//*****************************************************************************
//
//! The IOM to be used is defined by the Apollo3 MCU Engineering Board.
//
#define FIREBALL_IOM_MODULE     5

//
//! Define a board number.
//
#define FIREBALL_BOARD_NUM      1

//
// Helper macros
//
#define FB_CMD(op, pio)     (((op & 0x03) << 6) | ((pio & 0x3F) << 0))
#define FB_OP_LOW           0
#define FB_OP_HI            1
#define FB_OP_TRI           2

//
//! Fireball version info
//!   - Note: the 16-bit ID value is located on the Fireball Apollo IOS at 0x40/0x41.
//!          - FIREBALL_ID         0x7710
//!          - FIREBALL2_ID        0x7712
//!          - FIREBALL3_ID        0x7713
//
#define FIREBALL_OFFSET_ID      0x40    // (16 bits at 0x40/0x41)
#define FIREBALL_OFFSET_SWREV   0x42    // 1 byte

//
//! Apollo3 IOM5 pin definitions.
//! These should be defined in the BSP.  If not, use these for Apollo3.
//
#if 0
#define am_bsp_pin_enable(name)                                               \
    am_hal_gpio_pin_config(AM_BSP_GPIO_ ## name, AM_BSP_GPIO_CFG_ ## name);

#define AM_BSP_GPIO_IOM5_MISO           49
#define AM_BSP_GPIO_CFG_IOM5_MISO       AM_HAL_PIN_49_M5MISO
#define AM_BSP_GPIO_IOM5_MOSI           47
#define AM_BSP_GPIO_CFG_IOM5_MOSI       AM_HAL_PIN_47_M5MOSI
#define AM_BSP_GPIO_IOM5_SCK            48
#define AM_BSP_GPIO_CFG_IOM5_SCK        (AM_HAL_PIN_48_M5SCK | AM_HAL_GPIO_INPEN | AM_HAL_GPIO_HIGH_DRIVE)
#define AM_BSP_GPIO_FIREBALL_CE         30
#define AM_BSP_GPIO_CFG_FIREBALL_CE     (AM_HAL_PIN_30_NCE30 | (AM_REG_GPIO_CFGD_GPIO30OUTCFG_M5nCE3 >> 16) | AM_HAL_GPIO_DRIVE_12MA)
#define AM_BSP_FIREBALL_CE_CHNL         3
#endif


//*****************************************************************************
//
// Globals.
//
//*****************************************************************************
static void *g_pIOM5Handle = 0;
static uint32_t g_ui32FireballID = 0;
static uint32_t g_ui32FireballFWVer = 0;

static const am_hal_iom_config_t g_FireballIOM5Config =
{
    .eInterfaceMode     = AM_HAL_IOM_SPI_MODE,
    .ui32ClockFreq      = AM_HAL_IOM_1MHZ,
    .eSpiMode           = AM_HAL_IOM_SPI_MODE_0,
    .pNBTxnBuf          = 0,
    .ui32NBTxnBufLength = 0
};

static am_hal_iom_transfer_t g_sFireballTransaction =
{
    .uPeerInfo.ui32SpiChipSelect = 0,
    .ui32InstrLen     = 0,
    .ui32Instr        = 0,
    .ui32NumBytes     = 0,
    .eDirection       = AM_HAL_IOM_TX,
    .pui32TxBuffer    = 0,
    .pui32RxBuffer    = 0,
    .bContinue        = false,
    .ui8RepeatCount   = 0,
    .ui8Priority      = 0
};

static am_hal_iom_transfer_t g_sFireballReadIDTransaction =
{
    .uPeerInfo.ui32SpiChipSelect = 0,
    .ui32InstrLen     = 0,
    .ui32Instr        = 0,
    .ui32NumBytes     = 0,
    .eDirection       = AM_HAL_IOM_RX,
    .pui32TxBuffer    = 0,
    .pui32RxBuffer    = 0,
    .bContinue        = false,
    .ui8RepeatCount   = 0,
    .ui8Priority      = 0
};


//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************
static uint32_t fireball_id_get(uint32_t *pui32ID);


//*****************************************************************************
//
//! fireball_init()
//!
//! @param ui32Board
//! @param ui32Module
//! @return
//
//*****************************************************************************
static uint32_t
fireball_init(uint32_t ui32Board, uint32_t ui32Module)
{
    uint32_t ui32Ret;

    if ( ui32Board != FIREBALL_BOARD_NUM )
    {
        return 1;
    }

    if ( g_pIOM5Handle )
    {
        //
        // Already initialized
        //
        return 0;
    }

    //
    // Fireball is communicated with from the Apollo3 MCU engineering board via
    // SPI on IOM5.
    // The 1-byte commands are defined as follows:
    //  [7:6] = OP
    //  [5:0] = GPIO number
    // OP:
    //  0 = Set the GPIO low.
    //  1 = Set the GPIO high.
    //  2 = Set the GPIO tri-state.
    //  3 = Reserved.
    //
    // Note - the commands are handled by a macro, FB_CMD(op, pio).
    //

    //
    // Set up IOM5 to talk to Fireball.
    //
    if ( am_hal_iom_initialize(ui32Module, &g_pIOM5Handle)                  ||
         am_hal_iom_power_ctrl(g_pIOM5Handle, AM_HAL_SYSCTRL_WAKE, false)   ||
         am_hal_iom_configure(g_pIOM5Handle,
                              (am_hal_iom_config_t*)&g_FireballIOM5Config)  ||
         am_hal_iom_enable(g_pIOM5Handle))
    {
        return 2;
    }

    //
    // Set up the pins, including CE, for Apollo3 IOM5 pins that will
    // communicate with the Apollo IOS SPI device.
    //
    ui32Ret  = am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_SCK,    g_AM_BSP_GPIO_IOM5_SCK);
    ui32Ret |= am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_MISO,   g_AM_BSP_GPIO_IOM5_MISO);
    ui32Ret |= am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_MOSI,   g_AM_BSP_GPIO_IOM5_MOSI);
    ui32Ret |= am_hal_gpio_pinconfig(AM_BSP_GPIO_FIREBALL_CE, g_AM_BSP_GPIO_FIREBALL_CE);

    if ( ui32Ret )
    {
        return 3;
    }

    //
    // Initialize the write transaction structure.
    // After this, we should only need to set ui32InstrLen and ui32Instr.
    //
    g_sFireballTransaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_FIREBALL_CE_CHNL;
    g_sFireballTransaction.ui32InstrLen     = 0;
    g_sFireballTransaction.ui32Instr        = 0;
    g_sFireballTransaction.ui32NumBytes     = 0;
    g_sFireballTransaction.eDirection       = AM_HAL_IOM_TX;
    g_sFireballTransaction.pui32TxBuffer    = 0;
    g_sFireballTransaction.pui32RxBuffer    = 0;
    g_sFireballTransaction.bContinue        = false;
    g_sFireballTransaction.ui8RepeatCount   = 0;
    g_sFireballTransaction.ui8Priority      = 0;

    //
    // Initialize the read transaction structure.
    // After this, we should only need to set ui32InstrLen, ui32Instr, and
    // pui32RxBuffer.
    //
    g_sFireballReadIDTransaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_FIREBALL_CE_CHNL;
    g_sFireballReadIDTransaction.ui32InstrLen     = 0;
    g_sFireballReadIDTransaction.ui32Instr        = 0;
    g_sFireballReadIDTransaction.ui32NumBytes     = 0;
    g_sFireballReadIDTransaction.eDirection       = AM_HAL_IOM_RX;
    g_sFireballReadIDTransaction.pui32TxBuffer    = 0;
    g_sFireballReadIDTransaction.pui32RxBuffer    = 0;
    g_sFireballReadIDTransaction.bContinue        = false;
    g_sFireballReadIDTransaction.ui8RepeatCount   = 0;
    g_sFireballReadIDTransaction.ui8Priority      = 0;

    //
    // Get and save the Fireball ID.
    //
    if ( fireball_id_get(&g_ui32FireballID) != AM_HAL_STATUS_SUCCESS )
    {
        return 4;
    }

    //
    // Mask out the upper 16-bits of the ID.
    //
    g_ui32FireballID &= 0xFFFF;

    //
    // If the ID is 0xFFFF, this indicates that nothing is really out there.
    //
    if ( g_ui32FireballID == 0xFFFF )
    {
        return 5;
    }

    return 0;

} // fireball_init()

//*****************************************************************************
//
//! fireball_deinit()
//!
//! @param ui32Board
//! @return
//
//*****************************************************************************
static uint32_t
fireball_deinit(uint32_t ui32Board)
{
    if ( ui32Board != FIREBALL_BOARD_NUM )
    {
        return 1;
    }

    if ( g_pIOM5Handle )
    {
        //
        // am_hal_iom_uninitialize() calls am_hal_iom_disable(), etc.
        //
        am_hal_iom_uninitialize(g_pIOM5Handle);
        g_pIOM5Handle = 0;

        //
        // Disconnect pins.
        //
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_SCK,    g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_MISO,   g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM5_MOSI,   g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_FIREBALL_CE, g_AM_HAL_GPIO_DISABLE);
    }

    //
    // Invalidate the ID and version variables.
    //
    g_ui32FireballID = g_ui32FireballFWVer = 0;

    return 0;

} // fireball_deinit()

//*****************************************************************************
//
//! @brief This function assumes that fireball_init() has already been called!
//!
//! @return Return the Fireball currently in use.
//     - 2 = Fireball (original) or Fireball2.
//     - 3 = Fireball3.
//
//*****************************************************************************
static uint32_t
FBn_get(void)
{
    switch ( g_ui32FireballID )
    {
        case FIREBALL_ID:
        case FIREBALL2_ID:
            return 2;
        case FIREBALL3_ID:
            return 3;
        default:
            return 0xFFFFFFFF;
    }
} // FBn_get()


//*****************************************************************************
//! @brief
//!
//! @param ui32value
//! @param ui32GPIOnum
//! @return
//*****************************************************************************
static uint32_t
fireball_write_cmd(uint32_t ui32value, uint32_t ui32GPIOnum)
{
    uint32_t ui32FBn = FBn_get();
    uint32_t ui32Ret;

    if ( ui32FBn <= 2 )
    {
        //
        // On the Apollo slave, bit7 of the offset byte determines write or read.
        // 1=write, 0=read.
        //
        g_sFireballTransaction.ui32InstrLen     = 2;
        g_sFireballTransaction.ui32Instr        = (0x80 << 8)       |
                                                   FB_CMD(ui32value, ui32GPIOnum);
    }
    else if ( ui32FBn == 3 )
    {
        //
        // FB3 requires 3 bytes.
        // Whereas FB1/2 combined the high/low values with the GPIO number,
        // FB3 splits them into separate bytes.
        //  byte0: R/W and address, same as before.  On the Apollo IOS, bit7 of
        //         the offset determines wr or rd (1=wr, 0=rd), the lower 7 bits
        //         are the address.
        //  byte1: 0 to set low, 1 to set high.
        //  byte2: GPIO number
        //
        g_sFireballTransaction.ui32InstrLen     = 3;
        g_sFireballTransaction.ui32Instr        = (0x80                 << 16)  |
                                                  ((ui32GPIOnum & 0xFF) << 8)   |
                                                  ((ui32value   & 0x3)  << 0);
    }
    else
    {
        return 0xFFFFFFFF;
    }

   ui32Ret = am_hal_iom_blocking_transfer(g_pIOM5Handle,
                                           &g_sFireballTransaction);

    //
    // Give the FB plenty of time to process the command.
    // Recommendation is 10ms (although 1ms was found to work).
    //
    am_hal_flash_delay(FLASH_CYCLES_US(10000));

    return ui32Ret;

} // fireball_write_cmd()

//*****************************************************************************
//! @brief
//!
//! @param pui32ID
//! @return
//*****************************************************************************
static uint32_t
fireball_id_get(uint32_t *pui32ID)
{
    //
    // The ID is at offset 0x40, the SW Rev at 0x42.
    // We'll read all 3 bytes here.
    // On the Apollo slave, bit7 of the offset byte determines write or read.
    // 1=write, 0=read.
    //
    g_sFireballReadIDTransaction.ui32InstrLen     = 1;
    g_sFireballReadIDTransaction.ui32Instr        = 0x00 | FIREBALL_OFFSET_ID;
    g_sFireballReadIDTransaction.ui32NumBytes     = 3;
    g_sFireballReadIDTransaction.pui32RxBuffer    = pui32ID;

    return am_hal_iom_blocking_transfer(g_pIOM5Handle,
                                        &g_sFireballReadIDTransaction);

} // fireball_id_get()

//*****************************************************************************
//! @brief
//!
//! @param ui64GPIOLowMask
//! @param ui64GPIOHighMask
//! @return
//*****************************************************************************
static uint32_t
fireball_set(uint64_t ui64GPIOLowMask[2],  uint64_t ui64GPIOHighMask[2])
{
    uint32_t ui32GPIOnum, ui32Ret;

    ui32GPIOnum = 0;
    while ( ui64GPIOLowMask[0]  ||  ui64GPIOHighMask[0] )
    {
        if ( ui64GPIOLowMask[0] & 0x1 )
        {
            ui32Ret = fireball_write_cmd(FB_OP_LOW, ui32GPIOnum);
            if ( ui32Ret )
            {
                return (1 << 8) | ui32Ret;
            }

            //
            // Give the Fireball Apollo a little time to process the command.
            //
            am_hal_flash_delay(FLASH_CYCLES_US(1));
        }
        ui64GPIOLowMask[0] >>= 1;

        if ( ui64GPIOHighMask[0] & 0x1 )
        {
            ui32Ret = fireball_write_cmd(FB_OP_HI, ui32GPIOnum);
            if ( ui32Ret )
            {
                return (2 << 8) | ui32Ret;
            }

            //
            // Give the Fireball Apollo a little time to process the command.
            //
            am_hal_flash_delay(FLASH_CYCLES_US(1));
        }
        ui64GPIOHighMask[0] >>= 1;
        ui32GPIOnum++;
    }

    //
    // FB3 requires more than 64 pins.
    // Repeat the previous loop to handle the extended pins.
    //
    ui32GPIOnum = 64;
    while ( ui64GPIOLowMask[1]  ||  ui64GPIOHighMask[1] )
    {
        if ( ui64GPIOLowMask[1] & 0x1 )
        {
            ui32Ret = fireball_write_cmd(FB_OP_LOW, ui32GPIOnum);
            if ( ui32Ret )
            {
                return (1 << 8) | ui32Ret;
            }

            //
            // Give the Fireball Apollo a little time to process the command.
            //
            am_hal_flash_delay(FLASH_CYCLES_US(1));
        }
        ui64GPIOLowMask[1] >>= 1;

        if ( ui64GPIOHighMask[1] & 0x1 )
        {
            ui32Ret = fireball_write_cmd(FB_OP_HI, ui32GPIOnum);
            if ( ui32Ret )
            {
                return (2 << 8) | ui32Ret;
            }

            //
            // Give the Fireball Apollo a little time to process the command.
            //
            am_hal_flash_delay(FLASH_CYCLES_US(1));
        }
        ui64GPIOHighMask[1] >>= 1;
        ui32GPIOnum++;
    }

    return 0;

} // fireball_set()

//*****************************************************************************
//! @brief
//!
//! @param ui32GPIO
//! @param bAssertLow
//! @return
//*****************************************************************************
static uint32_t
device_reset(uint32_t ui32GPIO, bool bAssertLow)
{
    uint32_t ui32Assert, ui32Deassert;

    ui32Assert   = bAssertLow ? FB_OP_LOW : FB_OP_HI;
    ui32Deassert = bAssertLow ? FB_OP_HI  : FB_OP_LOW;

    //
    // Assert the reset signal.
    //
    if ( fireball_write_cmd(ui32Assert, ui32GPIO) )
    {
        return 1;
    }

    //
    // Reset pulse width.   (1ms)
    //
    am_hal_flash_delay(FLASH_CYCLES_US(1000));

    //
    // De-assert the reset signal.
    //
    if ( fireball_write_cmd(ui32Deassert, ui32GPIO) )
    {
        return 3;
    }

    return 0;

} // device_reset()

//*****************************************************************************
//
//  am_devices_fireball_control()
//  Set FIREBALL state.
//
//*****************************************************************************
uint32_t
am_devices_fireball_control(am_devices_fireball_control_e eControl,
                            void *pArgs)
{
    // FB3 requires more than 64 pin masks for some settings.
    uint64_t ui64GPIOLowMask[2], ui64GPIOHighMask[2];
    uint32_t ui32RetVal = AM_HAL_STATUS_SUCCESS;


    if ( eControl >= AM_DEVICES_FIREBALL_STATE_INVALID )
    {
        return 1;
    }

    if ( fireball_init(FIREBALL_BOARD_NUM, FIREBALL_IOM_MODULE) )
    {
        return 2;
    }

    //
    // Validate eControl against the detected Fireball board.
    //
    if ( (eControl >= AM_DEVICES_FIREBALL_STATE_FIRST)  &&
         (eControl <= AM_DEVICES_FIREBALL_STATE_LAST) )
    {
        //
        // Make sure we're running on an original Fireball.
        //
        if (( g_ui32FireballID != FIREBALL_ID )     &&
            ( g_ui32FireballID != FIREBALL2_ID )    &&
            ( g_ui32FireballID != FIREBALL3_ID ))
        {
            fireball_deinit(FIREBALL_BOARD_NUM);
            return 3;
        }
    }
    else if ( (eControl >= AM_DEVICES_FIREBALL2_STATE_FIRST)  &&
              (eControl <= AM_DEVICES_FIREBALL2_STATE_LAST) )
    {
        //
        // Make sure we're running on a Fireball 2.
        //
        if ( g_ui32FireballID != FIREBALL2_ID )
        {
            fireball_deinit(FIREBALL_BOARD_NUM);
            return 4;
        }
    }
    else if ( (eControl >= AM_DEVICES_FIREBALL3_STATE_FIRST)  &&
              (eControl <= AM_DEVICES_FIREBALL3_STATE_LAST) )
    {
        //
        // Make sure we're running on a Fireball 3.
        //
        if ( g_ui32FireballID != FIREBALL3_ID )
        {
            fireball_deinit(FIREBALL_BOARD_NUM);
            return 5;
        }
    }

    //
    // Initialize the mask variables to assume that we will NOT be setting the
    // Fireball state.  Both variables have to be touched.
    //
    ui64GPIOLowMask[0] = ui64GPIOHighMask[0] =
    ui64GPIOLowMask[1] = ui64GPIOHighMask[1] = 0xFFFFFFFF;

    if ( FBn_get() >= 3 )
    {
        //
        // If we're running on a FB3 but a FB2 state was specified, convert it
        //  to a FB3 state. Note that the states that are compatible between the
        //  Fireball 2 and 3 are expected to be in the same order and offset by
        //  a known quantity.
        //
        if ( (eControl >= AM_DEVICES_FIREBALL2_STATE_FIRST) &&
             (eControl <= AM_DEVICES_FIREBALL2_STATE_LAST) )
        {
            eControl += (AM_DEVICES_FIREBALL3_STATE_FIRST - AM_DEVICES_FIREBALL2_STATE_FIRST);
        }
    }

    switch ( eControl )
    {
        // ********************************************************************
        //
        // Begin Fireball(1) commands
        //
        // ********************************************************************
        case AM_DEVICES_FIREBALL_STATE_FBGEN_GET:
            if ( pArgs )
            {
                uint32_t ui32FBrev = 0;

                ui32RetVal = fireball_id_get(&g_ui32FireballID);
                g_ui32FireballFWVer = ui32RetVal ? 0 : (g_ui32FireballID >> 16) & 0xFF;
                g_ui32FireballID    = ui32RetVal ? 0 :  g_ui32FireballID & 0xFFFF;

                if ( g_ui32FireballID == FIREBALL_ID )
                {
                    ui32FBrev = 1;      // Original Fireball
                }
                else if ( g_ui32FireballID == FIREBALL2_ID )
                {
                    ui32FBrev = 2;      // Fireball2
                }
                else if ( g_ui32FireballID == FIREBALL3_ID )
                {
                    ui32FBrev = 3;      // Fireball3
                }
                *((uint32_t*)pArgs) = ui32FBrev;
            }
            else
            {
                ui32RetVal = AM_HAL_STATUS_INVALID_ARG;
            }
            break;

        case AM_DEVICES_FIREBALL_STATE_ID_GET:
            if ( pArgs )
            {
                ui32RetVal = fireball_id_get(&g_ui32FireballID);
                g_ui32FireballFWVer = ui32RetVal ? 0 : (g_ui32FireballID >> 16) & 0xFF;
                g_ui32FireballID    = ui32RetVal ? 0 :  g_ui32FireballID & 0xFFFF;
                *((uint32_t*)pArgs) = g_ui32FireballID;
            }
            else
            {
                ui32RetVal = AM_HAL_STATUS_INVALID_ARG;
            }
            break;

        case AM_DEVICES_FIREBALL_STATE_VER_GET:
            if ( pArgs )
            {
                ui32RetVal = fireball_id_get(&g_ui32FireballFWVer);
                g_ui32FireballID    = ui32RetVal ? 0 :  g_ui32FireballFWVer & 0xFFFF;
                g_ui32FireballFWVer = ui32RetVal ? 0 : (g_ui32FireballFWVer >> 16) & 0xFF;
                *((uint32_t*)pArgs) = g_ui32FireballFWVer;
            }
            else
            {
                ui32RetVal = AM_HAL_STATUS_INVALID_ARG;
            }
            break;

        case AM_DEVICES_FIREBALL_STATE_LED_BLINK:
            if ( pArgs )
            {
                uint32_t ui32Cnt = *(uint32_t*)pArgs;
                while (ui32Cnt)
                {
                    ui32RetVal = fireball_write_cmd(1, 39);
                    am_hal_flash_delay(FLASH_CYCLES_US(100000));    // 100ms
                    ui32RetVal = fireball_write_cmd(0, 39);
                    am_hal_flash_delay(FLASH_CYCLES_US(100000));    // 100ms
                    ui32Cnt--;
                }
            }
            else
            {
                ui32RetVal = AM_HAL_STATUS_INVALID_ARG;
            }
            break;
        case AM_DEVICES_FIREBALL_STATE_SPI_FLASH:
            //
            // GPIO LOW:  4-7,10,11
            // GPIO HIGH: -
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0xF << 4)  | ((uint64_t)0x3 << 10);
            ui64GPIOHighMask[0] = ((uint64_t)0x0 << 0);
            break;

        case AM_DEVICES_FIREBALL_STATE_SPI_FRAM:
            //
            // GPIO LOW:  4-7,10
            // GPIO HIGH: 11
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0xF << 4)  | ((uint64_t)0x1 << 10);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 11);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM0:
            //
            // GPIO LOW:  5,12-13
            // GPIO HIGH: 4
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x3 << 12);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM1:
            //
            // GPIO LOW:  5,12
            // GPIO HIGH: 4,13
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x1 << 12);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 13);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM2:
            //
            // GPIO LOW:  5,13
            // GPIO HIGH: 4,12
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x1 << 13);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 12);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM3:
            //
            // GPIO LOW:  5,14-15
            // GPIO HIGH: 4
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x3 << 14);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM4:
            //
            // GPIO LOW:  5,14
            // GPIO HIGH: 4,15
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x1 << 14);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 15);
            break;

        case AM_DEVICES_FIREBALL_STATE_I2C_IOM5:
            //
            // GPIO LOW:  5,15
            // GPIO HIGH: 4,14
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 5)  | ((uint64_t)0x1 << 15);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 14);
            break;

        case AM_DEVICES_FIREBALL_STATE_OCTAL_FLASH_CE0:
            //
            // GPIO LOW:  8-9,35-36
            // GPIO HIGH: 34,37
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 8)  | ((uint64_t)0x3 << 35);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 34) | ((uint64_t)0x1 << 37);
            break;

        case AM_DEVICES_FIREBALL_STATE_OCTAL_FLASH_CE1:
            //
            // GPIO LOW:  8-9,37
            // GPIO HIGH: 36
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 8) | ((uint64_t)0x1 << 37);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 36);
            break;

        case AM_DEVICES_FIREBALL_STATE_TWIN_QUAD_CE0_CE1:
            //
            // GPIO LOW:  8-9,35-37
            // GPIO HIGH: 34
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 8) | ((uint64_t)0x7 << 35);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 34);
            break;

        case AM_DEVICES_FIREBALL_STATE_ALL_RESET:
            //
            // SX9300:  43
            // BNO055:  42
            // MSPI:    41
            // MKB1:    40
            // MKB2:    39
            //
            device_reset(43, true);
            device_reset(42, true);
            device_reset(41, true);
            device_reset(40, false);
            device_reset(39, false);
            ui32RetVal = 0;
            break;

        case AM_DEVICES_FIREBALL_STATE_SX9300_RESET:
            ui32RetVal = device_reset(43, true);
            break;

        case AM_DEVICES_FIREBALL_STATE_BNO055_RESET:
            ui32RetVal = device_reset(42, true);
            break;

        case AM_DEVICES_FIREBALL_STATE_MSPI_RESET:
            ui32RetVal = device_reset(41, true);
            break;

        case AM_DEVICES_FIREBALL_STATE_MKB1_RESET:
            //
            // The 2 click reset signals actually drive a transistor.
            // Therefore, the signal ends up inverted.
            //
            ui32RetVal = device_reset(40, false);
            break;

        case AM_DEVICES_FIREBALL_STATE_MKB2_RESET:
            //
            // The 2 click reset signals actually drive a transistor.
            // Therefore, the signal ends up inverted.
            //
            ui32RetVal = device_reset(39, false);
            break;


        // ********************************************************************
        //
        // Begin Fireball2 commands
        //
        // ********************************************************************
        case AM_DEVICES_FIREBALL2_STATE_SPI_FRAM_PSRAM_1P8:
            //
            // GPIO LOW:  4-7,10
            // GPIO HIGH: 11
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0xF << 4) | ((uint64_t)0x1 << 10);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 11);
            break;

        case AM_DEVICES_FIREBALL2_STATE_SPI_PSRAM_FLASH_3P3:
            //
            // GPIO LOW:  4,6,10-11
            // GPIO HIGH: 5,7
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 10) | ((uint64_t)0x5 << 4);
            ui64GPIOHighMask[0] = ((uint64_t)0x5 << 5);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM0:
            //
            // GPIO LOW:  5,12,13
            // GPIO HIGH: 4
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 12) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM1:
            //
            // GPIO LOW:  5,12
            // GPIO HIGH: 4,13
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 12) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 13);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM2:
            //
            // GPIO LOW:  5,13
            // GPIO HIGH: 4,12
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 13) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 12);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM3:
            //
            // GPIO LOW:  5,14,15
            // GPIO HIGH: 4
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 14) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM4:
            //
            // GPIO LOW:  5,14
            // GPIO HIGH: 4,15
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 14) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 15);
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2C_IOM5:
            //
            // GPIO LOW:  5,15
            // GPIO HIGH: 4,14
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 15) | ((uint64_t)0x1 << 5);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 14);
            break;

        case AM_DEVICES_FIREBALL2_STATE_MSPI_FRAM_PSRAM_FLASH_1P8:
            //
            // GPIO LOW:  8,9,35,36
            // GPIO HIGH: 34,37
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 35) | ((uint64_t)0x3 << 8);
            ui64GPIOHighMask[0] = ((uint64_t)0x9 << 34);
            break;

        case AM_DEVICES_FIREBALL2_STATE_MSPI_PSRAM_FLASH_3P3:
            //
            // GPIO LOW:  8,35,37
            // GPIO HIGH: 9,34,36
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 8)  | ((uint64_t)0x5 << 35);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 9)  | ((uint64_t)0x5 << 34);
            break;

        case AM_DEVICES_FIREBALL2_STATE_SC_8_9_16:
            //
            // GPIO LOW:  4,11,16,17,22,23
            // GPIO HIGH: 5,10
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 22) | ((uint64_t)0x3 << 16) | ((uint64_t)0x1 << 11) | ((uint64_t)0x1 << 4);
            ui64GPIOHighMask[0] = ((uint64_t)0x5 << 10) | ((uint64_t)0x1 << 5);
            break;

        case AM_DEVICES_FIREBALL2_STATE_SC_17_32_26:
            //
            // GPIO LOW:  9,16,22
            // GPIO HIGH: 8,17,23
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 22) | ((uint64_t)0x1 << 16) | ((uint64_t)0x1 << 9);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 23) | ((uint64_t)0x1 << 17) | ((uint64_t)0x1 << 8);
            break;

        case AM_DEVICES_FIREBALL2_STATE_SC_19_18_46:
            //
            // GPIO LOW:  18-19,24-25,31
            // GPIO HIGH:
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 18) | ((uint64_t)0x3 << 24) | ((uint64_t)0x1 << 31);
            ui64GPIOHighMask[0] = 0;
            break;

        case AM_DEVICES_FIREBALL2_STATE_SC_31_37_21:
            //
            // GPIO LOW:  18,24,30-31
            // GPIO HIGH: 19,25
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 30) | ((uint64_t)0x1 << 24) | ((uint64_t)0x1 << 18);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 25) | ((uint64_t)0x1 << 19);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_10_29:
            //
            // GPIO LOW:  7,26-27
            // GPIO HIGH: 6
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 26) | ((uint64_t)0x1 << 7);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 6);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_12_15:
            //
            // GPIO LOW:  11,26
            // GPIO HIGH: 10,27
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 26) | ((uint64_t)0x1 << 11);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 27) | ((uint64_t)0x1 << 10);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_14_11:
            //
            // GPIO LOW:  11,27
            // GPIO HIGH: 10,26
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 27) | ((uint64_t)0x1 << 11);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 26) | ((uint64_t)0x1 << 10);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_22_34:
            //
            // GPIO LOW:  9,28-29
            // GPIO HIGH: 8
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 28) | ((uint64_t)0x1 << 9);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 8);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_37_36:
            //
            // GPIO LOW:  28,31
            // GPIO HIGH: 18-19,29
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 31) | ((uint64_t)0x1 << 28);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 29) | ((uint64_t)0x3 << 18);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_46_45:
            //
            // GPIO LOW:  29,31
            // GPIO HIGH: 24-25,28
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 31) | ((uint64_t)0x1 << 29);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 28) | ((uint64_t)0x3 << 24);
            break;

        case AM_DEVICES_FIREBALL2_STATE_PDM_AMP_IN:
            //
            // GPIO LOW:  33
            // GPIO HIGH:
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 33);
            ui64GPIOHighMask[0] = 0;
            break;

        case AM_DEVICES_FIREBALL2_STATE_I2S_DAC:
            //
            // GPIO LOW:  32
            // GPIO HIGH:
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 32);
            ui64GPIOHighMask[0] = 0;
            break;

        case AM_DEVICES_FIREBALL2_STATE_STEPPER:
            //
            // GPIO LOW:  34-35
            // GPIO HIGH: 18-19
            //
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 34);
            ui64GPIOHighMask[0] = ((uint64_t)0x3 << 18);
            break;

        //
        // Fireball2 resets
        //
        case AM_DEVICES_FIREBALL2_STATE_GLOBAL_RESET:
            ui32RetVal = device_reset(46, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_ALL_RESET:
            //
            // Reset each device individually.
            //
            device_reset(43, true);     // SX9300
            device_reset(42, true);     // BN0055
            device_reset(41, true);     // MSPI
            device_reset(40, false);    // MKB1
            device_reset(39, false);    // MKB2
            device_reset(45, false);    // GP30
            ui32RetVal = 0;
            break;

        case AM_DEVICES_FIREBALL2_STATE_SX9300_RESET:
            ui32RetVal = device_reset(43, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_BNO055_RESET:
            ui32RetVal = device_reset(42, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_MSPI_RESET:
            ui32RetVal = device_reset(41, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_MKB1_RESET:
            //
            // On Fireball2 the click board reset signals aren't really used,
            // so unlike FB there is no transistor and thus no inversion.  Even
            // though unsupported, we'll go ahead and issue a (normal) reset anyway.
            //
            ui32RetVal = device_reset(40, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_MKB2_RESET:
            ui32RetVal = device_reset(39, true);
            break;

        case AM_DEVICES_FIREBALL2_STATE_GP30_RESET:
            //
            // A general reset line for future use.
            //
            ui32RetVal = device_reset(45, true);
            break;

        // ********************************************************************
        //
        // Begin Fireball3 commands
        //
        // ********************************************************************
        case AM_DEVICES_FIREBALL3_STATE_SPI_FRAM_PSRAM_1P8:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 4)  | ((uint64_t)0xB << 7) | ((uint64_t)0x1 << 18);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 11);
            break;
        case AM_DEVICES_FIREBALL3_STATE_SPI_PSRAM_FLASH_3P3:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 4)  | ((uint64_t)0x3 << 10) | ((uint64_t)0x1 << 18);
            ui64GPIOHighMask[0] = ((uint64_t)0x3 << 7);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM0:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x3 << 12);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM1:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x1 << 12);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 13);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM2:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x1 << 13);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 12);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM3:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x9 << 14);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM4:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x1 << 14);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 17);
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2C_IOM5:
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7)  | ((uint64_t)0x1 << 17);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 4)  | ((uint64_t)0x1 << 14);
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI0_PSRAM_1P8:
            ui64GPIOLowMask[0]  = (uint64_t)0x05 << 53;
            ui64GPIOHighMask[0] = (uint64_t)0x17 << 50;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI0_FLASH_1P8:
            ui64GPIOLowMask[0]  = (uint64_t)0x0B << 52;
            ui64GPIOHighMask[0] = (uint64_t)0x13 << 50;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI0_PSRAM_3P3:
            ui64GPIOLowMask[0]  = (uint64_t)0x19 << 51;
            ui64GPIOHighMask[0] = (uint64_t)0x0D << 50;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI0_FT812:
            ui64GPIOLowMask[0]  = (uint64_t)0x33 << 50;
            ui64GPIOHighMask[0] = (uint64_t)0x03 << 52;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI0_DISPLAY:
            ui64GPIOLowMask[0]  = (uint64_t)0x05 << 52;
            ui64GPIOHighMask[0] = (uint64_t)0x2B << 50;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI1_PSRAM_1P8:
            ui64GPIOLowMask[0]  = (uint64_t)0x05 << 59;
            ui64GPIOHighMask[0] = (uint64_t)0x17 << 56;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI1_FLASH_1P8:
            ui64GPIOLowMask[0]  = (uint64_t)0x0B << 58;
            ui64GPIOHighMask[0] = (uint64_t)0x13 << 56;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI1_PSRAM_3P3:
            ui64GPIOLowMask[0]  = (uint64_t)0x19 << 57;
            ui64GPIOHighMask[0] = (uint64_t)0x0D << 56;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI1_FT812:
            ui64GPIOLowMask[0]  = (uint64_t)0x33 << 56;
            ui64GPIOHighMask[0] = (uint64_t)0x03 << 58;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI1_DISPLAY:
            ui64GPIOLowMask[0]  = (uint64_t)0x05 << 58;
            ui64GPIOHighMask[0] = (uint64_t)0x2B << 56;
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI2_PSRAM_1P8:
            ui64GPIOLowMask[0]   = 0;
            ui64GPIOHighMask[0]  = 0;
            ui64GPIOLowMask[1]  = (uint64_t)0x05 << (85 - 64);
            ui64GPIOHighMask[1] = (uint64_t)0x17 << (82 - 64);
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI2_FLASH_1P8:
            ui64GPIOLowMask[0]   = 0;
            ui64GPIOHighMask[0]  = 0;
            ui64GPIOLowMask[1]  = (uint64_t)0x0B << (84 - 64);
            ui64GPIOHighMask[1] = (uint64_t)0x13 << (82 - 64);
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI2_PSRAM_3P3:
            ui64GPIOLowMask[0]   = 0;
            ui64GPIOHighMask[0]  = 0;
            ui64GPIOLowMask[1]  = (uint64_t)0x19 << (83 - 64);
            ui64GPIOHighMask[1] = (uint64_t)0x0D << (82 - 64);
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI2_FT812:
            ui64GPIOLowMask[0]   = 0;
            ui64GPIOHighMask[0]  = 0;
            ui64GPIOLowMask[1]  = (uint64_t)0x33 << (82 - 64);
            ui64GPIOHighMask[1] = (uint64_t)0x03 << (84 - 64);
            break;
        case AM_DEVICES_FIREBALL3_STATE_MSPI2_DISPLAY:
            ui64GPIOLowMask[0]   = 0;
            ui64GPIOHighMask[0]  = 0;
            ui64GPIOLowMask[1]  = (uint64_t)0x05 << (84 - 64);
            ui64GPIOHighMask[1] = (uint64_t)0x2B << (82 - 64);
            break;

        case AM_DEVICES_FIREBALL3_STATE_SC_8_9_16:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 4) | ((uint64_t)0x1 << 11) | ((uint64_t)0x3 << 16) | ((uint64_t)0x3 << 22);
            ui64GPIOHighMask[0] = (uint64_t)0x21 << 5;
            break;
        case AM_DEVICES_FIREBALL3_STATE_SC_17_32_26:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 9) | ((uint64_t)0x1 << 16) | ((uint64_t)0x1 << 22);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 8) | ((uint64_t)0x1 << 17) | ((uint64_t)0x1 << 23);
            break;
        case AM_DEVICES_FIREBALL3_STATE_SC_19_18_46:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 18) | ((uint64_t)0x3 << 24) | ((uint64_t)0x1 << 31);
            ui64GPIOHighMask[0] = 0;
            break;
        case AM_DEVICES_FIREBALL3_STATE_SC_31_37_21:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 18) | ((uint64_t)0x1 << 24) | ((uint64_t)0x3 << 30);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 19) | ((uint64_t)0x1 << 25);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_10_29:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 7) | ((uint64_t)0x3 << 26);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 6);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_12_15:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 26) | ((uint64_t)0x1 << 11);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 27) | ((uint64_t)0x1 << 10);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_14_11:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 27) | ((uint64_t)0x1 << 11);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 26) | ((uint64_t)0x1 << 10);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_22_34:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 28) | ((uint64_t)0x1 << 9);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 8);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_37_36:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 31) | ((uint64_t)0x1 << 28);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 29) | ((uint64_t)0x3 << 18);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_46_45:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 31) | ((uint64_t)0x1 << 29);
            ui64GPIOHighMask[0] = ((uint64_t)0x1 << 28) | ((uint64_t)0x3 << 24);
            break;
        case AM_DEVICES_FIREBALL3_STATE_PDM_AMP_IN:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 33);
            ui64GPIOHighMask[0] = 0;
            break;
        case AM_DEVICES_FIREBALL3_STATE_I2S_DAC:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x1 << 32);
            ui64GPIOHighMask[0] = 0;
            break;
        case AM_DEVICES_FIREBALL3_STATE_STEPPER:
            // As of 9/28/19, these pins are shown on the FB3 Wiki page as "Needs updating"
            ui64GPIOLowMask[0]  = ((uint64_t)0x3 << 34);
            ui64GPIOHighMask[0] = ((uint64_t)0x3 << 18);
            break;

        // ********************************************************************
        //
        // Default
        //
        // ********************************************************************
        default:
            ui32RetVal = 0xdeadbeef;
            break;

    } // switch ()

    if ( ((ui64GPIOLowMask[0]  != 0xFFFFFFFF)    &&
          (ui64GPIOHighMask[0] != 0xFFFFFFFF))   ||
         ((ui64GPIOLowMask[1]  != 0xFFFFFFFF)    &&
          (ui64GPIOHighMask[1] != 0xFFFFFFFF)) )
    {

        ui64GPIOLowMask[1]  = ui64GPIOLowMask[1]  == 0xFFFFFFFF ? 0 : ui64GPIOLowMask[1];
        ui64GPIOHighMask[1] = ui64GPIOHighMask[1] == 0xFFFFFFFF ? 0 : ui64GPIOHighMask[1];

        ui32RetVal = fireball_set(ui64GPIOLowMask, ui64GPIOHighMask);
    }

    fireball_deinit(FIREBALL_BOARD_NUM);

    return ui32RetVal;

} // am_devices_fireball_control()

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

