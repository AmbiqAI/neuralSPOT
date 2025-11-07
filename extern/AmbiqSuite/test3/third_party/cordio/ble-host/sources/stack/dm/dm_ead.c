//*****************************************************************************
//
//! @file dm_ead.c
//!
//! @brief Device manager encrypted advertising data module
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_msg.h"
#include "sec_api.h"
#include "wsf_buf.h"
#include "wsf_trace.h"
#include "util/calc128.h"
#include "smp_api.h"
#include "dm_api.h"
#include "dm_main.h"
#include "dm_sec.h"
#define EAD_ADDITIONAL_AUTH
#if (BT_54)
#include "dm_ead.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/* Component function interface */
static const dmFcnIf_t dmEadFcnIf =
{
  dmEmptyReset,
  (dmHciHandler_t) dmEmptyHandler,
  (dmMsgHandler_t) dmEadMsgHandler
};

/*************************************************************************************************/
/*!
 *  \brief  DM encrypted advertising data event handler.
 *
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void dmEadMsgHandler(dmEadMsg_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case DM_EAD_MSG_API_ENC_CNF:
      pMsg->hdr.event = DM_EAD_ENC_CMPL_IND;
      (*dmCb.cback)((dmEvt_t *) pMsg);
      break;

    case DM_EAD_MSG_API_DEC_CNF:
      pMsg->hdr.event = DM_EAD_DEC_CMPL_IND;
      (*dmCb.cback)((dmEvt_t *) pMsg);
      break;

    default:
      break;
  }
}

void DmEadInit(void)
{
  /* set function interface table */
  dmFcnIfTbl[DM_ID_EAD] = (dmFcnIf_t *) &dmEadFcnIf;
}

/*************************************************************************************************/
/*!
 *  \brief  This function is called to execute the CCM-Mode encryption algorithm.
 *
 *  \param  pKey          Pointer to encryption key (SEC_CCM_KEY_LEN bytes).
 *  \param  pNonce        Pointer to nonce (SEC_CCM_NONCE_LEN bytes).
 *  \param  pPlainText    Pointer to text to encrypt.
 *  \param  textLen       Length of pPlainText in bytes.
 *  \param  pClear        Pointer to additional, unencrypted authentication text.
 *  \param  clearLen      Length of pClear in bytes.
 *  \param  micLen        Size of MIC in bytes (4, 8 or 16).
 *  \param  pResult       Buffer to hold result (returned in complete event).

 *  \return None.
 */
/*************************************************************************************************/
static void dmCcmEncrypt(uint8_t *pKey, uint8_t *pNonce, uint8_t *pPlainText, uint16_t textLen,
                        uint8_t *pClear, uint16_t clearLen, uint8_t micLen, uint8_t *pResult)
{
  SecCcmEnc(pKey, pNonce, pPlainText, textLen, pClear, clearLen, micLen, pResult,
            dmCb.handlerId, 0, DM_EAD_MSG_API_ENC_CNF);
}

/*************************************************************************************************/
/*!
 *  \brief  Eexecute the CCM-Mode verify and decrypt for encrypted data.
 *
 *  \param  pKey          Pointer to encryption key (SEC_CCM_KEY_LEN bytes).
 *  \param  pNonce        Pointer to nonce (SEC_CCM_NONCE_LEN bytes).
 *  \param  pCypherText   Pointer to text to decrypt.
 *  \param  textLen       Length of pCypherText in bytes.
 *  \param  pClear        Pointer to additional, unencrypted authentication text.
 *  \param  clearLen      Length of pClear in bytes.
 *  \param  pMic          Pointer to authentication digest.
 *  \param  micLen        Size of MIC in bytes (4, 8 or 16).
 *  \param  pResult       Buffer to hold result (returned in complete event).
 *
 *  \return None.
 */
/*************************************************************************************************/
static void dmCcmDecrypt(uint8_t *pKey, uint8_t *pNonce, uint8_t *pCypherText, uint16_t textLen,
                  uint8_t *pClear, uint16_t clearLen, uint8_t *pMic, uint8_t micLen, uint8_t *pResult)
{
  SecCcmDec(pKey, pNonce, pCypherText, textLen, pClear, clearLen, pMic, micLen, pResult,
            dmCb.handlerId, 0, DM_EAD_MSG_API_DEC_CNF);
}

/*************************************************************************************************/
/*!
  *  \brief  Execute the CCM encryption by advertiser for advertising data.
  *
  *  \param  sessionKey   Pointer to session key.
  *  \param  iv           Pointer to initialization vector.
  *  \param  len          Length of plain payload (Input data) in bytes.
  *  \param  pDataIn      Pointer to plain payload (Input data).
  *  \param  pDataOut     Pointer to encrypted payload (Output data).
  *
  *  \return None.
  */
