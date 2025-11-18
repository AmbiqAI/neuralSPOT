/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon Single Member PC be liable for any claim, damages or other
 *  liability, whether in an action of contract, tort or otherwise, arising
 *  from, out of or in connection with the software or the use or other dealings
 *  in the software.
 ******************************************************************************/

#ifndef NEMADC_DSI_H__
#define NEMADC_DSI_H__
//------------------------------------------------------------------------------
#include "nema_sys_defs.h"
//------------------------------------------------------------------------------
//---------------------------------------------------------
// NemaDC IPI registers values
//---------------------------------------------------------
#define NEMADC_IPI_VIDEO_MODE          0U<<0
#define NEMADC_IPI_DATA_MODE           1U<<0
#define NEMADC_IPI_COLOR_RGB565        0U<<1
#define NEMADC_IPI_COLOR_RGB666        1U<<1
#define NEMADC_IPI_COLOR_RGB666_LOS    2U<<1
#define NEMADC_IPI_COLOR_RGB888        3U<<1
#define NEMADC_IPI_COLOR_YCbYCr422     4U<<1 // NOT SUPPORTED
#define NEMADC_IPI_COLOR_YCbYCr422_LOS 5U<<1 // NOT SUPPORTED
#define NEMADC_IPI_COLOR_YCbYCr420     6U<<1 // NOT SUPPORTED
#define NEMADC_IPI_COLOR_COMPRESSED    7U<<1 // NOT SUPPORTED
#define NEMADC_IPI_ENABLE              1U<<31
//-----------------------------------------------------

#define NemaDC_dt_vsync_start                        (0x01U)
#define NemaDC_dt_vsync_end                          (0x11U)
#define NemaDC_dt_hsync_start                        (0x21U)
#define NemaDC_dt_hsync_end                          (0x31U)
#define NemaDC_dt_cmpr_mode                          (0x07U)
#define NemaDC_dt_end_of_trans                       (0x08U)
#define NemaDC_dt_pic_param                          (0x0aU)
#define NemaDC_dt_cmpr_pix_stream                    (0x0bU)
#define NemaDC_dt_color_mode_off                     (0x02U)
#define NemaDC_dt_color_mode_on                      (0x12U)
#define NemaDC_dt_shut_down_peripheral               (0x22U)
#define NemaDC_dt_turn_on_peripheral                 (0x32U)
#define NemaDC_dt_generic_short_write_param_no       (0x03U)
#define NemaDC_dt_generic_short_write_param_n1       (0x13U)
#define NemaDC_dt_generic_short_write_param_n2       (0x23U)
#define NemaDC_dt_generic_read_param_no              (0x04U)
#define NemaDC_dt_generic_read_param_n1              (0x14U)
#define NemaDC_dt_execute_queue                      (0x16U)
#define NemaDC_dt_generic_read_param_n2              (0x24U)
#define NemaDC_dt_DCS_short_write_param_no           (0x05U)
#define NemaDC_dt_DCS_short_write_param_n1           (0x15U)
#define NemaDC_dt_DCS_read_param_no                  (0x06U)
#define NemaDC_dt_set_max_return_packet_size         (0x37U)
#define NemaDC_dt_blanking_packet                    (0x19U)
#define NemaDC_dt_generic_long_write                 (0x29U)
#define NemaDC_dt_DCS_long_write                     (0x39U)
#define NemaDC_dt_packed_pixel_stream_rgb565         (0x0eU)
#define NemaDC_dt_packed_pixel_stream_rgb666         (0x1eU)
#define NemaDC_dt_loosely_packed_pixel_stream_rgb666 (0x2eU)
#define NemaDC_dt_loosely_packed_pixel_stream_rgb888 (0x3eU)
//-----------------------------------------------------
#define NemaDC_dcs_datacmd       (  0U  )
#define NemaDC_ge_data           (1U<<30)
#define NemaDC_ge_cmd            (1U<<31)
#define NemaDC_ge_datacmd        ((1U<<30U)|(1U<<31U))
//-----------------------------------------------------
// custom wrapper DSI signals
#define ENABLE_DSI                (1U<<12)
#define ENABLE_LOWPOWER           (1U<<11) // we can select only one mode
#define ENABLE_HIGHSPEED          (0U<<11) // we can select only one mode
#define GENERIC_CMD_ENABLE        (1U<<10)
#define MIPI_CMD_ENABLE           (0U<<10)
#define DSI_VC_0                  (0U<<8)
#define DSI_VC_1                  (1U<<8)
#define DSI_VC_2                  (2U<<8)
#define DSI_VC_3                  (3U<<8)
//------------------------------------------------------------------------------

/** \brief Send scanline command and start memory write
*
*/
void nemadc_dsi_start_frame_transfer(void);

/** \brief Send scanline command and start memory write (generic)
*
*/
void nemadc_dsi_start_frame_transfer_generic(void);

/** \brief DC DBI interface to DSI
 *
 * \param  data_type  Data (pixel) type
 * \param  cmd_type   Command type
 * \param  type       DSI command type
 */
void nemadc_dsi_ct(uint32_t data_type, uint32_t cmd_type, uint32_t type);


#endif // NEMADC_DSI_H__
