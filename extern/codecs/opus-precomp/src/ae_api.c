//*****************************************************************************
//
//! @file ae_api.c
//!
//! @brief OPUS codec APIs
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision ambiqvos-754956a890 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdio.h>
#include "opusenc.h"
#define OPUS_MEM_RAM1_SIZE (20*1024)

#if defined (keil)
char opus_mem_ram1[OPUS_MEM_RAM1_SIZE] __attribute__((aligned));
#else
char opus_mem_ram1[OPUS_MEM_RAM1_SIZE];
#endif
 int    opus_ram_ptr;
 int    opus_ram_ptr_save = 0;   
 int    opus_ram_count = 0;   
 int    opus_ram_count_save = 0;   

int spl_stack_init(int option)
{
  int ret  = 0;
  switch(option)
  {
  case 0:
     opus_ram_ptr = (int)&opus_mem_ram1[0];
     opus_ram_ptr_save = 0;   
     opus_ram_count = 0;   
     opus_ram_count_save = 0;   
    
    ret = OPUS_MEM_RAM1_SIZE;
    break;
  default:
    break;
  }
  return ret;
}
int spl_stack_check()
{
  
  return opus_ram_count;
}
void* alloca(int size)
{
  void* p = (void*)opus_ram_ptr;
  int size1 = ((size>>2)<<2) + 4;
  int t = (int)p;
  if(t&3==0)
    t = size1 ;
  if(size1&3)
  {
    size1= size;
  }
   opus_ram_ptr+= size1;
   opus_ram_count += size1;
   if(opus_ram_count > OPUS_MEM_RAM1_SIZE)
   {
     //am_app_utils_stdio_printf("ERROR : stack allocation failed %d max = %d\n", opus_ram_count, OPUS_MEM_RAM1_SIZE);
     return 0;
   }
  return p;
}
void* spl_free(int size)
{
  void* p = (void*)opus_ram_ptr;
  int size1 = ((size>>2)<<2) + 4;
    if(size1&3)
    {
      size1= size;
    }

   opus_ram_ptr-= size1;
   opus_ram_count -= size1;
   if(opus_ram_count <=0)
   {
     //am_app_utils_stdio_printf("ERROR : free allocation failed %d max = %d\n", opus_ram_count, OPUS_MEM_RAM1_SIZE);
     return 0;
   }
  return p;
}
int save_stack(void)
{
  opus_ram_ptr_save = opus_ram_ptr;
  opus_ram_count_save = opus_ram_count;
  return opus_ram_ptr;
}
int restore_stack(void)
{
  opus_ram_ptr = opus_ram_ptr_save;
  opus_ram_count = opus_ram_count_save;
  return opus_ram_ptr; 
}
int alloc_stack()
{
  return 0;
}
int spl_memcpy(void* dst, void* src, int n)
{
  for(int i = 0; i<n; i++)
  {
    ((char*)dst)[i] = ((char*)src)[i];
  }
  return 0;
}
#ifndef WIN32
//#define MEASURE_MIPS
#endif
#ifdef MEASURE_MIPS
volatile unsigned int *DWT_CYCCNT  ;
volatile unsigned int *DWT_CONTROL ;
volatile unsigned int *SCB_DEMCR   ;

void reset_timer(){
    DWT_CYCCNT   = (unsigned int volatile *)0xE0001004; //address of the register
    DWT_CONTROL  = (unsigned int volatile *)0xE0001000; //address of the register
    SCB_DEMCR    = (unsigned int volatile *)0xE000EDFC; //address of the register
    *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT  = 0; // reset the counter
    *DWT_CONTROL = 0; 
}

void start_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

void stop_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 0 ; // disable the counter    
}

unsigned int getCycles(){
    return *DWT_CYCCNT;
}
#define SAMPLING_RATE     (16000)
#define MIPS_DURATION_SEC (1)
#define FRAME_RATE        (320)
#define MIPS_BUFFER_LEN       (MIPS_DURATION_SEC*SAMPLING_RATE)/FRAME_RATE

struct t_mips_info
{
  int mips_mips[MIPS_BUFFER_LEN];
  int ptr_w;
  int ptr_r;
  int len;
  float ave;
  int min;
  int max;
  int stackmin;
  int stackmax;
} o_mips_info = {
  .ptr_w = 0,
  .ptr_r = 0,
  .len = MIPS_BUFFER_LEN,
  .ave = 0.0,
  .min = 100000000,
  .max = 0,
  .stackmin = 10000000,
  .stackmax = 0
};
float mips_update()
{
  int c = getCycles();
  o_mips_info.mips_mips[o_mips_info.ptr_w++] = c;
  if(c < o_mips_info.min)
  {
    o_mips_info.min = c;
  }
  else if(c > o_mips_info.max)
  {
    o_mips_info.max = c;
  }
  if(o_mips_info.ptr_w == o_mips_info.len)
  {
    float sum = 0;
    for(int i = 0; i < o_mips_info.len; i++)
    {
      sum += o_mips_info.mips_mips[i];
    }
    o_mips_info.ave = sum/o_mips_info.len;
    o_mips_info.ptr_w = 0;
    o_mips_info.min = 100000000;
    o_mips_info.max = 0;
    //am_app_utils_stdio_printf("mips = %f, stack = (%d-%d)\n", o_mips_info.ave*16000.0f/(320.0f*1000000.0f), o_mips_info.stackmin, o_mips_info.stackmax);
    o_mips_info.stackmin = 100000000;
    o_mips_info.stackmax = 0;
  }
   int m = spl_stack_check();
    if(m < o_mips_info.stackmin)
  {
    o_mips_info.stackmin = m;
  }
  else if(m > o_mips_info.stackmax)
  {
    o_mips_info.stackmax = m;
  }
  
  return o_mips_info.ave;
}

#endif /* MEASURE_MIPS */
spl_opus_encoder_h *p_spl_opus_encoder;

int audio_enc_init(int option)
{
  p_spl_opus_encoder = octopus_encoder_create(option);
  return 0;
}

int audio_enc_encode_frame(short *p_pcm_buffer, int n_pcm_samples, unsigned char *p_encoded_buffer)
{
  int n = 0;
  int size = 1500; //todo
      spl_stack_init(0);
#ifdef MEASURE_MIPS  
  reset_timer(); //reset timer
  start_timer(); //reset timer
#endif
      n = octopus_encode(p_spl_opus_encoder, p_pcm_buffer, n_pcm_samples, p_encoded_buffer, size);
#ifdef MEASURE_MIPS  
  stop_timer(); //reset timer
  mips_update(); //read number of cycles 
#endif 
      spl_stack_check();
      //printf("   stack usage = %d\n", m);
      
      return n;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
