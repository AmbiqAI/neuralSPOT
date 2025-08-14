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
static int16_t g_wav_block_buff[SWW_WINLEN_SAMPLES];
static int8_t  g_model_input  [SWW_MODEL_INPUT_SIZE];

uint8_t g_gp_buffer[G_GP_BUFF_BYTES]; // general-purpose buffer; for capturing a waveform or activations.
uint32_t g_gp_buff_bytes = G_GP_BUFF_BYTES;
int16_t * g_wav_record = NULL;;
int8_t *g_act_buff = NULL;

// length in (16b) samples, but I2S receives stereo, so actual length in time will be 1/2 this
uint32_t g_i2s_wav_len = 0;
uint32_t g_first_frame = 1;

LogBuffer g_log = { .buffer = {0}, .current_pos = 0 };


extern void *pI2SHandle;

uint32_t g_act_idx = 0;

static int8_t in_data[AI_SWW_MODEL_IN_1_SIZE] __attribute__((aligned(32)));;
static int8_t out_data[AI_SWW_MODEL_OUT_1_SIZE] __attribute__((aligned(32)));
int32_t sww_input_len = str_ww_ref_model_inputs_len[0];
int32_t sww_output_len = str_ww_ref_model_outputs_len[0];


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
	//	th_printf("]\r\n==== Done ====\r\n");
}

void print_bytes(const uint8_t *buffer, uint32_t num_bytes)
{
	const int vals_per_line = 16;
	th_printf("[");
	for(uint32_t i=0;i<num_bytes;i+= vals_per_line)
	{
		for(int j=0;j<vals_per_line;j++)
		{
			if(i+j >= num_bytes)
			{
				break;
			}
			th_printf("0x%X, ", buffer[i+j]);
		}
		th_printf("\r\n");
	}
	th_printf("]\r\n==== Done ====\r\n");
}


void print_vals_float(const float *buffer, uint32_t num_vals)
{
	const int vals_per_line = 8;
	char end_char; // don't add a ',' after the last value, because it breaks JSON
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
			th_printf("%3.5e%c ", buffer[i+j], end_char);
		}
		th_printf("\r\n");
	}
	// th_printf("]\r\n==== Done ====\r\n");
	 th_printf("]\r\n\r\n");
}
void log_printf(LogBuffer *log, const char *format, ...) {
    va_list args;
    char temp_buffer[LOG_BUFFER_SIZE];
    int written;

    // Initialize the variable argument list
    va_start(args, format);

    // Write formatted output to a temporary buffer
    written = vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);

    // End the variable argument list
    va_end(args);

    // Check if the formatted string fits in the remaining buffer
    if (log->current_pos + written >= LOG_BUFFER_SIZE) {
        // Buffer overflow: Zero out and reset to the beginning
        memset(log->buffer, 0, LOG_BUFFER_SIZE);
        log->current_pos = 0;
    }

    // Copy the formatted string to the log buffer
    if (written > 0) {
        size_t bytes_to_copy = (written < LOG_BUFFER_SIZE) ? written : LOG_BUFFER_SIZE - 1;
        strncpy(&log->buffer[log->current_pos], temp_buffer, bytes_to_copy);
        log->current_pos += bytes_to_copy;
    }
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


void run_model_on_test_data(char *cmd_args[]) {
//	acquire_and_process_data(in_data);
	const int8_t *input_source=NULL;
	uint16_t timer_start, timer_stop, timer_diff;

	th_printf("In run_model. about to run model\r\n");
	// if (strcmp(cmd_args[1], "class0") == 0) {
	// 	input_source = test_input_class0;
	// }
	// else if (strcmp(cmd_args[1], "class1") == 0) {
	// 	input_source = test_input_class1;
	// }
	// else if (strcmp(cmd_args[1], "class2") == 0) {
	// 	input_source = test_input_class2;
	// }
	// else {
	// 	th_printf("Unknown input tensor name, defaulting to test_input_class0\r\n");
	// 	input_source = test_input_class0;
	// }
	// for(int i=0;i<AI_SWW_MODEL_IN_1_SIZE;i++){
	// 	in_data[i] = (ai_i8)input_source[i];
	// }
	// set_processing_pin_high();
	// timer_start = __HAL_TIM_GET_COUNTER(&htim16);
	// /*  Call inference engine */
	// aiRun(in_data, out_data);
	// timer_stop = __HAL_TIM_GET_COUNTER(&htim16);
	// set_processing_pin_low();
	// timer_diff = timer_stop-timer_start;
	// th_printf("TIM16: aiRun took (%u : %u) = %u TIM16 cycles\r\n", timer_start, timer_stop, timer_diff);

	// th_printf("Output = [");
	// for(int i=0;i<AI_SWW_MODEL_OUT_1_SIZE;i++){
	// 	th_printf("%02d, ", out_data[i]);
	// }
	th_printf("]\r\n");
}

