#include "frame.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Test for invalid outputs and init state
void test_1() {

    uint64_t res1 = allocate_frame(-5);
    printf("test_1a allocate_frame(-5): expected -1, got %lld\n", res1);
    assert(res1 == -1);

    uint64_t res2 = allocate_frame(0);
    printf("test_1b allocate_frame(0): expected -1, got %lld", res1);
    assert(res2 == -1);
}

// Test for one frame allocation
void test_2() {
    uint64_t res1 = allocate_frame(1);
    printf("test_2a allocate_frame(1): expected 0, got %lld\n", res1);
    assert(0 == res1);
    
}

// Test for one frame allocation, multiple times
void test_3() {
    for (int x = 0; x < 200; x++){
        uint64_t res1 = allocate_frame(1);
        printf("test_3a allocate_frame(1): expected %d, got %lld\n",(x), res1);
        assert(res1 == (x));
    }
}

// Test for one frame deallocation
void test_4() {
    uint64_t res1 = allocate_frame(1);
    uint64_t res2 = deallocate_frame(0, 1);
    printf("test_4a deallocate_frame(1,1): expected 1, got %lld\n", res2);
    assert(1 == res2);
}

// Test for multiple frame deallocation
void test_5() {
    uint64_t res1 = allocate_frame(50);
    
    uint64_t res2 = deallocate_frame(0, 1);
    uint64_t res3 = deallocate_frame(5, 10);
    uint64_t res4 = deallocate_frame(24, 2);
    printf("test_5a multuple deallocations: expected %d, got: %lld\n", 2, res4);
    assert(res2 == 1);
    assert(res3 == 10);
    assert(res4 == 2);
    
}

// Test stack allocation
void test_6() {
    allocate_frame(50);
    deallocate_frame(3, 1);
    uint64_t res1 = allocate_frame(1);
    printf("test_2a allocate_frame(1): expected 3, got %lld\n", res1);
    assert(res1 == 3);
}

// Test stack allocation multiple times
void test_7() {
    allocate_frame(50);
    deallocate_frame(3, 1);
    uint64_t res1 = allocate_frame(1);
    allocate_frame(1);
    allocate_frame(1);
    allocate_frame(1);
    deallocate_frame(5, 20);
    allocate_frame(1);
    allocate_frame(1);
    allocate_frame(1);
}

// Testing max frame that can be allocated
void test_8() {
    uint64_t res1 = allocate_frame(180);
    printf("test_5a multiple deallocations got: %lld\n", res1);
}

int main() {
    frame_init();
    test_3();
}