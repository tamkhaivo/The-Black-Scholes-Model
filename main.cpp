

#include "Benchmark.h"
#include "The_Black_Scholes_Model_Reference.h"
#include "The_Black_Scholes_Model_Value.h"
#include <iostream>

using namespace std;
int main() {
  float currentAssetPrice = 100;
  float strikePrice = 100;
  float riskFreeRate = 0.05;
  float volatility = 0.2;
  float timeToMaturity = 1;
  cout << "=== Benchmark ===" << endl;
  Benchmark::runtimeStats(
      [&]() {
        BlackScholesModelValue::black_scholes(currentAssetPrice, strikePrice,
                                              riskFreeRate, volatility,
                                              timeToMaturity);
      },
      "benchmark_value", 1000000);
  Benchmark::runtimeStats(
      [&]() {
        BlackScholesModelReference::black_scholes(&currentAssetPrice,
                                                  &strikePrice, &riskFreeRate,
                                                  &volatility, &timeToMaturity);
      },
      "benchmark_reference", 1000000);
  return 0;
}