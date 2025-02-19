//*****************************************************************************
//
//! @file am_devices_display_generic.h
//!
//! @brief the generic display device driver provides unified display
//! initialization, frame-transfer operations etc via interfaces on Apollo4/Apollo5
//! SoC, like MSPI and DC (one layer only).
//!
//! @addtogroup disp_generic Display Devices Generic Driver
//! @ingroup devices
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_DEVICES_DISPLAY_GENERIC_H
#define AM_DEVICES_DISPLAY_GENERIC_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#include "am_devices_display_types.h"

#if defined (DISP_CTRL_IP)

#if (DISP_CTRL_IP == DISP_CTRL_IP_DC)
#include "nema_dc.h"
#include "am_devices_dc_xspi_raydium.h"
#include "am_devices_dc_dsi_raydium.h"
#endif //(DISP_CTRL_IP == DISP_CTRL_IP_DC)

#if (DISP_CTRL_IP == DISP_CTRL_IP_MSPI)
#include "am_devices_mspi_raydium.h"
#endif //(DISP_CTRL_IP == DISP_CTRL_IP_MSPI)

#define TE_GPIO_IDX         GPIO_NUM2IDX(DISPLAY_TE_PIN)

#if defined(AM_BSP_GPIO_TOUCH_INT)
#define TP_GPIO_IDX         GPIO_NUM2IDX(AM_BSP_GPIO_TOUCH_INT)
#if (TE_GPIO_IDX == TP_GPIO_IDX)
#define TE_TP_GPIO_IDX TE_GPIO_IDX
#endif
#endif

extern const IRQn_Type g_sInterrupts[];

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Global externs
//
//*****************************************************************************

//*****************************************************************************
//
//! External type definitions
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Display callback function type
//
//*****************************************************************************
typedef void (*am_devices_disp_handler_t)(void*);

//*****************************************************************************
//
//! External function definitions
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Initialize display subsystem
//!
//! @param ui16ResX            - display region resolution X
//! @param ui16ResY            - display region resolution Y
//! @param eColorMode          - display color mode to be used
//! @param bEnableTE           - turn on or off TE signal from the panel
//!
//! This function initializes display panel and display controller.
//!
//! @return status.
//
//*****************************************************************************
extern int32_t am_devices_display_init(uint16_t ui16ResX,
                        uint16_t ui16ResY,
                        am_devices_disp_color_e eColorMode,
                        bool bEnableTE);
//*****************************************************************************
//
//! @brief sets recommended scanline.
//!
//! @param TETimesPerFrame      - how many TE intervals transfer one frame
//!
//! This function sets recommended scanline,it's valid when TETimesPerFrame
//! equal to 1 or 2.
//!
//! @return Status.
//
//****************************************************************************
extern uint32_t
am_devices_display_set_scanline_recommended_parameter(uint8_t TETimesPerFrame);

//*****************************************************************************
//
//! @brief DC and display panel de-initialization function.
//!
//! This function de-Initialization the display panel and display controller.
//!
//! @return status.
//
//*****************************************************************************
extern int32_t am_devices_display_generic_deinit(void);
//*****************************************************************************
//
//! @brief Send one frame via MSPI interface, or via DC with only one layer
//!
//! @param ui16ResX            - frame-buffer resolution X
//! @param ui16ResY            - frame-buffer resolution Y
//! @param ui32Address         - frame-buffer address
//! @param fnXferDoneCb        - callback after transfer is finished
//! @param pArgXferDone        - callback after transfer is finished
//!
//! @note If DC multiple layer feature is used, please use DC/DSI and corresponding
//! panel driver directly
//!
//! @return status.
//
//*****************************************************************************
extern uint32_t am_devices_display_transfer_frame(uint16_t ui16ResX,    // buffer resolution X
                                  uint16_t ui16ResY,    // buffer resolution Y
                                  uint32_t ui32Address, // buffer address
                                  am_devices_disp_handler_t fnXferDoneCb,
                                  void *pArgXferDone);

//*****************************************************************************
//
//! @brief transfer one frame stripe by stripe
//!
//! @param  ui16ResX     - X resolution of frame buffer/layer
//! @param  ui16ResY     - Y resolution of frame buffer/layer
//! @param  ui32Address  - Address of frame buffer
//! @param  total_stripe  - total stripe
//! @param  fnStripeDoneCb - Stripe transfer done callback
//!
//! This function transfer one frame stripe by stripe, one stripe contains
//! width*(hight/stripe) pixels, for example your frame size is 440*440, and it is devided
//! into 4 stripes, then hight of one stripe is 110(440/4) and width is 440.
//! This function has same function as am_devices_display_transfer_frame but it will
//! trigger a interrupt at each stripe end(in the above example, four interrupts will be trigger).
//! Note: the parameter passed in the fnStripeDoneCb indicates the stripe number that just transfer
//! complete. For example, in the above setting, fnStripeDoneCb will be called four times, passing
//! 0, 1, 2, 3 as parameter.
//!
//! @return status.
//
//*****************************************************************************
uint32_t
am_devices_display_transfer_frame_by_stripe(uint16_t ui16ResX,
                                  uint16_t ui16ResY,
                                  uint32_t ui32Address,
                                  uint32_t total_stripe,
                                  am_devices_disp_handler_t fnStripeDoneCb);

//*****************************************************************************
//
//! @brief wait transfer completion
//!
//! calling this function would blockingly waits for transfer is done.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_display_wait_transfer_done(void);

//*****************************************************************************
//
//! @brief set display region.
//!
//! @param  ui16ResX            - display X resolution
//! @param  ui16ResY            - display Y resolution
//! @param  ui16MinX            - display X start address
//! @param  ui16MinY            - display Y start address
//!
//! this function sets display region.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_display_set_region(uint16_t ui16ResX,
                                        uint16_t ui16ResY,
                                        uint16_t ui16MinX,
                                        uint16_t ui16MinY);

//*****************************************************************************
//
//! @brief Set the flip along with the x/y-axis or both the x and y-axis.
//!
//! @param ui8FlipXY            - how to flip the display
//!
//! @note Register bits function for Driver IC RM69330 or CO5300.
//!       Bitfield 0 - Reserved                 (please don't set this bit)
//!       Bitfield 1 - Flip along with y-axis   (reserved for CO5300)
//!       Bitfield 2 - Reserved
//!       Bitfield 3 - RGB or BGR order
//!       Bitfield 4 - Flip along with x-axis   (reserved for CO5300)
//!       Bitfield 5 - Flip along y = x         (reserved for CO5300)
//!       Bitfield 6 - Flip along with y-axis
//!       Bitfield 7 - Flip along with x-axis
//! The x-axis will keep no flip when setting bitfields 4 and 7 same time. In the
//! same way, the same is true for the y-axis. Please be careful some of the flips
//! could cause the tear effect. Please adjust the touch coordinate when enabling
//! this feature.
//!
//! @return None.
//
//*****************************************************************************
uint32_t am_devices_display_flip(uint8_t ui8FlipXY);

//
// TODO: add them
//
// called when powercycle display subsystem//
//uint32_t am_devices_display_reinit();
//uint32_t am_devices_display_reinit_without_panel();
//
#ifdef __cplusplus
}
#endif

#endif // defined (DISP_CTRL_IP)

#endif // AM_DEVICES_DISPLAY_GENERIC_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
