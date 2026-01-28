#include "Benchmark.h"
#include <limits>

namespace Benchmark {

RuntimeStats::RuntimeStats()
    : count{0}, average{0.0L}, min{std::numeric_limits<long double>::max()},
      max{std::numeric_limits<long double>::lowest()}, median{0.0L},
      stdDev{0.0L}, capacity{0}, current_index{0}, allocator(0) {}

RuntimeStats::RuntimeStats(size_t max_samples)
    : count{0}, average{0.0L}, min{std::numeric_limits<long double>::max()},
      max{std::numeric_limits<long double>::lowest()}, median{0.0L},
      stdDev{0.0L}, capacity{max_samples}, current_index{0},
      allocator(max_samples * sizeof(long double)) {
  samples = static_cast<long double *>(
      allocator.allocate(max_samples * sizeof(long double)));
}

void RuntimeStats::add(long double duration) {
  if (current_index < capacity) {
    samples[current_index++] = duration;
  }
  if (duration < min) {
    min = duration;
  }
  if (duration > max) {
    max = duration;
  }
  count++;
}

void RuntimeStats::calculate() {
  if (current_index == 0)
    return;

  // Average
  long double sum = 0;
  for (size_t i = 0; i < current_index; ++i) {
    sum += samples[i];
  }
  average = sum / current_index;

  // Median
  std::sort(samples, samples + current_index);
  if (current_index % 2 == 0) {
    median =
        (samples[(current_index >> 1) - 1] + samples[current_index >> 1]) / 2.0;
  } else {
    median = samples[current_index >> 1];
  }

  // Standard Deviation
  long double variance_sum = 0;
  for (size_t i = 0; i < current_index; ++i) {
    variance_sum += std::pow(samples[i] - average, 2);
  }
  stdDev = std::sqrt(variance_sum / current_index);
}

void RuntimeStats::displayRuntimeStats(string_view name,
                                       const RuntimeStats *stats) {
  cout << "Function: " << name << endl;
  cout << *stats << endl;
}

std::ostream &operator<<(std::ostream &os, const RuntimeStats &stats) {
  os << "  Count: " << stats.count << "\n"
     << "  Average: " << stats.average << " ns\n"
     << "  Median: " << stats.median << " ns\n"
     << "  Min: " << stats.min << " ns\n"
     << "  Max: " << stats.max << " ns\n"
     << "  StdDev: " << stats.stdDev << " ns";
  return os;
}

} // namespace Benchmark