

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "ns_peripherals_button.h"
#include "ns_pmu_utils.h"
#include "algorithm.h"

#include "am_hal_mcuctrl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include <arm_math.h>
#include <arm_mve.h>   // Intrinsics for MVE

// Results so far
// 1.15
// Orig: arm/lp 8.65ms, (58uJ), gcc/lp 24ms (115uJ)
// adam: arm/lp 4.54 (36.5uJ), gcc/lp 12.8ns (71uJ)
// o1: arm/lp 4.48 (36uJ), itcm 4.49 (36uJ), gcc/lp  12.4ms (69uJ) itcm 12.36 (69uJ)
// 
// Orig: arm/lp 8.67ms (71uJ), gcc/lp 18.8ms (117uJ)
// adam: arm/lp 4.58 (45.5uJ), gcc/lp 5.7ns (88uJ)
// o1:  4.55ms (44.5uJ), gcc/lp 5.7ns

// All flags on:

// static uint32_t elapsedTime = 0;
// ns_timer_config_t tickTimer = {
//     .api = &ns_timer_V1_0_0,
//     .timer = NS_TIMER_COUNTER,
//     .enableInterrupt = false,
// };

// void tic() {
//     uint32_t oldTime = elapsedTime;
//     elapsedTime = ns_us_ticker_read(&tickTimer);
//     if (elapsedTime == oldTime) {
//         // We've saturated the timer, reset it
//         ns_timer_clear(&tickTimer);
//     }
// }

// uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }

//=============================================================================
// Moc algorithm input configuration 
//
//=============================================================================
#define SAMPLE_RATE      25.0f
#define SIGNAL_FREQUENCY  3.4f
#define SIGNAL_LEVEL      1.0f
#define SIGNAL_PHASE      0.4f
#define NOISE_LEVEL      11.4f
// Algorithm instances
static struct algorithm algo;
static struct input_generator gen;
static float32_t tempBuffer[3 * 1024 + 512];
static volatile int algorithm_run_count = 0;
volatile atomic_uint timer_interrupt_count=0;
volatile int button_pressed = false;
enum mode {
    MODE_OFF,  // Deep sleep
    MODE_ON,   // Continuous operation
    MODE_1HZ,  // 1 Hz interrupts with simulated FIFO length of 25
    MODE_5HZ,  // 5 Hz interrupts with simulated FIFO length of 5
    MODE_25HZ, // 25 Hz interrupts without simulated FIFO
    NUMBER_OF_MODES
};

//=============================================================================
//
// Timer 0 configs
//
//=============================================================================
#define TIMER_0         0           // Timer number to be used in the example
#define PERIOD_1Hz      (uint32_t)32768       // Timer interrupr intervals
#define PERIOD_5Hz      (uint32_t)(32768/5)  
#define PERIOD_25Hz     (uint32_t)(32768/25)  

