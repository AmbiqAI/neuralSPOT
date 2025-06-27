

# File ns\_peripherals\_psram.h

[**File List**](files.md) **>** [**includes-api**](dir_fe04d02a8bfca0247bd216a75646089c.md) **>** [**ns\_peripherals\_psram.h**](ns__peripherals__psram_8h.md)

[Go to the documentation of this file](ns__peripherals__psram_8h.md)


```C++

#ifndef NS_PERIPHERAL_PSRAM
#define NS_PERIPHERAL_PSRAM

#ifdef __cplusplus
extern "C" {
#endif
#include "ns_core.h"

#define NS_PSRAM_V0_0_1                                                                           \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_PSRAM_V1_0_0                                                                           \
    { .major = 1, .minor = 0, .revision = 0 }
#define NS_PSRAM_OLDEST_SUPPORTED_VERSION NS_PSRAM_V0_0_1
#define NS_PSRAM_CURRENT_VERSION NS_PSRAM_V1_0_0

extern const ns_core_api_t ns_psram_V0_0_1;
// extern const ns_core_api_t ns_psram_V1_0_0;
extern const ns_core_api_t ns_psram_oldest_supported_version;
extern const ns_core_api_t ns_psram_current_version;
#define NS_PSRAM_API_ID 0xCA000A

typedef enum {
    PSRAM_TYPE_HEX = 0,
    PSRAM_TYPE_OCT = 1,
} psram_type_e;

typedef struct {
    ns_core_api_t const *api;              
    bool psram_enable;                     
    psram_type_e psram_type;               
    uint8_t psram_block;                   
    uint32_t psram_size;                   
    uint32_t psram_base_address;           
} ns_psram_config_t;

extern uint32_t ns_psram_init(ns_psram_config_t *);

#ifdef __cplusplus
}
#endif

#endif
```


