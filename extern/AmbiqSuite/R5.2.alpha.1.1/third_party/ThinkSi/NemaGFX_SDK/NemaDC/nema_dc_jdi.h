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

#ifndef NEMA_DC_JDI_H__
#define NEMA_DC_JDI_H__

#include "nema_sys_defs.h"
#include "nema_dc.h"

#define JDI_CMD_NOP          (0x0U)
#define JDI_CMD_NOUPDATE     (0x0U)
#define JDI_CMD_BLINKOFF     (0x0U)
#define JDI_CMD_BLINKBLACK   (0x10U)
#define JDI_CMD_BLINKWHITE   (0x18U)
#define JDI_CMD_BLINKINVERT  (0x14U)
#define JDI_CMD_CLEAR        (0x20U)

#define JDI_DATAMODE_3BIT    (0x80U)
#define JDI_DATAMODE_1BIT    (0x88U)
#define JDI_DATAMODE_4BIT    (0x90U)
#define JDI_DATAMODE_SHARP   (0xA0U)

#define JDI_PHY_SPI4        (0U)
#define JDI_PHY_SPI4_SHARP  (1U)
#define JDI_PHY_SPI3        (2U)


typedef struct __MiP_display_config_t {
    int resx;              /**< Panel Horizontal Resolution  */
    int resy;              /**< Panel Vertical Resolution  */
    int XRST_INTB_delay ;  /**< Delay inserted prior of XRST or INTB in multiples of format_clk */
    int XRST_INTB_width ;  /**< Width of High state of XRST or INTB in multiples of format_clk */
    int VST_GSP_delay   ;  /**< Delay inserted prior of VST or GSP in multiples of format_clk */
    int VST_GSP_width   ;  /**< Width of High state of VST or GSP in multiples of format_clk */
    int VCK_GCK_delay   ;  /**< Delay inserted prior of VCK or GCK in multiples of format_clk */
    int VCK_GCK_width   ;  /**< Width of High state of VCK or GCK in multiples of format_clk */
    int VCK_GCK_closing_pulses  ;  /**< Number of VCK or GCK pulses without ENB or GEN signal at the end of frame */
    int HST_BSP_delay   ;  /**< Delay inserted prior of HST or BSP in multiples of format_clk */
    int HST_BSP_width   ;  /**< Width of High state of HST or BSP in multiples of format_clk */
    int HCK_BCK_data_start  ;  /**< The HCK or BCK cycle the pixel data should start at */
    int ENB_GEN_delay   ;  /**< Delay inserted prior of ENB or GEN in multiples of format_clk */
    int ENB_GEN_width   ;  /**< Width of High state of ENB or GEN in multiples of format_clk */
} MiP_display_config_t;

/** \brief Set MiP panel parameters
 *
 * \param MiP_display_config_t MiP panel struct
 *
 */
void nemadc_set_mip_panel_parameters( MiP_display_config_t *display);


/** \brief Setup MiP interface to send a full frame or multiple partials on a frame
 *
 * \param int Indication if Layer 0 is active (Active: 1, Inactive: 0)
 * \param nemadc_layer_t Layer 0 Struct
 * \param int Indication if Layer 1 is active (Active: 1, Inactive: 0)
 * \param nemadc_layer_t Layer 1 Struct
 * \param int Indication if Layer 2 is active (Active: 1, Inactive: 0)
 * \param nemadc_layer_t Layer 2 Struct
 * \param int Indication if Layer 3 is active (Active: 1, Inactive: 0)
 * \param nemadc_layer_t Layer 3 Struct
 * \param int number of partial regions (if full frame update set this to 0)
 * \param int partial_start / partial_end parameters (up to 16 pairs)
 *
 */
void nemadc_mip_setup( int layer0_active, nemadc_layer_t *layer0,
                       int layer1_active, nemadc_layer_t *layer1,
                       int layer2_active, nemadc_layer_t *layer2,
                       int layer3_active, nemadc_layer_t *layer3,
                       int partial_regions, ...);

void nemadc_jdi_configure(uint32_t phy, uint32_t data_mode, uint32_t extra_flags);
void nemadc_jdi_send_one_frame(int starty);
void nemadc_jdi_clear(void);
void nemadc_jdi_blink_off(void);
void nemadc_jdi_blink_inv_colors(void);
void nemadc_jdi_blink_white(void);
void nemadc_jdi_blink_black(void);
#endif