/*************************************************************************************************/
void DmEadEncrypt(uint8_t *sessionKey, uint8_t *iv, uint16_t len, uint8_t *pDataIn, uint8_t *pDataOut)
{
  WSF_ASSERT((sessionKey != NULL) && (iv != NULL) && (pDataIn != NULL) && (pDataOut != NULL) && (len > 0));

  static uint8_t nonce[DM_EAD_NONCE_SIZE];
  uint8_t randomizer[DM_EAD_RANDOMIZER_SIZE];
#ifdef EAD_ADDITIONAL_AUTH
  static  uint8_t aad[DM_EAD_AAD_SIZE] = {DM_EAD_AAD_VALUE}; // Additional Authenticated Data
#endif // EAD_ADDITIONAL_AUTH

  /* Generate the randomizer and nonce */
  SecRand(&randomizer[0], DM_EAD_RANDOMIZER_SIZE);
  randomizer[DM_EAD_RANDOMIZER_SIZE - 1] |= 1 << DM_EAD_RANDOMIZER_DIR_BIT;
  memcpy(&nonce[0], &randomizer[0], DM_EAD_RANDOMIZER_SIZE);
  memcpy(&nonce[DM_EAD_RANDOMIZER_SIZE], iv, DM_EAD_IV_SIZE);

  /* Put the randomizer to the head of output payload */
  memcpy(pDataOut, nonce, DM_EAD_RANDOMIZER_SIZE);

#ifdef EAD_ADDITIONAL_AUTH
  dmCcmEncrypt(sessionKey, nonce, pDataIn, len, aad, DM_EAD_AAD_SIZE,
               DM_EAD_MIC_SIZE, &pDataOut[DM_EAD_RANDOMIZER_SIZE]);
#else
  dmCcmEncrypt(sessionKey, nonce, pDataIn, len, NULL, 0,
               DM_EAD_MIC_SIZE, &pDataOut[DM_EAD_RANDOMIZER_SIZE]);
#endif // EAD_ADDITIONAL_AUTH
}

/*************************************************************************************************/
/*!
  *  \brief  Execute the CCM decryption by scanner for encrypted advertising data.
  *
  *  \param  sessionKey   Pointer to session key.
  *  \param  iv           Pointer to initialization vector.
  *  \param  len          Length of encrypted payload (Input data) in bytes.
  *  \param  pDataIn      Pointer to encrypted payload (Input data).
  *  \param  pDataOut     Pointer to decrypted payload (Output data).
  *
  *  \return None.
  */
/*************************************************************************************************/
void DmEadDecrypt(uint8_t *sessionKey, uint8_t *iv, uint16_t len, uint8_t *pDataIn, uint8_t *pDataOut)
{
  WSF_ASSERT((sessionKey != NULL) && (iv != NULL) && (pDataIn != NULL) && (pDataOut != NULL)
    && (len > (DM_EAD_RANDOMIZER_SIZE + DM_EAD_MIC_SIZE)));
   
  static uint8_t nonce[DM_EAD_NONCE_SIZE];
  uint8_t *randomizer;
  uint8_t *payload;
  uint8_t *mic;
  uint8_t payloadLen;
#ifdef EAD_ADDITIONAL_AUTH
  static uint8_t aad[DM_EAD_AAD_SIZE] = {DM_EAD_AAD_VALUE};
#endif // EAD_ADDITIONAL_AUTH

  /* Retrieve the nonce from encrypted payload and IV */
  randomizer = pDataIn;
  memcpy(&nonce[0], randomizer, DM_EAD_RANDOMIZER_SIZE);
  memcpy(&nonce[DM_EAD_RANDOMIZER_SIZE], iv, DM_EAD_IV_SIZE);

  /* Get the encrypted payload and payload length */
  payload = &pDataIn[DM_EAD_RANDOMIZER_SIZE];
  payloadLen = len - DM_EAD_RANDOMIZER_SIZE - DM_EAD_MIC_SIZE;
  mic = &pDataIn[len - DM_EAD_MIC_SIZE];

#ifdef EAD_ADDITIONAL_AUTH
  dmCcmDecrypt(sessionKey, nonce, payload, payloadLen, aad, DM_EAD_AAD_SIZE,
               mic, DM_EAD_MIC_SIZE, pDataOut);
#else
  dmCcmDecrypt(sessionKey, nonce, payload, payloadLen, NULL, 0,
               mic, DM_EAD_MIC_SIZE, pDataOut);
#endif // EAD_ADDITIONAL_AUTH
}

#endif // BT_54
