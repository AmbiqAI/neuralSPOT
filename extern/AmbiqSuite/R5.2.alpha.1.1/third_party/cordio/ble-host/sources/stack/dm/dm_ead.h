//*****************************************************************************
//
//! @file dm_ead.h
//!
//! @brief Device manager encrypted advertising data module interface
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef DM_EAD_H
#define DM_EAD_H

#include "wsf_os.h"
#include "smp_api.h"
#include "dm_main.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (BT_54)
/**************************************************************************************************
  Macros
**************************************************************************************************/

/* DM sec event handler messages */
enum
{
  DM_EAD_MSG_API_ENC_CNF = DM_MSG_START(DM_ID_EAD),
  DM_EAD_MSG_API_DEC_CNF,
};


/**************************************************************************************************
  Data types
**************************************************************************************************/

/* Data type for DM_EAD_MSG_API_ENC_CNF */
typedef struct
{
  secCcmEncMsg_t        msg;
} dmEadEncrypt_t;

/* Data type for DM_EAD_MSG_API_DEC_CNF */
typedef struct
{
  secCcmDecMsg_t        msg;
} dmEadDecrypt_t;

typedef union
{
  wsfMsgHdr_t           hdr;
  dmEadEncrypt_t        eadEnc;
  dmEadDecrypt_t        eadDec;
} dmEadMsg_t;

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/**************************************************************************************************
  Function declarations
**************************************************************************************************/

/* component interface */
void dmEadMsgHandler(dmEadMsg_t *pMsg);

#endif // BT_54

#ifdef __cplusplus
};
#endif

#endif /* DM_EAD_H */
