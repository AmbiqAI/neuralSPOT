
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_ambiqsuite_harness.h"

//*****************************************************************************
//
// Prints a character.
//
//*****************************************************************************
void unity_output_char(int c) {
#if 1
    am_util_stdio_printf("%c", c);
#else
    am_hal_itm_stimulus_reg_byte_write(0, c);
#endif
}

#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) ||          \
    defined(AM_PART_APOLLO4L)
//#define UNITY_PRINT_OUTPUT_MEMORY 1
#endif

#if UNITY_PRINT_OUTPUT_MEMORY

am_bsp_memory_printf_state_t g_sPrintfState = {
    .pui8Buffer = (uint8_t *)(0x10010000),
    .ui32Size = 4096,
    .ui32Index = 0,
};

#endif

//*****************************************************************************
//
// Initialize ITM printing interface for unity.
//
//*****************************************************************************
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO4_API) ||       \
    defined(AM_PART_APOLLO5_API)
void unity_output_init(void) {
    #if UNITY_PRINT_OUTPUT_UART

    #elif UNITY_PRINT_OUTPUT_MEMORY
    am_bsp_memory_printf_enable(&g_sPrintfState);
    #else
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    ns_itm_printf_enable();
    #endif

    //
    // Start with a "start of transmission" character.
    //
    #if 1
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    #else
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    #endif
}

#else // NOT (AM_PART_APOLLO3) || (AM_PART_APOLLO3P) || (AM_PART_APOLLO4) || (AM_PART_APOLLO4B) ||
      // (AM_PART_APOLLO4P)  || (AM_PART_APOLLO4L)

void unity_output_init(void) {
    #if UNITY_PRINT_OUTPUT_UART
    //
    // Initialize the printf interface for UART output.
    //
    am_util_stdio_printf_init((am_util_stdio_print_char_t)am_bsp_uart_string_print);
    am_bsp_uart_printf_init(AM_BSP_UART_PRINT_INST, (am_hal_uart_config_t *)0);
    #else
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    ns_itm_printf_enable();

    #endif

    //
    // Enable debug printf messages
    //
    // am_bsp_debug_printf_enable();

    //
    // Start with a "start of transmission" character.
    //
    #if 1
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    ns_lp_printf("%c", 0x2);
    #else
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    am_hal_itm_stimulus_reg_byte_write(0, 0x2);
    #endif
}
#endif // AM_PART_APOLLO3

//*****************************************************************************
//
// End the printf output stream.
//
//*****************************************************************************
void unity_output_end(void) {
    //
    // End with an "end of transmission" character before we shut off the ITM.
    //
#if 1
    #if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) ||      \
        defined(AM_PART_APOLLO4L) || defined(AM_PART_BRONCO) || defined(AM_PART_APOLLO5A) ||       \
        defined(AM_PART_APOLLO5B)
    am_hal_delay_us(100000);
    #else
    am_util_delay_ms(100);
    #endif
    ns_lp_printf("%c", 0x3);
#else
    am_hal_itm_stimulus_reg_byte_write(0, 0x3);
#endif
    am_bsp_debug_printf_disable();
}
