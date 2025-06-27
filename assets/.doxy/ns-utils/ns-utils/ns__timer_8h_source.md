

# File ns\_timer.h

[**File List**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_timer.h**](ns__timer_8h.md)

[Go to the documentation of this file](ns__timer_8h.md)


```C++

#ifndef NS_TIMER
    #define NS_TIMER

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_core.h"

    #define NS_TIMER_V0_0_1                                                                        \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_TIMER_V1_0_0                                                                        \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_TIMER_OLDEST_SUPPORTED_VERSION NS_TIMER_V0_0_1
    #define NS_TIMER_CURRENT_VERSION NS_TIMER_V1_0_0
    #define NS_TIMER_API_ID 0xCA0002

extern const ns_core_api_t ns_timer_V0_0_1;
extern const ns_core_api_t ns_timer_V1_0_0;
extern const ns_core_api_t ns_timer_oldest_supported_version;
extern const ns_core_api_t ns_timer_current_version;

struct ns_timer_config;
typedef void (*ns_timer_callback_cb)(struct ns_timer_config *);

typedef enum {
    NS_TIMER_COUNTER = 0,   
    NS_TIMER_INTERRUPT = 1, 
    NS_TIMER_USB = 3,       
    NS_TIMER_TEMPCO = 4     
} ns_timers_e;

typedef struct ns_timer_config {
    const ns_core_api_t *api;      
    ns_timers_e timer;             
    bool enableInterrupt;          
    uint32_t periodInMicroseconds; 
    ns_timer_callback_cb callback; 
} ns_timer_config_t;

extern uint32_t ns_timer_init(ns_timer_config_t *cfg);

extern uint32_t ns_us_ticker_read(ns_timer_config_t *cfg);

extern uint32_t ns_timer_clear(ns_timer_config_t *cfg);

    #ifdef __cplusplus
}
    #endif
#endif
```


