//*****************************************************************************
//
//! @file am_hal_ipc_mbox.h
//!
//! @brief Functions for managing Inter-Processor Communication (IPC) Mailbox.
//!
//! @addtogroup ipc_mbox_ap510L IPC Mailbox - Inter-Processor Communication
//! @ingroup apollo330P_hal
//! @{
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

#ifndef AM_HAL_IPC_MBOX_H
#define AM_HAL_IPC_MBOX_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define IPC_MBOX_FIFO_MAX_DEPTH       (32)
#define IPC_MBOX_MAX_THRESHOLD        (IPC_MBOX_FIFO_MAX_DEPTH - 1)
#define IPC_MBOX_MIN_THRESHOLD        (1)
#define IPC_MBOX_DEFAULT_THRESHOLD    (1)

#define AM_HAL_IPC_MBOX_STATUS_IN_EMPTY     _FLD2VAL(CM55IPC_STATUS_D2MEMPTY, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_IN_FULL      _FLD2VAL(CM55IPC_STATUS_D2MFULL, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_IN_THACTIVE  _FLD2VAL(CM55IPC_STATUS_D2MTHRESHOLDACTIVE, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_IN_PEND      _FLD2VAL(CM55IPC_STATUS_D2MPEND, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_OUT_EMPTY    _FLD2VAL(CM55IPC_STATUS_M2DEMPTY, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_OUT_FULL     _FLD2VAL(CM55IPC_STATUS_M2DFULL, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_OUT_THACTIVE _FLD2VAL(CM55IPC_STATUS_M2DTHRESHOLDACTIVE, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_STATUS_OUT_PEND     _FLD2VAL(CM55IPC_STATUS_M2DPEND, CM55IPC->STATUS)
#define AM_HAL_IPC_MBOX_ERROR_D2MEMPTY      _FLD2VAL(CM55IPC_D2MERROR_D2MEMPTYERROR, CM55IPC->D2MERROR)
#define AM_HAL_IPC_MBOX_ERROR_D2MFULL       _FLD2VAL(CM55IPC_D2MERROR_D2MFULLERROR, CM55IPC->D2MERROR)
#define AM_HAL_IPC_MBOX_ERROR_M2DFULL       _FLD2VAL(CM55IPC_D2MERROR_M2DFULLERROR, CM55IPC->D2MERROR)
#define AM_HAL_IPC_MBOX_ERROR_IPCINIT       _FLD2VAL(CM55IPC_D2MERROR_IPCINIT, CM55IPC->D2MERROR)
#define AM_HAL_IPC_MBOX_WR_DATA(data)      (CM55IPC->M2DDATA = (data))
#define AM_HAL_IPC_MBOX_RD_DATA(data)      ((data) = CM55IPC->D2MDATA)
//*****************************************************************************
//
// Global definitions
//
//*****************************************************************************
enum
{
    AM_HAL_IPC_MBOX_SIGNAL_MSG_START = 0xA868,
    //
    // The M2D and D2M are only used for IPC TX/RX packets interrupt notification
    // which involves the IPC service.
    //
    AM_HAL_IPC_MBOX_SIGNAL_MSG_M2D = AM_HAL_IPC_MBOX_SIGNAL_MSG_START,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_D2M,

    //
    // There is some specific scenario to use mailbox for signalling and the IPC
    // serivce will not be involved.
    //

    //
    // The application core may request the RFXTAL clock from the network processing
    // core. In general it should write the request magic number to the mailbox FIFO
    // and register a handler (which will be serviced in ISR) to the response magic
    // number and wait for the success response from network core. A timeout should
    // be implemented for the request in case there is error on network core.
    // The network core should service this clock request prior to general IPC service
    // data even the data has been queued.
    //
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_ON_REQ,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_ON_RSP,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_OFF_REQ,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_OFF_RSP,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_CONFIG_REQ,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_RFXTAL_CONFIG_RSP,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_IPC_SHM_CONFIG_REQ,
    AM_HAL_IPC_MBOX_SIGNAL_MSG_IPC_SHM_CONFIG_RSP,

    AM_HAL_IPC_MBOX_SIGNAL_MSG_END
};

typedef enum
{
    AM_HAL_IPC_MBOX_INT_CTRL_DISABLE,
    AM_HAL_IPC_MBOX_INT_CTRL_ENABLE,
    AM_HAL_IPC_MBOX_INT_CTRL_NUM
} am_hal_ipc_mbox_int_ctrl_e;

typedef enum
{
    AM_HAL_IPC_MBOX_INT_CHANNEL_THRESHOLD,
    AM_HAL_IPC_MBOX_INT_CHANNEL_ERROR,
    AM_HAL_IPC_MBOX_INT_CHANNEL_NUM
} am_hal_ipc_mbox_int_channel_e;

typedef enum
{
    AM_HAL_IPC_MBOX_CHANNEL_DIR_IN,
    AM_HAL_IPC_MBOX_CHANNEL_DIR_OUT,
    AM_HAL_IPC_MBOX_CHANNEL_DIR_NUM
} am_hal_ipc_mbox_channel_direction_e;

typedef enum
{
    AM_HAL_IPC_MBOX_INIT_STATE_NOT_READY,
    AM_HAL_IPC_MBOX_INIT_STATE_IPCINIT_RECEIVED,
    AM_HAL_IPC_MBOX_INIT_STATE_INITIALIZED,
    AM_HAL_IPC_MBOX_INIT_STATE_NUM
} am_hal_ipc_mbox_init_state_e;

