//*****************************************************************************
//
//! @file ns_ipc_ring_buffer.h
//!
//! @brief Some helper functions for implementing and managing a ring buffer.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2017, Ambiq Micro
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
// This is part of revision 1.2.9 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef NS_IPC_RING_BUFFER_H
#define NS_IPC_RING_BUFFER_H

#include <stdint.h>
#include <string.h>
  
//*****************************************************************************
//
// Ring buffer setup structure definitions
//
//*****************************************************************************
typedef struct
{
    int32_t indx;
    volatile uint8_t* pData;
    volatile uint32_t ui32ByteSize;
} ns_ipc_ringbuff_setup_t;

//*****************************************************************************
//
// Ring buffer structure definitions
//
//*****************************************************************************
typedef struct
{
    volatile uint8_t *pui8Data;
    volatile uint32_t ui32BufferTail_write;
    volatile uint32_t ui32BufferHead_read;
    volatile uint32_t ui32OverWriting;
    volatile uint32_t ui32Capacity;
} ns_ipc_ring_buffer_t;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

extern void ns_ipc_ring_buffer_init(ns_ipc_ring_buffer_t* ring_buffs, ns_ipc_ringbuff_setup_t setup);

// void ns_ipc_ring_buffer_init_all(ns_ipc_ring_buffer_t* ring_buffs, const ns_ipc_ringbuff_setup_t* setup_array, uint32_t ui32BufferCount);


extern uint32_t ns_ipc_ring_buffer_push(ns_ipc_ring_buffer_t *psBuffer, void *pvSource, uint32_t ui32Bytes, bool bFullCheck);


extern uint32_t ns_ipc_ring_buffer_pop(ns_ipc_ring_buffer_t *psBuffer, void *pvDest,
                         uint32_t ui32Bytes);

uint32_t ns_ipc_get_ring_buffer_status(ns_ipc_ring_buffer_t *psBuffer);

uint8_t ns_ipc_ring_buffer_empty(ns_ipc_ring_buffer_t *psBuffer);

void ns_ipc_flush_ring_buffer(ns_ipc_ring_buffer_t *psBuffer);

uint32_t ns_ipc_ring_process(ns_ipc_ring_buffer_t *psSource, void *pvDest, uint32_t process_frame_bytes);

#endif // NS_IPC_RING_BUFFER_H

