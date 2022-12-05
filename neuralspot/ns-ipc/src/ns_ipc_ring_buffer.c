//*****************************************************************************
//
//! @file ns_ipc_ring_buffer.c
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

#include <stdbool.h>
#include <stdint.h>

#include "am_mcu_apollo.h"
#include "ns_ipc_ring_buffer.h"

uint8_t
ns_ipc_ring_buffer_empty(ns_ipc_ring_buffer_t *psBuffer) {
    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32OverWriting = psBuffer->ui32OverWriting;
    return (ui32BufferTail_write == ui32BufferHead_read && (!ui32OverWriting));
}

uint8_t
ns_ipc_ring_buffer_full(ns_ipc_ring_buffer_t *psBuffer) {
    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32OverWriting = psBuffer->ui32OverWriting;
    return (ui32BufferTail_write == ui32BufferHead_read && (ui32OverWriting));
}

uint8_t
ns_ipc_ring_buffer_overwrite(ns_ipc_ring_buffer_t *psBuffer) {
    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32OverWriting = psBuffer->ui32OverWriting;
    return (ui32BufferTail_write != ui32BufferHead_read && (ui32OverWriting));
}

//*****************************************************************************
//
//! @brief Initializes a ring buffer structure.
//!
//! @param psBuffer is a pointer to the buffer structure to be initialized.
//!
//! @param pvArray is a pointer to the array that the new ringbuffer will use
//! for storage space
//!
//! @param ui32Bytes is the total number of bytes that the ringbuffer will be
//! allowed to use.
//!
//! This function should be called on a ring buffer structure before it is
//! used. If this function is called on a ring buffer that is already being
//! used, it will "erase" the buffer, effectively removing all of the buffer
//! contents from the perspective of the other ring buffer access functions.
//! The data will remain in memory, but it will be overwritten as the buffer is
//! used.
//!
//! @note This operation is not inherently thread-safe, so the caller must make
//! sure that it is appropriately guarded from interrupts and context switches.
//!
//! @return
//
//*****************************************************************************
void
ns_ipc_ring_buffer_init(ns_ipc_ring_buffer_t *ring_buffs, ns_ipc_ringbuff_setup_t setup) {
    ring_buffs[setup.indx].ui32BufferHead_read = 0;
    ring_buffs[setup.indx].ui32BufferTail_write = 0;
    ring_buffs[setup.indx].ui32OverWriting = 0;
    ring_buffs[setup.indx].ui32Capacity = setup.ui32ByteSize;
    ring_buffs[setup.indx].pui8Data = setup.pData;
}

// void
// ns_ipc_ring_buffer_init_all(
//     ns_ipc_ring_buffer_t *ring_buffs,
//     const ns_ipc_ringbuff_setup_t *setup_array,
//     uint32_t ui32BufferCount) {
//     uint32_t ringbuff = 0;
//     for (ringbuff = 0; ringbuff < ui32BufferCount; ringbuff++) {
//         ns_ipc_ring_buffer_init(ring_buffs, setup_array[ringbuff]);
//     }
// }

