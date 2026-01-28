#include "The_Black_Scholes_Model_Value.h"
#include <cmath>
#include <numbers>

namespace BlackScholesModelValue {

using namespace std;

float norm_cdf(float x) {
  // Constants for approximation of the cumulative distribution function
  float a1 = 0.319381530;
  float a2 = -0.356563782;
  float a3 = 1.781477937;
  float a4 = -1.821255978;
  float a5 = 1.330274429;

  float l = abs(x);
  float k = 1.0 / (1.0 + 0.2316419 * l);
  float w = 1.0 - 1.0 / sqrt(2 * std::numbers::pi_v<float>) * exp(-l * l / 2) *
                      (a1 * k + a2 * k * k + a3 * pow(k, 3) + a4 * pow(k, 4) +
                       a5 * pow(k, 5));

  if (x < 0) {
    return 1.0 - w;
  } else {
    return w;
  }
}

float black_scholes(const float currentAssetPrice, const float strikePrice,
                    const float riskFreeRate, const float volatility, const float timeToMaturity) {
  float d1 = (log(currentAssetPrice / strikePrice) +
              (riskFreeRate + volatility * volatility / 2) * timeToMaturity) /
             (volatility * sqrt(timeToMaturity));
  float d2 = d1 - volatility * sqrt(timeToMaturity); // d2 = d1 - sigma * sqrt(T)

  return currentAssetPrice * norm_cdf(d1) -
         strikePrice * exp(-riskFreeRate * timeToMaturity) * norm_cdf(d2);
}

void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        CallPrices[i] = black_scholes(S[i], K[i], r[i], v[i], T[i]);
    }
}

} // namespace BlackScholesModelValue
