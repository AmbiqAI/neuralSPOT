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

#ifndef NEMA_RINGBUFFER_H__
#define NEMA_RINGBUFFER_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
//@function nema_rb_submit_cmdlist
//@brief Enqueue a Command List to the Ring Buffer for execution
//@param nema_buffer_t *bo desc: Pointer to the buffer object of the Command List
//@param uint32_t size desc: Size of the populated Command List
//@return int desc: Return submission id
//--------------------------------------------------
/** \private */
int32_t nema_rb_submit_cmdlist(nema_buffer_t *bo, int size);

int32_t nema_rb_submit_cmdlist2(uintptr_t base_phys, int size);

void nema_rb_reset(void);

//--------------------------------------------------
//@function nema_rb_inline_cmd
//@brief Enqueue a Command to the Ring Buffer for execution
//@param uint32_t reg desc: Hardware Register to be written
//@param uint32_t data desc: Data to be written
//--------------------------------------------------
/** \private */
void nema_rb_inline_cmd(uint32_t reg, uint32_t data);

/** \private */
void nema_rb_force_flush(void);

/** \private */
void nema_rb_submit_cl_id(int cl_id);

#ifdef __cplusplus
}
#endif

#endif
