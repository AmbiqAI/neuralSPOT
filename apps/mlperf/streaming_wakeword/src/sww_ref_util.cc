/*
 * sww_util.c
 *
 *  Created on: Jan 16, 2025
 *      Author: jeremy
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include "am_hal_cachectrl.h"
#include "str_ww_ref_model_model.h"
#include "sww_ref_util.h"


#include "feature_extraction.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "fixed_data.h"

#define G_GP_BUFF_BYTES 64000
#define  MAX_CMD_TOKENS 8 // maximum number of tokens in a command, including the command and arguments
#define AI_SWW_MODEL_IN_1_SIZE (1200)
#define AI_SWW_MODEL_OUT_1_SIZE (3)
extern volatile i2s_state_t g_i2s_state;
extern void th_printf(const char *p_fmt, ...);
extern int16_t AM_SHARED_RW g_i2s_buffer0[1024/sizeof(int16_t)] __attribute__((aligned(32)));
extern int16_t AM_SHARED_RW g_i2s_buffer1[1024/sizeof(int16_t)] __attribute__((aligned(32)));
extern am_hal_i2s_config_t g_sI2S0Config;
// Command buffer (incoming commands from host)
char g_cmd_buf[EE_CMD_SIZE + 1];
size_t g_cmd_pos = 0u;

// variables for I2S receive
uint32_t g_int16s_read = 0;
// chunk should be a 'window-stride' long = 32ms stride * 16kS/s * 2B/sample = 1024
// then double because we receive stereo (2 samples per time point)
uint32_t g_i2s_chunk_size_bytes = 1024;
uint32_t g_i2s_status = AM_HAL_STATUS_SUCCESS; // status of the last I2S operation
// two ping-pong byte buffers for DMA transfers from I2S port.
alignas(16) NS_PUT_IN_TCM int16_t g_wav_block_buff[SWW_WINLEN_SAMPLES];
static int8_t  g_model_input  [SWW_MODEL_INPUT_SIZE];

uint8_t g_gp_buffer[G_GP_BUFF_BYTES]; // general-purpose buffer; for capturing a waveform or activations.
uint32_t g_gp_buff_bytes = G_GP_BUFF_BYTES;
int16_t * g_wav_record = NULL;;
int8_t *g_act_buff = NULL;

// length in (16b) samples, but I2S receives stereo, so actual length in time will be 1/2 this
uint32_t g_i2s_wav_len = 0;
uint32_t g_first_frame = 1;

extern void *pI2SHandle;

uint32_t g_act_idx = 0;

static int8_t in_data[AI_SWW_MODEL_IN_1_SIZE] __attribute__((aligned(32)));;
static int8_t out_data[AI_SWW_MODEL_OUT_1_SIZE] __attribute__((aligned(32)));
int32_t sww_input_len = str_ww_ref_model_inputs_len[0];
int32_t sww_output_len = str_ww_ref_model_outputs_len[0];
am_hal_cachectrl_range_t start_det_dcache_range {
	.ui32StartAddr = (uint32_t)&g_i2s_buffer0[0],
	.ui32Size = 1024
};

str_ww_ref_model_model_context_t str_ww_ref_model_model_ctx = {
    .input_data = {
        in_data,
    },
    .input_len = {
        sww_input_len,
    },
    .output_data = {
        out_data,
    },
    .output_len = {
        sww_output_len,
    },
    // This is optional, but useful for profiling or debugging
    // .callback = str_ww_ref_model_model_operator_cb,
    .user_data = NULL
};

extern  arm_rfft_fast_instance_f32 rfft_s;
const uint32_t block_length=SWW_WINLEN_SAMPLES;

void print_vals_int16(const int16_t *buffer, uint32_t num_vals)
{
	const int vals_per_line = 16;
	char end_char;

	th_printf("[");
	for(uint32_t i=0;i<num_vals;i+= vals_per_line)
	{
		for(int j=0;j<vals_per_line;j++)
		{
			end_char = (i+j==num_vals-1) ? ']' : ',';
			if(i+j >= num_vals)
			{
				break;
			}
			th_printf("%d%c ", buffer[i+j], end_char);
		}
		th_printf("\r\n");
	}
}


void print_vals_int8(const int8_t *buffer, uint32_t num_vals)
{
	const int vals_per_line = 16;
	char end_char;

	th_printf("[");
	for(uint32_t i=0;i<num_vals;i+= vals_per_line)
	{
		for(int j=0;j<vals_per_line;j++)
		{
			end_char = (i+j==num_vals-1) ? ' ' : ',';
			if(i+j >= num_vals)
			{
				break;
			}
			th_printf("%d%c ", buffer[i+j], end_char);
		}
		th_printf("\r\n");
	}
	th_printf("]\r\n");
	th_printf("]\r\n==== Done ====\r\n");
}



/**
 * This function assembles a command string from the UART. It should be called
 * from the UART ISR for each new character received. When the parser sees the
 * termination character, the user-defined th_command_ready() command is called.
 * It is up to the application to then dispatch this command outside the ISR
 * as soon as possible by calling ee_serial_command_parser_callback(), below.
 */
