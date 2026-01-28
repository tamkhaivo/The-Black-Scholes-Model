#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <numeric>

namespace Benchmark {

class RuntimeStats {
  int64_t count;
  double average;
  double min;
  double max;
  double median;
  double stdDev;
  std::vector<double> samples;

public:
  RuntimeStats();
  explicit RuntimeStats(size_t max_samples);
  
  void add(double duration_ns);
  void calculate();
  
  static void displayRuntimeStats(const RuntimeStats& stats, std::string_view name);
  friend std::ostream& operator<<(std::ostream& os, const RuntimeStats& stats);
};

// Measures a single run in nanoseconds
template <typename Func> 
double runtimeNanoseconds(Func&& func) {
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration<double, std::nano>(end - start).count();
}

/**
 * @brief Benchmark a function with statistical tracking.
 * 
 * @param func Function to benchmark
 * @param iterations Total number of individual calls to measure
 */
template <typename Func>
RuntimeStats runtimeStats(Func&& func, size_t iterations = 1000000) {
  RuntimeStats stats{iterations};

  // Warmup
  for(int i=0; i<100; ++i) func();

  for (size_t i = 0; i < iterations; ++i) {
    stats.add(runtimeNanoseconds(func));
  }
  
  stats.calculate();
  return stats;
}

} // namespace Benchmark
