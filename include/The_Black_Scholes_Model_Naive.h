#ifndef THE_BLACK_SCHOLES_MODEL_NAIVE_H
#define THE_BLACK_SCHOLES_MODEL_NAIVE_H

#include <cstddef>

namespace BlackScholesModelNaive {

/**
 * @brief Calculates Call Option Price using standard library math functions.
 * 
 * "Naive" implementation emphasizing readability and standard math correctness,
 * potentially at the cost of performance compared to optimized approximations.
 * 
 * @param S Current Asset Price
 * @param K Strike Price
 * @param r Risk-Free Interest Rate
 * @param v Volatility
 * @param T Time to Maturity
 * @return float Option Price
 */
float black_scholes(float S, float K, float r, float v, float T);

void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count);

} // namespace BlackScholesModelNaive

#endif // THE_BLACK_SCHOLES_MODEL_NAIVE_H
