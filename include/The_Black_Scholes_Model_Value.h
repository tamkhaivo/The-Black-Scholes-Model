#pragma once

namespace BlackScholesModelValue {

float black_scholes(float currentAssetPrice, float strikePrice,
                    float riskFreeRate, float volatility, float timeToMaturity);
float norm_cdf(float x);

} // namespace BlackScholesModelValue