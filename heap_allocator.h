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
