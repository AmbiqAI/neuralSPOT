/* Copyright (c) 2002-2008 Jean-Marc Valin
   Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/**
   @file mathops.h
   @brief Various math functions
*/
/*
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

#ifndef MATHOPS_H
#define MATHOPS_H

#include "arch.h"
#include "entcode.h"
#include "os_support.h"

#define PI 3.141592653f

/* Multiplies two 16-bit fractional values. Bit-exactness of this macro is important */
#define FRAC_MUL16(a,b) ((16384+((opus_int32)(opus_int16)(a)*(opus_int16)(b)))>>15)

unsigned isqrt32(opus_uint32 _val);

/* CELT doesn't need it for fixed-point, by analysis.c does. */
#if !defined(FIXED_POINT) || defined(ANALYSIS_C)
#define cA 0.43157974f
#define cB 0.67848403f
#define cC 0.08595542f
#define cE ((float)PI/2)
static OPUS_INLINE float fast_atan2f(float y, float x) {
   float x2, y2;
   x2 = x*x;
   y2 = y*y;
   /* For very small values, we don't care about the answer, so
      we can just return 0. */
   if (x2 + y2 < 1e-18f)
   {
      return 0;
   }
   if(x2<y2){
      float den = (y2 + cB*x2) * (y2 + cC*x2);
      return -x*y*(y2 + cA*x2) / den + (y<0 ? -cE : cE);
   }else{
      float den = (x2 + cB*y2) * (x2 + cC*y2);
      return  x*y*(x2 + cA*y2) / den + (y<0 ? -cE : cE) - (x*y<0 ? -cE : cE);
   }
}
#undef cA
#undef cB
#undef cC
#undef cE
#endif


#ifndef OVERRIDE_CELT_MAXABS16
static OPUS_INLINE opus_val32 celt_maxabs16(const opus_val16 *x, int len)
{
   int i;
   opus_val16 maxval = 0;
   opus_val16 minval = 0;
   for (i=0;i<len;i++)
   {
      maxval = MAX16(maxval, x[i]);
      minval = MIN16(minval, x[i]);
   }
   return MAX32(EXTEND32(maxval),-EXTEND32(minval));
}
#endif

#ifndef OVERRIDE_CELT_MAXABS32
#ifdef FIXED_POINT
static OPUS_INLINE opus_val32 celt_maxabs32(const opus_val32 *x, int len)
{
   int i;
   opus_val32 maxval = 0;
   opus_val32 minval = 0;
   for (i=0;i<len;i++)
   {
      maxval = MAX32(maxval, x[i]);
      minval = MIN32(minval, x[i]);
   }
   return MAX32(maxval, -minval);
}
#else
#define celt_maxabs32(x,len) celt_maxabs16(x,len)
#endif
#endif


#ifndef FIXED_POINT
#include <math.h>
#define FLOOR(x)     ((x) < 0) ? (int)(x-1):(int)(x)   //(int)floor(x)
#define COS(x)  cos(x)
#define SQRT(x) sqrt(x)
//#define EXP(x)  exp(x)
#define FABS(x) spl_fabs(x)
//#define LOG(x)  log(x)
#define ABS16(x)  (x) < 0 ? (-(x)) : (x)
#define ABS32(x)  spl_abs(x) // < 0 ? (-(x)) : (x)
#ifndef DIS_SPL_ASM_1

#ifdef keil
static inline int spl_udiv(opus_uint32 n, opus_uint32 d)
{
  int res;
    __asm{
        udiv    res, n, d
    }
return res;
}
#else
static inline int spl_udiv(opus_uint32 n, opus_uint32 d)
{
  int res;
__asm("udiv     %0, %1, %2" : "=r"(res) : "r"(n) , "r"(d));
return res;
}
#endif

#ifdef keil

static inline int spl_sdiv(opus_int32 n, opus_int32 d)
{
  int res;
	__asm{
		sdiv    res, n, d
	}
return res;
}
#else
static inline int spl_sdiv(opus_int32 n, opus_int32 d)
{
  int res;
__asm("sdiv     %0, %1, %2" : "=r"(res) : "r"(n) , "r"(d));
return res;
}
#endif
#ifdef keil
static inline int ec_ilog(opus_uint32 _v){
   int res;	
  __asm{
    clz     res, _v
    }
	
  __asm{
    rsb     res, res, #32
    }

return res; 
}
#else
static inline int ec_ilog(opus_uint32 _v){
   int res;
__asm("clz     %0, %1" : "=r"(res) : "r"(_v));
__asm("rsb     %0, %1, #32" : "=r"(res) : "r"(res));

return res;
}
#endif
// static inline int ec_ilog_tst(opus_uint32 _v){
//    int res = ec_ilog(_v);
//    int t = ec_ilog_c(_v);

//    if(t != res)
//    {
//      /* printf("%d %d ", t, res); */
//    }

// return res;
// }
#else
static inline int spl_udiv(opus_uint32 n, opus_uint32 d)
{
	int res = n/d;
	return res;
}
static inline int spl_sdiv(opus_int32 n, opus_int32 d)
{
	int res = n/d;
	return res;
}
#endif
//static OPUS_INLINE opus_uint32 celt_udiv(opus_uint32 n, opus_uint32 d) {


