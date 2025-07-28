/**
 * @file deep_sleep_benchmark.c
 * @brief Benchmark for measuring performance impact of deep sleep transitions.
 * @version 0.1
 * @date 2025-07-25
 *
 * This application periodically wakes the MCU from deep sleep, executes a
 * configurable dummy workload, and then returns to sleep. The GPIO pair
 * defined in ns_energy_monitor.h (NS_POWER_MONITOR_GPIO_0/1) is toggled
 * to allow external power‑measurement tools (e.g., Joulescope) to capture
 * active vs. sleep energy.
 *
 * Compile‑time knobs:
 *   - WORKLOAD_ITERATIONS  : Loop iterations executed each wake‑up (default 10000)
 *   - WAKEUP_FREQUENCY_HZ  : Wake‑up rate in Hz (default 50)
 *
 * Example build overrides:
 *   make -j EXAMPLE=examples/deepsleep_benchmark deploy \
 *        CFLAGS="-DWORKLOAD_ITERATIONS=50000 -DWAKEUP_FREQUENCY_HZ=10"
 */

 #include "ns_core.h"
 #include "ns_timer.h"
 #include "ns_energy_monitor.h"
 #include "ns_peripherals_power.h"
 #include "ns_ambiqsuite_harness.h"
 #include "ns_peripherals_button.h"

 #ifndef WORKLOAD_ITERATIONS
 #define WORKLOAD_ITERATIONS 200  /* Number of loop iterations executed each wake‑up */
 #endif
 
 #ifndef WAKEUP_FREQUENCY_HZ
 #define WAKEUP_FREQUENCY_HZ 50     /* How many times per second to wake up */
 #endif
 
 /* --- Derived constants ---*/
 #define WAKEUP_PERIOD_US (1000000 / WAKEUP_FREQUENCY_HZ)
 
 ////////////////////////////////////////////////
// Button config and application state
// buttonPressed - will be set by neuralSPOT button helper
static int volatile buttonPressed = 0;

// Button Peripheral Config Struct
ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                    .button_0_enable = true,
                                    .button_1_enable = false,
                                    .joulescope_trigger_enable = false,
                                    .button_0_flag = &buttonPressed,
                                    .button_1_flag = NULL,
                                    .joulescope_trigger_flag = NULL};

 /* Forward declaration of timer config */
 static ns_timer_config_t g_timer_cfg;
 
 /* Timer ISR: executes workload then re‑enters sleep */
 static void workload_isr(ns_timer_config_t *cfg)
 {
     /* Mark beginning of workload for external measurement */
    //  ns_set_power_monitor_state(NS_DATA_COLLECTION);
 
     volatile uint32_t acc = 0;
     for (volatile uint32_t i = 0; i < WORKLOAD_ITERATIONS; i++) {
         acc += i;            /* Simple arithmetic workload */
     }
     (void)acc;               /* Prevent optimization */
    //  ns_lp_printf("Workload completed\n");
     /* Mark end of workload */
    //  ns_set_power_monitor_state(NS_IDLE);
 
     /* Clear timer (best practice, no‑op on some HW) */
     ns_timer_clear(cfg);
 }
 
 /* Application entry */
 int main(void)
 {
     /* Basic chip & power init */
     ns_core_config_t core_cfg = { .api = &ns_core_V1_0_0 };
     NS_TRY(ns_core_init(&core_cfg), "core init failed\n");
     NS_TRY(ns_power_config(&ns_development_default), "power init failed\n");
     NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");

     NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")

     ns_itm_printf_enable();
     ns_interrupt_master_enable();

     /* Power‑state GPIOs */
     ns_init_power_monitor_state();
     ns_set_power_monitor_state(NS_IDLE);

     /* Configure wake‑up timer */
     g_timer_cfg = (ns_timer_config_t) {
         .api = &ns_timer_V1_0_0,
         .timer = NS_TIMER_INTERRUPT,
         .enableInterrupt = true,
         .periodInMicroseconds = WAKEUP_PERIOD_US,
         .callback = workload_isr
     };

     // Configure the button0, use it to start the benchmark
     ns_lp_printf("Press Button 0 to start the benchmark\n");
     ns_set_power_monitor_state(3);

     while (!buttonPressed) {
         ns_deep_sleep();
     }
     buttonPressed = 0;
     ns_lp_printf("Button 0 pressed, starting benchmark\n");
     ns_set_power_monitor_state(0);
     ns_lp_printf("Deep‑sleep benchmark started: %d iterations every %d us\n",
                  WORKLOAD_ITERATIONS, WAKEUP_PERIOD_US);
    NS_TRY(ns_timer_init(&g_timer_cfg), "timer init failed\n");

     /* Enable global interrupts now that timer is configured */
     ns_interrupt_master_enable();
 
     /* Main loop: go to deep sleep and wait for interrupts */
     while (1) {
         ns_deep_sleep();
     }
 
     return 0;
 }
 