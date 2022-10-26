//*****************************************************************************
//
//! @file ns-ambiqsuite-harness.c
//!
//! @brief Wrappers for common AmbiqSuite operations
//!
//!
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_ambiqsuite_harness.h"

/**
 * @brief Wrap prints in enable/disable to work with deep sleep mode
 * 
 * @param format 
 * @param ... 
 */
void ns_lp_printf(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);
    ns_debug_printf_enable();
    am_util_stdio_vprintf(format, myargs);
    ns_debug_printf_disable();
    va_end(myargs);
}

