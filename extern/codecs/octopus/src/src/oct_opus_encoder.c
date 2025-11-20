/* Copyright (c) 2010-2011 Xiph.Org Foundation, Skype Limited
   Written by Jean-Marc Valin and Koen Vos */
/* org
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include "celt.h"
#include "entenc.h"
#include "modes.h"
//#include "API.h"
#include "SigProc_FIX.h"
#include "stack_alloc.h"
#include "float_cast.h"
#include "opus.h"
#include "arch.h"
#include "pitch.h"
#include "opus_private.h"
#include "os_support.h"
#include "cpu_support.h"
#include "analysis.h"
#include "mathops.h"
#include "tuning_parameters.h"
#ifdef FIXED_POINT
//#include "fixed/structs_FIX.h"
#else
//#include "float/structs_FLP.h"
#endif

//
// Version information
//
#define PACKAGE_VERSION "1.3.1.d"
volatile const char lib_version_string[] = {
    "Ambiq Micro OPUS version: 1.0.0\n"
};
volatile const char package_version_string[] = {
    PACKAGE_VERSION
};

#ifdef WIN32
typedef unsigned int uint32_t;

#endif
#define MAX_ENCODER_BUFFER 480

#ifndef DISABLE_FLOAT_API

#define PSEUDO_SNR_THRESHOLD 316.23f    /* 10^(25/10) */
#endif

typedef struct {
   opus_val32 XX, XY, YY;
   opus_val16 smoothed_width;
   opus_val16 max_follower;
} StereoWidthState;
#if 1
typedef struct _spl_opus_encoder_t
{
	CELTEncoder *p_celt_enc;
	int rangeFinal;
	int Fs;
	opus_val16   delay_buffer[MAX_ENCODER_BUFFER * 2];
	int encoder_buffer;
	int variable_HP_smth2_Q15;
	opus_val32   hp_mem[4];
	opus_val16   prev_HB_gain;
	int arch;
        int header;
}spl_opus_encoder_t;

spl_opus_encoder_t o_spl_opus_encoder;
void* get_celt_handle();

char* audio_get_lib_version(void)
{
    return (char*)lib_version_string;
}
char* audio_get_package_version(void)
{
    return (char*)package_version_string;
}

int opus_packet_get_nb_channels(const unsigned char *data)
{
	return (data[0] & 0x4) ? 2 : 1;
}


int opus_packet_get_nb_frames(const unsigned char packet[], opus_int32 len)
{
	int count;
	if (len < 1)
		return OPUS_BAD_ARG;
	count = packet[0] & 0x3;
	if (count == 0)
		return 1;
	else if (count != 3)
		return 2;
	else if (len < 2)
		return OPUS_INVALID_PACKET;
	else
		return packet[1] & 0x3F;
}
spl_opus_encoder_t* octopus_encoder_create(int option);
int octopus_encode(spl_opus_encoder_t *st, const short *pcm, int frame_size, unsigned char *p_out_data, int out_data_bytes);


spl_opus_encoder_t* octopus_encoder_create(int option)
{
	int err;
	//int ret;
	int Fs = 16000;
	int channels = 1;
	spl_opus_encoder_t *st = &o_spl_opus_encoder;

	st->arch = opus_select_arch();
	int complexity = 4;

	st->Fs = Fs;
	st->encoder_buffer = st->Fs / 100;
	st->variable_HP_smth2_Q15 = silk_LSHIFT(EC_ILOG(VARIABLE_HP_MIN_CUTOFF_HZ), 8);
	st->p_celt_enc = get_celt_handle();
	/* Create CELT encoder */
	/* Initialize CELT encoder */
	err = celt_encoder_init(st->p_celt_enc, Fs, channels, st->arch);
	if (err != OPUS_OK)
		return NULL;

	celt_encoder_ctl(st->p_celt_enc, CELT_SET_SIGNALLING(0));
	celt_encoder_ctl(st->p_celt_enc, OPUS_SET_COMPLEXITY(complexity));

#ifdef SPL_ENABLE_TONAL_ANALYSIS
#ifndef DISABLE_FLOAT_API
	tonality_analysis_init(&st->analysis, st->Fs);
	st->analysis.application = st->application;
#endif
#endif
	st->prev_HB_gain = Q15ONE;
        st->header = option;
	return st;
}

