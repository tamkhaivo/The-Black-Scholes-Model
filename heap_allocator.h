#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>

namespace Memory {

class HeapAllocator {
public:
  HeapAllocator(size_t size) : m_size(size), m_offset(0) {
    m_storage = new uint8_t[size];
  }

  ~HeapAllocator() { delete[] m_storage; }

  // Delete copy constructor and copy assignment
  HeapAllocator(const HeapAllocator &) = delete;
  HeapAllocator &operator=(const HeapAllocator &) = delete;

  // Implement move constructor
  HeapAllocator(HeapAllocator &&other) noexcept
      : m_size(other.m_size), m_offset(other.m_offset),
        m_storage(other.m_storage) {
    other.m_size = 0;
    other.m_offset = 0;
    other.m_storage = nullptr;
  }

  // Implement move assignment
  HeapAllocator &operator=(HeapAllocator &&other) noexcept {
    if (this != &other) {
      delete[] m_storage;
      m_size = other.m_size;
      m_offset = other.m_offset;
      m_storage = other.m_storage;

      other.m_size = 0;
      other.m_offset = 0;
      other.m_storage = nullptr;
    }
    return *this;
  }

  void *allocate(size_t size) {
    // aligning to 8 bytes
    size_t alignedSize = (size + 7) & ~7;

    if (m_offset + alignedSize > m_size) {
      throw std::bad_alloc();
    }

    void *ptr = m_storage + m_offset;
    m_offset += alignedSize;
    return ptr;
  }

  void reset() { m_offset = 0; }

private:
  size_t m_size;
  size_t m_offset;
  uint8_t *m_storage;
};

} // namespace Memory

#endif // HEAP_ALLOCATOR_H
