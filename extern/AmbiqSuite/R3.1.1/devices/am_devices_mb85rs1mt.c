//*****************************************************************************
//
//! @file am_devices_mb85rs1mt.c
//!
//! @brief Fujitsu 64K SPI FRAM driver.
//!
//! @addtogroup mb85rs1mt MB85RS1MT SPI FRAM driver
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

#include "am_mcu_apollo.h"
#include "am_devices_mb85rs1mt.h"
#include "am_bsp.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
typedef struct
{
    uint32_t                    ui32Module;
    uint32_t                    ui32CS;
    void                        *pIomHandle;
    bool                        bOccupied;
} am_devices_iom_mb85rs1mt_t;

am_devices_iom_mb85rs1mt_t gAmMb85rs1mt[AM_DEVICES_MB85RS1MT_MAX_DEVICE_NUM];

am_hal_iom_config_t     g_sIomMb85rs1mtCfg =
{
    .eInterfaceMode       = AM_HAL_IOM_SPI_MODE,
    .ui32ClockFreq        = AM_HAL_IOM_1MHZ,
    .eSpiMode             = AM_HAL_IOM_SPI_MODE_0,
    .ui32NBTxnBufLength   = 0,
    .pNBTxnBuf = NULL,
};

//*****************************************************************************
//
//! @brief Generic Command Write function.
//!
//! @param pHandle
//! @param ui32InstrLen
//! @param ui64Instr
//! @param pData
//! @param ui32NumBytes
//!
//! @return status from am_devices_mb85rs1mt_status_t
//
//*****************************************************************************
static uint32_t
am_device_command_write(void *pHandle, uint32_t ui32InstrLen, uint64_t ui64Instr,
                        uint32_t *pData, uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    //
    // Create the transaction.
    //
    Transaction.ui32InstrLen    = ui32InstrLen;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui64Instr;
#else
    Transaction.ui32Instr       = (uint32_t)ui64Instr;
#endif
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32TxBuffer   = pData;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Generic Command Read function.
//!
//! @param pHandle
//! @param ui32InstrLen
//! @param ui64Instr
//! @param pData
//! @param ui32NumBytes
//!
//! @return status from am_devices_mb85rs1mt_status_t
//
//*****************************************************************************
static uint32_t
am_device_command_read(void *pHandle, uint32_t ui32InstrLen, uint64_t ui64Instr,
                       uint32_t *pData, uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    //
    // Create the transaction.
    //
    Transaction.ui32InstrLen    = ui32InstrLen;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui64Instr;
#else
    Transaction.ui32Instr       = (uint32_t)ui64Instr;
#endif
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = pData;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Initialize the mb85rs1mt driver.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_init(uint32_t ui32Module, am_devices_mb85rs1mt_config_t *pDevConfig, void **ppHandle, void **ppIomHandle)
{
    void *pIomHandle;
    am_hal_iom_config_t     stIOMMB85RS1MTSettings;

    uint32_t g_CS[AM_REG_IOM_NUM_MODULES] =
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
#if defined(APOLLO3P_EVB_CYGNUS)
    {
        0,
        AM_BSP_IOM1_CS_CHNL,
        0,
        AM_BSP_IOM3_CS_CHNL,
        AM_BSP_IOM4_CS_CHNL,
        0
    };
#elif defined(APOLLO3_EVB_CYGNUS)
    {
        0,
        AM_BSP_IOM1_CS_CHNL,
        0,
        AM_BSP_IOM3_CS_CHNL,
        AM_BSP_IOM4_CS_CHNL,
        0
    };
#else
    {
      AM_BSP_IOM0_CS_CHNL,
      AM_BSP_IOM1_CS_CHNL,
      AM_BSP_IOM2_CS_CHNL,
      AM_BSP_IOM3_CS_CHNL,
      AM_BSP_IOM4_CS_CHNL,
      AM_BSP_IOM5_CS_CHNL,
    };
#endif
#else
    {0, 0, 0, 0, 0, 0, 0, 0};
#endif
    uint32_t      ui32Index = 0;

    // Allocate a vacant device handle
    for ( ui32Index = 0; ui32Index < AM_DEVICES_MB85RS1MT_MAX_DEVICE_NUM; ui32Index++ )
    {
        if ( gAmMb85rs1mt[ui32Index].bOccupied == false )
        {
            break;
        }
    }
    if ( ui32Index == AM_DEVICES_MB85RS1MT_MAX_DEVICE_NUM )
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    if ( (ui32Module > AM_REG_IOM_NUM_MODULES)  || (pDevConfig == NULL) )
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Configure the IOM pins.
    //
    am_bsp_iom_pins_enable(ui32Module, AM_HAL_IOM_SPI_MODE);

    //
    // Enable fault detection.
    //
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    am_hal_fault_capture_enable();
#else
#if AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_FAULT_CAPTURE_ENABLE, 0);
#else // AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_fault_capture_enable();
#endif // AM_APOLLO3_MCUCTRL
#endif

    stIOMMB85RS1MTSettings = g_sIomMb85rs1mtCfg;
    stIOMMB85RS1MTSettings.ui32NBTxnBufLength = pDevConfig->ui32NBTxnBufLength;
    stIOMMB85RS1MTSettings.pNBTxnBuf = pDevConfig->pNBTxnBuf;
    stIOMMB85RS1MTSettings.ui32ClockFreq = pDevConfig->ui32ClockFreq;

    //
    // Initialize the IOM instance.
    // Enable power to the IOM instance.
    // Configure the IOM for Serial operation during initialization.
    // Enable the IOM.
    // HAL Success return is 0
    //
    if (am_hal_iom_initialize(ui32Module, &pIomHandle) ||
        am_hal_iom_power_ctrl(pIomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(pIomHandle, &stIOMMB85RS1MTSettings) ||
        am_hal_iom_enable(pIomHandle))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    else
    {
        gAmMb85rs1mt[ui32Index].bOccupied = true;
        gAmMb85rs1mt[ui32Index].ui32Module = ui32Module;
        gAmMb85rs1mt[ui32Index].ui32CS = g_CS[ui32Module];
        *ppIomHandle = gAmMb85rs1mt[ui32Index].pIomHandle = pIomHandle;
        *ppHandle = (void *)&gAmMb85rs1mt[ui32Index];
        //
        // Return the status.
        //
        return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
    }
}

//*****************************************************************************
//
//  De-Initialize the mb85rs1mt driver.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_term(void *pHandle)
{
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    if ( pIom->ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    // Disable the pins
    am_bsp_iom_pins_disable(pIom->ui32Module, AM_HAL_IOM_SPI_MODE);

    //
    // Disable the IOM.
    //
    am_hal_iom_disable(pIom->pIomHandle);

    //
    // Disable power to and uninitialize the IOM instance.
    //
    am_hal_iom_power_ctrl(pIom->pIomHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);

    am_hal_iom_uninitialize(pIom->pIomHandle);

    // Free this device handle
    pIom->bOccupied = false;

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Reads the ID of the external flash and returns the value.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_read_id(void *pHandle, uint32_t *pDeviceID)
{
    //
    // Send the command sequence to read the Device ID.
    //
    if (am_device_command_read(pHandle, 1, AM_DEVICES_MB85RS1MT_READ_DEVICE_ID, pDeviceID, 4))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;

}

//*****************************************************************************
//
// Reads the current status of the external flash
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_status_get(void *pHandle, uint32_t *pStatus)
{
    //
    // Send the command sequence to read the device status.
    //
    if (am_device_command_read(pHandle, 1, AM_DEVICES_MB85RS1MT_READ_STATUS, pStatus, 1))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Sends a specific command to the device (blocking).
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_command_send(void *pHandle, uint32_t ui32Cmd)
{
    uint32_t Dummy;

    if ( ui32Cmd == AM_DEVICES_MB85RS1MT_CMD_WREN )
    {
        //
        // Send the command to enable writing.
        //
        if (am_device_command_write(pHandle, 1, AM_DEVICES_MB85RS1MT_WRITE_ENABLE, &Dummy, 0))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }

    }
    else if ( ui32Cmd == AM_DEVICES_MB85RS1MT_CMD_WRDI )
    {
        //
        // Send the command to enable writing.
        //
        if (am_device_command_write(pHandle, 1, AM_DEVICES_MB85RS1MT_WRITE_DISABLE, &Dummy, 0))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }
    }
    else
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;

}

//*****************************************************************************
//
//  Programs the given range of flash addresses.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_blocking_write(void *pHandle, uint8_t *pui8TxBuffer,
                                    uint32_t ui32WriteAddress,
                                    uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    uint32_t              Dummy;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    //
    // Send the WRITE ENABLE command to enable writing.
    //
    if (am_device_command_write(pHandle, 1, AM_DEVICES_MB85RS1MT_WRITE_ENABLE, &Dummy, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Write the command to the device.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_WRITE;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE;
#endif
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = true;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Write the offset (24 bits) to the device.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 3;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui32WriteAddress & 0x00FFFFFF;
#else
    Transaction.ui32Instr       = ui32WriteAddress & 0x00FFFFFF;
#endif
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Send the command to disable writing.
    //
    if (am_device_command_write(pHandle, 1, AM_DEVICES_MB85RS1MT_WRITE_DISABLE, &Dummy, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Programs the given range of flash addresses.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_write_adv(void *pHandle, uint8_t *pui8TxBuffer,
                                           uint32_t ui32WriteAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_iom_callback_t pfnCallback,
                                           void *pCallbackCtxt)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    //
    // Common transaction parameters
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = ui32PauseCondition;

    //
    // Set up the IOM transaction to send the WREN device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_WRITE_ENABLE;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_ENABLE;
#endif
    Transaction.ui32NumBytes    = 0;        // WREN CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;
    Transaction.ui32StatusSetClr = 0;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(pIom->pIomHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }


    //
    // Set up the IOM transaction to write the FRAM command to the device.
    // This one needs to keep CE asserted (via continue).
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_WRITE;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE;
#endif
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.bContinue       = true;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(pIom->pIomHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    if (ui32NumBytes)
    {
        //
        // Set up the IOM transaction to write the offset (address) and data.
        //
        Transaction.eDirection      = AM_HAL_IOM_TX;
        Transaction.ui32InstrLen    = 3;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
        Transaction.ui64Instr       = ui32WriteAddress & 0x00FFFFFF;
#else
        Transaction.ui32Instr       = ui32WriteAddress & 0x00FFFFFF;
#endif
        Transaction.ui32NumBytes    = ui32NumBytes;
        Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
        Transaction.bContinue       = false;
        Transaction.ui32PauseCondition = 0;
        // Now set the post processing condition
        Transaction.ui32StatusSetClr = ui32StatusSetClr;

        //
        // Start the transaction
        //
        if (am_hal_iom_nonblocking_transfer(pIom->pIomHandle, &Transaction, pfnCallback, pCallbackCtxt))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }
    }
    // No Need to issue AM_DEVICES_MB85RS1MT_WRITE_DISABLE, as WEL gets deasserted
    // following the WRITE transaction

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Programs the given range of flash addresses.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_write(void *pHandle, uint8_t *pui8TxBuffer,
                                       uint32_t ui32WriteAddress,
                                       uint32_t ui32NumBytes,
                                       am_hal_iom_callback_t pfnCallback,
                                       void *pCallbackCtxt)
{
#if 0
    am_hal_iom_transfer_t Transaction;

    //
    // Common transaction parameters
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition       = 0;
    Transaction.ui32StatusSetClr = 0;

#if 0
    //
    // Set up the IOM transaction to send the WREN device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_ENABLE;
    Transaction.ui32NumBytes    = 0;        // WREN CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(pHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
#endif

    // Do the transaction (no callback).
    if (am_devices_mb85rs1mt_nonblocking_write_adv(pui8TxBuffer,
                                                   ui32WriteAddress,
                                                   ui32NumBytes,
                                                   0, 0, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction to send the WRDI (write disable) device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_DISABLE;
    Transaction.ui32NumBytes    = 0;        // WRDI CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;

    //
    // Start the transaction, sending the callback.
    //
    if (am_hal_iom_nonblocking_transfer(pHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
#else
    return (am_devices_mb85rs1mt_nonblocking_write_adv(pHandle, pui8TxBuffer,
                                                   ui32WriteAddress,
                                                   ui32NumBytes,
                                                   0, 0, pfnCallback, pCallbackCtxt));
#endif
}

//*****************************************************************************
//
//  Reads the contents of the fram into a buffer.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_blocking_read(void *pHandle, uint8_t *pui8RxBuffer,
                                   uint32_t ui32ReadAddress,
                                   uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition       = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Write the command to the device.
    //

    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_READ;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_READ;
#endif
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = true;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction to write to the device with offset address.
    //
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 3;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui32ReadAddress & 0x00FFFFFF;
#else
    Transaction.ui32Instr       = ui32ReadAddress & 0x00FFFFFF;
#endif
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Reads the contents of the fram into a buffer.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_read(void *pHandle, uint8_t *pui8RxBuffer,
                                      uint32_t ui32ReadAddress,
                                      uint32_t ui32NumBytes,
                                      am_hal_iom_callback_t pfnCallback,
                                      void *pCallbackCtxt)
{
    am_hal_iom_transfer_t      Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Set up the IOM transaction to write the FRAM command to the device.
    // This one needs to keep CE asserted (via continue).
    //

    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_READ;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_READ;
#endif
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = true;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(pIom->pIomHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction.
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 3;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui32ReadAddress & 0x00FFFFFF;
#else
    Transaction.ui32Instr       = ui32ReadAddress & 0x00FFFFFF;
#endif
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_nonblocking_transfer(pIom->pIomHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}


//*****************************************************************************
//
//  Reads the contents of the fram into a buffer.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_read_hiprio(void *pHandle, uint8_t *pui8RxBuffer,
                                             uint32_t ui32ReadAddress,
                                             uint32_t ui32NumBytes,
                                             am_hal_iom_callback_t pfnCallback,
                                             void *pCallbackCtxt)
{
    am_hal_iom_transfer_t      Transaction;
    am_devices_iom_mb85rs1mt_t *pIom = (am_devices_iom_mb85rs1mt_t *)pHandle;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Set up the IOM transaction to write the FRAM command to the device.
    // This one needs to keep CE asserted (via continue).
    //

    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = AM_DEVICES_MB85RS1MT_READ;
#else
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_READ;
#endif
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = true;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_highprio_transfer(pIom->pIomHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction.
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 3;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    Transaction.ui64Instr       = ui32ReadAddress & 0x00FFFFFF;
#else
    Transaction.ui32Instr       = ui32ReadAddress & 0x00FFFFFF;
#endif
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = pIom->ui32CS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_highprio_transfer(pIom->pIomHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

