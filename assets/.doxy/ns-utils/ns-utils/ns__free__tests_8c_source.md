

# File ns\_free\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**tests**](dir_62cfc4ab5fe382f357338287112ab49a.md) **>** [**ns\_free\_tests.c**](ns__free__tests_8c.md)

[Go to the documentation of this file](ns__free__tests_8c.md)

```C++

#include "ns_malloc.h"
#include "unity/unity.h"
#include "ns_core.h"
#define configTOTAL_HEAP_SIZE NS_MALLOC_HEAP_SIZE_IN_K * 1024
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024] __attribute__ ((aligned (4)));
size_t const ucHeapSize = configTOTAL_HEAP_SIZE;
void ns_free_tests_pre_test_hook() {
    // pre hook if needed
}
void ns_free_tests_post_test_hook() {
    // post hook if needed
}

// Undefined behavior
void ns_free_test_basic() {
    // Allocate memory for an array of integers
    int *ptr = ns_malloc(5 * sizeof(int));

    // Check if malloc succeeded
    TEST_ASSERT_TRUE(ptr != NULL);

    // Fill the allocated memory with some values
    for (int i = 0; i < 5; ++i) {
        ptr[i] = i * 10;
    }

    // Free allocated memory
    ns_free(ptr);
}

void ns_free_test_null_pointer() {
    // Nothing should happen when freeing a NULL pointer
    int *ptr = NULL;
    ns_free(ptr);
    TEST_ASSERT_TRUE(ptr == NULL);
}

// Undefined behavior
void ns_free_test_twice() {
    // Freeing a pointer twice should not cause any issues
    int * ptr = ns_malloc(sizeof(int));
    ns_free(ptr);
    ns_free(ptr);
    TEST_ASSERT_TRUE(ptr == NULL);
}

// Undefined behavior
void ns_free_test_non_malloced_pointer() {
    // Freeing a pointer that was not allocated with malloc should not cause any issues
    int x = 5;
    int *ptr = &x;
    ns_free(ptr);
    TEST_ASSERT_TRUE(ptr == &x);
}

void ns_free_test_memory_fragmentation() {
    void *allocs[1000];
    int i;

    // Allocate memory in a pattern that can cause fragmentation
    for (i = 0; i < 1000; ++i) {
        allocs[i] = ns_malloc(10);
        TEST_ASSERT_NOT_NULL(allocs[i]);
    }

    // Free every other allocation
    for (i = 1; i < 1000; i += 2) {
        ns_free(allocs[i]);
    }
    // Allocate a large block after inducing fragmentation
    void *large_alloc = ns_malloc(1000);
    if (large_alloc == NULL) {
        // Allocation failed due to fragmentation
        TEST_FAIL_MESSAGE("Failed to allocate a large block due to memory fragmentation.");
    } else {
        // Allocation successful
        ns_free(large_alloc);
    }

    // Free remaining allocations
    for (i = 0; i < 1000; i += 2) {
        ns_free(allocs[i]);
    }
}

```

