#include "Benchmark.h"
#include "The_Black_Scholes_Model_Naive.h"
#include "The_Black_Scholes_Model_Optimized.h"
#include "The_Black_Scholes_Model_AI_Optimized.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// Helper to benchmark a batch function
template <typename Func>
void benchmark_batch(string_view name, Func func, size_t iterations, 
                    float* S, float* K, float* r, float* v, float* T, float* CallPrices) {
    
    // Warmup
    func(S, K, r, v, T, CallPrices, iterations);

    int runs = 100;
    auto start = chrono::high_resolution_clock::now();
    for(int i=0; i<runs; ++i) {
        func(S, K, r, v, T, CallPrices, iterations);
    }
    auto end = chrono::high_resolution_clock::now();
    
    double total_time = chrono::duration<double, std::nano>(end - start).count();
    double avg_total_time = total_time / runs;
    double ns_per_option = avg_total_time / iterations;

    cout << "Function: " << name << endl;
    cout << "  Count:   " << iterations * runs << endl;
    cout << "  Average: " << ns_per_option << " ns (per option)" << endl << endl;
}

int main() {
  float currentAssetPrice = 100;
  float strikePrice = 100;
  float riskFreeRate = 0.05;
  float volatility = 0.2;
  float timeToMaturity = 1;
  size_t iterations = 10000000;

  cout << "=== Batch Benchmark (10M options x 100 runs) ===" << endl;
  
  // Prepare Data
  vector<float> S(iterations, currentAssetPrice);
  vector<float> K(iterations, strikePrice);
  vector<float> r(iterations, riskFreeRate);
  vector<float> v(iterations, volatility);
  vector<float> T(iterations, timeToMaturity);
  vector<float> CallPrices(iterations);

  // 1. Naive Implementation
  benchmark_batch("Batch_Naive (std::erfc)", 
      BlackScholesModelNaive::black_scholes_batch, 
      iterations, S.data(), K.data(), r.data(), v.data(), T.data(), CallPrices.data());

  // 2. Optimized Implementation (AVX2 Basic)
  benchmark_batch("Batch_Optimized (AVX2 Basic)", 
      BlackScholesModelOptimized::black_scholes_batch, 
      iterations, S.data(), K.data(), r.data(), v.data(), T.data(), CallPrices.data());

  // 3. AI Optimized Implementation (AVX2 + unroll + FMA + fast math)
  benchmark_batch("Batch_AI_Optimized (AVX2+FMA+Unroll)", 
      BlackScholesModelAIOptimized::black_scholes_batch, 
      iterations, S.data(), K.data(), r.data(), v.data(), T.data(), CallPrices.data());

  return 0;
}