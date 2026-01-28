#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace Memory {
using namespace std;
/**
 * @brief Linear allocator over a heap-allocated buffer.
 * 
 * Manages a large block of heap memory and dishes out smaller chunks linearly.
 * Uses std::unique_ptr for automatic resource management.
 */
class HeapAllocator {
public:
    // Enforce robust alignment by default
    static constexpr size_t DEFAULT_ALIGNMENT = alignof(max_align_t);

    explicit HeapAllocator(size_t size) 
        : m_size(size), m_offset(0), m_storage(make_unique<byte[]>(size)) {
    }

    // Default destructor works because of unique_ptr
    ~HeapAllocator() noexcept = default;

    // Non-copyable
    HeapAllocator(const HeapAllocator&) = delete;
    HeapAllocator& operator=(const HeapAllocator&) = delete;

    // Move-constructible (default unique_ptr move is sufficient)
    HeapAllocator(HeapAllocator&&) noexcept = default;
    HeapAllocator& operator=(HeapAllocator&&) noexcept = default;

    /**
     * @brief Allocates aligned memory from the heap buffer.
     * 
     * @param size Bytes to allocate
     * @param align Alignment (power of 2)
     * @return void* Pointer to allocated memory
     * @throws std::bad_alloc if exhausted
     */
    [[nodiscard]]
    [[nodiscard]]
    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) {
        // Current pointer
        void* ptr = static_cast<void*>(m_storage.get() + m_offset);
        
        // Remaining space
        size_t space = m_size - m_offset;

        // Use std::align to adjust ptr and check space
        void* aligned_ptr = std::align(alignment, size, ptr, space);

        if (!aligned_ptr) {
            throw bad_alloc();
        }

        // Calculate how much we actually advanced (padding + size)
        size_t padding = static_cast<byte*>(aligned_ptr) - (m_storage.get() + m_offset);
        size_t total_size = padding + size;
        
        // Double check bounds
        if (m_offset + total_size > m_size) {
            throw bad_alloc();
        }

        m_offset += total_size;
        return aligned_ptr;
    }

    /**
     * @brief Type-safe allocation helper.
     */
    template <typename T>
    [[nodiscard]]
    T* allocate_type(size_t count = 1) {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }

    /**
     * @brief Resets the allocator offset. Does NOT free the heap memory.
     * Invalidates all previous allocations from this allocator.
     */
    void reset() noexcept {
        m_offset = 0;
    }

    [[nodiscard]]
    size_t used() const noexcept { return m_offset; }

    [[nodiscard]]
    size_t capacity() const noexcept { return m_size; }

private:
    size_t m_size;
    size_t m_offset;
    unique_ptr<byte[]> m_storage;
};

} // namespace Memory

#endif // HEAP_ALLOCATOR_H