//*****************************************************************************
// Function: ring buffer push function
// Paras:
//      psBuffer: the target ring buffer needs to be pushed
//      pvSource: the data source
//      ui32Bytes: target transfer data length
//      bFullCheck: whether checking the target ring buffer full or not. If
//      true, data can't be pushed when buffer is full.
//                  If false, data could be pushed no matter buffer is full or
//                  not.
// Return:
//      bytes pushed into the ring buffer
//*****************************************************************************
uint32_t
ns_ipc_ring_buffer_push(ns_ipc_ring_buffer_t *psBuffer, void *pvSource, uint32_t ui32Bytes,
                        bool bFullCheck) {
    uint32_t ui32CopyLen = 0;
    uint32_t ui32ReturnPushLen = 0;
    uint32_t ui32TempLen = 0;
    uint8_t *pui8Source;

    ui32CopyLen = ui32Bytes;
    pui8Source = (uint8_t *)pvSource;

    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32Capacity = psBuffer->ui32Capacity;

    if (bFullCheck == true) {
        AM_CRITICAL_BEGIN;
        // won't push any data if buffer is full
        if (ns_ipc_ring_buffer_full(psBuffer)) {
            ui32CopyLen = 0;
            ui32ReturnPushLen = 0;
            return ui32ReturnPushLen;
        }

        // push data until the buffer is full
        if (ns_ipc_ring_buffer_empty(psBuffer)) {
            if (ui32CopyLen >= ui32Capacity) {
                psBuffer->ui32OverWriting = 1;
                ui32CopyLen = ui32Capacity;
            }
        } else {
            if (((ui32BufferHead_read + ui32Capacity - ui32BufferTail_write) % ui32Capacity) <=
                ui32CopyLen) {
                psBuffer->ui32OverWriting = 1;
                ui32CopyLen =
                    ((ui32BufferHead_read + ui32Capacity - ui32BufferTail_write) % ui32Capacity);
            }
        }

        ui32ReturnPushLen = ui32CopyLen;

        while ((ui32BufferTail_write + ui32CopyLen) >= ui32Capacity) {
            ui32TempLen = ui32Capacity - ui32BufferTail_write;
            memcpy((void *)&psBuffer->pui8Data[ui32BufferTail_write], pui8Source, ui32TempLen);
            ui32BufferTail_write = psBuffer->ui32BufferTail_write =
                ((ui32BufferTail_write + ui32TempLen) % ui32Capacity);
            ui32CopyLen -= ui32TempLen;
        }
        //        configASSERT((ui32BufferTail_write + ui32CopyLen) <
        //        psBuffer->ui32Capacity);

        memcpy((void *)&psBuffer->pui8Data[ui32BufferTail_write], &pui8Source[ui32TempLen],
               ui32CopyLen);
        ui32BufferTail_write = psBuffer->ui32BufferTail_write =
            ((ui32BufferTail_write + ui32CopyLen) % ui32Capacity);
        AM_CRITICAL_END;
        return ui32ReturnPushLen;
    } else // if no full check, the return bytes are always ui32Bytes.
    {
        AM_CRITICAL_BEGIN;
        // push data until the buffer is full
        if (ns_ipc_ring_buffer_empty(psBuffer)) {
            if (ui32CopyLen >= ui32Capacity) {
                psBuffer->ui32OverWriting = 1;
            }
        } else {
            if (((ui32BufferHead_read + ui32Capacity - ui32BufferTail_write) % ui32Capacity) <=
                ui32CopyLen) {
                psBuffer->ui32OverWriting = 1;
            }
        }

        ui32ReturnPushLen = ui32CopyLen;

        while ((ui32BufferTail_write + ui32CopyLen) >= ui32Capacity) {
            ui32TempLen = ui32Capacity - ui32BufferTail_write;
            memcpy((void *)&psBuffer->pui8Data[ui32BufferTail_write], pui8Source, ui32TempLen);
            ui32BufferTail_write = psBuffer->ui32BufferTail_write =
                ((ui32BufferTail_write + ui32TempLen) % ui32Capacity);
            ui32CopyLen -= ui32TempLen;
        }
        //        configASSERT((ui32BufferTail_write + ui32CopyLen) <
        //        ui32Capacity);

        memcpy((void *)&psBuffer->pui8Data[ui32BufferTail_write], &pui8Source[ui32TempLen],
               ui32CopyLen);
        ui32BufferTail_write = psBuffer->ui32BufferTail_write =
            ((ui32BufferTail_write + ui32CopyLen) % ui32Capacity);

        //
        // Keep read and write at same position
        //
        if (psBuffer->ui32OverWriting == 1) {
            ui32BufferHead_read = psBuffer->ui32BufferHead_read = (ui32BufferTail_write);
        }
        AM_CRITICAL_END;
        return ui32ReturnPushLen;
    }
}

