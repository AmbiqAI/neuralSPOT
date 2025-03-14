

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "am_hal_mcuctrl.h"
#include "ns_uart.h"
#include <stdio.h>
#include <string.h>
#include "ns_peripherals_button.h"
#include <stdlib.h>


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------
// Original kernel function using inline assembly exactly as given.
// Processes 'rhs_cols' blocks of 16 bytes each.
// The function updates the input pointer variables (passed by pointer)
// and produces 32-bit accumulators for four rows plus a horizontal sum.
//----------------------------------------------------------
void original_kernel(
    uint8_t **col_ptr,   // pointer to pointer to column data
    uint8_t **row0_ptr,  // pointer to pointer for row0 data
    uint8_t **row1_ptr,  // pointer to pointer for row1 data
    uint8_t **row2_ptr,  // pointer to pointer for row2 data
    uint8_t **row3_ptr,  // pointer to pointer for row3 data
    int rhs_cols,        // number of 16-byte blocks to process
    int32_t *acc0,       // accumulator output for row0
    int32_t *acc1,       // accumulator output for row1
    int32_t *acc2,       // accumulator output for row2
    int32_t *acc3,       // accumulator output for row3
    int32_t *sum_out     // accumulator for horizontal sum over col vector blocks
)
{
    __asm volatile(
        " .p2align 2                                 \n" // align loop start to 4-byte boundary
        "   wlstp.8         lr, %[cnt], 1f           \n" // Set up tail-predicated while loop using WLSTP.8;
                                                      // if %[cnt]==0, branch to label 1.
        "   mov             %[out0], 0               \n" // Zero accumulator for row0
        "   mov             %[out1], 0               \n" // Zero accumulator for row1
        "   mov             %[out2], 0               \n" // Zero accumulator for row2
        "   mov             %[out3], 0               \n" // Zero accumulator for row3
        "   mov             %[sum], 0                \n" // Zero the scalar sum accumulator
        "   vldrb.8         q0, [%[col]], #16        \n" // Load 16 bytes from [col] into vector q0; post-increment col by 16
        "2:                                          \n" // Label for loop body
        "   vaddva.s8      %[sum], q0                \n" // Horizontal add: sum all 16 8-bit elements in q0 and add into %[sum]
        "   vldrb.8         q1, [%[row0]], #16       \n" // Load 16 bytes from row0 into q1; post-increment row0 pointer
        "   vmladava.s8     %[out0], q0, q1          \n" // Multiply-accumulate: out0 += dot(q0, q1)
        "   vldrb.8         q2, [%[row1]], #16       \n" // Load 16 bytes from row1 into q2
        "   vmladava.s8     %[out1], q0, q2          \n" // out1 += dot(q0, q2)
        "   vldrb.8         q3, [%[row2]], #16       \n" // Load 16 bytes from row2 into q3
        "   vmladava.s8     %[out2], q0, q3          \n" // out2 += dot(q0, q3)
        "   vldrb.8         q4, [%[row3]], #16       \n" // Load 16 bytes from row3 into q4
        "   vmladava.s8     %[out3], q0, q4          \n" // out3 += dot(q0, q4)
        "   vldrb.8         q0, [%[col]], #16        \n" // Load next 16 bytes from col into q0
        "   letp            lr, 2b                   \n" // Loop End: decrement lr by vector length and branch to label 2 if not done
        "1:                                          \n" // Loop exit label
        : // Outputs:
          [col] "+r" (*col_ptr),
          [sum] "=Te" (*sum_out),
          [row0] "+r" (*row0_ptr),
          [row1] "+r" (*row1_ptr),
          [row2] "+r" (*row2_ptr),
          [row3] "+r" (*row3_ptr),
          [out0] "=Te" (*acc0),
          [out1] "=Te" (*acc1),
          [out2] "=Te" (*acc2),
          [out3] "=Te" (*acc3)
        : // Inputs:
          [cnt] "r" (rhs_cols)
        : // Clobbers:
          "q0", "q1", "q2", "q3", "q4", "memory", "r14"
    );
}


