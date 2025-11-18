#ifndef NEMA_RASTER_STROKED_ARC_AA_EX
#define NEMA_RASTER_STROKED_ARC_AA_EX

#include "stdint.h"

//*****************************************************************************
//
//! @brief nema_raster_stroked_arc_aa_ex
//!
//! sPosition[i].p0_x = x0 + r * nema_cos(end_angle) - w/2*nema_cos(end_angle);
//! this get the PO cornor position
//! param x0 start x position
//! param y0 start y position
//! param r  arc radius
//! param w  arc width
//! param start_angle  start arc angle degree
//! param start_round_ending_enable flag to if enable the start round ending
//! param end_angle  end arc angle degree
//! param end_round_ending_enable flag to if enable the end round ending
//! param color drawing color
//!
//! @return none
//
//*****************************************************************************
void nema_raster_stroked_arc_aa_ex(float x0, float y0, float r, float w, 
                                   float start_angle, 
                                   bool start_round_ending_enable, 
                                   float end_angle, 
                                   bool end_round_ending_enable, 
                                   uint32_t color);

//*****************************************************************************
//
//! @brief nema_blend_lut_with_mask
//!
//! Blend a LUT format texture with masking effect.
//! It is supposed that mask is bind to NEMA_TEX3. 
//!
//! @return none
//
//*****************************************************************************
void nema_blend_lut_with_mask(uint32_t blending_mode);

#endif
