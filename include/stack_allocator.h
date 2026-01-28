#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace StackAllocator {
using namespace std;
/**
 * @brief Fixed-size stack-based allocator.
 * 
 * Guarantees zero heap allocations. Storage is embedded directly within the object.
 * Not thread-safe by design (thread-local usage assumed for maximum speed).
 * 
 * @tparam SizeBytes Total size of the stack buffer in bytes.
 */
template <size_t SizeBytes> 
class StackAllocator {
public:
    // Enforce alignment to max_align_t to ensure safe storage for any standard type.
    static constexpr size_t ALIGNMENT = alignof(max_align_t);

    StackAllocator() noexcept : m_Current{0} {}

    // Disable copy/move to prevent accidental slicing or pointer invalidation
    // of the internal buffer if pointers are stored externally.
    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;
    StackAllocator(StackAllocator&&) = delete;
    StackAllocator& operator=(StackAllocator&&) = delete;

    ~StackAllocator() noexcept = default;

    /**
     * @brief Allocates aligned memory from the internal stack buffer.
     * 
     * @param size Number of bytes to allocate.
     * @param align Alignment requirement (must be a power of 2).
     * @return void* Pointer to the allocated memory.
     * @throws std::bad_alloc if stack space is exhausted.
     */
    [[nodiscard]] 
    void* allocate(size_t size, size_t align = ALIGNMENT) {        
        void* ptr = static_cast<void*>(m_Storage + m_Current);
        size_t space = SizeBytes - m_Current;
        void* aligned_ptr = align(align, size, ptr, space);

        if (aligned_ptr == nullptr) {
            throw bad_alloc(); 
        }

        size_t aligned_offset = static_cast<byte*>(aligned_ptr) - m_Storage;
        size_t new_current = aligned_offset + size;

        if (new_current > SizeBytes) {
             throw bad_alloc(); 
        }

        m_Current = new_current;
        return aligned_ptr;
    }

    /**
     * @brief Templated helper for explicit type allocation.
     */
    template <typename T>
    [[nodiscard]] 
    T* allocate_type(size_t count = 1) {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }

    /**
     * @brief Reset the allocator, invalidating all previous allocations.
     * Dangerous! Only use when you know all pointers are dropped.
     */
    void reset() noexcept {
        m_Current = 0;
    }

    [[nodiscard]] 
    size_t used() const noexcept {
        return m_Current;
    }
    
    [[nodiscard]]
    size_t remaining() const noexcept {
        return SizeBytes - m_Current;
    }

private:
    alignas(ALIGNMENT) byte m_Storage[SizeBytes];
    size_t m_Current; 
};

} // namespace StackAllocator
#endif // STACK_ALLOCATOR_H

