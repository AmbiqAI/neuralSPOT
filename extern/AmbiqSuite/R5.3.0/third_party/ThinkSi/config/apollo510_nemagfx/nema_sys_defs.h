/*******************************************************************************
 * Copyright (c) 2022 Think Silicon S.A.
 *
   Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon S.A. be liable for any claim, damages or other liability, whether
 *  in an action of contract, tort or otherwise, arising from, out of or in
 *  connection with the software or the use or other dealings in the software.
 ******************************************************************************/

#ifndef NEMA_SYS_DEFS_H__
#define NEMA_SYS_DEFS_H__

//Start of User Definitions
//-------------------------

#define DONT_SUPPORT_NEMATS
//#define NEMA_MULTI_PROCESS
//#define NEMA_MULTI_THREAD
#define NEMA_ENABLE_BREAKPOINTS

#ifdef NEMA_MULTI_THREAD
#error "The NemaSDK has been compiled into a library without multi-threading support."
#endif

//use multiple memory pools (implemented in nema_hal.c)
//#define NEMA_MULTI_MEM_POOLS

//if NEMA_MULTI_MEM_POOLS is defined, use NEMA_MULTI_MEM_POOLS_CNT pools
//must be equal or less than 4
#ifndef NEMA_MULTI_MEM_POOLS_CNT
#define NEMA_MULTI_MEM_POOLS_CNT	1
#endif

//Memory pool definition
#define NEMA_MEM_POOL_CL_RB         0
#define NEMA_MEM_POOL_FB_TEX        1
#define NEMA_MEM_POOL_MISC          2

//Alias for the memory pool. These are defined to provide backward compatibility.
#define NEMA_MEM_POOL_CL            NEMA_MEM_POOL_CL_RB
#define NEMA_MEM_POOL_FB            NEMA_MEM_POOL_FB_TEX
#define NEMA_MEM_POOL_ASSETS        NEMA_MEM_POOL_FB_TEX
#define MIP_WA
//End of User Definitions
//-------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#if defined(NEMA_MULTI_PROCESS) || defined(NEMA_MULTI_THREAD)

#endif

#ifdef NEMA_MULTI_THREAD

    #ifdef __STDC_NO_THREADS__
    #define TLS_VAR __thread
    #else
    // https://en.cppreference.com/w/c/thread
    // If the macro constant __STDC_NO_THREADS__(C11) is defined by the
    // compiler, the header <threads.h> and all of the names listed here
    // are not provided.
    // YE: Some compilers implementing C11 don't define __STDC_NO_THREADS__
    // but still don't provide <threads.h>
    #include <threads.h>
    #define TLS_VAR thread_local
    #endif

#else
#define TLS_VAR
#endif

// turn off printf, or map to am_util_stdio_printf
#define printf(...)

//
// declaration of display controller interrupt callback function.
//
typedef void (*nema_dc_interrupt_callback)(void*, uint32_t);
typedef void (*nema_gfx_interrupt_callback)(int);

typedef enum 
{
    DISP_INTERFACE_DBIDSI,
    DISP_INTERFACE_QSPI,
    DISP_INTERFACE_DSPI,
    DISP_INTERFACE_SPI4,
    DISP_INTERFACE_JDI,
    DISP_INTERFACE_DPI,
    DISP_INTERFACE_DBI
} display_interface_t;

