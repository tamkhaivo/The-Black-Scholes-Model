#include "Benchmark.h"
#include <limits>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace Benchmark {

RuntimeStats::RuntimeStats()
    : count{0}, average{0.0}, min{std::numeric_limits<double>::max()},
      max{std::numeric_limits<double>::lowest()}, median{0.0},
      stdDev{0.0} {}

RuntimeStats::RuntimeStats(size_t max_samples)
    : RuntimeStats() {
    samples.reserve(max_samples);
}

void RuntimeStats::add(double duration) {
  samples.push_back(duration);
  if (duration < min) min = duration;
  if (duration > max) max = duration;
  count++;
}

void RuntimeStats::calculate() {
  if (samples.empty()) return;

  // Average
  double sum = std::reduce(samples.begin(), samples.end(), 0.0);
  average = sum / samples.size();

  // Median
  std::sort(samples.begin(), samples.end());
  size_t n = samples.size();
  if (n % 2 == 0) {
    median = (samples[n / 2 - 1] + samples[n / 2]) / 2.0;
  } else {
    median = samples[n / 2];
  }

  // Standard Deviation
  double variance_sum = 0.0;
  for (double val : samples) {
      variance_sum += (val - average) * (val - average);
  }
  stdDev = std::sqrt(variance_sum / n);
}

void RuntimeStats::displayRuntimeStats(const RuntimeStats& stats, std::string_view name) {
  std::cout << "Function: " << name << "\n" << stats << std::endl;
}

std::ostream& operator<<(std::ostream& os, const RuntimeStats& stats) {
  os << "  Count:   " << stats.count << "\n"
     << "  Average: " << stats.average << " ns\n"
     << "  Median:  " << stats.median  << " ns\n"
     << "  Min:     " << stats.min     << " ns\n"
     << "  Max:     " << stats.max     << " ns\n"
     << "  StdDev:  " << stats.stdDev  << " ns";
  return os;
}

} // namespace Benchmark