//----------------------------------------------------------
// Optimized kernel function implementing all suggested improvements:
// 1. Use DLS.8 instead of WLSTP.8 (assumes rhs_cols > 0).
// 2. Unroll the loop by a factor of 2 (process two 16-byte blocks per iteration).
// 3. Remove per-iteration horizontal add (vaddva.s8) by accumulating q0 values into a vector accumulator (q5)
//    using elementwise addition (vadd.s8) and then perform one horizontal reduction after the loop.
// 4. (Additional reordering of loads is applied by grouping the MAC operations in each unrolled block.)
//----------------------------------------------------------
void optimized_kernel(
    uint8_t **col_ptr,
    uint8_t **row0_ptr,
    uint8_t **row1_ptr,
    uint8_t **row2_ptr,
    uint8_t **row3_ptr,
    int rhs_cols,       // number of 16-byte blocks to process (assumed even)
    int32_t *acc0,
    int32_t *acc1,
    int32_t *acc2,
    int32_t *acc3,
    int32_t *sum_out
)
{
    __asm volatile(
        " .p2align 2                                 \n" // Align to 4-byte boundary
        // Use DLS.8 (Do Loop Start) because rhs_cols is non-zero.
        "   dls.8         lr, %[cnt], 1f           \n" // Set up loop; if count==0, branch to label 1
        // Zero the accumulators for each row.
        "   mov           %[out0], 0               \n"
        "   mov           %[out1], 0               \n"
        "   mov           %[out2], 0               \n"
        "   mov           %[out3], 0               \n"
        // Zero the scalar sum.
        "   mov           %[sum], 0                \n"
        // Initialize a vector accumulator q5 to 0 (for accumulating column data sums).
        "   vdup.8        q5, %[sum]               \n" // q5 = 0 in all lanes
        // Preload first 16-byte block from column data into q0.
        "   vldrb.8      q0, [%[col]], #16         \n"
        "2:                                          \n" // Loop start label
        // --- Unrolled iteration: Process first block ---
        "   vldrb.8      q1, [%[row0]], #16        \n" // Load 16 bytes from row0 into q1
        "   vmladava.s8 %[out0], q0, q1           \n" // out0 += dot(q0, q1)
        "   vldrb.8      q2, [%[row1]], #16        \n" // Load from row1
        "   vmladava.s8 %[out1], q0, q2           \n" // out1 += dot(q0, q2)
        "   vldrb.8      q3, [%[row2]], #16        \n" // Load from row2
        "   vmladava.s8 %[out2], q0, q3           \n" // out2 += dot(q0, q3)
        "   vldrb.8      q4, [%[row3]], #16        \n" // Load from row3
        "   vmladava.s8 %[out3], q0, q4           \n" // out3 += dot(q0, q4)
        // Accumulate the current column vector (q0) into q5 (elementwise add).
        "   vadd.s8      q5, q5, q0                \n"
        // --- Unrolled iteration: Process second block ---
        "   vldrb.8      q0, [%[col]], #16         \n" // Load next block from col into q0
        "   vldrb.8      q1, [%[row0]], #16        \n" // Load next from row0
        "   vmladava.s8 %[out0], q0, q1           \n" // out0 += dot(q0, q1)
        "   vldrb.8      q2, [%[row1]], #16        \n" // Load row1
        "   vmladava.s8 %[out1], q0, q2           \n" // out1 += dot(q0, q2)
        "   vldrb.8      q3, [%[row2]], #16        \n" // Load row2
        "   vmladava.s8 %[out2], q0, q3           \n" // out2 += dot(q0, q3)
        "   vldrb.8      q4, [%[row3]], #16        \n" // Load row3
        "   vmladava.s8 %[out3], q0, q4           \n" // out3 += dot(q0, q4)
        "   vadd.s8      q5, q5, q0                \n" // Accumulate this q0 into q5
        // Preload next block from col for next iteration.
        "   vldrb.8      q0, [%[col]], #16         \n"
        // End loop: decrement loop counter and branch if not finished.
        "   letp         lr, 2b                  \n"
        "1:                                          \n" // Loop exit label
        // After loop: horizontally add all lanes of vector q5 to get a scalar sum.
        "   vaddva.s8   %[sum], q5                \n"
        : // Outputs (update the pointers and accumulators):
          [col]   "+r" (*col_ptr),
          [sum]   "=Te" (*sum_out),
          [row0]  "+r" (*row0_ptr),
          [row1]  "+r" (*row1_ptr),
          [row2]  "+r" (*row2_ptr),
          [row3]  "+r" (*row3_ptr),
          [out0]  "=Te" (*acc0),
          [out1]  "=Te" (*acc1),
          [out2]  "=Te" (*acc2),
          [out3]  "=Te" (*acc3)
        : // Inputs:
          [cnt]   "r" (rhs_cols)
        : // Clobbered registers:
          "q0", "q1", "q2", "q3", "q4", "q5", "memory", "r14"
    );
}