void ee_serial_callback(char c) {
  if (c == EE_CMD_TERMINATOR) {
    g_cmd_buf[g_cmd_pos] = (char)0;
    process_command(g_cmd_buf);
    g_cmd_pos = 0;
  } else {
    g_cmd_buf[g_cmd_pos] = c;
    g_cmd_pos = g_cmd_pos >= EE_CMD_SIZE ? EE_CMD_SIZE : g_cmd_pos + 1;
  }
}


int sww_model_init() {
    int status = str_ww_ref_model_model_init(&str_ww_ref_model_model_ctx); // model init with minimal defaults
	return status;
}


void stop_detection(char *cmd_args[]) {
switch(g_i2s_state) {
// the stopping/idle combination may not be necessary, but it was originally set up
// to go to Stopping then wait for the current transaction to complete before Idle.
// But sometimes the current transaction never completes, leaving the program hung.
case Streaming:
	g_i2s_state = Stopping;
	g_i2s_status = am_hal_i2s_dma_transfer_complete(pI2SHandle);
	am_hal_i2s_disable(pI2SHandle);
	g_i2s_state = Idle;
	th_timestamp(); // this timestamp will stop the measurement of power
	th_printf("Streaming stopped.\r\n");
	th_printf("target activations: \r\n");
	print_vals_int8(g_act_buff, g_act_idx); // jhdbg
	g_act_buff = NULL;
	break;
case FileCapture:
	g_i2s_state = Stopping;
	g_i2s_status = am_hal_i2s_dma_transfer_complete(pI2SHandle);
	am_hal_i2s_disable(pI2SHandle);
	g_i2s_state = Idle;
	g_wav_record = NULL;
	th_printf("Wav capture stopped.\r\n");
	break;
case Idle:
	th_printf("I2S is already idle.  Ignoring stop request\r\n");
	break;
case Stopping:
	th_printf("Stop already requested.\r\n");
	break;
default:
	th_printf("Unknown state %d detected. Requesting stop.\r\n", g_i2s_state);
	g_i2s_state = Stopping;
}
}

