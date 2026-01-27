#include "The_Black_Scholes_Model_Reference.h"
#include <cmath>

namespace BlackScholesModelReference {
using namespace std;

float norm_cdf(float x) {
  float k = 1.0 / (1.0 + 0.2316419 * abs(x));
  float y =
      1.0 -
      1.0 / sqrt(2 * M_PI) * exp(-0.5 * x * x) *
          (k * (0.319381530 +
                k * (0.356563782 +
                     k * (1.781477937 + k * (1.821255978 + k * 1.70464237)))));
  return y;
}

/*
Current Asset Price (S0): The spot price of the underlying stock.
Strike Price (K): The pre-determined price at which the option can be exercised.
Time to Maturity (T): The time remaining until the option expires, usually
expressed in years.
Risk-Free Interest Rate (r): The theoretical rate of return
on an investment with zero risk (commonly the yield on U.S. Treasury bills).
Volatility (σ): The standard deviation of the stock's returns. This is the most
critical and difficult parameter to estimate.

*/
float black_scholes(float *currentAssetPrice, float *strikePrice,
                    float *riskFreeRate, float *volatility,
                    float *timeToMaturityInYears) {
  float d1 = (log(*currentAssetPrice / *strikePrice) +
              (*riskFreeRate + 0.5 * *volatility * *volatility) *
                  *timeToMaturityInYears) /
             (*volatility * sqrt(*timeToMaturityInYears));

  float d2 = d1 - *volatility * sqrt(*timeToMaturityInYears);
  return *currentAssetPrice * norm_cdf(d1) -
         *strikePrice * exp(-*riskFreeRate * *timeToMaturityInYears) *
             norm_cdf(d2);
}

} // namespace BlackScholesModelReference
