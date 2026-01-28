#ifndef THE_BLACK_SCHOLES_MODEL_AI_OPTIMIZED_H
#define THE_BLACK_SCHOLES_MODEL_AI_OPTIMIZED_H

#include <cstddef>

namespace BlackScholesModelAIOptimized {

// No scalar function exposed - only the batch processor for maximum throughput.
void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count);

} 

#endif