void start_detection(char *cmd_args[]) {
	if(g_i2s_state != Idle) {
			th_printf("I2S Rx currently in progress. Ignoring request\r\n");
	}
	else {
		g_i2s_state = Streaming;

		g_act_buff = (int8_t *)g_gp_buffer;
		if( !g_act_buff ) {
			th_printf("WARNING:  Activation buffer malloc failed.  Activation logging will not work.\r\n");
		}
		g_int16s_read = 0; // jhdbg -- only needed when we're capturing the waveform in addition to detecting
		g_first_frame = 1; // on the first frame of a recording we pulse the detection GPIO to synchronize timing.

		memset(g_act_buff, 0, g_gp_buff_bytes);
		g_act_idx = 0;

		th_printf("Listening for I2S data ... \r\n");

		// these memsets are not really needed, but they make it easier to tell
		// if the write never happened.
		memset(g_i2s_buffer0, 0xFF, g_i2s_chunk_size_bytes);
		memset(g_i2s_buffer1, 0xFF, g_i2s_chunk_size_bytes);

		// first several cycles won't fully populate g_model_input, so initialize
		// it with 0s to avoid unpredictable detections at the beginning
		memset(g_model_input, 0x00, SWW_MODEL_INPUT_SIZE*sizeof(int8_t));
		memset(g_wav_block_buff, 0x00, SWW_WINLEN_SAMPLES*sizeof(int16_t));
		
		am_hal_cachectrl_dcache_invalidate(&start_det_dcache_range, true);
		start_det_dcache_range.ui32StartAddr = (uint32_t)&g_i2s_buffer1[0];
		am_hal_cachectrl_dcache_invalidate(&start_det_dcache_range, true);

		th_timestamp(); // this timestamp will start the measurement of power
		set_processing_pin_low();  // end of processing, used for duty cycle measurement
		// pulse processing pin for 1us to align the duty cycle, energy measurements, and detections
		set_processing_pin_high();  // end of processing, used for duty cycle measurement
		ns_delay_us(1);
		set_processing_pin_low();  // end of processing, used for duty cycle measurement
		g_i2s_status = am_hal_i2s_dma_transfer_start(pI2SHandle, &g_sI2S0Config);
		th_printf("DMA receive initiated.\r\n");
	}
}

void i2s_capture(char *cmd_args[]) {
	if(g_i2s_state != Idle ) {
		 th_printf("I2S Rx currently in progress. Ignoring request\r\n");
		 return;
	}

	if (cmd_args[1]) {
		g_i2s_wav_len = atoi(cmd_args[1]);
		if( g_i2s_wav_len > g_gp_buff_bytes/2) {
			th_printf("Requested length %lu exceeds available memory. Capturing %lu samples\r\n",
					g_i2s_wav_len, g_gp_buff_bytes/2);
			g_i2s_wav_len = g_gp_buff_bytes/4;
		}
	}
	else {
		g_i2s_wav_len = g_gp_buff_bytes/2; // 2 bytes/sample
		th_printf("No length specified.  Capturing %lu samples\r\n", g_i2s_wav_len);
	}

	g_i2s_state = FileCapture;
	g_int16s_read = 0;
	g_wav_record = (int16_t *)g_gp_buffer; // g_gp_buff_bytes bytes
	if( !g_wav_record ) {
		th_printf("WARNING: Recording buffer has no allocated memory. I2S Capture will fail.\r\n");
	}
	th_printf("Listening for I2S data ... \r\n");
	memset(g_wav_record, 0, g_gp_buff_bytes); // *2 b/c wav_len is int16s
	// these memsets are not really needed, but they make it easier to tell
	// if the write never happened.
	// memset(g_i2s_buffer0, 0xFF, sizeof(g_i2s_buffer0));
	// memset(g_i2s_buffer1, 0xFF, sizeof(g_i2s_buffer1));

	g_i2s_status = am_hal_i2s_dma_transfer_start(pI2SHandle, &g_sI2S0Config);
	// you can also check hsai->State
	// th_printf("DMA receive initiated. status=%lu, state=%d\r\n", g_i2s_status, hsai_BlockA1.State);
	th_printf("    Status: 0=OK, 1=Error, 2=Busy, 3=Timeout; State: 0=Reset, 1=Ready, 2=Busy (internal process), 18=Busy (Tx), 34=Busy (Rx)\r\n");
}

