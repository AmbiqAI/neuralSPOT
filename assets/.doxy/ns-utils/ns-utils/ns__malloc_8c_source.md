

# File ns\_malloc.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_malloc.c**](ns__malloc_8c.md)

[Go to the documentation of this file](ns__malloc_8c.md)


```C++

#include "ns_malloc.h"
#include "ns_ambiqsuite_harness.h"

// uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024];

uint8_t ns_malloc_init() { return 0; };

int total_allocated = 0;
void *ns_malloc(size_t size) {
    void *ptr = NULL;

    if (size > 0) {
        ptr = pvTasklessPortMalloc(size);
    } // else NULL if there was an error
    total_allocated += size;
    // ns_lp_printf("ns_malloc(%d) total %d returning 0x%x\n", size, total_allocated, ptr);
    return ptr;
}

void ns_free(void *ptr) {
    if (ptr) {
        vTasklessPortFree(ptr);
    }
    // ns_lp_printf("ns_free(0x%x)\n", ptr);
}
```


