#pragma once

#include "heap_allocator.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace Benchmark {
using namespace std;

class RuntimeStats {
  int64_t count;
  long double average;
  long double min;
  long double max;
  long double median;
  long double stdDev;
  long double *samples;
  size_t capacity;
  size_t current_index;
  Memory::HeapAllocator allocator;

public:
  RuntimeStats(size_t max_samples);
  void add(long double duration);
  void calculate();
  static void displayRuntimeStats(string_view name, const RuntimeStats *stats);
  friend std::ostream &operator<<(std::ostream &os, const RuntimeStats &stats);
};

template <typename Func> long double runtimeNanoseconds(Func func) {
  auto start = chrono::steady_clock::now();
  func();
  auto end = chrono::steady_clock::now();
  auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
  return duration.count();
}

template <typename Func>
void runtimeStats(Func func, string_view name, size_t turnsCount = 10000000) {
  RuntimeStats stats{turnsCount};

  for (size_t i = 0; i < turnsCount; i++) {
    long double duration = runtimeNanoseconds(func);
    stats.add(duration);
  }
  stats.calculate();

  RuntimeStats::displayRuntimeStats(name, &stats);
}

} // namespace Benchmark