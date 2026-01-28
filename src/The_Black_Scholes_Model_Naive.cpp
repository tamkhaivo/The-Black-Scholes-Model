#include "The_Black_Scholes_Model_Naive.h"
#include <cmath>
#include <numbers> // C++20 standard constants

namespace BlackScholesModelNaive {

using namespace std;

// Standard Normal Cumulative Distribution Function using std::erfc
float norm_cdf(float x) {
    return 0.5f * std::erfc(-x * (1.0f / std::numbers::sqrt2_v<float>));
}

/*
 * Naive Implementation:
 * - Uses standard <cmath> functions (log, sqrt, exp, erfc).
 * - No manual optimizations (like precomputed constants if not essentially part of formula).
 * - Focus is on exact formula translation.
 */
float black_scholes(float S, float K, float r, float v, float T) {
    // Prevent division by zero or log of zero/negative
    if (S <= 0 || K <= 0 || v <= 0 || T <= 0) return 0.0f;

    float sqrt_T = std::sqrt(T);
    float d1 = (std::log(S / K) + (r + 0.5f * v * v) * T) / (v * sqrt_T);
    float d2 = d1 - v * sqrt_T;

    float call_price = S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
    
    return call_price;
}

void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        CallPrices[i] = black_scholes(S[i], K[i], r[i], v[i], T[i]);
    }
}

} // namespace BlackScholesModelNaive
