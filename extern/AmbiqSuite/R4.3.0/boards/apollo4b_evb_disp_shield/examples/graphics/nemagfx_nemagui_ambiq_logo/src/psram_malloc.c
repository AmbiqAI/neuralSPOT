//*****************************************************************************
//
//! @file psram_malloc.c
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

#include "nema_sys_defs.h"
#include "psram_malloc.h"
#include "nema_hal.h"

static pool_t g_sPools[MAX_MEM_POOLS] = {{0}};
#define HEAD (g_sPools[i32Pool].head_of_empty_list)

#define IF_POOL_IS_INVALID \
    if ( i32Pool < 0 || i32Pool >= MAX_MEM_POOLS || HEAD == NULL )

static inline int32_t
pool_from_virt_addr(void *addr)
{
    uintptr_t addr_u = (uintptr_t)addr;
    for (int32_t i32Pool = 0; i32Pool < MAX_MEM_POOLS; ++i32Pool)
    {
        if ( HEAD != NULL )
        {
            if (addr_u >= g_sPools[i32Pool].base_virt && addr_u < g_sPools[i32Pool].end_virt)
            {
                return i32Pool;
            }
        }
    }

    return -1;
}

//pointer to next cell
static inline cell_t *
next_cell(int32_t i32Pool, cell_t *pCurCell)
{
    uintptr_t cur_cell_u = (uintptr_t)g_sPools[i32Pool].head_of_empty_list + pCurCell->next_offset;
    return (cell_t *)cur_cell_u;
}

//cell to pointer
static inline uintptr_t
c2p(cell_t *c)
{
    return (uintptr_t)(c) + (uintptr_t)cell_t_size;
}

//pointer to cell
static inline cell_t *
p2c(void *p)
{
    uintptr_t ptr = (uintptr_t)(p) - (uintptr_t)cell_t_size;
    return (cell_t *)ptr;
}

//pC1 is adjascent to pC2
static inline bool
isadj(cell_t *pC1, cell_t *pC2)
{
    uintptr_t c1_p_ptr = c2p(pC1);
    uintptr_t c1_next_c_ptr = c1_p_ptr + (uintptr_t)(int32_t)(pC1)->size;
    uintptr_t c2_c_ptr = (uintptr_t)pC2;
    return c1_next_c_ptr == c2_c_ptr;
}

static int32_t
psram_malloc_init_pool(int32_t i32Pool,
                       void *base_virt,
                       uintptr_t base_phys,
                       int32_t size,
                       int32_t reset)
{
    if (i32Pool >= MAX_MEM_POOLS || i32Pool < 0)
    {
        return -1;
    }

    HEAD = (cell_t *)base_virt;
    g_sPools[i32Pool].end_virt = (uintptr_t)base_virt + (uintptr_t)(int32_t)size;

    g_sPools[i32Pool].base_phys = base_phys;
    g_sPools[i32Pool].base_virt = (uintptr_t)base_virt;
    g_sPools[i32Pool].size      = size;

    if (reset != 0)
    {
        HEAD->size  = 0;
        HEAD->next_offset = cell_t_size;
        HEAD->flags = FLAG_EMPTY;

        cell_t *next = next_cell(i32Pool, HEAD);
        next->size = size-2*cell_t_size;
        next->next_offset = 0;
        next->flags = FLAG_EMPTY;
    }

    return 0;
}

