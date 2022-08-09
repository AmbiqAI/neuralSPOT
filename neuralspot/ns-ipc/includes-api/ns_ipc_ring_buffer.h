//*****************************************************************************
//
//! @file am_app_utils_ring_buffer.h
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
#ifndef AM_UTIL_RING_BUFFER_H
#define AM_UTIL_RING_BUFFER_H

//#include "am_vos_sys_config.h"

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
}am_app_utils_ringbuff_setup_t;

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
}am_app_utils_ring_buffer_t;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

void am_app_utils_ring_buffer_init(am_app_utils_ring_buffer_t* ring_buffs, am_app_utils_ringbuff_setup_t setup);

void am_app_utils_ring_buffer_init_all(am_app_utils_ring_buffer_t* ring_buffs, const am_app_utils_ringbuff_setup_t* setup_array, uint32_t ui32BufferCount);


uint32_t am_app_utils_ring_buffer_push(am_app_utils_ring_buffer_t *psBuffer, void *pvSource, uint32_t ui32Bytes, bool bFullCheck);


uint32_t am_app_utils_ring_buffer_pop(am_app_utils_ring_buffer_t *psBuffer, void *pvDest,
                         uint32_t ui32Bytes);

uint32_t am_app_utils_get_ring_buffer_status(am_app_utils_ring_buffer_t *psBuffer);

uint8_t am_app_utils_ring_buffer_empty(am_app_utils_ring_buffer_t *psBuffer);

void am_app_utils_flush_ring_buffer(am_app_utils_ring_buffer_t *psBuffer);

uint32_t am_app_utils_ring_process(am_app_utils_ring_buffer_t *psSource, void *pvDest, uint32_t process_frame_bytes);

#endif // AM_UTIL_RING_BUFFER_H