typedef struct 
{
    display_interface_t eInterface;
    uint16_t ui16ResX;
    uint16_t ui16ResY;
    union
    {
        //
        // Anonymous structure for interfaces DSI,QSPI,DSPI,SPI4,DPI and DBI.
        //
        struct 
        {
            uint32_t ui32FrontPorchX;
            uint32_t ui32FrontPorchY;
            uint32_t ui32BlankingX;
            uint32_t ui32BlankingY;
            uint32_t ui32BackPorchX;
            uint32_t ui32BackPorchY;
            uint32_t ui32PixelFormat;
            bool bTEEnable;
            float fCLKMaxFreq;
        };

        //
        // Anonymous structure for the JDI interface only.
        //
        struct 
        {
            uint32_t ui32XRSTINTBDelay;           // Delay inserted prior of XRST or INTB in multiples of format_clk
            uint32_t ui32XRSTINTBWidth;           // Width of High state of XRST or INTB in multiples of format_clk
            uint32_t ui32VSTGSPDelay;             // Delay inserted prior of VST or GSP in multiples of format_clk
            uint32_t ui32VSTGSPWidth;             // Width of High state of VST or GSP in multiples of format_clk
            uint32_t ui32VCKGCKDelay;             // Delay inserted prior of VCK or GCK in multiples of format_clk
            uint32_t ui32VCKGCKWidth;             // Width of High state of VCK or GCK in multiples of format_clk
            uint32_t ui32VCKGCKClosingPulses;     // Number of VCK or GCK pulses without ENB or GEN signal at the end of frame
            uint32_t ui32HSTBSPDelay;             // Delay inserted prior of HST or BSP in multiples of format_clk
            uint32_t ui32HSTBSPWidth;             // Width of High state of HST or BSP in multiples of format_clk
            uint32_t ui32HCKBCKDataStart;         // The HCK or BCK cycle the pixel data should start at
            uint32_t ui32ENBGENDelay;             // Delay inserted prior of ENB or GEN in multiples of format_clk
            uint32_t ui32ENBGENWidth;             // Width of High state of ENB or GEN in multiples of format_clk

            float fVCKGCKFFMaxFreq;         // Maximum GCK frequency(MHz),this value depend on the panel
            float fHCKBCKMaxFreq;           // Maximum BCK frequency(MHz),this value depend on the panel
        };
    };
} nemadc_initial_config_t;

//*****************************************************************************
//
//! @brief Backup the registers before power down
//!
//! this function can be called to backup the present configuration of DC
//!
//! @return None.
//
//*****************************************************************************
void nemadc_backup_registers(void);

//*****************************************************************************
//
//! @brief Restore the registers after power up
//!
//! this function should be called after nemadc_init() to restore the original
//! configuration.
//!
//! @return true if the registers have been backup,otherwise return false.
//
//*****************************************************************************
bool nemadc_restore_registers(void);

//*****************************************************************************
//
//! @brief Declaration of the GFX interrupt callback initialize function
//!
//! @param  fnGFXCallback                - GFX interrupt callback function
//!
//! this function hooks the Nema GFX GPU interrupt with a callback function.
//!
//! The fisrt paramter to the callback is a volatile int containing the ID of
//! the last serviced command list. This is useful for quickly responding
//! to the completion of an issued CL.
//!
//! @return None.
//
//*****************************************************************************
void nemagfx_set_interrupt_callback(nema_gfx_interrupt_callback fnGFXCallback);

//*****************************************************************************
//
//! @brief Reset last_cl_id variable.
//! 
//! Note: This API should only be called after the nema_reinit API, if it is called
//!       anywhere else, the GPU internal status will be broken.
//!
//! @return None.
//
//*****************************************************************************
void nema_reset_last_cl_id (void);

//*****************************************************************************
//
//! @brief Read last_cl_id variable.
//! 
//! last_cl_id indicates the last complete command list id, it is updated in the 
//! command list completion interrupt.
//!
//! @return None.
//
//*****************************************************************************
int nema_get_last_cl_id(void);

//*****************************************************************************
//
//! @brief Read last_submission_id variable.
//! 
//! last_submission_id indicates the last submitted command list id, it is accumulated
//! each time a command list is submitted. Generally speaking. the GPU will enter idle state when
//! last_submission_id==last_cl_id.
//!
//! @return None.
//
//*****************************************************************************
int nema_get_last_submission_id(void);

//*****************************************************************************
//
//! @brief declaration of DC's interrupt callback initialize function
//!
//! @param  fnTECallback                - DC TE interrupt callback function
//!
//! this function used to initialize display controller te interrupt
//! callback function.
//!
//! @return None.
//
//*****************************************************************************
void
nemadc_set_te_interrupt_callback(nema_dc_interrupt_callback fnTECallback);

//*****************************************************************************
//
//! @brief declaration of DC's interrupt callback initialize function
//!
//! @param  fnVsyncCallback - DC Vsync interrupt callback function
//! @param  arg             - DC Vsync interrupt callback argument
//!
//! this function used to initialize display controller vsync interrupt
//! callback function.
//!
//! @return None.
//
//*****************************************************************************
void
nemadc_set_vsync_interrupt_callback(nema_dc_interrupt_callback fnVsyncCallback,
                                    void* arg);

//*****************************************************************************
//
//! @brief nemadc_wait_te in blocking mode until NemaDC finished
//!
//!
//! @return None.
//
//*****************************************************************************
void nemadc_wait_te(void);