static unsigned char gen_toc(int max_data_bytes)
{
	int period;
	unsigned char toc;
	//int mode = MODE_CELT_ONLY;
	int framerate = 50;
	//int frame_size = 20;
	int bandwidth = OPUS_BANDWIDTH_WIDEBAND;
	int channels = 1;

	period = 0;
	while (framerate < 400)
	{
		framerate <<= 1;
		period++;
	}

		int tmp = bandwidth - OPUS_BANDWIDTH_MEDIUMBAND;
		if (tmp < 0)
			tmp = 0;
		toc = 0x80;
		toc |= tmp << 5;
		toc |= period << 3;

	toc |= (channels == 2) << 2;



	//	toc |= MODE_CELT_ONLY;
	return toc;
}
static void dc_reject(const short *in, opus_int32 cutoff_Hz, opus_val16 *out, opus_val32 *hp_mem, int len, int channels, opus_int32 Fs)
{
	int i;
	float coef, coef2;
	coef = 6.3f*cutoff_Hz / Fs;
	coef2 = 1 - coef;
	if (channels == 2)
	{
		float m0, m2;
		m0 = hp_mem[0];
		m2 = hp_mem[2];
		for (i = 0; i < len; i++)
		{
			opus_val32 x0, x1, out0, out1;
			x0 = in[2 * i + 0];
			x1 = in[2 * i + 1];
			out0 = x0 - m0;
			out1 = x1 - m2;
			m0 = coef * x0 + VERY_SMALL + coef2 * m0;
			m2 = coef * x1 + VERY_SMALL + coef2 * m2;
			out[2 * i + 0] = out0;
			out[2 * i + 1] = out1;
		}
		hp_mem[0] = m0;
		hp_mem[2] = m2;
	}
	else {
		float m0;
		m0 = hp_mem[0];
		for (i = 0; i < len; i++)
		{
			opus_val32 x, y;
			x = in[i]/32768.0f;
			y = x - m0;
			m0 = coef * x + VERY_SMALL + coef2 * m0;
			out[i] = y;
		}
		hp_mem[0] = m0;
	}
}

static void gain_fade(const opus_val16 *in, opus_val16 *out, opus_val16 g1, opus_val16 g2,
	int overlap48, int frame_size, int channels, const opus_val16 *window, opus_int32 Fs)
{
	int i;
	int inc;
	int overlap;
	int c;
	inc = 48000 / Fs;
	overlap = overlap48 / inc;
	if (channels == 1)
	{
		for (i = 0; i < overlap; i++)
		{
			opus_val16 g, w;
			w = MULT16_16_Q15(window[i*inc], window[i*inc]);
			g = SHR32(MAC16_16(MULT16_16(w, g2),
				Q15ONE - w, g1), 15);
			out[i] = MULT16_16_Q15(g, in[i]);
		}
	}
	else {
		for (i = 0; i < overlap; i++)
		{
			opus_val16 g, w;
			w = MULT16_16_Q15(window[i*inc], window[i*inc]);
			g = SHR32(MAC16_16(MULT16_16(w, g2),
				Q15ONE - w, g1), 15);
			out[i * 2] = MULT16_16_Q15(g, in[i * 2]);
			out[i * 2 + 1] = MULT16_16_Q15(g, in[i * 2 + 1]);
		}
	}
	c = 0; do {
		for (i = overlap; i < frame_size; i++)
		{
			out[i*channels + c] = MULT16_16_Q15(g2, in[i*channels + c]);
		}
	} while (++c < channels);
}
//typedef unsigned int uint32_t;

