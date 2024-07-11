#include "ns_malloc.h"
#include "unity/unity.h"
#include "ns_core.h"

#define configTOTAL_HEAP_SIZE NS_MALLOC_HEAP_SIZE_IN_K * 1024
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024] __attribute__ ((aligned (4)));
size_t const ucHeapSize = configTOTAL_HEAP_SIZE;
void ns_malloc_tests_pre_test_hook() {
    // pre hook if needed
}
void ns_malloc_tests_post_test_hook() {
    // post hook if needed
}

void ns_malloc_test_basic_allocation() {
    char *char_ptr = ns_malloc(sizeof(char));
    int *int_ptr = ns_malloc(sizeof(int));
    double *double_ptr = ns_malloc(sizeof(double));
    float *float_ptr = ns_malloc(sizeof(float));
    TEST_ASSERT_NOT_NULL(char_ptr);
    TEST_ASSERT_NOT_NULL(int_ptr);
    TEST_ASSERT_NOT_NULL(double_ptr);
    TEST_ASSERT_NOT_NULL(float_ptr);
    ns_free(char_ptr);
    ns_free(int_ptr);
    ns_free(double_ptr);
    ns_free(float_ptr);
}

// Allocating largest unsigned int value
void ns_malloc_test_past_max_size() {
    char *large_double = ns_malloc(32745);
    TEST_ASSERT_NULL(large_double);
    ns_free(large_double);
}

// Allocating 0 bytes should return NULL
void ns_malloc_test_allocate_zero() {
    double *zero_double = ns_malloc(0);
    TEST_ASSERT_NULL(zero_double);
}

void ns_malloc_test_no_overlap_in_heap_allocations() {
    // Allocate large chunks of memory for different types
    char *char_ptr = ns_malloc(50 * sizeof(char));
    int *int_ptr = ns_malloc(sizeof(int));
    double *double_ptr = ns_malloc(100 * sizeof(double));
    float *float_ptr = ns_malloc(50 * sizeof(float));
    
    // Get uintptr_t addresses of allocated pointers
    uintptr_t char_start = (uintptr_t)char_ptr;
    uintptr_t char_end = char_start + sizeof(char);
    
    uintptr_t int_start = (uintptr_t)int_ptr;
    uintptr_t int_end = int_start + sizeof(int);
    
    uintptr_t double_start = (uintptr_t)double_ptr;
    uintptr_t double_end = double_start + sizeof(double);
    
    uintptr_t float_start = (uintptr_t)float_ptr;
    uintptr_t float_end = float_start + sizeof(float);
    
    // Check for no overlap
    TEST_ASSERT_TRUE(char_end < int_start || char_start > int_end);
    TEST_ASSERT_TRUE(char_end < double_start || char_start > double_end);
    TEST_ASSERT_TRUE(char_end < float_start || char_start > float_end);

    TEST_ASSERT_TRUE(int_end < char_start || int_start > char_end);
    TEST_ASSERT_TRUE(int_end < double_start || int_start > double_end);
    TEST_ASSERT_TRUE(int_end < float_start || int_start > float_end);

    TEST_ASSERT_TRUE(double_end < int_start || double_start > int_end);
    TEST_ASSERT_TRUE(double_end < char_start || double_start > char_end);
    TEST_ASSERT_TRUE(double_end < float_start || double_start > float_end);

    TEST_ASSERT_TRUE(float_end < int_start || float_start > int_end);
    TEST_ASSERT_TRUE(float_end < char_start || float_start > char_end);
    TEST_ASSERT_TRUE(float_end < double_start || float_start > double_end);


    // Free allocated memory
    ns_free(char_ptr);
    ns_free(int_ptr);
    ns_free(double_ptr);
    ns_free(float_ptr);
}