void print_help(char *cmd_args[]) {
	char help_message[] =
	"name        -- print out an identifying message\r\n"
	"run_model   -- run the NN model. An optional  argument class0, class1, or class2 runs the model\r\n"
	"               on a selected input that is expected to return 0 (WW), 1 (silent), or 2(other)\r\n"
	"extract     -- run the feature extractor on the first block of a predefined wav form (test_wav_marvin)\r\n"
	"i2scap      -- Captures about 1s of stereo audio over an I2S link\r\n"
	"start       -- Start wakeword detection.  Repeatedly runs feature extraction and model on incoming I2S wav data"
    "stop        -- Stop wakeword detection"
    "state       -- print out the current operating mode (as int) and status of the I2S link"
	"log         -- The I2S capture function can write debug messages to a log. Prints and clears that log.\r\n"
	"help        -- Print this help message\r\n%"
	;

	th_printf(help_message);
}

void print_state(char *cmd_args[]) {
	//  th_printf("g_i2s_status=%lu, SAI state=%d\r\n", g_i2s_status, hsai_BlockA1.State);
	 th_printf("    Status: 0=OK, 1=Error, 2=Busy, 3=Timeout; State: 0=Reset, 1=Ready, 2=Busy (internal process), 18=Busy (Tx), 34=Busy (Rx)\r\n");
	 th_printf("g_i2s_state = %d, g_int16s_read=%lu\r\n", g_i2s_state, g_int16s_read);
}

void process_command(char *full_command) {
	char *cmd_args[MAX_CMD_TOKENS] = {NULL};

    th_printf("Received command: %s\r\n", full_command);

    // Split the command on spaces, so cmd_args[0] is the command itself
    char* token = strtok(full_command, " ");
    cmd_args[0] = token;

    // and cmd_args[1:] are the arguments
    for(int i=1;i<MAX_CMD_TOKENS;i++) {
        cmd_args[i] = strtok(NULL, " ");
        if(cmd_args[i] == NULL)
            break;
    }

    // Uncomment this block to print out the sub-commands individually for debugging.
    //    // print out the command and args 1 by 1 (for debug; remove later)
	//    for(int i=0;i<MAX_CMD_TOKENS && cmd_args[i] != NULL;i++) {
	//        th_printf("[%d]: %p=>%s\r\n", i, (void *)cmd_args[i], cmd_args[i]);
	//    }

	// full_command should be "<command> <arg1> <arg2>" (command and args delimited by spaces)
	// put the command and arguments into the array cmd_arg[]
	if (strcmp(cmd_args[0], "name") == 0) {
		th_printf(EE_MSG_NAME, EE_DEVICE_NAME, TH_VENDOR_NAME_STRING);
	}
	else if (strcmp(cmd_args[0], "profile") == 0) {
	    th_printf("m-profile-[%s]\r\n", EE_FW_VERSION);
	    th_printf("m-model-[%s]\r\n", TH_MODEL_VERSION);
	}
	// else if(strcmp(cmd_args[0], "run_model") == 0) {
	// 	run_model_on_test_data(cmd_args);
	// }
	// else if(strcmp(cmd_args[0], "extract") == 0) {
	// 	run_extraction(cmd_args);
	// }
	else if(strcmp(cmd_args[0], "i2scap") == 0) {
		i2s_capture(cmd_args);
	}
	// else if(strcmp(cmd_args[0], "log") == 0) {
	// 	print_and_clear_log(cmd_args);
	// }
	else if(strcmp(cmd_args[0], "start") == 0) {
		start_detection(cmd_args);
	}
	else if(strcmp(cmd_args[0], "stop") == 0) {
		stop_detection(cmd_args);
	}
	// else if(strcmp(cmd_args[0], "state") == 0) {
	// 	print_state(cmd_args);
	// }
	// else if(strcmp(cmd_args[0], "db") == 0) {
	// 	load_or_print_buff(cmd_args);
	// }
	else if(strcmp(cmd_args[0], "help") == 0) {
		print_help(cmd_args);
	}
	else if(strcmp(cmd_args[0], "timestamp") == 0) {
		th_timestamp(); // mostly useful for testing the timestamp code
	}
	// These next two are mostly useful for testing
	else if(strcmp(cmd_args[0], "proc_hi") == 0) {
		set_processing_pin_high();
	}
	else if(strcmp(cmd_args[0], "proc_lo") == 0) {
		set_processing_pin_low();
	}
	// else if(strcmp(cmd_args[0], "infer_wav") == 0) {
	// 	infer_static_wav(cmd_args);
	// }
	// else if(strcmp(cmd_args[0], "extract_uart_stream") == 0) {
	// 	extract_features_on_chunk(cmd_args);
	// }
	else if(cmd_args[0] == 0) {
		th_printf("Empty command (only a %% read).  Type 'help%%' for help\r\n"); // %% => %
	}
	else {
		th_printf("Unrecognized command %s\r\n", full_command);
	}
	th_printf(EE_MSG_READY);
}