//*****************************************************************************
//
//! @brief nemadc_get_vsync
//!
//!
//! @return 32-bit am_hal_status_e status
//! @return AM_HAL_STATUS_IN_USE means NemaDC is still in processing
//! @return AM_HAL_STATUS_SUCCESS means NemaDC is complete all operation
//
//*****************************************************************************
am_hal_status_e nemadc_get_vsync(void);

//*****************************************************************************
//
//! @brief nemadc_get_te
//!
//!
//! @return 32-bit am_hal_status_e status
//! @return AM_HAL_STATUS_IN_USE means TE is not arrived
//! @return AM_HAL_STATUS_SUCCESS means TE is arrived
//
//*****************************************************************************
am_hal_status_e nemadc_get_te(void);

//*****************************************************************************
//
//! @brief Check wether the core ring buffer is full or not
//!
//! @return True, the core ring buffer is full, we need wait for GPU before 
//!         submit the next CL.
//!         False, the core ring buffer is not full, we can submit the next CL.
//*****************************************************************************
extern bool nema_rb_check_full(void);

//*****************************************************************************
//
//! @brief nema_get_cl_status
//!
//!
//! @return 32-bit am_hal_status_e status
//! @return AM_HAL_STATUS_IN_USE means GPU still busy in processing
//! @return AM_HAL_STATUS_SUCCESS means GPU is complete all operation
//
//*****************************************************************************
am_hal_status_e nema_get_cl_status(int32_t cl_id);

//*****************************************************************************
//
//! @brief Configure NemaDC.
//!
//! @param  psDCConfig     - NemaDC configuration structure.
//!
//! This function configures NemaDC display interface, output color format,
//! clock settings, TE setting and timing.
//!
//! @return None.
//
//*****************************************************************************
extern void nemadc_configure(nemadc_initial_config_t *psDCConfig);

//*****************************************************************************
//
//! @brief Prepare operations before writing memory start
//!
//! @param  bAutoLaunch    - true:launch transfer in DC TE interrupt implicitly.
//!
//! This function configures clock gating, sends MIPI_write_memory_start
//! command before sending frame. If DBIDSI interface is selected, this function
//! also configures HS/LP mode and data/command type of DSI.
//! Note: bAutoLaunch takes effect in the DC TE interrupt handler, which means
//! if GPIO TE is used or TE signal is ignored, setting this parameter to true or false
//! makes no difference, user still need to call nemadc_transfer_frame_launch manually.
//!
//! @return None.
//
//*****************************************************************************
extern void nemadc_transfer_frame_prepare(bool bAutoLaunch);

//*****************************************************************************
//
//! @brief Prepare operations before writing memory continue
//!
//! @param  bAutoLaunch    - true:launch transfer in interrupt implicitly.(Not recommended)
//!                        - false: please launch the transfer explicitly.(recommended)
//!
//! Setting the parameter(bAutoLaunch) to true is not recommended because this 
//! could spot a severe tear effect on the display.
//! @return None.
//
//*****************************************************************************
extern void nemadc_transfer_frame_continue(bool bAutoLaunch);

//*****************************************************************************
//
//! @brief Launch frame transfer
//!
//! This function enables DC frame end interrupt and launches frame transfer.
//!
//! @return None.
//
//*****************************************************************************
extern void nemadc_transfer_frame_launch(void);

//*****************************************************************************
//
//! @brief Send SPI4/DSPI/QSPI/DSI command via Display Controller(DC)
//!
//! @param  ui8Command      - command
//! @param  p_ui8Para       - pointer of parameters to be sent
//! @param  ui8ParaLen      - length of parameters to be sent
//! @param  bDCS            - DCS command or generic command
//! @param  bHS             - high speed mode or low power mode (escape mode)
//!
//! This function sends commands to display drive IC.
//!
//! @return None.
//
//****************************************************************************
extern uint32_t
nemadc_mipi_cmd_write(uint8_t ui8Command,
                      uint8_t* p_ui8Para,
                      uint8_t ui8ParaLen,
                      bool bDCS,
                      bool bHS);

