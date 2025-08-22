/* Copyright 2020 The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
/// \file
/// \brief Main function to run benchmark on device.

/// NeuralSPOT Includes
#include "sww_ref_util.h"
#include "main.h"
#include "ns_perf_profile.h"
#include "ns_pmu_map.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_pmu_utils.h"

static volatile uint64_t g_cyc_isr = 0, g_lsu_isr = 0;
static volatile uint64_t g_cyc_proc = 0, g_lsu_proc = 0;
static ns_perf_counters_t g_wall_start;
static uint32_t g_frame_count=0;
#define REPORT_EVERY 200
#define CORE_CLOCK_HZ 96000000u

static void sww_perf_report_and_reset(void);

void my_rx_cb(ns_uart_transaction_t *t)
{
    int c;
    c = th_getchar();
    ee_serial_callback(c);
}

static int16_t local_buf[2][512] __attribute__((aligned(32)));
static volatile uint8_t wr_idx = 0;   // ISR writes
static volatile uint8_t rd_idx = 1;   // main reads
static volatile uint8_t ready=0;
static volatile uint32_t overrun_count = 0;

am_hal_cachectrl_range_t dcache_range {
    .ui32StartAddr = (uint32_t)&g_i2s_buffer0[0],
    .ui32Size = 1024
    };
extern "C" void am_dspi2s0_isr(void)
{
    // Start timing as soon as we enter
    // ns_perf_counters_t s_isr, e_isr, d_isr;
    // ns_capture_perf_profiler(&s_isr);

    uint32_t status;
    am_hal_i2s_interrupt_status_get(pI2SHandle, &status, true);
    am_hal_i2s_interrupt_clear(pI2SHandle, status);

    if (!(status & AM_HAL_I2S_INT_RXDMACPL))
        return;

    int16_t * dma_buf = (int16_t*)am_hal_i2s_dma_get_buffer(pI2SHandle, AM_HAL_I2S_XFER_RX);
    dcache_range.ui32StartAddr = (uint32_t)dma_buf;
    am_hal_cachectrl_dcache_invalidate(&dcache_range, false);

    // if (ready >= 2) {
    //     // Backlog full: DROP this frame (don't publish/flip) and record it.
    //     // th_printf("I2S overrun detected. Dropping frame.\r\n");
    //     overrun_count++;
    // } else {
        memcpy(local_buf[wr_idx], dma_buf, 512*sizeof(int16_t));
        __DMB();                 // ensure data is visible before publish
        rd_idx = wr_idx;         // publish
        wr_idx ^= 1;             // flip writer
        ready++;                 // 1 or 2
    // }

    am_hal_i2s_interrupt_service(pI2SHandle, status, &g_sI2S0Config);
    
    // ns_capture_perf_profiler(&e_isr);
    // ns_delta_perf(&s_isr, &e_isr, &d_isr);
    // g_cyc_isr += d_isr.cyccnt;
    // g_lsu_isr += d_isr.lsucnt;
}


void ns_power_memory_config(const ns_power_config_t *pCfg) {
    // configure SRAM & other memories
    #define ALL_RETAIN 1
    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eROMMode       = AM_HAL_PWRCTRL_ROM_AUTO,
 
        #if defined(AM_PART_APOLLO5A)
        .bEnableCache   = true,
        .bRetainCache   = true,
        #if ALL_RETAIN
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
        #else
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
        #endif
        #elif defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510)
        #if ALL_RETAIN
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
        #else
                .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
                .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
        #endif
        #endif
        #if defined(AM_PART_APOLLO5A)
                .bEnableNVM     = true,
        #else
        // .eNVMCfg        = AM_HAL_PWRCTRL_NVM0_AND_NVM1,
        .eNVMCfg        = AM_HAL_PWRCTRL_NVM0_ONLY,
        #endif
        .bKeepNVMOnInDeepSleep     = false
    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
 
#if defined(AM_PART_APOLLO5A)
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
#else
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0LPREN = 1;
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0SLPEN = 0;
    MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0PWRCTRL = 1;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1LPREN = 1;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1SLPEN = 0;
    // MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1PWRCTRL = 1;
#endif
    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
    .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_1M,
    // .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_NONE,
    // .eActiveWithMCU   = PWRCTRL_SSRAMPWREN_PWRENSSRAM_ALL,
    .eActiveWithMCU   = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithGFX   = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithDISP  = AM_HAL_PWRCTRL_SRAM_NONE,          
    .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_1M
    // .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_NONE
    };       
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);
}

int main(int argc, char *argv[]) {
    uint32_t uart_status;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_mlperf_mode2), "Power Init Failed.\n");
    ns_power_memory_config(&ns_mlperf_mode2);
    NS_TRY(sww_model_init(), "Model init failed.\n");
    gpio_init();

    // ns_init_perf_profiler();
    // ns_start_perf_profiler();	
    // ns_capture_perf_profiler(&g_wall_start);

    th_serialport_initialize();
    i2s_init();
    th_timestamp();
    ns_interrupt_master_enable();
    for (;;) {
        // consume all pending frames safely
        while (1) {
            NVIC_DisableIRQ(I2S0_IRQn);
            if (ready == 0) {
                NVIC_EnableIRQ(I2S0_IRQn);
                break;
            }
            uint8_t idx = rd_idx;
            ready--;
            NVIC_EnableIRQ(I2S0_IRQn);
            __DMB();
            
            // ns_perf_counters_t s_proc, e_proc, d_proc;

            switch (g_i2s_state) {
                case Streaming:
                    // ns_capture_perf_profiler(&s_proc);
                    process_chunk_and_cont_streaming(local_buf[idx]);
                    // ns_capture_perf_profiler(&e_proc);
                    // ns_delta_perf(&s_proc, &e_proc, &d_proc);
                    // g_cyc_proc += d_proc.cyccnt;
                    // g_lsu_proc += d_proc.lsucnt;

                    // g_frame_count++;
                    // if ((g_frame_count % REPORT_EVERY) == 0) {
                    //     sww_perf_report_and_reset();
                    // }
                    break;
                case FileCapture:
                    process_chunk_and_cont_capture(local_buf[idx]);
                    break;
                case Stopping:
                    g_i2s_state = Idle;
                    break;
                default:
                    break;
            }
        }
        ns_deep_sleep();
    }
    return 0;
}


static void gpio_init() {
    // timestamp pin
    am_hal_gpio_pinconfig(22, am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
    // processing pin
    am_hal_gpio_pinconfig(29, am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(29, AM_HAL_GPIO_OUTPUT_CLEAR);
    set_processing_pin_high();
    // wakeword detected pin
    am_hal_gpio_pincfg_t ww_det_out = am_hal_gpio_pincfg_output;
    am_hal_gpio_pinconfig(62, ww_det_out);
    am_hal_gpio_state_write(62, AM_HAL_GPIO_OUTPUT_SET);
}

static void i2s_init(void)
{
    // pll clock
    // am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_SYSPLL, 24576000, NULL);

    // hfrc2 clock
    // am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_HFRC2, AM_HAL_CLKMGR_HFRC2_FREQ_ADJ_196P608MHZ, NULL);

    // hfrc clock
    // am_hal_clkmgr_clock_config(AM_HAL_CLKMGR_CLK_ID_HFRC, AM_HAL_CLKMGR_HFRC_FREQ_FREE_RUN_APPROX_48MHZ, NULL);

    // Configure the necessary pins.
    am_bsp_i2s_pins_enable(I2S_MODULE_0, false);
    // Configure I2S0
    am_hal_i2s_initialize(I2S_MODULE_0, &pI2SHandle);

    am_hal_i2s_power_control(pI2SHandle, AM_HAL_I2S_POWER_ON, false);

    if (AM_HAL_STATUS_SUCCESS != am_hal_i2s_configure(pI2SHandle, &g_sI2S0Config))
    {
        th_printf("ERROR: Invalid I2S0 configuration.\nNote: For Apollo5 Rev.B0, I2S can only use PLL as the clock source.\n");
    }

    am_hal_i2s_enable(pI2SHandle);
    NVIC_SetPriority(I2S0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(I2S0_IRQn);

    am_hal_i2s_dma_configure(pI2SHandle, &g_sI2S0Config, &sTransfer0);

}


void th_timestamp(void) {
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_CLEAR);
    for (int i=0; i<100000; ++i) {
        __asm__ volatile("nop");
    }
    am_hal_gpio_state_write(22, AM_HAL_GPIO_OUTPUT_SET);
}

void th_serialport_initialize(void) {
  NS_TRY(ns_uart_init(&uart_config, &uart_handle), 
          "Failed to initialize UART for serial port");
  ns_uart_register_callbacks(uart_handle, my_rx_cb, NULL);
  am_util_stdio_printf_init(uart_stdio_print);
}

static char th_getchar() { 
  char data;
  ns_uart_nonblocking_receive_data(&uart_config, &data, 1);
  return data; 
}

void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

static void uart_stdio_print(char *pcBuf)
{
    size_t len = strlen(pcBuf);
    ns_uart_blocking_send_data(&uart_config, pcBuf, len);
}


static void sww_perf_report_and_reset(void)
{
    // --- Atomic snapshot/reset to avoid tearing while ISR runs ---
    NVIC_DisableIRQ(I2S0_IRQn);

    ns_perf_counters_t wall_now, wall_d;
    ns_capture_perf_profiler(&wall_now);
    ns_delta_perf(&g_wall_start, &wall_now, &wall_d);

    // Snap current window accumulators
    const uint64_t proc_cyc  = g_cyc_proc;
    const uint64_t isr_cyc   = g_cyc_isr;
    const uint64_t proc_lsu  = g_lsu_proc;
    const uint64_t isr_lsu   = g_lsu_isr;
    const uint32_t frames    = g_frame_count;

    // Reset for next window
    g_cyc_proc = g_lsu_proc = 0;
    g_cyc_isr  = g_lsu_isr  = 0;
    g_frame_count = 0;
    g_wall_start = wall_now;

    NVIC_EnableIRQ(I2S0_IRQn);
    // --- End atomic section ---

    // Helpers (keep it simple)
    const double wall_us  = (double)wall_d.cyccnt * 1e6 / (double)CORE_CLOCK_HZ;
    const double proc_us  = (double)proc_cyc     * 1e6 / (double)CORE_CLOCK_HZ;
    const double isr_us   = (double)isr_cyc      * 1e6 / (double)CORE_CLOCK_HZ;
    double idle_us        = wall_us - proc_us - isr_us;
    if (idle_us < 0.0) idle_us = 0.0;

    const double pct = (wall_us > 0.0) ? (100.0 / wall_us) : 0.0;

    const double avg_proc_us_per_frame = (frames ? (proc_us / (double)frames) : 0.0);
    const double avg_isr_us_per_frame  = (frames ? (isr_us  / (double)frames) : 0.0);
    const double avg_idle_us_per_frame = (frames ? (idle_us / (double)frames) : 0.0);

    const double proc_share = proc_us * pct;
    const double isr_share  = isr_us  * pct;
    const double idle_share = idle_us * pct;

    const double proc_lsu_pct = (proc_cyc ? (100.0 * (double)proc_lsu / (double)proc_cyc) : 0.0);
    const double isr_lsu_pct  = (isr_cyc  ? (100.0 * (double)isr_lsu  / (double)isr_cyc)  : 0.0);

    th_printf("\r\n=== System profile over %lu frames ===\r\n", (unsigned long)frames);

    th_printf("processing():  avg %.4f us/frame | share %.1f%% | LSU %.1f%%\r\n",
              avg_proc_us_per_frame, proc_share, proc_lsu_pct);

    th_printf("I2S ISR:       avg %.4f us/frame | share %.1f%% | LSU %.1f%%\r\n",
              avg_isr_us_per_frame,  isr_share,  isr_lsu_pct);

    th_printf("idle(main):    avg %.4f us/frame | share %.1f%%\r\n",
              avg_idle_us_per_frame, idle_share);

    // Optional sanity line: average wall per frame
    const double avg_wall_us_per_frame = (frames ? (wall_us / (double)frames) : 0.0);
    th_printf("TOTAL (wall):  avg %.4f us/frame\r\n", avg_wall_us_per_frame);
}