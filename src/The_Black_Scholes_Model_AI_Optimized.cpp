#include "The_Black_Scholes_Model_AI_Optimized.h"
#include <immintrin.h>
#include <cmath>

namespace BlackScholesModelAIOptimized {

// --------------------------------------------------------------------------------
// FAST MATH INTRINSICS HELPER
// Disregarding standard practices: Inline everything, use rcp/rsqrt approximations.
// --------------------------------------------------------------------------------

// Fast exp using FMA
// Approx: exp(x) = 2^k * 2^f  where x = (k+f)*ln(2)
inline __m256 fast_exp_fma(__m256 x) {
    // Generated constants
    __m256 k_exp = _mm256_set1_ps(12102203.161654067f); // 2^23 / ln(2)
    __m256 bias = _mm256_set1_ps(1064986820.0f);        // 127 << 23
    
    // No clamping for speed - caller must ensure inputs aren't excessively large/small
    // val = x * k + bias
    __m256 val = _mm256_fmadd_ps(k_exp, x, bias);
    
    __m256i i = _mm256_cvttps_epi32(val);
    return _mm256_castsi256_ps(i);
}

// Fast log using FMA and polynomial
inline __m256 fast_log_fma(__m256 x) {
    __m256i i = _mm256_castps_si256(x);
    __m256i mask_mantissa = _mm256_set1_epi32(0x007FFFFF);
    
    // Extract exponent and mantissa
    // e = (i >> 23) - 127; 
    // m = (i & 0x7FFFFF) | 0x3F800000; (normalized 1.0..2.0)
    // Actually using the reinterpretation trick directly on the bits
    
    __m256 mx = _mm256_cvtepi32_ps(_mm256_and_si256(i, mask_mantissa));
    __m256 y  = _mm256_cvtepi32_ps(i);
    
    __m256 scale = _mm256_set1_ps(1.1920928955078125e-7f); // 1 / 2^23
    y = _mm256_mul_ps(y, scale);
    
    __m256 sub = _mm256_set1_ps(126.94252533f);
    __m256 c = _mm256_set1_ps(4.50305e-6f);
    
    // y = y - sub - c * mx;
    // FMA: a*b - c? No, usage is sub - a*b?
    // We want: (y - sub) - (c*mx)
    // _mm256_fnmadd_ps(a, b, c)  -> -(a*b) + c = c - a*b
    // Let's rely on standard sub/fnmadd
    
    // result = y - 126.9425 - 4.5e-6 * mx
    __m256 res = _mm256_sub_ps(y, sub);
    res = _mm256_fnmadd_ps(c, mx, res); // res - c*mx
    return res;
}

// Fast CDF using FMA and RCP
inline __m256 fast_cdf_fma(__m256 x) {
     __m256 zero = _mm256_setzero_ps();
    __m256 one  = _mm256_set1_ps(1.0f);
    __m256 sign_bit = _mm256_set1_ps(-0.0f);
    
    // abs(x)
    __m256 abs_x = _mm256_andnot_ps(sign_bit, x);

    // t = 1 / (1 + p*x)
    // Approximate RCP is much faster than div
    __m256 p = _mm256_set1_ps(0.2316419f);
    __m256 den = _mm256_fmadd_ps(p, abs_x, one); // 1 + p*x
    __m256 t = _mm256_rcp_ps(den); // Approx 1/den. Could do 1 NR step for precision, but skipping for speed.

    // Polynomial coefficients
    __m256 b1 = _mm256_set1_ps(0.319381530f);
    __m256 b2 = _mm256_set1_ps(-0.356563782f);
    __m256 b3 = _mm256_set1_ps(1.781477937f);
    __m256 b4 = _mm256_set1_ps(-1.821255978f);
    __m256 b5 = _mm256_set1_ps(1.330274429f);

    // Horner FMA chain
    __m256 poly = _mm256_fmadd_ps(t, b5, b4);
    poly = _mm256_fmadd_ps(t, poly, b3);
    poly = _mm256_fmadd_ps(t, poly, b2);
    poly = _mm256_fmadd_ps(t, poly, b1);
    poly = _mm256_mul_ps(t, poly);
    
    // PDF = 0.39894228 * exp(-0.5 * x*x)
    __m256 c_pdf = _mm256_set1_ps(0.39894228f);
    __m256 m_half = _mm256_set1_ps(-0.5f);
    
    __m256 sq = _mm256_mul_ps(x, x);
    __m256 expo = _mm256_mul_ps(m_half, sq);
    expo = fast_exp_fma(expo);
    
    __m256 pdf = _mm256_mul_ps(c_pdf, expo);
    
    // prob = 1 - pdf*poly
    __m256 prob = _mm256_fnmadd_ps(pdf, poly, one); // -(pdf*poly) + 1
    
    // Blend based on sign
    // if x < 0, result is 1 - prob
    __m256 is_neg = _mm256_cmp_ps(x, zero, _CMP_LT_OQ);
    __m256 res_neg = _mm256_sub_ps(one, prob);
    
    return _mm256_blendv_ps(prob, res_neg, is_neg);
}

void black_scholes_batch(const float* S, const float* K, const float* r, 
                        const float* v, const float* T, 
                        float* CallPrices, size_t count) {
    size_t i = 0;
    
    // Constants
    __m256i mask_sign = _mm256_set1_epi32(0x80000000);
    __m256 half = _mm256_set1_ps(0.5f);
    __m256 inv_sqrt2 = _mm256_set1_ps(0.70710678f); // Not needed? Naive used it.
    
    // Loop unrolled 4x (32 floats per iteration)
    size_t count32 = count & ~31;
    
    for (; i < count32; i += 32) {
        // Load independent blocks to hide FMA latency
        __m256 S0 = _mm256_loadu_ps(S + i);
        __m256 S1 = _mm256_loadu_ps(S + i + 8);
        __m256 S2 = _mm256_loadu_ps(S + i + 16);
        __m256 S3 = _mm256_loadu_ps(S + i + 24);
        
        __m256 K0 = _mm256_loadu_ps(K + i);
        __m256 K1 = _mm256_loadu_ps(K + i + 8);
        __m256 K2 = _mm256_loadu_ps(K + i + 16);
        __m256 K3 = _mm256_loadu_ps(K + i + 24);
        
        __m256 r0 = _mm256_loadu_ps(r + i);
        __m256 r1 = _mm256_loadu_ps(r + i + 8);
        __m256 r2 = _mm256_loadu_ps(r + i + 16);
        __m256 r3 = _mm256_loadu_ps(r + i + 24);
        
        __m256 v0 = _mm256_loadu_ps(v + i);
        __m256 v1 = _mm256_loadu_ps(v + i + 8);
        __m256 v2 = _mm256_loadu_ps(v + i + 16);
        __m256 v3 = _mm256_loadu_ps(v + i + 24);
        
        __m256 T0 = _mm256_loadu_ps(T + i);
        __m256 T1 = _mm256_loadu_ps(T + i + 8);
        __m256 T2 = _mm256_loadu_ps(T + i + 16);
        __m256 T3 = _mm256_loadu_ps(T + i + 24);

        // RSQRT(T) which is 1/sqrt(T). Much faster than div(sqrt).
        // rsqrt approx + 0 NR steps (raw speed)
        __m256 inv_sqrt_T0 = _mm256_rsqrt_ps(T0);
        __m256 inv_sqrt_T1 = _mm256_rsqrt_ps(T1);
        __m256 inv_sqrt_T2 = _mm256_rsqrt_ps(T2);
        __m256 inv_sqrt_T3 = _mm256_rsqrt_ps(T3);
        
        // sqrt(T) = T * inv_sqrt_T approximately, or just use _mm256_sqrt_ps if needed for v*sqrt(T).
        // Actually, d1 = ... / (v * sqrt(T)) = ... * (1/v * 1/sqrt(T))
        // Let's compute v_sqrt_T = v / inv_sqrt_T ?? No.
        // sqrt(T) ~= T * rsqrt(T)
        // v_sqrt_T = v * (T * rsqrt(T))
        __m256 sqrt_T0 = _mm256_mul_ps(T0, inv_sqrt_T0);
        __m256 sqrt_T1 = _mm256_mul_ps(T1, inv_sqrt_T1);
        __m256 sqrt_T2 = _mm256_mul_ps(T2, inv_sqrt_T2);
        __m256 sqrt_T3 = _mm256_mul_ps(T3, inv_sqrt_T3);
        
        __m256 v_sqrt_T0 = _mm256_mul_ps(v0, sqrt_T0);
        __m256 v_sqrt_T1 = _mm256_mul_ps(v1, sqrt_T1);
        __m256 v_sqrt_T2 = _mm256_mul_ps(v2, sqrt_T2);
        __m256 v_sqrt_T3 = _mm256_mul_ps(v3, sqrt_T3);

        // Inv transform for division
        // d1 denom = v * sqrt(T). We need 1/denom.
        // 1/(v * sqrt(T)) = 1/v * 1/sqrt(T) = rcp(v) * rsqrt(T)
        // Optimization: use RCP on v?
        // Let's just use RCP on the product v_sqrt_T
        __m256 inv_den0 = _mm256_rcp_ps(v_sqrt_T0);
        __m256 inv_den1 = _mm256_rcp_ps(v_sqrt_T1);
        __m256 inv_den2 = _mm256_rcp_ps(v_sqrt_T2);
        __m256 inv_den3 = _mm256_rcp_ps(v_sqrt_T3);

        // Ratio S/K. log(S/K) = log(S * rcp(K))
        __m256 rcp_K0 = _mm256_rcp_ps(K0);
        __m256 rcp_K1 = _mm256_rcp_ps(K1);
        __m256 rcp_K2 = _mm256_rcp_ps(K2);
        __m256 rcp_K3 = _mm256_rcp_ps(K3);
        
        __m256 ratio0 = _mm256_mul_ps(S0, rcp_K0);
        __m256 ratio1 = _mm256_mul_ps(S1, rcp_K1);
        __m256 ratio2 = _mm256_mul_ps(S2, rcp_K2);
        __m256 ratio3 = _mm256_mul_ps(S3, rcp_K3);
        
        __m256 log0 = fast_log_fma(ratio0);
        __m256 log1 = fast_log_fma(ratio1);
        __m256 log2 = fast_log_fma(ratio2);
        __m256 log3 = fast_log_fma(ratio3);
        
        // (r + 0.5 * v*v)
        __m256 v_sq0 = _mm256_mul_ps(v0, v0);
        __m256 v_sq1 = _mm256_mul_ps(v1, v1);
        __m256 v_sq2 = _mm256_mul_ps(v2, v2);
        __m256 v_sq3 = _mm256_mul_ps(v3, v3);
        
        __m256 drift0 = _mm256_fmadd_ps(half, v_sq0, r0);
        __m256 drift1 = _mm256_fmadd_ps(half, v_sq1, r1);
        __m256 drift2 = _mm256_fmadd_ps(half, v_sq2, r2);
        __m256 drift3 = _mm256_fmadd_ps(half, v_sq3, r3);
        
        // num = log + drift*T
        __m256 num0 = _mm256_fmadd_ps(drift0, T0, log0);
        __m256 num1 = _mm256_fmadd_ps(drift1, T1, log1);
        __m256 num2 = _mm256_fmadd_ps(drift2, T2, log2);
        __m256 num3 = _mm256_fmadd_ps(drift3, T3, log3);
        
        // d1 = num * inv_den
        __m256 d1_0 = _mm256_mul_ps(num0, inv_den0);
        __m256 d1_1 = _mm256_mul_ps(num1, inv_den1);
        __m256 d1_2 = _mm256_mul_ps(num2, inv_den2);
        __m256 d1_3 = _mm256_mul_ps(num3, inv_den3);
        
        // d2 = d1 - v_sqrt_T
        __m256 d2_0 = _mm256_sub_ps(d1_0, v_sqrt_T0);
        __m256 d2_1 = _mm256_sub_ps(d1_1, v_sqrt_T1);
        __m256 d2_2 = _mm256_sub_ps(d1_2, v_sqrt_T2);
        __m256 d2_3 = _mm256_sub_ps(d1_3, v_sqrt_T3);
        
        // CDFs
        __m256 nd1_0 = fast_cdf_fma(d1_0);
        __m256 nd1_1 = fast_cdf_fma(d1_1);
        __m256 nd1_2 = fast_cdf_fma(d1_2);
        __m256 nd1_3 = fast_cdf_fma(d1_3);
        
        __m256 nd2_0 = fast_cdf_fma(d2_0);
        __m256 nd2_1 = fast_cdf_fma(d2_1);
        __m256 nd2_2 = fast_cdf_fma(d2_2);
        __m256 nd2_3 = fast_cdf_fma(d2_3);
        
        // Discount exp(-rT)
        // 0 - r*T
        __m256 neg_rt0 = _mm256_fnmadd_ps(r0, T0, _mm256_setzero_ps()); 
        __m256 neg_rt1 = _mm256_fnmadd_ps(r1, T1, _mm256_setzero_ps());
        __m256 neg_rt2 = _mm256_fnmadd_ps(r2, T2, _mm256_setzero_ps());
        __m256 neg_rt3 = _mm256_fnmadd_ps(r3, T3, _mm256_setzero_ps());
        
        __m256 disc0 = fast_exp_fma(neg_rt0);
        __m256 disc1 = fast_exp_fma(neg_rt1);
        __m256 disc2 = fast_exp_fma(neg_rt2);
        __m256 disc3 = fast_exp_fma(neg_rt3);
        
        // Final: S*nd1 - K*disc*nd2
        // K*disc*nd2
        __m256 termR0 = _mm256_mul_ps(K0, _mm256_mul_ps(disc0, nd2_0));
        __m256 termR1 = _mm256_mul_ps(K1, _mm256_mul_ps(disc1, nd2_1));
        __m256 termR2 = _mm256_mul_ps(K2, _mm256_mul_ps(disc2, nd2_2));
        __m256 termR3 = _mm256_mul_ps(K3, _mm256_mul_ps(disc3, nd2_3));
        
        // S*nd1 - termR
        // fmsub? a*b - c
        __m256 call0 = _mm256_fmsub_ps(S0, nd1_0, termR0);
        __m256 call1 = _mm256_fmsub_ps(S1, nd1_1, termR1);
        __m256 call2 = _mm256_fmsub_ps(S2, nd1_2, termR2);
        __m256 call3 = _mm256_fmsub_ps(S3, nd1_3, termR3);
        
        _mm256_storeu_ps(CallPrices + i, call0);
        _mm256_storeu_ps(CallPrices + i + 8, call1);
        _mm256_storeu_ps(CallPrices + i + 16, call2);
        _mm256_storeu_ps(CallPrices + i + 24, call3);
    }
    
    // Cleanup remainder (reuse the scalar log/exp from previous optimization if we could, 
    // but we can't depend on another file safely here if we want isolation. 
    // Just minimal scalar ref loop)
    for (; i < count; ++i) {
        // Super naive slow fallback, user cares about batch throughput.
        // Copy-paste basic logic to be self-contained
        float S_ = S[i], K_ = K[i], r_ = r[i], v_ = v[i], T_ = T[i];
        float d1 = (std::log(S_/K_) + (r_ + 0.5f*v_*v_)*T_) / (v_*std::sqrt(T_));
        float d2 = d1 - v_*std::sqrt(T_);
        // Use standard erf for tail
        auto ncdf = [](float x){ return 0.5f * std::erfc(-x * 0.70710678f); };
        CallPrices[i] = S_ * ncdf(d1) - K_ * std::exp(-r_ * T_) * ncdf(d2);
    }
}

} // namespace