static void *
psram_malloc_pool(int32_t i32Pool, int32_t size)
{
    IF_POOL_IS_INVALID
    {
        return NULL;
    }

    cell_t *pC1, *c_prev;

    if ( size < cell_t_size )
    {
        size = cell_t_size;
    }

    size = ALIGN(size);

    c_prev = HEAD;
    pC1 = HEAD;

    //find a big enough cell (pC1->next)
    while (pC1->size < size)
    {
        if ( IS_LAST(pC1) /*|| next_cell(i32Pool, pC1)->flags == FLAG_NONEMPTY*/)
        {
            //returned to head_of_empty_list without finding space
            return NULL;
        }

        //try next cell
        c_prev = pC1;
        pC1 = next_cell(i32Pool, pC1);
    }

    //pC1 is a big enough empty cell
    if (pC1->size > (cell_t_size + size))
    {
        //pC1 is bigger than needed
        //split it
        uintptr_t next_ptr = c2p(pC1) + (uintptr_t)(int32_t)size;
        cell_t *c_next = (cell_t *)(next_ptr);
        c_next->size  = pC1->size - (size + cell_t_size);
        c_next->flags = FLAG_EMPTY;
        c_next->next_offset  = pC1->next_offset;

        c_prev->next_offset = OFFSET(c_next);

        pC1->size = size;
    }
    else
    {
        c_prev->next_offset = pC1->next_offset;
    }

    pC1->flags = FLAG_NONEMPTY;

    uintptr_t ptr = c2p(pC1);
    return (void *)(char *)ptr;
}

void
psram_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    int32_t i32Pool = pool_from_virt_addr(ptr);

    IF_POOL_IS_INVALID
    {
        return;
    }

    cell_t *pC1, *pC2, *pC3;
    bool j1, j2;

    //find cell from pointer
    char *_ptr = ptr; //misra stuff...
    pC2 = p2c(_ptr);
    if (pC2->flags != FLAG_NONEMPTY)
    {
        return;
    }

    pC1 = HEAD;

    pC2->flags = FLAG_EMPTY;

    uintptr_t next_ptr = (uintptr_t)next_cell(i32Pool, pC1);
    uintptr_t c2_ptr = (uintptr_t)pC2;

    while ( next_ptr < c2_ptr && !IS_LAST(pC1))  /* find insertion point */
    {
        pC1 = next_cell(i32Pool, pC1);
    }

    pC3 = next_cell(i32Pool, pC1);

    if (pC1 == HEAD)
    {
        j1 = false;
    }
    else
    {
        j1 = isadj(pC1, pC2); /* pC1 and pC2 need to be joined */
    }

    j2 = isadj(pC2, pC3);     /* pC2 and pC3 need to be joined */

    //make pC1 point to pC2
    pC1->next_offset = OFFSET(pC2);
    //make pC2 point to pC3
    pC2->next_offset = OFFSET(pC3);

    if (j1)
    {
        //join pC1 with pC2
        pC1->size += cell_t_size + pC2->size;
        pC1->next_offset = OFFSET(pC3);
        pC2 = pC1;
    }

    if (j2)
    {
        //join pC2 with pC3
        pC2->size += cell_t_size + pC3->size;
        pC2->next_offset = pC3->next_offset;
    }
}

uintptr_t
psram_virt2phys(void *addr)
{
#ifdef USE_GLOBAL_MMU
    return (uintptr_t)addr;
#else
    int32_t i32Pool = pool_from_virt_addr(addr);
    IF_POOL_IS_INVALID
    {
        return 0U;
    }

    char *_addr = addr; //misra stuff...

    uintptr_t offset = OFFSET(_addr);
    uintptr_t phys_addr = offset + g_sPools[i32Pool].base_phys;
    return phys_addr;
#endif
}

nema_buffer_t
nema_psram_buffer_create(int32_t size)
{
    nema_mutex_lock(MUTEX_MALLOC);

    nema_buffer_t bo;
#ifdef USE_TSI_MALLOC
    bo.base_virt = psram_malloc((size_t)size);
#else
    // TODO: align to 8 bytes
    //   size += 8;
    //   base_addr = (base_addr+7)&(~(0x7));

/*     bo.base_virt = pvPortMalloc((size_t)size); */
    bo.base_virt = current_buf;
    current_buf += size;
#endif

    bo.base_phys = (uintptr_t) (bo.base_virt);
    bo.size      = size;
    bo.fd        = 0;

    nema_mutex_unlock(MUTEX_MALLOC);
    return bo;
}

void*
psram_malloc(int32_t size)
{
    return psram_malloc_pool(0, size);
}

int32_t
psram_malloc_init( void *base_virt,
                   uintptr_t base_phys,
                   int32_t size,
                   int32_t reset)
{
    return psram_malloc_init_pool(0, base_virt, base_phys, size, reset);
}