//
//! The IPC mailbox handler type includes a void* parameter.
//
typedef void (*am_hal_ipc_mbox_handler_t)(void *pArg);

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Set IPC mailbox DSP to MCU interrupt threshold
//!
//! @param threshold - threshold to set, valid range is 1~31
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//
//*****************************************************************************
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_threshold_set(uint32_t threshold);

//*****************************************************************************
//
//! @brief Get IPC mailbox M2D or D2M interrupt threshold
//!
//! @param dir - from which channel direction
//!
//! @param pui32Threshold - store the read threshold
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//
//*****************************************************************************
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_threshold_get(am_hal_ipc_mbox_channel_direction_e dir,
                                              uint32_t *pui32Threshold);

//*****************************************************************************
//
//! @brief Configure IPC mailbox DSP to MCU interrupt
//!
//! @param eControl: select one fo interrupt control, see am_hal_ipc_mbox_int_ctrl_e
//!
//! @param eChannel: select one of interrupt channel, see am_hal_ipc_mbox_int_channel_e
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_interrupt_configure(am_hal_ipc_mbox_int_ctrl_e eControl,
                                                    am_hal_ipc_mbox_int_channel_e eChannel);

//*****************************************************************************
//
//! @brief Get the IPC mailbox interrupt status
//!
//! @param dir - from which channel direction
//!
//! @param pui32IntStatus: store the interrupt status
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_interrupt_status_get(am_hal_ipc_mbox_channel_direction_e dir,
                                                     uint32_t *pui32IntStatus);

//*****************************************************************************
//
//! @brief Get the IPC mailbox interrupt enable state
//!
//! @param dir - from which channel direction
//!
//! @param pui32IntEnable: store the interrupt enable state
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_interrupt_enable_get(am_hal_ipc_mbox_channel_direction_e dir,
                                                     uint32_t *pui32IntEnable);

//*****************************************************************************
//! @brief Read certain length mailbox data from the FIFO
//!
//! @param data: buffer point of read data
//!
//! @param len: length of data to read
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_data_read(uint32_t *data, uint32_t len);

//*****************************************************************************
//! @brief Flush mailbox data in the read FIFO
//!
//! @param data: buffer point of read data, set be NULL if no need to store
//!          the read data.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_data_flush(uint32_t *data);

//*****************************************************************************
//! @brief Write certain length mailbox data to the FIFO
//!
//! @param data: buffer point of write data
//!
//! @param len: length of data to write
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_data_write(uint32_t *data, uint32_t len);

//*****************************************************************************
//! @brief Get IPC mailbox's status
//!
//! @param status: return the status
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_status_get(uint32_t *status);

//*****************************************************************************
//! @brief: Get IPC mailbox error
//!
//! @param pui32IntMsk Interrupt mask needs to be cleared
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_error_status_get(uint32_t *pui32IntMsk);

//*****************************************************************************
//! @brief: Clear IPC mailbox error
//!
//! @param ui32IntMsk Interrupt mask needs to be cleared
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_error_clear(uint32_t ui32IntMsk);

//*****************************************************************************
//! @brief: Clear IPC mailbox interrupt status
//!
//! @param eChannel select one of interrupt channel, see am_hal_ipc_mbox_int_channel_e
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_interrupt_clear(am_hal_ipc_mbox_int_channel_e eChannel);

//*****************************************************************************
//! @brief: Set the IPC mailbox initialization state
//!
//! @return: AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_init_state_set(am_hal_ipc_mbox_init_state_e eState);

//*****************************************************************************
//! @brief: Get the IPC mailbox initialization state
//!
//! @return: IPC mailbox initialization state
//*****************************************************************************
extern am_hal_ipc_mbox_init_state_e am_hal_ipc_mbox_init_state_get(void);

//*****************************************************************************
//! @brief: Initialize the IPC mailbox
//!
//! @return: AM_HAL_STATUS_SUCCESS or applicable IPC_MBOX errors.
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_init(void);

//*****************************************************************************
//
//! @brief Register an handler for a specific mailbox signal.
//!
//! @param ui32Signal Mailbox signal to register an handler.
//! @param pfnHandler is a function pointer for an handler for this signal
//! @param pArg is the parameter pointer needed to pass through
//!
//! This routine was designed to work with \e am_hal_ipc_mbox_service().
//! This function adds the \e pfnHandler argument to a table of handlers
//! so that \e am_hal_ipc_mbox_service() can call it in response to signal
//! from mailbox interrupt.
//!
//! @return Standard HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_handler_register(uint32_t ui32Signal,
                                                 am_hal_ipc_mbox_handler_t pfnHandler,
                                                 void *pArg);

//*****************************************************************************
//
//! @brief Relay interrupts from the main IPC mailbox to individual handlers.
//!
//! @param ui32Signal - mailbox signal read
//!
//! This routine was designed to work with \e am_hal_ipc_mbox_handler_register().
//! Once an handler has been registered with \e am_hal_ipc_mbox_handler_register(),
//! this function will call it in response to mailbox signals.
//!
//! @return Standard HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_ipc_mbox_service(uint32_t ui32Signal);

//*****************************************************************************
//
//! @brief mbox pending message handler. Override this weak funciton to
//!        customize handling of pending message.
//
//*****************************************************************************
void am_hal_ipc_mbox_msg_handler(void);


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_IPC_MBOX_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
