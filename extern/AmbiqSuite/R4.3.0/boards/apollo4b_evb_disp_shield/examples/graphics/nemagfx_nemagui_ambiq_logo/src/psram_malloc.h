//*****************************************************************************
//
//! @file psram_malloc.h
//!
//! @brief NemaGFX example.
//
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
// -----------------------------------------------------------------------------
// Copyright (c) 2019 Think Silicon S.A.
// Think Silicon S.A. Confidential Proprietary
// -----------------------------------------------------------------------------
//     All Rights reserved - Unpublished -rights reserved under
//         the Copyright laws of the European Union
//
//  This file includes the Confidential information of Think Silicon S.A.
//  The receiver of this Confidential Information shall not disclose
//  it to any third party and shall protect its confidentiality by
//  using the same degree of care, but not less than a reasonable
//  degree of care, as the receiver uses to protect receiver's own
//  Confidential Information. The entire notice must be reproduced on all
//  authorised copies and copies may only be made to the extent permitted
//  by a licensing agreement from Think Silicon S.A..
//
//  The software is provided 'as is', without warranty of any kind, express or
//  implied, including but not limited to the warranties of merchantability,
//  fitness for a particular purpose and noninfringement. In no event shall
//  Think Silicon S.A. be liable for any claim, damages or other liability, whether
//  in an action of contract, tort or otherwise, arising from, out of or in
//  connection with the software or the use or other dealings in the software.
//
//
//                    Think Silicon S.A.
//                    http://www.think-silicon.com
//                    Patras Science Park
//                    Rion Achaias 26504
//                    Greece
// -----------------------------------------------------------------------------

#ifndef PSRAM_MALLOC_H__
#define PSRAM_MALLOC_H__

#include "nema_hal.h"

#ifndef MAX_MEM_POOLS
#define MAX_MEM_POOLS 8
#endif // MAX_MEM_POOLS

#define ALIGNNUM  (16)
#define ALIGNMASK (ALIGNNUM-1)
#define ALIGN(s) ((((s) + ALIGNMASK) / ALIGNNUM) * ALIGNNUM)

#define FLAG_EMPTY    0xf1fa1U
#define FLAG_NONEMPTY 0xf1fa2U

#define IS_LAST(c) ( (c)->next_offset == 0U )
#define OFFSET(c) ((uintptr_t)(c) - (uintptr_t)HEAD)

typedef struct cell
{
    int size;
    unsigned flags;
    uintptr_t next_offset;
} cell_t;

static const int cell_t_size = (ALIGN((int)sizeof(cell_t)));

typedef struct pool
{
    uintptr_t base_phys;
    uintptr_t base_virt;
    uintptr_t end_virt;
    cell_t   *head_of_empty_list;
    int size;
} pool_t;

void* psram_malloc(int32_t size);

int32_t psram_malloc_init(void *base_virt,
                          uintptr_t base_phys,
                          int32_t size,
                          int32_t reset);

void  psram_free(void *ptr);
uintptr_t psram_virt2phys(void *addr);
nema_buffer_t nema_psram_buffer_create(int32_t size);

#endif
