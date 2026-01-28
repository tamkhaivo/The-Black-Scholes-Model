#include "heap_allocator.h"
#include <iostream>
#include <cassert>
#include <vector>

void test_basic_allocation() {
    std::cout << "[Test] Basic Allocation... ";
    Memory::HeapAllocator allocator(1024);
    
    int* i = allocator.allocate_type<int>();
    *i = 42;
    assert(*i == 42);
    
    double* d = allocator.allocate_type<double>();
    *d = 3.14159;
    assert(*d == 3.14159);
    
    std::cout << "Passed." << std::endl;
}

void test_alignment_correctness() {
    std::cout << "[Test] Alignment Correctness... ";
    Memory::HeapAllocator allocator(1024);
    
    // Allocate 1 byte to misalign
    (void)allocator.allocate(1, 1);
    
    // Request 32-byte alignment
    struct alignas(32) LargeAlign { char data[32]; };
    LargeAlign* ptr = allocator.allocate_type<LargeAlign>();
    
    assert(reinterpret_cast<uintptr_t>(ptr) % 32 == 0);
    std::cout << "Passed." << std::endl;
}

void test_overflow_safety() {
    std::cout << "[Test] Overflow Safety... ";
    Memory::HeapAllocator allocator(128); // Small buffer
    
    try {
        (void)allocator.allocate(100, 1);
        (void)allocator.allocate(50, 1); // Should fail
        assert(false && "Should have thrown bad_alloc");
    } catch (const std::bad_alloc&) {
        std::cout << "Passed (caught bad_alloc)." << std::endl;
    }
}

void test_unique_ptr_cleanup() {
    std::cout << "[Test] Cleanup Safety... ";
    {
        Memory::HeapAllocator allocator(1024);
        (void)allocator.allocate(100);
    } // Destructor runs here
    std::cout << "Passed (implicit)." << std::endl;
}

int main() {
    std::cout << "Running HeapAllocator Tests..." << std::endl;
    
    test_basic_allocation();
    test_alignment_correctness();
    test_overflow_safety();
    test_unique_ptr_cleanup();
    
    std::cout << "All HeapAllocator tests passed." << std::endl;
    return 0;
}
