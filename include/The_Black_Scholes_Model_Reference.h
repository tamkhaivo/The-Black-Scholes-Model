#pragma once
#include <cstddef>

namespace BlackScholesModelReference {

float black_scholes(const float *currentAssetPrice, const float *strikePrice,
                    const float *riskFreeRate, const float *volatility,
                    const float *timeToMaturity);
void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count);
float norm_cdf(float x);

} // namespace BlackScholesModelReference