//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val );
uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}
int octopus_encode(spl_opus_encoder_t *st, const short *pcm, int frame_size, unsigned char *p_out_data, int out_data_bytes)
{
    int is_add_header = st->header;
	int channels = 1;
//	void *silk_enc;
	CELTEncoder *celt_enc;
//	int i;
	int ret = 0;
//	opus_int32 nBytes;
	ec_enc enc;
//	int bytes_target;
//	int prefill = 0;
	int start_band = 0;
	int redundancy = 0;
	int redundancy_bytes = 0; /* Number of bytes to use for redundancy frame */
	int celt_to_silk = 0;
	VARDECL(opus_val16, pcm_buf);
	int nb_compr_bytes;
//	int to_celt = 0;
	opus_uint32 redundant_rng = 0;
	int hp_freq_smth1;
//	int voice_est; /* Probability of voice in Q7 */
//	opus_int32 equiv_rate;
//	int delay_compensation;
//	int frame_rate;
//	opus_int32 max_rate; /* Max bitrate we're allowed to use */
//	int curr_bandwidth;
        unsigned char *data = 0;
	opus_val16 HB_gain;
	opus_int32 max_data_bytes; /* Max number of bytes we're allowed to use */
	int total_buffer = 0;
//	opus_val16 stereo_width;
	const CELTMode *celt_mode;
#ifndef DISABLE_FLOAT_API
	AnalysisInfo analysis_info;
//	int analysis_read_pos_bak = -1;
//	int analysis_read_subframe_bak = -1;
//	int is_silence = 0;
#endif
//	VARDECL(opus_val16, tmp_prefill);

    SAVE_STACK;
	ALLOC_STACK;

    if(is_add_header)
    {
        data = &p_out_data[8];
    }
    else
    {
        data = &p_out_data[0];
    }

    //
    // Check "Tegridy"
    //
    // if(MCUCTRL->VENDORID != 0x414D4251)
    // {
    //     // you're stoned...
    //     return 84;
    // }

	max_data_bytes = IMIN(1276, out_data_bytes);

	st->rangeFinal = 0;

	if (frame_size <= 0 || max_data_bytes <= 0)
	{
		RESTORE_STACK;
		return OPUS_BAD_ARG;
	}

	/* Cannot encode 100 ms in 1 byte */
	if (max_data_bytes == 1 && st->Fs == (frame_size * 10))
	{
		RESTORE_STACK;
		return OPUS_BUFFER_TOO_SMALL;
	}
	{
	int bitrate_bps = 32000; // todo
		/* Multiply by 12 to make sure the division is exact. */
		int frame_rate12 = 12 * st->Fs/320;
		/* We need to make sure that "int" values always fit in 16 bits. */
		int cbrBytes = IMIN((12 * bitrate_bps / 8 + frame_rate12 / 2) / frame_rate12, max_data_bytes);
		bitrate_bps = cbrBytes * (opus_int32)frame_rate12 * 8 / 12;
		/* Make sure we provide at least one byte to avoid failing. */
		max_data_bytes = IMAX(1, cbrBytes);
	}

	celt_enc = st->p_celt_enc;
        celt_encoder_ctl(celt_enc, OPUS_SET_LSB_DEPTH(16));

	celt_encoder_ctl(celt_enc, CELT_GET_MODE(&celt_mode));
#ifndef DISABLE_FLOAT_API
	analysis_info.valid = 0;
#endif
	/* Update equivalent rate for channels decision. */
	//equiv_rate = bitrate_bps;

	//curr_bandwidth = OPUS_BANDWIDTH_WIDEBAND;

        /* leave first bye for toc */
	ec_enc_init(&enc, data + 1, max_data_bytes - 1);

	ALLOC(pcm_buf, (total_buffer + frame_size)*channels, opus_val16);
	OPUS_COPY(pcm_buf, &st->delay_buffer[(st->encoder_buffer - total_buffer)*channels], total_buffer*channels);

	hp_freq_smth1 = silk_LSHIFT(EC_ILOG(VARIABLE_HP_MIN_CUTOFF_HZ), 8);

	st->variable_HP_smth2_Q15 = silk_SMLAWB(st->variable_HP_smth2_Q15,
		hp_freq_smth1 - st->variable_HP_smth2_Q15, SILK_FIX_CONST(VARIABLE_HP_SMTH_COEF2, 16));

	/* convert from log scale to Hertz */
	//cutoff_Hz = silk_log2lin(silk_RSHIFT(st->variable_HP_smth2_Q15, 8));


		dc_reject(pcm, 3, &pcm_buf[total_buffer*channels], st->hp_mem, frame_size, channels, st->Fs);
#ifndef FIXED_POINT
	if (0)// float_api)
	{
		opus_val32 sum;
		sum = celt_inner_prod(&pcm_buf[total_buffer*channels], &pcm_buf[total_buffer*channels], frame_size*channels, st->arch);
		/* This should filter out both NaNs and ridiculous signals that could
		   cause NaNs further down. */
		if (!(sum < 1e9f) || celt_isnan(sum))
		{
			OPUS_CLEAR(&pcm_buf[total_buffer*channels], frame_size*channels);
			st->hp_mem[0] = st->hp_mem[1] = st->hp_mem[2] = st->hp_mem[3] = 0;
		}
	}
#endif


	/* SILK processing */
	HB_gain = Q15ONE;

	/* CELT processing */
	{
		int endband = 17;

		celt_encoder_ctl(celt_enc, CELT_SET_END_BAND(endband));
		celt_encoder_ctl(celt_enc, CELT_SET_CHANNELS(channels));
	}
	celt_encoder_ctl(celt_enc, OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
	celt_encoder_ctl(celt_enc, CELT_SET_PREDICTION(1));

//	ALLOC(tmp_prefill, channels*st->Fs / 400, opus_val16);

	if (channels*(st->encoder_buffer - (frame_size + total_buffer)) > 0)
	{
		OPUS_MOVE(st->delay_buffer, &st->delay_buffer[channels*frame_size], channels*(st->encoder_buffer - frame_size - total_buffer));
		OPUS_COPY(&st->delay_buffer[channels*(st->encoder_buffer - frame_size - total_buffer)],
			&pcm_buf[0],
			(frame_size + total_buffer)*channels);
	}
	else {
		OPUS_COPY(st->delay_buffer, &pcm_buf[(frame_size + total_buffer - st->encoder_buffer)*channels], st->encoder_buffer*channels);
	}
	/* gain_fade() and stereo_fade() need to be after the buffer copying
	   because we don't want any of this to affect the SILK part */
	if (st->prev_HB_gain < Q15ONE || HB_gain < Q15ONE) {
		gain_fade(pcm_buf, pcm_buf,
			st->prev_HB_gain, HB_gain, celt_mode->overlap, frame_size, channels, celt_mode->window, st->Fs);
	}

    //
    // Check "Tegridy"
    //
    // if(*(volatile uint32_t*)0x40020010 != 0x414D4251)
    // {
    //     // you're stoned...
    //     return 84;
    // }

	st->prev_HB_gain = HB_gain;
	start_band = 0;
	nb_compr_bytes = (max_data_bytes - 1) - redundancy_bytes;
	ec_enc_shrink(&enc, nb_compr_bytes);
#ifndef DISABLE_FLOAT_API
	celt_encoder_ctl(celt_enc, CELT_SET_ANALYSIS(&analysis_info));
#endif
	/* 5 ms redundant frame for CELT->SILK */
	if (redundancy && celt_to_silk)
	{
		int err;
		celt_encoder_ctl(celt_enc, CELT_SET_START_BAND(0));
		celt_encoder_ctl(celt_enc, OPUS_SET_VBR(0));
		celt_encoder_ctl(celt_enc, OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
		err = celt_encode_with_ec(celt_enc, pcm_buf, st->Fs / 200, data + nb_compr_bytes, redundancy_bytes, NULL);
		if (err < 0)
		{
			RESTORE_STACK;
			return OPUS_INTERNAL_ERROR;
		}
		celt_encoder_ctl(celt_enc, OPUS_GET_FINAL_RANGE(&redundant_rng));
		celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);
	}

	celt_encoder_ctl(celt_enc, CELT_SET_START_BAND(start_band));
        static int mode = 1002;
        static int prev_mode = 0;
            VARDECL(opus_val16, tmp_prefill);
    ALLOC(tmp_prefill, channels*st->Fs/400, opus_val16);
    if (mode != MODE_SILK_ONLY && mode != prev_mode && prev_mode > 0)
    {
       OPUS_COPY(tmp_prefill, &st->delay_buffer[(st->encoder_buffer-total_buffer-st->Fs/400)*channels], channels*st->Fs/400);
    }
    if (1)
    {
        if (mode != prev_mode && prev_mode > 0)
        {
           unsigned char dummy[2];
           celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);

           /* Prefilling */
           celt_encode_with_ec(celt_enc, tmp_prefill, st->Fs/400, dummy, 2, NULL);
           celt_encoder_ctl(celt_enc, CELT_SET_PREDICTION(0));
        }
        /* If false, we already busted the budget and we'll end up with a "PLC frame" */
        if (ec_tell(&enc) <= 8*nb_compr_bytes)
        {

           celt_encoder_ctl(celt_enc, OPUS_SET_VBR(0));
           ret = celt_encode_with_ec(celt_enc, pcm_buf, frame_size, NULL, nb_compr_bytes, &enc);
           if (ret < 0)
           {
              RESTORE_STACK;
              return OPUS_INTERNAL_ERROR;
           }
           /* Put CELT->SILK redundancy data in the right place. */
           if (0)
           {
              OPUS_MOVE(data+ret, data+nb_compr_bytes, redundancy_bytes);
              nb_compr_bytes = nb_compr_bytes+redundancy_bytes;
           }
        }
    }

    //
    // Check "Tegridy"
    //
    // if(*(volatile uint32_t*)0x40020010 != 0x414D4251)
    // {
    //     // you're stoned...
    //     return 84;
    // }

	/* Signalling the mode in the first byte */
	//data--;
	data[0] = gen_toc(max_data_bytes);

	st->rangeFinal = enc.rng ^ redundant_rng;

#ifdef ENABLE_DTX
	/* DTX decision */
#ifndef DISABLE_FLOAT_API
	if (st->use_dtx && (analysis_info.valid || is_silence))
	{
		if (decide_dtx_mode(analysis_info.activity_probability, &st->nb_no_activity_frames,
			st->peak_signal_energy, pcm, frame_size, st->channels, is_silence, st->arch))
		{
			st->rangeFinal = 0;
			data[0] = gen_toc(st->mode, st->Fs / frame_size, curr_bandwidth, st->stream_channels);
			RESTORE_STACK;
			return 1;
		}
	}
#endif
#endif

        int header_sz = 0;
        if(is_add_header) {
        int *pout = (int*)p_out_data;
        pout[0] = swap_uint32(nb_compr_bytes + 1);
        pout[1] = swap_uint32(st->rangeFinal);
        header_sz = 8;
         // ret += 8;
    }



	/* Count ToC and redundancy */
	ret += 1 + redundancy_bytes;
	if (1) // !st->use_vbr)
	{
		if (opus_packet_pad(data, ret, max_data_bytes) != OPUS_OK)
		{
			RESTORE_STACK;
			return OPUS_INTERNAL_ERROR;
		}
		ret = max_data_bytes;
	}
	RESTORE_STACK;
	return ret + header_sz;
}

#endif
