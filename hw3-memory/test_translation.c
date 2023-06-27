#include "frame.h"
#include "translation.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Test for simple vm_map allocation
void test_1() {
    int res1 = vm_map(0,0,10,0);
    printf("Test2: Res1 value is: %d, expected 10", res1);
    assert(10 == res1);
}

// Test for 512 consecutive allocations, multiple times
void test_2() {
    int res1;
    for (int x = 0; x < 1021; x++) { // 1021 is the max. Only 1024 tables can be created
        res1 = vm_map(x*512,0,512,0);
    }
    printf("Test2: Res1 value is: %d, expected 1", res1);
    assert(1==res1);
}

// Test for unmapping
void test_3() {
    vm_map(0, 0, 10, 5);
    int res1 = vm_unmap(0,3);
    printf("Test3: Res1 value is: %d, expected 1", res1);
    assert(1==res1);
}

// Test for locating within L3 table
void test_4() {
    vm_map(0,0,3,2);
    vm_map(5,0,505,2);
    vm_map(512, 0, 10, 10);
    uint64_t res1 =  vm_locate(20);
    printf("Test5: Res1 value is: %llu, expected 521\n", res1);  
    
}

// Test for traversing up and down a level for vm_locate
void test_5() {
    // Fill up an L2 table, and force to create second L2 table
    for (int x = 0; x < 600; x++) { 
        vm_map(x*512,0,511,2);
    }

    // Move one more slot down
    vm_map(600*512,0,10,2);
    uint64_t res1 =  vm_locate(20);
    printf("Test5: Res1 value is: %llu, expected 307209\n", res1); 
    assert(res1 == 600*512+10-1);
}

// Test for unsuccessful vm_translate
void test_6() {
    uint64_t res1 = vm_translate(1000);
    printf("Test6: Res1 value is: %llu, expected UINT64_MAX\n", res1); 
    assert(res1 == UINT64_MAX);
}

// Test for successful vm_translate accross two L3 tables
void test_7() {
 
    for (int x = 0; x < 1000; x++) {
        vm_map(x, x, 1, 5);
    }

    for (int x = 0; x<1000; x++) {
        uint64_t res1 = vm_translate(x);
        assert(res1 == x);
    }
}


int main() { 
    frame_init();
    test_7();
}