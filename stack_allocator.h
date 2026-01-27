#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <cstddef>
#include <new>

namespace StackAllocator {

template <size_t Size> class StackAllocator {
public:
  StackAllocator() {
    m_Storage = new char[Size];
    m_Current = m_Storage;
  }

  ~StackAllocator() { delete[] m_Storage; }

  void *allocate(size_t size) {
    if (m_Current + size > m_Storage + Size) {
      throw std::bad_alloc();
    }
    void *ptr = m_Current;
    m_Current += size;
    return ptr;
  }

private:
  char *m_Storage;
  char *m_Current;
};
} // namespace StackAllocator
#endif
