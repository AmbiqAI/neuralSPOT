//*****************************************************************************
//!
//! @file apollo3_secbl_image.h
//!
//! @brief the header file of secondary bootloader image validation functions
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

#ifndef APOLLO3_SECBL_IMAGE_H
#define APOLLO3_SECBL_IMAGE_H

#include "apollo3_secbl_secure.h"
#include "apollo3_secbl_flash.h"

#ifdef AM_PART_APOLLO3P
#define SRAM_SIZE (768 << 10)
#else
#error "Please set specify the SRAM size accroding to your chip"
#endif

#ifdef SECURE_BOOT
//*****************************************************************************
//
//! @brief verify the boot image
//!
//! @param pComHdr - the pointer that holds the image header information
//! @param pFlash - the flash driver instance that the image resides
//!
//! @return ture if the image is valid
//
//*****************************************************************************
bool ssbl_validate_boot_image(am_image_hdr_common_t *pComHdr, am_ssbl_flash_t *pFlash);
#endif

//*****************************************************************************
//
//! @brief verify the ota image
//!
//! @param ui32OtaImageAddr - the ota image start address
//! @param pFlash - the flash driver instance that the image resides
//! @param pDecryptInfo - the information that decryption algorithm needs
//! @param pMainHdr - the pointer that holds the main image header information
//!
//! @return true if the ota image is valid
//
//*****************************************************************************
bool ssbl_validate_ota_image(uint32_t ui32OtaImageAddr, am_ssbl_flash_t *pFlash, ssbl_decrypt_info_t *pDecryptInfo, am_main_image_hdr_t *pMainHdr);
//*****************************************************************************
//
//! @brief verify the child image
//!
//! @param pChildPtr - the child image start address
//!
//! @return true if the child image is valid
//
//*****************************************************************************
bool ssbl_validate_child_images(uint32_t *pChildPtr);

//*****************************************************************************
//
//! @brief verify the main image
//!
//! @param ppVtor - hold the address that bootloader can jump to
//!
//!  Verify the main image & any linked child images
//!  Also applies necessary protections on successful validation
//!  Returns the Start of the image
//!
//! @return AM_HAL_STATUS_SUCCESS if all images are valid
//
//*****************************************************************************
uint32_t ssbl_validate_main_image(uint32_t **ppVtor);

#endif