#define RHS_COLS 4
#define TOTAL_BYTES (RHS_COLS * 16)

//----------------------------------------------------------
// Unit test code
//----------------------------------------------------------
int main_test(void)
{
    // For testing, let’s assume rhs_cols = 4 (i.e. process 4 blocks of 16 bytes => 64 bytes per pointer).
    int rhs_cols = 4;
    int total_bytes = rhs_cols * 16;

    // Allocate and initialize buffers for "col" and each "row"
    uint8_t col_buf[TOTAL_BYTES];
    uint8_t row0_buf[TOTAL_BYTES];
    uint8_t row1_buf[TOTAL_BYTES];
    uint8_t row2_buf[TOTAL_BYTES];
    uint8_t row3_buf[TOTAL_BYTES];

    // Fill the buffers with known test data.
    // For "col", fill with an increasing pattern.
    for (int i = 0; i < total_bytes; i++) {
        col_buf[i] = (uint8_t)(i & 0xFF);
    }
    // For the rows, use constant values.
    memset(row0_buf, 10, total_bytes); // all 10
    memset(row1_buf, 20, total_bytes); // all 20
    memset(row2_buf, 30, total_bytes); // all 30
    memset(row3_buf, 40, total_bytes); // all 40

    // Make copies for the original kernel because our functions update the pointers.
    uint8_t col_buf_orig[TOTAL_BYTES];
    uint8_t row0_buf_orig[TOTAL_BYTES];
    uint8_t row1_buf_orig[TOTAL_BYTES];
    uint8_t row2_buf_orig[TOTAL_BYTES];
    uint8_t row3_buf_orig[TOTAL_BYTES];

    memcpy(col_buf_orig, col_buf, total_bytes);
    memcpy(row0_buf_orig, row0_buf, total_bytes);
    memcpy(row1_buf_orig, row1_buf, total_bytes);
    memcpy(row2_buf_orig, row2_buf, total_bytes);
    memcpy(row3_buf_orig, row3_buf, total_bytes);

    // Prepare accumulators for original and optimized versions.
    int32_t orig_acc0 = 0, orig_acc1 = 0, orig_acc2 = 0, orig_acc3 = 0, orig_sum = 0;
    int32_t opt_acc0  = 0, opt_acc1  = 0, opt_acc2  = 0, opt_acc3  = 0, opt_sum  = 0;

    // Set pointer variables for original kernel.
    uint8_t *col_ptr_orig  = col_buf_orig;
    uint8_t *row0_ptr_orig = row0_buf_orig;
    uint8_t *row1_ptr_orig = row1_buf_orig;
    uint8_t *row2_ptr_orig = row2_buf_orig;
    uint8_t *row3_ptr_orig = row3_buf_orig;

    // Set pointer variables for optimized kernel.
    uint8_t *col_ptr_opt  = col_buf;
    uint8_t *row0_ptr_opt = row0_buf;
    uint8_t *row1_ptr_opt = row1_buf;
    uint8_t *row2_ptr_opt = row2_buf;
    uint8_t *row3_ptr_opt = row3_buf;

    // Call the original kernel.
    original_kernel(&col_ptr_orig, &row0_ptr_orig, &row1_ptr_orig, &row2_ptr_orig, &row3_ptr_orig,
                    rhs_cols, &orig_acc0, &orig_acc1, &orig_acc2, &orig_acc3, &orig_sum);

    // Call the optimized kernel.
    optimized_kernel(&col_ptr_opt, &row0_ptr_opt, &row1_ptr_opt, &row2_ptr_opt, &row3_ptr_opt,
                     rhs_cols, &opt_acc0, &opt_acc1, &opt_acc2, &opt_acc3, &opt_sum);

    // Print results from both functions.
    ns_lp_printf("Original kernel results:\n");
    ns_lp_printf("  acc0: %d, acc1: %d, acc2: %d, acc3: %d, sum: %d\n", orig_acc0, orig_acc1, orig_acc2, orig_acc3, orig_sum);
    ns_lp_printf("Optimized kernel results:\n");
    ns_lp_printf("  acc0: %d, acc1: %d, acc2: %d, acc3: %d, sum: %d\n", opt_acc0, opt_acc1, opt_acc2, opt_acc3, opt_sum);

    // Compare outputs.
    if (orig_acc0 == opt_acc0 && orig_acc1 == opt_acc1 &&
        orig_acc2 == opt_acc2 && orig_acc3 == opt_acc3 && orig_sum == opt_sum) {
        ns_lp_printf("Unit test PASSED: Both implementations produce identical outputs.\n");
    } else {
        ns_lp_printf("Unit test FAILED: Outputs differ between implementations.\n");
    }

    // Free allocated memory.
    // free(col_buf);
    // free(row0_buf);
    // free(row1_buf);
    // free(row2_buf);
    // free(row3_buf);
    // free(col_buf_orig);
    // free(row0_buf_orig);
    // free(row1_buf_orig);
    // free(row2_buf_orig);
    // free(row3_buf_orig);

    return 0;
}







