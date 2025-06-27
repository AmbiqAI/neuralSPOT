

# File ns\_malloc.h

[**File List**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_malloc.h**](ns__malloc_8h.md)

[Go to the documentation of this file](ns__malloc_8h.md)


```C++

#ifndef NS_MALLOC
    #define NS_MALLOC

    #ifndef NS_MALLOC_HEAP_SIZE_IN_K
        #define NS_MALLOC_HEAP_SIZE_IN_K 16
    #endif

    #ifdef __cplusplus
extern "C" {
    #endif
    #include "FreeRTOS.h"
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "portable.h"
    #include "portmacro.h"
    #include "rtos.h"

// extern alignas(4) uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024];

extern uint8_t ns_malloc_init();
extern void *ns_malloc(size_t size);
extern void ns_free(void *ptr);

    #ifdef __cplusplus
}
    #endif
#endif // end of ns-malloc
```


