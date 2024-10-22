

# File ns\_peripherals\_power.h

[**File List**](files.md) **>** [**includes-api**](dir_fe04d02a8bfca0247bd216a75646089c.md) **>** [**ns\_peripherals\_power.h**](ns__peripherals__power_8h.md)

[Go to the documentation of this file](ns__peripherals__power_8h.md)

```C++


//*****************************************************************************
//
// Copyright (c) 2021, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef NS_POWER
    #define NS_POWER

    #ifdef __cplusplus
extern "C" {
    #endif
    #include "ns_core.h"
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"

    #define NS_POWER_V0_0_1                                                                        \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_POWER_V1_0_0                                                                        \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_POWER_OLDEST_SUPPORTED_VERSION NS_POWER_V0_0_1
    #define NS_POWER_CURRENT_VERSION NS_POWER_V1_0_0
    #define NS_POWER_API_ID 0xCA0007

extern const ns_core_api_t ns_power_V0_0_1;
extern const ns_core_api_t ns_power_V1_0_0;
extern const ns_core_api_t ns_power_oldest_supported_version;
extern const ns_core_api_t ns_power_current_version;

typedef enum {
    NS_MINIMUM_PERF = 0, 
    NS_MEDIUM_PERF = 1,  
    NS_MAXIMUM_PERF = 2  
} ns_power_mode_e;

typedef struct {
    const ns_core_api_t *api;     
    ns_power_mode_e eAIPowerMode; 
    bool bNeedAudAdc;             
    bool bNeedSharedSRAM;         
    bool bNeedCrypto;             
    bool bNeedBluetooth;          
    bool bNeedUSB;                
    bool bNeedIOM;                
    bool bNeedAlternativeUART;    
    bool b128kTCM;                
    bool bEnableTempCo;           
    bool bNeedITM;                
    bool bNeedXtal;               
} ns_power_config_t;

extern const ns_power_config_t ns_development_default; 
extern const ns_power_config_t ns_debug_default;       
extern const ns_power_config_t ns_good_default;  
extern const ns_power_config_t ns_mlperf_mode1;  
extern const ns_power_config_t ns_mlperf_mode2;  
extern const ns_power_config_t ns_mlperf_mode3;  
extern const ns_power_config_t ns_audio_default; 

extern uint32_t ns_power_config(const ns_power_config_t *);

extern void ns_deep_sleep(void);

uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode);

    #ifdef __cplusplus
}
    #endif
#endif // NS_POWER

```