void set_processing_pin_high(void) {
 am_hal_gpio_state_write(29, AM_HAL_GPIO_OUTPUT_SET);
}

void set_processing_pin_low(void) {
    am_hal_gpio_state_write(29, AM_HAL_GPIO_OUTPUT_CLEAR);
}

// Prototype now takes the buffer pointer instead of an SAI handle.
void process_chunk_and_cont_capture(int16_t *idle_buffer)
{
    int reading_complete = 0;

    // One chunk has finished; account for 16‑bit *samples* read.
    g_int16s_read +=  g_i2s_chunk_size_bytes/2;
    /*
     * Determine whether we’ve captured the requested length.
     * (g_i2s_wav_len is in int16 samples.)
     */
    if(g_int16s_read + g_i2s_chunk_size_bytes/2 <= g_i2s_wav_len)
    {
    }
    else
    {
        reading_complete = 1;   // Last partial block – will stop below
    }

    /* Optional GPIO toggle for profiling (replace with am_hal_gpio_… if desired) */
    // am_hal_gpio_state_write(GPIOB, AM_HAL_GPIO_OUTPUT_SET);

    /* Copy audio into capture buffer. */
    memcpy((uint8_t *)(g_wav_record + g_int16s_read - g_i2s_chunk_size_bytes/2),
           idle_buffer,
           g_i2s_chunk_size_bytes);
    if (reading_complete)
    {
        th_printf("DMA Receive completed %lu int16s read out of %lu requested\r\n",
                  g_int16s_read, g_i2s_wav_len);
        print_vals_int16(g_wav_record, g_int16s_read);

        g_wav_record = NULL;
        g_i2s_state  = Idle;
    }
}