//*****************************************************************************
//
//! @brief Send SPI4/DSPI/QSPI/DSI read command via Display Controller(DC)
//!
//! @param  ui8Command          - command
//! @param  p_ui8Para           - pointer of parameters to be sent
//! @param  ui8ParaLen          - length of parameters to be sent
//! @param  p_ui32Data          - pointer of data to be read
//! @param  ui8DataLen          - length of data to be read (number of bytes)
//! @param  bDCS                - DCS command or generic command
//! @param  bHS                 - high speed mode or low power mode (escape mode)
//!
//! This function sends read commands to display drive IC.
//!
//! @return Status.
//
//****************************************************************************
extern uint32_t
nemadc_mipi_cmd_read(uint8_t ui8Command,
                     uint8_t* p_ui8Para,
                     uint8_t ui8ParaLen,
                     uint32_t* p_ui32Data,
                     uint8_t ui8DataLen,
                     bool bDCS,
                     bool bHS);

//*****************************************************************************
//
//! @brief DBI-B interface read the frame buffer
//!
//! @param ui8ReceiveBuffer     - pointer of receive buffer
//! @param ui32ParaLen          - the read count
//!
//! @note During a read cycle the host processor reads data from the display module  
//! via the interface. The Type B interface utilizes D/CX, RDX and WRX signals as  
//! well as all eight (D[7:0]) information signals.RDX is driven from high to low 
//! then allowed to be pulled back to high during the read cycle. The display module 
//! provides information to the host processor during the read cycle while the host 
//! processor reads the display module information on the rising edge of RDX. D/CX 
//! is driven high during the read cycle.
//!
//! @return pointer.
//
//*****************************************************************************
extern void
dbi_frame_read(uint8_t *ui8ReceiveBuffer, uint32_t ui32ParaLen);

//*****************************************************************************
//
//! @brief Controls the power state of the GPU peripheral.
//!
//! @param ePowerState - The desired power state (e.g., wake, normal sleep, 
//!                      deep sleep).
//! @param bRetainState - Indicates whether to reinitialize the NemaSDK and 
//!                       NemaVG when waking up the GPU peripheral.
//!
//! This function manages the power state of the GPU peripheral based on
//! the requested power state. It checks the current power status and either
//! powers up or powers down the peripheral as needed.
//!
//! When waking up the GPU (`AM_HAL_SYSCTRL_WAKE`), if \e bRetainState is true,
//! the function reinitializes the NemaSDK and NemaVG. If powering down, the
//! function ensures the peripheral is inactive before disabling power.
//!
//! \e ePowerState The desired power state. Valid values are:
//! - AM_HAL_SYSCTRL_WAKE: Power up.
//! - AM_HAL_SYSCTRL_NORMALSLEEP or AM_HAL_SYSCTRL_DEEPSLEEP: Power down.
//!
//! \e bRetainState Determines whether to reinitialize the NemaSDK and NemaVG
//! when powering up. This is used for scenarios where the GPU context needs
//! to be retained.
//!
//! @note Ensure that the GPU peripheral is not in use before attempting to
//! power it down to avoid conflicts.This API is not thread-safe. If it is
//! called from multiple threads or from an interrupt, appropriate critical
//! section protection must be added.
//!
//! @return Returns the status of the operation. Possible return values are:
//! - AM_HAL_STATUS_SUCCESS: Operation was successful.
//! - AM_HAL_STATUS_IN_USE: Peripheral is currently in use and cannot be powered 
//!                         down.
//! - AM_HAL_STATUS_INVALID_OPERATION: Invalid power state requested.
//! - AM_HAL_STATUS_FAIL: Reinitialization of NemaSDK or NemaVG failed.
//
//*****************************************************************************
uint32_t
nemagfx_power_control(am_hal_sysctrl_power_state_e ePowerState,
                   bool bRetainState);

//*****************************************************************************
//
//! @brief DC power control function
//!
//! @param ePowerState  - the desired power state to move the peripheral to.
//! @param retainState  - flag (if true) to save/restore perhipheral state upon
//!                       power state change.
//!
//! This function updates the peripheral to a given power state.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t
nemadc_power_control(am_hal_sysctrl_power_state_e ePowerState,
                     bool bRetainState);

//*****************************************************************************
//
//! @brief Suppress warning message for MDKv5.27 and later
//!
//!
//! @note #61-D: integer operation result is out of range
//! @note #1295-D: Deprecated declaration nema_rand - give arg types
//! @note #1295-D: Deprecated declaration nema_rand - give arg types
//! @note waiting ThinkSilicon release the new Nema SDK
//
//*****************************************************************************
#if defined(__CC_ARM)
#pragma diag_suppress 61
#pragma diag_suppress 1295
#pragma diag_suppress 1
#endif

#endif