void load_or_print_buff(char *cmd_args[]) {
	// process the 'db' command
	// `db load N` -- prepares to load N bytes.
	// `db ff0055aa` -- loads 5 bytes ([0xff, 0x00, 0x55, 0xaa])
	// `db print [N]` prints N bytes from the buffer, defaulting to the whole thing

	static int db_state = 0;  // 0=idle, 1=after 'db load', waiting for bytes'
	static int transfer_size = 0; // `db load N` sets transfer_size to N
	static int bytes_loaded = 0;  // bytes loaded since last `db load`

	char *byte_buff = (char *)g_i2s_buffer0; // g_i2s_buffer0 is in int16 pointer
	int buff_size = g_i2s_chunk_size_bytes;

	if (cmd_args[1] == NULL) {
		th_printf("Error: db requires a sub-command: 'db load <Nbytes>'; 'db print [Nbytes]', 'db <hexstring>'\r\n");
	}
	else if (strcmp(cmd_args[1], "load") == 0) {
		transfer_size = atoi(cmd_args[2]);
		if (transfer_size == 0) {
			th_printf("Error: Transfer size (%s) must be valid int; greater than 0.\r\n", cmd_args[2]);
			th_printf("Usage: 'db load N'; N>0\r\n");
			db_state = 0;
			return;
		}
		db_state = 1;
		bytes_loaded = 0;
		th_printf("Expecting %d bytes\r\n", transfer_size);
		return;
	}
	else if (isxdigit((int)cmd_args[1][0])) { // e.g. `db ff001234` actually loads the data`
		int num_chars = strlen(cmd_args[1]);
		uint8_t next_byte = 0;

		if (db_state != 1) {
			th_printf("Error: Must issue db load <Nbytes> command before transmitting data.\r\n");
			return;
		}
		if (num_chars % 2 != 0) {
			th_printf("Error: number of hex digits in data string must be even. Received %d\r\n", num_chars);
			th_printf("Still waiting for data\r\n");
			return;
		}
		char tmp_str[3] = {'\0', '\0', '\0'};

		for (int i=0;i<num_chars;) {
			tmp_str[0] = cmd_args[1][i++];
			tmp_str[1] = cmd_args[1][i++];

			if (!isxdigit((int)tmp_str[0]) || !isxdigit((int)tmp_str[1])) {
				th_printf("Error: Received non-hex digit in character pair '%s' at location %d\r\n", tmp_str, i);
				th_printf("Canceling segment upload. Still waiting for data\r\n");
				return;
			}
			next_byte = (uint8_t) strtol(tmp_str, NULL, 16);
			byte_buff[bytes_loaded++] = next_byte;

			if(bytes_loaded >= buff_size || bytes_loaded >= transfer_size) {
				db_state = 0;
				th_printf("m-load-done\r\n");
				return;
			}
		}
		th_printf("%d bytes received\r\n", bytes_loaded);
	}
	else if (strcmp(cmd_args[1], "getptr") == 0) {
		th_printf("m-buff-ptr-%d\r\n", bytes_loaded);
	}
	else if (strcmp(cmd_args[1], "setptr") == 0) {
		if (cmd_args[2] != NULL) {
			bytes_loaded = atoi(cmd_args[2]);
		}
		else {
			th_printf("Error: setptr requires a numeric argument: 'db setptr 123%%'");
		}
	}
	else if (strcmp(cmd_args[1], "print") == 0) {
		int bytes_to_print = 0;
		if (cmd_args[2] != NULL) {
			bytes_to_print = atoi(cmd_args[2]);
		}
		if (bytes_to_print <= 0 || bytes_to_print > buff_size) {
			bytes_to_print = buff_size;
		}
		th_printf("m-buffer-");
		for(int i=0; i<bytes_to_print; i++){
			th_printf("%02x", byte_buff[i]);
			if( i < bytes_to_print-1) {
				th_printf("-");
			}
			else {
				th_printf("\r\n");
			}
		}
	}
	else if (strcmp(cmd_args[1], "print_i16") == 0) {
		int vals_to_print = 0;
		if (cmd_args[2] != NULL) {
			vals_to_print = atoi(cmd_args[2]);
		}
		if (vals_to_print <= 0 || vals_to_print > buff_size/2) {
			vals_to_print = buff_size/2;
		}
		print_vals_int16((int16_t *)byte_buff, vals_to_print);
	}
	else {
		th_printf("Error: db: Unrecognized sub-command %s\r\n", cmd_args[1]);
	}
}
void run_extraction(char *cmd_args[]) {

	// Feature extraction work
	float32_t test_out[1024] = {0.0};
	float32_t dsp_buff[1024] = {0.0};
	// this will only operate on the first block_size (1024) elements of the input wav

	uint32_t timer_start, timer_stop;
	char *endptr;
	uint32_t offset;

    // Optional offset arg.  "extract 1024", if cmd_arg[1] is present, convert to long
    // if (cmd_args[1] != NULL && *cmd_args[1] != '\0') {
    // 	offset = strtol(cmd_args[1], &endptr, 10);
    // }
    // else {
    // 	offset = 0;
    // }
	// timer_start = __HAL_TIM_GET_COUNTER(&htim16);
	// compute_lfbe_f32(test_wav_marvin+offset, test_out, dsp_buff);
	// timer_stop = __HAL_TIM_GET_COUNTER(&htim16);

	th_printf("TIM16: compute_lfbe_f32 took (%lu : %lu) = %lu TIM16 cycles\r\n", timer_start, timer_stop, timer_stop-timer_start);
	th_printf("\r\n{\r\n");
	th_printf("\"Input\": ");
	print_vals_int16(test_wav_marvin+offset, 1024);
	th_printf(",\r\n \"Output\": ");
	print_vals_float(test_out, 40);
	th_printf("}\r\n");
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

void print_and_clear_log(char *cmd_args[]) {
	th_printf("Log contents[cp=%u]:\r\n<%s>\r\n", g_log.current_pos, g_log.buffer);
	memset(g_log.buffer, 0, LOG_BUFFER_SIZE);
	g_log.current_pos = 0;
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
	else if(strcmp(cmd_args[0], "run_model") == 0) {
		run_model_on_test_data(cmd_args);
	}
	else if(strcmp(cmd_args[0], "extract") == 0) {
		run_extraction(cmd_args);
	}
	else if(strcmp(cmd_args[0], "i2scap") == 0) {
		i2s_capture(cmd_args);
	}
	else if(strcmp(cmd_args[0], "log") == 0) {
		print_and_clear_log(cmd_args);
	}
	else if(strcmp(cmd_args[0], "start") == 0) {
		start_detection(cmd_args);
	}
	else if(strcmp(cmd_args[0], "stop") == 0) {
		stop_detection(cmd_args);
	}
	else if(strcmp(cmd_args[0], "state") == 0) {
		print_state(cmd_args);
	}
	else if(strcmp(cmd_args[0], "db") == 0) {
		load_or_print_buff(cmd_args);
	}
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
	else if(strcmp(cmd_args[0], "infer_wav") == 0) {
		infer_static_wav(cmd_args);
	}
	else if(strcmp(cmd_args[0], "extract_uart_stream") == 0) {
		extract_features_on_chunk(cmd_args);
	}
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

void infer_static_wav(char *cmd_args[]) {
	// // feature_buff is used internally as a 2nd internal scratch space,
	// in the FFT domain, so it needs to be winlen_samples long, even though
	// ultimately it will only hold NUM_MEL_FILTERS values.  This can probably
	// be improved with a refactored compute_lfbe_f32().
	static float32_t feature_buff[SWW_WINLEN_SAMPLES];
	static float32_t dsp_buff[SWW_WINLEN_SAMPLES];
	int num_steps;  // jhdbg
	int offset;
	uint32_t wav_len=0;
	const int16_t *wav_ptr=NULL;

	offset = atoi(cmd_args[1]);
	wav_ptr = test_wav_long + offset;
	wav_len = test_wav_long_len-offset;
	th_printf("Infering on static wav with offset = %d\r\n", offset);

	num_steps = (wav_len - (SWW_WINLEN_SAMPLES - SWW_WINSTRIDE_SAMPLES))/SWW_WINSTRIDE_SAMPLES;
	// TfLiteTensor *input_tensor = model.model_input[0];

	// extract the input scale factor from the (file-global) ai_input
	float32_t input_scale_factor = str_ww_ref_model_inputs_scale[0];

	// initialize model input buffer to 0s.
	for(int i=0;i<SWW_MODEL_INPUT_SIZE;i++) {
		g_model_input[i] = 0;
	}

	for(int idx_step=0; idx_step<num_steps; idx_step++) {

		compute_lfbe_f32(wav_ptr+(idx_step*SWW_WINSTRIDE_SAMPLES), feature_buff, dsp_buff);

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
		// print out the newest vector of features as int8
		th_printf("(");
		print_vals_int8(g_model_input+SWW_MODEL_INPUT_SIZE-NUM_MEL_FILTERS, NUM_MEL_FILTERS);
		th_printf(", ");

		/*  Call inference engine */
		str_ww_ref_model_model_run(&str_ww_ref_model_model_ctx);
		if( out_data[0] > DETECT_THRESHOLD || g_first_frame) {
			th_printf("[%d]: Detection (%d).  g_first_frame=%lu\r\n", idx_step, out_data[0], g_first_frame);
			log_printf(&g_log, "[%d]: Detection (%d).  g_first_frame=%lu\r\n", idx_step, out_data[0], g_first_frame);
			g_first_frame = 0;
		}
		else if( out_data[0] > 100) {
			th_printf("[%d]: Near miss (%d). \r\n", idx_step, out_data[0]);
		}

		th_printf("%d), \r\n", out_data[0]);
	}
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


void extract_features_on_chunk(char *cmd_args[]) {
	// // feature_buff is used internally as a 2nd internal scratch space,
	// // in the FFT domain, so it needs to be winlen_samples long, even though
	// // ultimately it will only hold NUM_MEL_FILTERS values.  This can probably
	// // be improved with a refactored compute_lfbe_f32().
	// static float32_t feature_buff[SWW_WINLEN_SAMPLES];
	// static float32_t dsp_buff[SWW_WINLEN_SAMPLES];
	// static int num_calls=0;

	// TfLiteTensor *input_tensor = model.model_input[0];
	// float32_t input_scale_factor = input_tensor->params.scale;


	// if( num_calls == 0) {
	// 	for(int i=0;i<SWW_MODEL_INPUT_SIZE;i++) {
	// 		g_model_input[i] = 0;
	// 	}
	// }

	// // wav samples should be in g_i2s_buffer0

    // // g_wav_block_buff[SWW_WINSTRIDE_SAMPLES:<end>]  are old samples to be
    // // shifted to the beginning of the clip. After this block,
    // // g_wav_block_buff[0:(winlen-winstride)] is populated
    // for(int i=SWW_WINSTRIDE_SAMPLES;i<SWW_WINLEN_SAMPLES;i++) {
    // 	g_wav_block_buff[i-SWW_WINSTRIDE_SAMPLES] = g_wav_block_buff[i];
    // }

    // // Now fill in g_wav_block_buff[(winlen-winstride):] with winstride new samples
	// for(int i=SWW_WINLEN_SAMPLES-SWW_WINSTRIDE_SAMPLES;i<SWW_WINLEN_SAMPLES;i++) {
	// 	// no 2* here because UART transmits mono, unlike I2S buffer, which is in stereo
	// 	g_wav_block_buff[i] = g_i2s_buffer0[i-(SWW_WINLEN_SAMPLES-SWW_WINSTRIDE_SAMPLES)];
	// }

	// compute_lfbe_f32(g_wav_block_buff, feature_buff, dsp_buff);

	// // shift current features in g_model_input[] and add new ones.
	// for(int i=0;i<SWW_MODEL_INPUT_SIZE-NUM_MEL_FILTERS;i++) {
	// 	g_model_input[i] = g_model_input[i+NUM_MEL_FILTERS];
	// }

	// for(int i=0;i<NUM_MEL_FILTERS;i++) {
	// 	g_model_input[i+SWW_MODEL_INPUT_SIZE-NUM_MEL_FILTERS] = (int8_t)(feature_buff[i]/input_scale_factor-128);
	// }

	// for(int i=0;i<AI_SWW_MODEL_IN_1_SIZE;i++){
	// 	in_data[i] = (int8_t)g_model_input[i];
	// }
	// TfLiteTensor* t = model.interpreter->input(0);
	// memcpy(t->data.int8, in_data, AI_SWW_MODEL_IN_1_SIZE);

	// /*  Call inference engine */
	// model.interpreter->Invoke();
	// TfLiteTensor * output = model.model_output[0];
	// memcpy(out_data,
	// 	output->data.int8,
	// 	AI_SWW_MODEL_OUT_1_SIZE * sizeof(int8_t));


    // num_calls++;

	// th_printf("m-features-[");
	// for(int i=0;i<NUM_MEL_FILTERS;i++) {
	// 	th_printf("%+3d", (int8_t)(feature_buff[i]/input_scale_factor-128));
	// 	if( i < NUM_MEL_FILTERS -1){
	// 		th_printf(", ");
	// 	}
	// }
	// th_printf("]\r\n");

	// th_printf("m-activations-[%+3d, %+3d, %+3d]\r\n", out_data[0], out_data[1], out_data[2]);


}

void process_chunk_and_cont_streaming(int16_t *idle_buffer) {
	// feature_buff is used internally as a 2nd internal scratch space,
	// in the FFT domain, so it needs to be winlen_samples long, even though
	// ultimately it will only hold NUM_MEL_FILTERS values.  This can probably
	// be improved with a refactored compute_lfbe_f32().
	static float32_t feature_buff[SWW_WINLEN_SAMPLES];
	static float32_t dsp_buff[SWW_WINLEN_SAMPLES];
	static int num_calls = 0;  // jhdbg

	// TfLiteTensor *input_tensor = model.model_input[0];
	float32_t input_scale_factor = str_ww_ref_model_inputs_scale[0];
	int32_t zero_point = str_ww_ref_model_inputs_zero_point[0];
	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
	set_processing_pin_high(); // start of processing, used for duty cycle measurement


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

	// compute_lfbe_f32(g_wav_block_buff, feature_buff, dsp_buff);

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
	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
	/*  Call inference engine */
	// str_ww_ref_model_model_run(&str_ww_ref_model_model_ctx);
	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);

	if(out_data[0] > DETECT_THRESHOLD || g_first_frame) {
		am_hal_gpio_state_write(36, AM_HAL_GPIO_OUTPUT_SET);
        ns_delay_us(1);
		am_hal_gpio_state_write(36, AM_HAL_GPIO_OUTPUT_CLEAR);
        g_first_frame = 0;
	}

    if ( g_act_idx < (g_gp_buff_bytes/sizeof(g_act_buff[0])) ) {
    	g_act_buff[g_act_idx++] = out_data[0];
    }

    num_calls++;
    set_processing_pin_low();  // end of processing, used for duty cycle measurement
	// am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
}


void compute_lfbe_f32(const int16_t *pSrc, float32_t *pDst, float32_t *pTmp)
{
	const uint32_t block_length=SWW_WINLEN_SAMPLES;
	const float32_t inv_block_length=1.0/SWW_WINLEN_SAMPLES;
	const uint32_t spec_len = SWW_WINLEN_SAMPLES/2+1;
	const float32_t preemphasis_coef = 0.96875; // 1.0 - 2.0 ** -5;
	const float32_t power_offset = 52.0;
	const uint32_t num_filters = 40;
	int i; // for looping
	// to maintain continuity in pre-emphasis over segment boundaries
	static float32_t last_value = 0.0;
	arm_status op_result = ARM_MATH_SUCCESS;

	// convert int16_t pSrc to float32_t.  range [-32768:32767] => [-1.0,1.0)
	// WINLEN - WINSTRIDE of these have already been converted once, so a little speedup
	// could probably be gained by factoring this out into process_chunk_and_continue_streaming
    for(i=0;i<block_length;i++){
    	pDst[i] = ((float32_t)pSrc[i])/32768.0;
    }

	// Apply pre-emphasis:  zero-pad input by 1, then x' = x[1:]-pe_coeff*x[:-1], so len(x')==len(x)
	// Start by scaling w/ coeff; pTmp = preemphasis_coef * input
	arm_scale_f32(pDst, preemphasis_coef, pTmp, block_length);
	// calculate pDst[0] separately since it uses a value from the last segment
	pDst[0] = pDst[0] - last_value*preemphasis_coef;

	// in the next frame pDst[SWW_WINSTRIDE_SAMPLES-1] will be 1 sample older than the 1st sample,
	// so it will be used in the pre-emphasis for pDst[0]
	last_value = pDst[SWW_WINSTRIDE_SAMPLES-1];

	// use pDst as a 2nd temp buffer pDst[1:] - pTmp => pDst[1:]
	arm_sub_f32 (pDst+1, pTmp, pDst+1, block_length-1);

	// apply hamming window to pDst and put results in pTmp.
	arm_mult_f32(pDst, hamm_win_1024, pTmp, block_length);


	/* RFFT based implementation */
	arm_rfft_fast_instance_f32 rfft_s;
	op_result = arm_rfft_fast_init_f32(&rfft_s, block_length);
	if (op_result != ARM_MATH_SUCCESS) {
		th_printf("Error %d in arm_rfft_fast_init_f32", op_result);
	}
	arm_rfft_fast_f32(&rfft_s,pTmp,pDst,0); // use config rfft_s; FFT(pTmp) => pDst, ifft=0

	// Now we need to take the magnitude of the spectrum.  For block_length=1024, it will be 513 elements
	// we'll use pTmp as an array of block_length/2+1 real values.
	// the N/2th element is real and stuck in pDst[1] (where fft[0].imag=0 should be)
	// move that to pTmp[block_length/2]
	pTmp[block_length/2] = pDst[1]; // real value corresponding to fsamp/2
	pDst[1] = 0; // so now pDst[0,1] = real,imag elements at f=0 (always real, so imag=0)
	arm_cmplx_mag_f32(pDst,pTmp,block_length/2); // mag(pDst) => pTmp.  pTmp[512] already set.

	//    powspec = (1 / data_config['window_size_samples']) * tf.square(magspec)
	arm_mult_f32(pTmp, pTmp,pDst, spec_len); // pDst[0:513] = pTmp[0:513]^2
	arm_scale_f32(pDst, inv_block_length, pTmp, spec_len);


	// The original lin2mel matrix is spec_len x num_filters, where each column holds one mel filter,
	// lin2mel_packed_<X>x<Y> has all the non-zero elements packed together in one 1D array
	// _filter_starts are the locations in each *original* column where the non-zero elements start
	// _filter_lens is how many non-zero elements are in each original column
	// So the i_th filter start in lin2mel_packed at sum(_filter_lens[:i])
	// And the corresponding spectrum segment starts at linear_spectrum[_filter_starts[i]]
	int lin2mel_coeff_idx = 0;
	/* Apply MEL filters; linear spectrum is now in pTmp[0:spec_len], put mel spectrum in pDst[0:num_filters] */
	for(i=0; i<num_filters; i++)
	{
		arm_dot_prod_f32 (pTmp+lin2mel_513x40_filter_starts[i],
				lin2mel_packed_513x40+lin2mel_coeff_idx,
				lin2mel_513x40_filter_lens[i],
				pDst+i);

		lin2mel_coeff_idx += lin2mel_513x40_filter_lens[i];
	}

	//    powspec_max = tf.reduce_max(input_tensor=powspec)
	//    powspec = tf.clip_by_value(powspec, 1e-30, powspec_max) # prevent -infinity on log
	for(i=0;i<num_filters;i++){
		pDst[i] = (pDst[i] > 1e-30) ? pDst[i] : 1e-30;
	}

	for(i=0; i<num_filters; i++){
		pDst[i] = 10*log10(pDst[i]);
	}

	//log_mel_spec = (log_mel_spec + power_offset - 32 + 32.0) / 64.0
	arm_offset_f32 (pDst, power_offset, pDst, num_filters);
	arm_scale_f32(pDst, (1.0/64.0), pTmp, num_filters);


	//log_mel_spec = tf.clip_by_value(log_mel_spec, 0, 1)
	for(i=0; i<num_filters; i++){
		pDst[i] = (pTmp[i] < 0.0) ? 0.0 : ((pTmp[i] > 1.0) ? 1.0 : pTmp[i]);
	}
}


