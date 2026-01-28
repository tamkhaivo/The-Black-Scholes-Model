#ifndef THE_BLACK_SCHOLES_MODEL_OPTIMIZED_H
#define THE_BLACK_SCHOLES_MODEL_OPTIMIZED_H

#include <cstddef> // size_t

namespace BlackScholesModelOptimized {

/**
 * @brief Optimized Black-Scholes for Scalar inputs.
 * Uses fast math approximations for speed over absolute precision.
 */
float black_scholes(float S, float K, float r, float v, float T);

/**
 * @brief Batch processing for potential SIMD auto-vectorization.
 * Processes 'count' options in linear arrays.
 * 
 * @param S Array of Stock Prices
 * @param K Array of Strike Prices
 * @param r Array of Risk-Free Rates
 * @param v Array of Volatilities
 * @param T Array of Times to Maturity
 * @param CallPrices Output array for Call Prices
 * @param count Number of elements
 */
void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count);

} // namespace BlackScholesModelOptimized

#endif // THE_BLACK_SCHOLES_MODEL_OPTIMIZED_H
