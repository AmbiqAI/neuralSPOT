//*****************************************************************************
//!
//! @file apollo3_secbl_flash.h
//!
//! @brief the header file of secondary bootloader flash interface
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

#ifndef APOLLO3_SECBL_FLASH_H
#define APOLLO3_SECBL_FLASH_H

#include <stdbool.h>
#include <stdint.h>

//
// Max number of flash devices supported (including internal flash
// External flash devices could be connected through IOM or MSPI
// MSPI devices could also use the XIP aperture and store resident code
//
#define MAX_FLASH_DEV      3
#define FLASH_DEV_NAME_LEN 20

#define SSBL_FLASH_INIT_SUCCEED 0
#define SSBL_FLASH_INIT_FAIL    1

//*****************************************************************************
//
//! @name ssbl flash driver data structure
//! @brief the attributes and operation functions of the internal or external
//!        flash
//! @{
//
//*****************************************************************************
typedef struct
{
    uint8_t  devName[FLASH_DEV_NAME_LEN]; // flash device name for test purpose
    uint32_t ui32BaseAddr; // start address of the flash in the memory space
    uint32_t ui32Size; // size of the flash device
    bool     bXip; // true - if the flash support the XIP mode
    bool     bInternal; // true - if the flash is internal flash
    uint32_t (*flash_init)(void); // for external flash it needs to initialize the IOM or MSPI peripherals firstly
    uint32_t (*flash_erase)(uint32_t ui32StartAddr, uint32_t ui32NumBytes); // Note: ui32StartAddr should be erase block size aligned
    uint32_t (*flash_enable_xip)(void); // enable the XIP if flash device supports XIP
    uint32_t (*flash_disable_xip)(void); // disable the XIP
    uint32_t (*flash_write)(uint8_t *pui8WrBuf, uint32_t ui32WriteAddr, uint32_t ui32NumBytes); // Note: ui32WriteAddr should page size aligned
    uint32_t (*flash_read)(uint8_t *pui8RdBuf, uint32_t ui32ReadAddr, uint32_t ui32NumBytes); // Note: ui32ReadAddr should be page size aligned
    uint32_t (*flash_write_protect)(uint32_t ui32StartAddr, uint32_t ui32Size);
    uint32_t (*flash_copy_protect)(uint32_t ui32StartAddr, uint32_t ui32Size);
} am_ssbl_flash_t;

//*****************************************************************************
//
//! @brief find the flash driver instance by the image address
//!
//! @param otaImagePtr - image address
//! @param size - image size
//! @param ppFlashIf - the pointer to the flash instance
//!
//! This function is used to locate the flash driver instance by image address
//!
//! @return true if flash driver instance can be found
//
//*****************************************************************************
bool ssbl_find_flash_of_image(uint32_t otaImagePtr, uint32_t size, am_ssbl_flash_t **ppFlashIf);

//*****************************************************************************
//
//! @brief flash init function
//!
//! @param None
//!
//! initialize the flash management global variable and add the internal
//! flash support to the flash list
//!
//! @return None
//
//*****************************************************************************
void ssbl_flash_init(void);

//*****************************************************************************
//
//! @brief add the external flash driver instance
//!
//! @param pFlash - external flash driver instance
//!
//! @return SSBL_FLASH_INIT_SUCCEED if succeeded and SSBL_FLASH_INIT_FAIL if failed.
//
//*****************************************************************************
uint32_t ssbl_flash_add(am_ssbl_flash_t *pFlashIf);

#endif // APOLLO3_SECBL_FLASH_H
