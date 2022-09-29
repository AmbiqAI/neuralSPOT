//*****************************************************************************
//
// @file ng_tuples.h
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

/**
 * @file
 * @brief Tuples are core data structs used by NemaGUI API
 *
 * A major use of these data structs is in the event handling mechanism. Callback functions accept a void pointer
 * in their arguments (signature contraint). Depending on the specific action that should take place during an event
 * this void pointer is casted inside the implementation of each callback to specific data structs, most of which are
 * defined here.
 */

#ifndef NG_TUPLES_H__
#define NG_TUPLES_H__

#include "ng_typedefs.h"
#include "ng_gitem.h"
#include "ng_animation.h"
#include "nema_core.h"
#include "nema_easing.h"
#include "nema_utils.h"

/** Data struct that contains point (x,y coordinates) */
typedef struct _ng_point_t
{
    int x; /**< x coordinate*/
    int y; /**< y coordinate*/
} ng_point_t;

/** Data struct that contains a pointer to a \a gitem_base_t and a \a uint32_t value */
typedef struct _ng_git_uint32_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    uint32_t val;      /**< uint32_t value*/
} ng_git_uint32_t;

/** Data struct that contains a pointer to a \a gitem_base_t, two \a uint32_t values and a pointer to an easing function */
typedef struct _ng_gitem_uint_uint_ez_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    uint32_t val_0;    /**< First uint32_t value*/
    uint32_t val_1;    /**< Second uint32_t value*/
    easing_f easing;   /**< Pointer to easing function*/
} ng_git_uint32_uint32_ez_t;

/** Data struct that contains a pointer to a \a gitem_base_t and a \a void pointer */
typedef struct _ng_git_ptr_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    void    *ptr;      /**< Void to a graphics item*/
} ng_git_ptr_t;

/** Data struct that contains a pointer to a \a gitem_base_t and a \a float value */
typedef struct _ng_git_float_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    float val;         /**< Floating point value*/
} ng_git_float_t;

/** Data struct that contains a pointer to a \a gitem_base_t, two \a float values and a pointer to an easing function */
typedef struct _ng_gitem_float_float_ez_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    float val_0;       /**< First float value*/
    float val_1;       /**< Second float value*/
    easing_f easing;   /**< Pointer to easing function*/
} ng_git_float_float_ez_t;

/** Data struct that contains a pointer to a \a gitem_base_t and two \a int values */
typedef struct _ng_git_int_int_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    int a;             /**< First int value*/
    int b;             /**< Second int value*/
} ng_git_int_int_t;

/** Data struct that contains a pointer to a \a gitem_base_t, two \a int values and a pointer to an easing function */
typedef struct _ng_git_int_int_ez_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    int a;             /**< First int value*/
    int b;             /**< First int value*/
    easing_f easing;   /**< Pointer to an easing function*/
} ng_git_int_int_ez_t;

/** Data struct that contains a pointer to a \a gitem_base_t, two pairs of \a int values and a pointer to an easing function */
typedef struct _ng_git_int_int_pair_ez_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
    int a0;            /**< First value of the first pair*/
    int a1;            /**< Second value of the first pair*/
    int b0;            /**< First value of the second pair*/
    int b1;            /**< Second value of the second pair*/
    easing_f easing;   /**< Pointer to an easing function*/
} ng_git_int_int_pair_ez_t;

/** Data struct that contains a pointer to a \a tree_node_t, a transition effect  and a direction value (according to the defines in ng_animation.h )*/
typedef struct _ng_node_effect_direction_t
{
    tree_node_t *node;          /**< Pointer to a tree node*/
    nema_transition_t effect;   /**< Transition effect*/
    int direction;              /**< Direction (see the defines in  ng_animation.h)*/
} ng_node_effect_direction_t;

/** Data struct that contains two pointers to a \a tree_node_t data structs*/
typedef struct _ng_node_node_t
{
    tree_node_t *node0; /**< Pointer to the first tree node*/
    tree_node_t *node1; /**< Pointer to the second tree node*/
} ng_node_node_t;

/** Utility data struct that contains a \a gitem_base_t pointer*/
typedef struct _ng_gitptr_t
{
    gitem_base_t *git; /**< Pointer to a graphics item*/
} ng_gitptr_t;

/** Utility data struct that contains a \a tree_node_t pointer*/
typedef struct _ng_nodeptr_t
{
    tree_node_t *node; /**< Pointer to a tree node*/
} ng_tree_node_ptr_t;

#endif // NG_TUPLES_H__
