// tst_octopus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
typedef void* spl_opus_encoder_h;
spl_opus_encoder_h* octopus_encoder_create();
int octopus_encode(spl_opus_encoder_h *st, const short *pcm, int frame_size, unsigned char *data, int out_data_bytes);
#if 0
#define OPUS_MEM_RAM1_SIZE (140*1024)
char opus_mem_ram1[OPUS_MEM_RAM1_SIZE];
int opus_ram_ptr = (int)&opus_mem_ram1[0];

void* alloca(int size)
{
  void* p = (void*)opus_ram_ptr;

   opus_ram_ptr+=size;

  return p;
}
#endif
#if 0
spl_opus_encoder_h *p_spl_opus_encoder;
short pcm_buffer[12*320];
char out_buffer[1000];
int size = 320 / 8;
int oe_init()
{
  p_spl_opus_encoder = octopus_encoder_create();
  return 0;
}

int oe_encode_frame(short *p_pcm_buffer, int n_pcm_samples, char *p_encoded_buffer)
{
	int n = octopus_encode(p_spl_opus_encoder, p_pcm_buffer, n_pcm_samples, (unsigned char*)p_encoded_buffer, size);
        return n;
}
#endif
#define FILEIO
short in[320];
char out_bistream[100];
int frame_count = 0;
int tot_samples = 0;
#ifdef FILEIO
char *inFile, *outFile;
FILE *fout = NULL;
FILE *fin = NULL;
unsigned char fbytes[10*1024];
int max_frame_size = 1200;

int init_samples_io(char *fin_name, char* fout_name)
{
	outFile = fout_name;// argv[argc - 1];
	fout = fopen(outFile, "wb+");
	if (!fout)
	{
#ifdef WIN32
		fprintf(stderr, "Could not open output file %s\n", outFile);
#endif
		return -10;
	}

	inFile = fin_name; //argv[argc-2];
	fin = fopen(inFile, "rb");
	if (!fin)
	{
#ifdef WIN32
		fprintf(stderr, "Could not open input file %s\n", inFile);
#endif
		return -12;
	}
	char hdr[44];

	int num_read = fread(hdr, 1, 44, fin); // skip header

	return 0;
}
int close_samples_io(void)
{
	if (!fout)
	{
		fclose(fout);
	}
	if (!fin)
	{
		fclose(fin);
	}
	return 0;
}

int get_pcm_samples(short *p_pcm, int n)
{
	if (!fin)
	{
		return -20;
	}

	int channels = 1;
	int num_read = fread(fbytes, sizeof(short)*channels, n, fin);
	if (num_read != n)
	{
		return -1;
	}
	for (int i = 0; i < n; i++)
	{
		int s;
		s = fbytes[2 * i + 1] << 8 | fbytes[2 * i];
		s = ((s & 0xFFFF) ^ 0x8000) - 0x8000;
		p_pcm[i] = s;
	}
	return num_read;
}

int put_bitstream(char *p_stream, int n, int rng)
{
	int hdr_size = 8;
	if (!fout)
	{
		return -10;
	}

//	fwrite((char*)&n, 4, 1, fout);
//	fwrite((char*)&rng, 4, 1, fout);

	if (fwrite(&p_stream[0], 1, n, fout) != (unsigned)n) {
#ifdef WIN32
		fprintf(stderr, "Error writing.\n");
#endif
		return -10;
	}

	return 0;
}
#else
const
#include "../../vectors/fre07_clip1_c.h"  // capture_usp0905_short
//#include "../../vectors/capture_usp0905_short.h"  //
char out_sample[20000];
int in_sample_count = 0;
int out_sample_count = 0;

int init_samples_io(void)
{
	in_sample_count = 0;
	out_sample_count = 0;
	return 0;
}
int close_samples_io(void)
{
#ifdef WIN32
	FILE *fout = fopen("out.opus", "w+b");
	if (fwrite(out_sample, 1, out_sample_count, fout) != (unsigned)out_sample_count) {
		fprintf(stderr, "Error writing.\n");
		return -10;
	}
	else
	{
		printf("writting %d bytes to output file", out_sample_count);
	}
	fclose(fout);
#endif
	return 0;
}
int get_pcm_samples(short *p_pcm, int n)
{
	if (in_sample_count + n > sizeof(in_sample) / sizeof(short))
		return -10;

	for (int i = 0; i < n; i++)
	{
		p_pcm[i] = in_sample[in_sample_count++];
	}
	return 0;
}
int put_bitstream(char *p_stream, int n, int rng)
{
	int hdr = 8;
	if (out_sample_count + n + hdr > sizeof(out_sample) / sizeof(char))
		return -10;
#if 0
	char *p = (char*)&n;
	for (int i = 0; i < 4; i++)
		out_sample[out_sample_count++] = p[3 - i];

	p = (char*)&rng;
	for (int i = 0; i < 4; i++)
		out_sample[out_sample_count++] = p[3 - i];
#endif
	for (int i = 0; i < n; i++)
	{
		out_sample[out_sample_count++] = p_stream[i];
	}
	return 0;
}


#endif
#include "ae_api.h"
#ifdef FILEIO
int main(int argc, char **argv)
#else
void main(void)
#endif
{
#ifdef FILEIO
	if (argc == 3)
	{
		init_samples_io(argv[1], argv[2]);
	}
	else
	{
		printf("ERROR : error in argument.\n    usage tst_octopus.exe <infile.pcm> <outfile.pcm>");
		return -1;
	}
#else
	init_samples_io();
#endif
	audio_enc_init(1); // with hdr

	printf("testing opus\n");

	while (1)
	{
		int ret = 0;
		int rng = 0;
		int frame_size = 320;
		ret = get_pcm_samples(in, frame_size);
		if (ret < 0)
		{
#if 1//def WIN32
			printf("   EOF : %d\n", ret);
#endif
			break;
		}

		int output_len = audio_enc_encode_frame(in, frame_size, (unsigned char*)out_bistream);
		ret = put_bitstream((char*)&out_bistream[0], output_len, rng);
		if (ret < 0)
		{
#if 1//def WIN32
			printf("   output buff full : %d\n", ret);
#endif
			break;
		}
#ifdef WIN32
	//	printf("%d ", frame_count++);
#endif
		// prn_encoded_buffer(data[toggle], len[toggle], len[toggle] + 8, enc_final_range[toggle]);
		tot_samples += output_len;
	}
	close_samples_io();
}