void process_chunk_and_cont_streaming(int16_t *idle_buffer) {
	// ns_perf_counters_t s_process,e_process,d_process;
	// ns_capture_perf_profiler(&s_process);
	
	// feature_buff is used internally as a 2nd internal scratch space,
	// in the FFT domain, so it needs to be winlen_samples long, even though
	// ultimately it will only hold NUM_MEL_FILTERS values.  This can probably
	// be improved with a refactored compute_lfbe_f32().
	alignas(16) NS_PUT_IN_TCM static float32_t feature_buff[SWW_WINLEN_SAMPLES];
	alignas(16) NS_PUT_IN_TCM static float32_t dsp_buff[SWW_WINLEN_SAMPLES];
	static int num_calls = 0;  // jhdbg

	float32_t input_scale_factor = str_ww_ref_model_inputs_scale[0];
	int32_t zero_point = str_ww_ref_model_inputs_zero_point[0];

	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
	
	set_processing_pin_high(); // start of processing, used for duty cycle measurement

	// ns_perf_counters_t s1, e1, d1;
	// ns_capture_perf_profiler(&s1);

    // g_wav_block_buff[SWW_WINSTRIDE_SAMPLES:<end>]  are old samples to be
    // shifted to the beginning of the clip. After this block,
    // g_wav_block_buff[0:(winlen-winstride)] is populated

	for(int i=SWW_WINSTRIDE_SAMPLES;i<SWW_WINLEN_SAMPLES;i++) {
    	g_wav_block_buff[i-SWW_WINSTRIDE_SAMPLES] = g_wav_block_buff[i];
    }

    // // Now fill in g_wav_block_buff[(winlen-winstride):] with winstride new samples
	for(int i=SWW_WINLEN_SAMPLES-SWW_WINSTRIDE_SAMPLES;i<SWW_WINLEN_SAMPLES;i++) {
		// 2* is because the I2S buffer is in stereo
		g_wav_block_buff[i] = idle_buffer[(i-(SWW_WINLEN_SAMPLES-SWW_WINSTRIDE_SAMPLES))];
	}

	// ns_capture_perf_profiler(&e1);
	// ns_delta_perf(&s1, &e1, &d1);
	// g_cyc_slide += d1.cyccnt;
	// g_lsu_slide += d1.lsucnt;

	// ns_perf_counters_t s2, e2, d2;
	// ns_capture_perf_profiler(&s2);
	compute_lfbe_f32(g_wav_block_buff, feature_buff, dsp_buff);
	// ns_capture_perf_profiler(&e2);
	// ns_delta_perf(&s2,&e2,&d2);
	// g_cyc_feat += d2.cyccnt;
	// g_lsu_feat += d2.lsucnt;


	// ---- pack_features ----
	// ns_perf_counters_t s3,e3,d3;
	// ns_capture_perf_profiler(&s3);

	// shift current features in g_model_input[] and add new ones.
	for(int i=0;i<SWW_MODEL_INPUT_SIZE-NUM_MEL_FILTERS;i++) {
		g_model_input[i] = g_model_input[i+NUM_MEL_FILTERS];
	}
	for(int i=0;i<NUM_MEL_FILTERS;i++) {
		g_model_input[i+SWW_MODEL_INPUT_SIZE-NUM_MEL_FILTERS] = (int8_t)(feature_buff[i]/input_scale_factor-128);
	}

	for(int i=0;i<AI_SWW_MODEL_IN_1_SIZE;i++){
		in_data[i] = (int8_t)g_model_input[i];
	}

	// ns_capture_perf_profiler(&e3);
	// ns_delta_perf(&s3,&e3,&d3);
	// g_cyc_pack += d3.cyccnt;
	// g_lsu_pack += d3.lsucnt;

	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);

	/*  Call inference engine */
	// ns_perf_counters_t s4,e4,d4;
	// ns_capture_perf_profiler(&s4);

	str_ww_ref_model_model_run(&str_ww_ref_model_model_ctx);
	
	// ns_capture_perf_profiler(&e4);
	// ns_delta_perf(&s4,&e4,&d4);
	// g_cyc_infer += d4.cyccnt;
	// g_lsu_infer += d4.lsucnt;

	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);

	if(out_data[0] > DETECT_THRESHOLD || g_first_frame) {
		am_hal_gpio_state_write(62, AM_HAL_GPIO_OUTPUT_CLEAR);
        ns_delay_us(1);
		am_hal_gpio_state_write(62, AM_HAL_GPIO_OUTPUT_SET);
        g_first_frame = 0;
	}

    if ( g_act_idx < (g_gp_buff_bytes/sizeof(g_act_buff[0])) ) {
    	g_act_buff[g_act_idx++] = out_data[0];
    }

    num_calls++;
    set_processing_pin_low();  // end of processing, used for duty cycle measurement
	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);

// 	ns_capture_perf_profiler(&e_process);
// 	ns_delta_perf(&s_process,&e_process,&d_process);
// 	g_cyc_process += d_process.cyccnt;
// 	g_lsu_process += d_process.lsucnt;

// 	g_frame_count++;
// 	if (g_frame_count % g_report_every == 0) {
// 		sww_perf_report_and_reset();
// 	}
}

const float32_t inv_block_length=1.0/SWW_WINLEN_SAMPLES;
const uint32_t spec_len = SWW_WINLEN_SAMPLES/2+1;
const float32_t preemphasis_coef = 0.96875; // 1.0 - 2.0 ** -5;
const float32_t power_offset = 52.0;
const uint32_t num_filters = 40;