//=============================================================================
//
// Timer init
//
//=============================================================================
void timer_init(void)
{
am_hal_timer_config_t   TimerConfig;

    //
    // Set up the desired TIMER.
    // The default config parameters include:
    //  eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16
    //  eFunction = AM_HAL_TIMER_FN_EDGE
    //  Compare0 and Compare1 maxed at 0xFFFFFFFF
    //
    am_hal_timer_default_config_set(&TimerConfig);

    //
    // Modify the default parameters.
    // Configure the timer to a 1s period via ui32Compare1.
    //
    TimerConfig.eFunction			= AM_HAL_TIMER_FN_UPCOUNT;
		TimerConfig.eInputClock  	= AM_HAL_TIMER_CLOCK_XT;
    TimerConfig.ui32PatternLimit = 0;
    
    //
    // Configure the timer
    //
		am_hal_timer_config(TIMER_0, &TimerConfig);
    am_hal_timer_stop(TIMER_0);
    
    //
    // Clear the timer and its interrupt
    //
    am_hal_timer_interrupt_enable(AM_HAL_TIMER_INT_TMR0_CMP0);
    
    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_SetPriority(TIMER0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(TIMER0_IRQn);
    
} // timer_init()


//=============================================================================
//
// Timr isr
//
//=============================================================================
void am_timer00_isr(void)
{
uint32_t pui32IntStatus;

  am_hal_timer_interrupt_status_get(true, &pui32IntStatus);
  am_hal_timer_interrupt_clear(AM_HAL_TIMER_INT_TMR0_CMP0);

  //am_devices_led_toggle(am_bsp_psLEDs, 0);
  timer_interrupt_count++;

}

// #define EPSILON (1e-5f)
// struct algorithm originalInst;
// struct algorithm optimizedInst;
//     // We'll store temporary buffers, same sizes used by your existing code
//     float origTempBuffer[4 * 1024];
//     float optTempBuffer[4 * 1024];
// uint32_t origTime = 0;
// uint32_t optTime = 0;
// /**
//  * Test that feeds a sine wave into both the original and new algorithms
//  * and compares their frequency outputs.
//  */
// bool test_sine_wave(void)
// {
//     // Number of input samples to feed
//     const int kNumSamples = 6000;

//     // Create data structures


//     // Use the same sample rate for both
//     float sampleRate = 48000.0f;
//     float sampleInterval = 1.0f / sampleRate;

//     // Sine wave at 1000 Hz
//     struct input_generator gen;
//     input_generator_init(&gen,
//                          /* frequency  = */ 1000.0f,
//                          /* amplitude  = */ 1.0f,
//                          /* phase      = */ 0.0f,
//                          /* sample_rate= */ sampleRate);

//     // Initialize both algorithms
//     algorithm_init(&originalInst);
//     algorithm_init(&optimizedInst);



//     // We assume 1-channel input for simplicity; adapt if needed
//     struct algorithm_input in;
//     in.num_channels = 1;
//     in.sample_interval = sampleInterval;

//     // The output arrays each get 1 float from the algorithm() calls 
//     float origOutput[1];
//     float optOutput[1];

//     // Run through many samples, feed each algorithm, track the last valid output
//     float lastOrigFreq = 0.0f;
//     float lastOptFreq  = 0.0f;

//     for (int i = 0; i < kNumSamples; i++)
//     {
//         // Generate 1-sample of sine
//         float sample = generate_sine(&gen);

//         // Prepare input
//         in.channels[0] = sample;

//         // Call original
//         // tic();
//         bool origReady = orig_algorithm(&originalInst, &in,
//                                             origTempBuffer, origOutput);
//         // origTime += toc();
//         // ns_lp_printf("Original time: %d\n", toc());
//         if (origReady) {
//             // We have a new frequency estimate
//             lastOrigFreq = origOutput[0];
//         }

//         // Call optimized
//         // tic();
//         bool optReady = new_algorithm(&optimizedInst, &in,
//                                             optTempBuffer, optOutput);
//         // ns_lp_printf("Optimized time: %d\n", toc());
//         // optTime += toc();
//         if (optReady) {
//             // We have a new frequency estimate
//             lastOptFreq = optOutput[0];
//         }
//     }
//     // ns_lp_printf("Original time:  %d\n", origTime);
//     // ns_lp_printf("Optimized time: %d\n", optTime);

//     // Now compare final results 
//     // (or you could compare at each iteration if you wanted)
//     float diff = fabsf(lastOrigFreq - lastOptFreq);
//     ns_lp_printf("Original freq = %f, Optimized freq = %f, diff = %f\n",
//            lastOrigFreq, lastOptFreq, diff);

//     // Decide pass/fail
//     bool pass = (diff < EPSILON);
//     if (!pass) {
//         ns_lp_printf("TEST FAILED: difference is too large!\n");
//     } else {
//         ns_lp_printf("TEST PASSED!\n");
//     }

//     return pass;
// }

//=============================================================================
//
//  Set MCU cock frequency 
//
//=============================================================================
#define HIGH_PERFORMANCE  0

void set_mcu_clock_freq(void)
{
#if(HIGH_PERFORMANCE==1)
  am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
#endif
}

//=============================================================================
//
//  Reset MCU cock frequency
//
//=============================================================================
void reset_mcu_clock_freq(void)
{
#if (HIGH_PERFORMANCE==1)
  am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
#endif
}

static bool generate_input_and_run_algorithm(void) {
    struct algorithm_input input;

    float clean = generate_sine(&gen);
    for (int i = 0; i < MAX_INPUT_CHANNELS; i++) {
        input.channels[i] = clean + generate_white_noise(NOISE_LEVEL);
    }
    input.num_channels = MAX_INPUT_CHANNELS;
    input.sample_interval = 1.0f / SAMPLE_RATE;

    float output;
    return algorithm(&algo, &input, tempBuffer, &output);
}

void init_algorithm_and_generator(void) {
    algorithm_init(&algo);
    input_generator_init(&gen, SIGNAL_FREQUENCY, SIGNAL_LEVEL, SIGNAL_PHASE, SAMPLE_RATE);
}

void run_mode_until_button_press(enum mode mode) 
{
    uint32_t input_interval_ms;
    int fifo_len;
    
    if (mode == MODE_1HZ) {
        input_interval_ms = 1000;
        fifo_len = 25;
    }
    else if (mode == MODE_5HZ) {
        input_interval_ms = 200;
        fifo_len = 5;
    }
    else { // MODE_25HZ
        input_interval_ms = 40;
        fifo_len = 1;
    }

    if (mode == MODE_OFF) {
        // IMPLEMENT: Wait for button interrupt in deep sleep 
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
        button_pressed=false;
    }
    else if (mode == MODE_ON) 
    {
        // IMPLEMENT: Set desired clock frequency
        set_mcu_clock_freq();
        
        while (1) 
        {
            if (generate_input_and_run_algorithm() == true) {
                algorithm_run_count++;
            }
            // IMPLEMENT: Break if button is pressed
            if(button_pressed == true)
              {
              ns_lp_printf("Button pressed during mode %d\n", mode);
              button_pressed = false;
              break;;
              }
        }
        // IMPLEMENT: Reset clock frequency
        reset_mcu_clock_freq();
        
    }
    else {
        // IMPLEMENT: Start periodic timer (input_interval_ms) 
        am_hal_timer_compare0_set(TIMER_0,(32768*input_interval_ms)/1000);
        am_hal_timer_clear(TIMER_0) ;
        ns_lp_printf("Timer started for mode %d, interval %d\n", mode, input_interval_ms);
        am_hal_timer_start(TIMER_0);
        while (1) {
            // Handle all pending inputs or wait for next timer interrupt
            while (timer_interrupt_count == 0) {
                // IMPLEMENT: Wait for periodic timer interrupt in deep sleep
                am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
            }
            atomic_fetch_sub(&timer_interrupt_count, 1);
            // IMPLEMENT: Set desired clock frequency
            set_mcu_clock_freq();
            
            for (int i = 0; i < fifo_len; i++) { // Simulate FIFO
                if (generate_input_and_run_algorithm() == true) {
                    algorithm_run_count++;
                }
            }
            // IMPLEMENT: Reset clock frequency (if needed to allow deep sleep)
            reset_mcu_clock_freq();
            
            // IMPLEMENT: Break if button is pressed
            if(button_pressed == true)
              {
              button_pressed = false;
              break;
              }
        }
        // IMPLEMENT: Stop periodic timer
        am_hal_timer_stop(TIMER_0);
    }
}

ns_pmu_config_t ns_microProfilerPMU;

int ftc() {
    // Add the function here.
    for (int i = 0; i < 200; i++) {
        generate_input_and_run_algorithm();
    }
    // generate_input_and_run_algorithm();   
    return 0;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_set_performance_mode(NS_MINIMUM_PERF);
    ns_microProfilerPMU.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&ns_microProfilerPMU);
    // Any events, will be overriden by 
    // ns_pmu_event_create(&ns_microProfilerPMU.events[0], 0x01, NS_PMU_EVENT_COUNTER_SIZE_32); 
    // ns_pmu_event_create(&ns_microProfilerPMU.events[1], 0x02, NS_PMU_EVENT_COUNTER_SIZE_32); 
    // ns_pmu_event_create(&ns_microProfilerPMU.events[2], 0x03, NS_PMU_EVENT_COUNTER_SIZE_32);
    // ns_pmu_event_create(&ns_microProfilerPMU.events[3], 0x04, NS_PMU_EVENT_COUNTER_SIZE_32);
    // ns_set_performance_mode(NS_MAXIMUM_PERF);
    // NS_TRY(ns_timer_init(&tickTimer), "Timer Init Failed\n");
    ns_button_config_t button_config = {
        .api = &ns_button_V1_0_0,
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &button_pressed,
        .button_1_flag = NULL};
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");
    timer_init();

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    ns_lp_printf("Base NS init done\n");
    // init_hann_window();
    // Initialize the algorithm, input, and temp buffer
    // test_sine_wave();

    init_algorithm_and_generator();
    ns_pmu_characterize_function(&ftc, &ns_microProfilerPMU);

    enum mode selected_mode = MODE_OFF;
      
    init_algorithm_and_generator();

		 //am_devices_led_toggle(am_bsp_psLEDs, 0);

    while (1) {
        run_mode_until_button_press(selected_mode);
        ns_lp_printf("mode %d, algorithm run count %d\n", selected_mode, algorithm_run_count);
        selected_mode = (selected_mode + 1) % NUMBER_OF_MODES;
  
    }

    while(1);
    return 0;
}