uint32_t
ns_ipc_ring_buffer_pop(ns_ipc_ring_buffer_t *psBuffer, void *pvDest, uint32_t ui32Bytes) {
    uint32_t ui32CopyLen = 0;
    uint32_t ui32TempLen = 0;
    uint32_t ui32DataLen = 0;
    uint32_t ui32ReturnPopLen = 0;
    uint8_t *pui8Dest;

    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32Capacity = psBuffer->ui32Capacity;

    pui8Dest = (uint8_t *)pvDest;

    ui32DataLen = ns_ipc_get_ring_buffer_status(psBuffer);

    AM_CRITICAL_BEGIN;
    if (ns_ipc_ring_buffer_overwrite(psBuffer)) {
        ui32BufferHead_read = psBuffer->ui32BufferHead_read = (ui32BufferTail_write);
    }

    // pop len can't exceed the length of buffer
    ui32CopyLen = ui32Bytes < ui32DataLen ? ui32Bytes : ui32DataLen;

    ui32ReturnPopLen = ui32CopyLen;

    while ((ui32BufferHead_read + ui32CopyLen) >= ui32Capacity) {
        ui32TempLen = ui32Capacity - ui32BufferHead_read;
        memcpy(pui8Dest, (void *)&psBuffer->pui8Data[ui32BufferHead_read], ui32TempLen);
        ui32BufferHead_read = psBuffer->ui32BufferHead_read =
            ((ui32BufferHead_read + ui32TempLen) % ui32Capacity);
        ui32CopyLen -= ui32TempLen;
    }

    // configASSERT((ui32BufferHead_read + ui32CopyLen) < ui32Capacity);

    memcpy(&pui8Dest[ui32TempLen], (void *)&psBuffer->pui8Data[ui32BufferHead_read], ui32CopyLen);

    ui32BufferHead_read = psBuffer->ui32BufferHead_read =
        ((ui32BufferHead_read + ui32CopyLen) % ui32Capacity);

    psBuffer->ui32OverWriting = 0;
    AM_CRITICAL_END;
    return ui32ReturnPopLen;
}
//*****************************************************************************
//
//! @brief get the number of data left in ring buffer .
//!
//! @param psBuffer is the address of the ring buffer structure to be read.
//!
//! @note This operation is not inherently thread-safe, so the caller must make
//! sure that it is appropriately guarded from interrupts and context switches.
//!
//! @return The number of data left.
//
//*****************************************************************************
uint32_t
ns_ipc_get_ring_buffer_status(ns_ipc_ring_buffer_t *psBuffer) {
    uint32_t ui32NewDataLen = 0;

    uint32_t ui32BufferTail_write = psBuffer->ui32BufferTail_write;
    uint32_t ui32BufferHead_read = psBuffer->ui32BufferHead_read;
    uint32_t ui32Capacity = psBuffer->ui32Capacity;

    if (ns_ipc_ring_buffer_overwrite(psBuffer) || ns_ipc_ring_buffer_full(psBuffer)) {
        AM_CRITICAL_BEGIN;
        ui32NewDataLen = ui32Capacity;
        AM_CRITICAL_END;
        return ui32NewDataLen;
    }
    AM_CRITICAL_BEGIN;
    ui32NewDataLen = ((ui32BufferTail_write + ui32Capacity - ui32BufferHead_read) % ui32Capacity);
    AM_CRITICAL_END;
    return ui32NewDataLen;
}

//*****************************************************************************
//
//! @brief flush the ring buffer
//!
//! @param psBuffer is the address of the ring buffer which needs to be flushed.
//!
//! @note None.
//!
//! @return None.
//
//*****************************************************************************
void
ns_ipc_flush_ring_buffer(ns_ipc_ring_buffer_t *psBuffer) {
    psBuffer->ui32OverWriting = 0;
    psBuffer->ui32BufferHead_read = psBuffer->ui32BufferTail_write;
}

//*****************************************************************************
//
//! @brief used in while loop to process all data in ring buffer
//!        e.g. while(ns_ipc_ring_process(src, dst, FRAME_SIZE))){}
//!
//! @param psSource: target ring buffer of ring processing
//!        pvDest: pointer of processing parameters (array)
//!        process_frame_bytes: number of bytes processed by a round
//!
//! @note None.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
ns_ipc_ring_process(ns_ipc_ring_buffer_t *psSource, void *pvDest, uint32_t process_frame_bytes) {
    uint32_t ui32BuffDataBytes = 0;
    ui32BuffDataBytes = ns_ipc_get_ring_buffer_status(psSource);
    if (ui32BuffDataBytes >= process_frame_bytes) {
        ns_ipc_ring_buffer_pop(psSource, pvDest, process_frame_bytes);
        return 1;
    } else {
        return 0;
    }
}