// assumes: hamm_win_1024 is 16B aligned in TCM; rfft_s is static-initialized
void compute_lfbe_f32(const int16_t * __restrict pSrc,
                      float32_t    * __restrict pDst,  // reused later by FFT
                      float32_t    * __restrict pTmp)  // scratch
{
    const float32_t k = 1.0f / 32768.0f;           // int16 -> float
    static float32_t last_x = 0.0f;                // cross-frame continuity

    // 1) Convert + pre-emphasize + window  ==> time buffer for FFT in pTmp
    float32_t prev = last_x;
#pragma clang loop vectorize(enable) interleave(enable)
    for (int i = 0; i < block_length; ++i) {
        float32_t x = (float32_t)pSrc[i] * k;      // convert
        float32_t y = x - preemphasis_coef * prev; // pre-emphasis
        pTmp[i] = y * hamm_win_1024[i];            // window
        prev = x;
    }
    last_x = (float32_t)pSrc[SWW_WINSTRIDE_SAMPLES - 1] * k;

    // 2) FFT: time pTmp -> packed complex pDst
    arm_rfft_fast_f32(&rfft_s, pTmp, pDst, 0);

    // 3) Power spectrum directly (no sqrt then square)
    float32_t *powspec = pTmp;                     // reuse scratch
    powspec[0]        = pDst[0] * pDst[0];         // DC is real
    powspec[block_length/2]      = pDst[1] * pDst[1];         // Nyquist is real
#pragma clang loop vectorize(enable) interleave(enable)
    for (int kbin = 1; kbin < block_length/2; ++kbin) {
        float32_t re = pDst[2*kbin];
        float32_t im = pDst[2*kbin + 1];
        powspec[kbin] = re*re + im*im;
    }

    arm_scale_f32(powspec, 1.0f/block_length, powspec, block_length/2 + 1);

    // 4) Mel filterbank (packed sparse)
    int idx = 0;
    for (int m = 0; m < 40; ++m) {
        arm_dot_prod_f32(powspec + lin2mel_513x40_filter_starts[m],
                         lin2mel_packed_513x40 + idx,
                         lin2mel_513x40_filter_lens[m],
                         pDst + m);
        idx += lin2mel_513x40_filter_lens[m];
    }
    // 5) floor → log10 → offset/scale → clip
	for(int i=0;i<num_filters;i++){
		pDst[i] = (pDst[i] > 1e-30) ? pDst[i] : 1e-30;
	}
	// const float32x4_t vmin = vdupq_n_f32(1e-30f);
	// for (int m = 0; m +3 < num_filters; m+=4) {
	// 	float32x4_t x = vldrwq_f32(&pDst[m]);
	// 	x = vmaxnmq_f32(x, vmin); // max(x, 1e-30f)
	// 	vstrwq_f32(&pDst[m], x); // store back

	// }
	#pragma clang loop vectorize(enable) interleave(enable)
    for (int m = 0; m < num_filters; ++m) {
		pDst[m] = 10 * log10(pDst[m]);
	}

	const float32_t s = 1.0f/64.0f;
	const float32_t b = power_offset * s;

	// const float32x4_t v0 = vdupq_n_f32(0.0f); // min clip
    // const float32x4_t v1 = vdupq_n_f32(1.0f); // max clilp
    // const float32x4_t vb = vdupq_n_f32(b);

	// for (int m = 0; m +3 < num_filters; m+=4) {
	// 	float32x4_t x = vldrwq_f32(&pDst[m]);
    //     x = vfmaq_n_f32(vb, x, s);           // x = vb + x*s
    //     x = vmaxnmq_f32(x, v0);				// clamp min 0
    //     x = vminnmq_f32(x, v1);				// clamp max 1
    //     vstrwq_f32(&pDst[m], x);                        // store back
	// }
	#pragma clang loop vectorize(enable) interleave(enable)
	for (int m = 0; m < 40; ++m) {
		float32_t y = pDst[m] * s + b;   // (x + power_offset) * (1/64)
		// clip to [0,1]
		pDst[m] = fminf(fmaxf(y, 0.f), 1.f);
	}
}