#define celt_sqrt(x) ((float)sqrt(x))
#define celt_rsqrt(x) (1.f/celt_sqrt(x))
#define celt_rsqrt_norm(x) (celt_rsqrt(x))
#define celt_cos_norm(x) ((float)cos((.5f*PI)*(x)))
#define celt_rcp(x) (1.f/(x))
#define celt_div(a,b) ((a)/(b))
#define celt_udiv(a,b) ((a)/(b))
#define frac_div32(a,b) ((float)(a)/(b))
#define opus_uint32(a,b) ((a)/(b))

#ifdef keil
static inline int spl_abs(int x)
{
int res;
#ifndef DIS_SPL_ASM_1
int tmp;
//__asm("eor     %0, %1, %2, asr #31" : "=r"(tmp) : "r"(x) , "r"(x));
//__asm("sub     %0, %1, %2, asr #31" : "=r"(res) : "r"(tmp) , "r"(x));
	
        __asm{
            eor     tmp, x, x, asr #31
        }
        __asm{
            sub     res, tmp, x, asr #31
        }
//  eor     r1, r0, r0, asr #31
//   sub     r0, r1, r0, asr #31
#else
	res = abs(x);
#endif
return res;
}
#else
static inline int spl_abs(int x)
{
int res;
#ifndef DIS_SPL_ASM_1
int tmp;
__asm("eor     %0, %1, %2, asr #31" : "=r"(tmp) : "r"(x) , "r"(x));

__asm("sub     %0, %1, %2, asr #31" : "=r"(res) : "r"(tmp) , "r"(x));
//  eor     r1, r0, r0, asr #31
//   sub     r0, r1, r0, asr #31
#else
	res = abs(x);
#endif
return res;
}
#endif

#ifdef keil
static inline float spl_fabs(float x)
{
float res;
#ifndef DIS_SPL_ASM_1
    __asm
        {
            vabs.f32 res, x
        }	
#else
res = (float)fabs(x);
#endif
return res;
}
#else
static inline float spl_fabs(float x)
{
float res;
#ifndef DIS_SPL_ASM_1
__asm("vabs.f32 %0, %1" : "=t"(res) : "t"(x));
#else
res = (float)fabs(x);
#endif
return res;
}
#endif

#ifndef keil
static inline int spl_fooru(float x)
{
int res;
#ifndef DIS_SPL_ASM_1
//int tmp;
//__asm("eor     %0, %1, %2, asr #31" : "=r"(tmp) : "r"(x) , "r"(x));

__asm("ftosis      %0, %1" : "=t"(res) : "t"(x) );
#else
res = (int)floor(x);
#endif
return res;
}

static inline float spl_fsqrt(float x)
{
float res;
#ifndef DIS_SPL_ASM_1
__asm("vsqrt.f32 %0, %1" : "=t"(res) : "t"(x));
#else
res = (float)sqrt(x);
#endif
return res;
}
#endif

#ifndef DIS_SPL_INLINE
#ifdef keil
#define inline __attribute__(( always_inline))
#else
#pragma inline=forced
#endif
#endif
static float spl_cos(float x)
{
  float ret = 0;
#define     COS_c1 ( 0.99999999999999806767f   )
#define     COS_c2 (-0.4999999999998996568f    )
#define     COS_c3 ( 0.04166666666581174292f   )
#define     COS_c4 (-0.001388888886113613522f  )
#define     COS_c5 ( 0.000024801582876042427f  )
#define     COS_c6 (-0.0000002755693576863181f )
#define     COS_c7 ( 0.0000000020858327958707f )
#define     COS_c8 (-0.000000000011080716368f  )

  float x2 = x*x;
  ret = COS_c1 + x2*(COS_c2 + x2*(COS_c3 + x2*(COS_c4 + x2*(COS_c5 +
                            x2*(COS_c6 + x2*(COS_c7 + x2*COS_c8))))));

  return ret;

}
/** Base-2 log approximation (log2(x)). */
static OPUS_INLINE float celt_log2(float x)
{
   int integer;
   float frac;
   union {
      float f;
      opus_uint32 i;
   } in;
   in.f = x;
   integer = (in.i>>23)-127;
   in.i -= integer<<23;
   frac = in.f - 1.5f;
   frac = -0.41445418f + frac*(0.95909232f
          + frac*(-0.33951290f + frac*0.16541097f));
   return 1+integer+frac;
}

/** Base-2 exponential approximation (2^x). */
static OPUS_INLINE float celt_exp2(float x)
{
   int integer;
   float frac;
   union {
      float f;
      opus_uint32 i;
   } res;
   integer = FLOOR(x);
   if (integer < -50)
      return 0;
   frac = x-integer;
   /* K0 = 1, K1 = log(2), K2 = 3-4*log(2), K3 = 3*log(2) - 2 */
   res.f = 0.99992522f + frac * (0.69583354f
           + frac * (0.22606716f + 0.078024523f*frac));
   res.i = (res.i + (integer<<23)) & 0x7fffffff;
   return res.f;
}

#define ABS16(x)  (x) < 0 ? (-(x)) : (x)
#endif
#endif /* MATHOPS_H */