ns_uart_config_t uart_config = {
    .api=&ns_uart_V0_0_1,
    .uart_config = &g_sUartConfig,
    .tx_blocking = true,
    .rx_blocking = true
};

ns_uart_handle_t uart_handle = NULL;
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0); // nada
#endif
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();                                     // nada
    ns_lp_printf("Calling main test\n");


    main_test();
    ns_lp_printf("main test done\n");
    while(1);

    NS_TRY(ns_uart_init(&uart_config, &uart_handle), "UART init failed.\n");
    ns_interrupt_master_enable();

    ns_lp_printf("UART init done\n");
    
    char buffer[256];
    char txbuffer[256];
    char size[3];
    int num_bytes = 0;
    while(1) {
        if(ns_uart_data_available()) {
            // Receive number of bytes to read
            uint32_t status = ns_uart_blocking_receive_data(&uart_config, size, 3);
            if (status == AM_HAL_STATUS_SUCCESS) {
                num_bytes = atoi(size); // Convert the received size to an integer
                status = ns_uart_blocking_receive_data(&uart_config, buffer, num_bytes);
                int buffer_size = strlen(buffer);
                if(status == AM_HAL_STATUS_SUCCESS && num_bytes == buffer_size) {
                    memcpy(txbuffer, buffer, num_bytes);
                    ns_uart_blocking_send_data(&uart_config, txbuffer, num_bytes);
                    memset(buffer, 0, num_bytes);
                    memset(txbuffer, 0, num_bytes);
                }
        }
        ns_delay_us(10000);
    }
    }
}