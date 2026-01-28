#include "stack_allocator.h"
#include <iostream>
#include <cassert>
#include <vector>

// Helper to check alignment
bool is_aligned(void* ptr, size_t alignment) {
    return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
}

struct alignas(32) AVXStruct {
    float data[8];
};

void test_basic_allocation() {
    std::cout << "[Test] " << __func__ << "... ";
    StackAllocator::StackAllocator<1024> allocator;
    
    int* i = allocator.allocate_type<int>();
    *i = 42;
    assert(*i == 42);
    assert(is_aligned(i, alignof(int)));
    
    double* d = allocator.allocate_type<double>();
    *d = 3.14159;
    assert(*d == 3.14159);
    assert(is_aligned(d, alignof(double)));
    
    std::cout << "Passed." << std::endl;
}

void test_alignment_correctness() {
    std::cout << "[Test] " << __func__ << "... ";
    StackAllocator::StackAllocator<1024> allocator;
    
    // Allocate a byte to intentionally misalign the next available address
    allocator.allocate(1, 1);
    
    // Request highly aligned memory
    AVXStruct* ptr = allocator.allocate_type<AVXStruct>();
    assert(is_aligned(ptr, 32));
    assert(reinterpret_cast<uintptr_t>(ptr) % 32 == 0);
    
    std::cout << "Passed." << std::endl;
}

void test_overflow_safety() {
    std::cout << "[Test] " << __func__ << "... ";
    StackAllocator::StackAllocator<128> allocator;
    
    try {
        // Fill it up
        allocator.allocate(100, 1);
        // This should fail
        allocator.allocate(50, 1);
        assert(false && "Should have thrown bad_alloc");
    } catch (const std::bad_alloc&) {
        std::cout << "Passed (caught bad_alloc)." << std::endl;
    } catch (...) {
        assert(false && "Caught wrong exception type");
    }
}

int main() {
    std::cout << "Running StackAllocator Tests..." << std::endl;
    
    test_basic_allocation();
    test_alignment_correctness();
    test_overflow_safety();
    
    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
