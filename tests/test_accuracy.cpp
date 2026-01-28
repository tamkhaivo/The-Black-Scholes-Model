#include "../include/The_Black_Scholes_Model_Naive.h"
#include "../include/The_Black_Scholes_Model_Optimized.h"
#include "../include/The_Black_Scholes_Model_AI_Optimized.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;

struct TestCase {
    string name;
    float S, K, r, v, T;
};

int main() {
    vector<TestCase> cases = {
        {"Standard",      100.0f, 100.0f, 0.05f, 0.2f, 1.0f},
        {"ITM Call",      120.0f, 100.0f, 0.05f, 0.2f, 1.0f},
        {"OTM Call",       80.0f, 100.0f, 0.05f, 0.2f, 1.0f},
        {"Deep ITM",      200.0f, 100.0f, 0.05f, 0.2f, 1.0f},
        {"Deep OTM",       50.0f, 100.0f, 0.05f, 0.2f, 1.0f},
        {"High Vol",      100.0f, 100.0f, 0.05f, 1.0f, 1.0f},
        {"Low Vol",       100.0f, 100.0f, 0.05f, 0.05f, 1.0f},
        {"Long Mat",      100.0f, 100.0f, 0.05f, 0.2f, 10.0f},
        {"Short Mat",     100.0f, 100.0f, 0.05f, 0.2f, 0.01f},
        {"High Rate",     100.0f, 100.0f, 0.20f, 0.2f, 1.0f},
        {"Zero Rate",     100.0f, 100.0f, 0.00f, 0.2f, 1.0f},
        {"Tiny Price",      1.0f,   1.0f, 0.05f, 0.2f, 1.0f},
        {"Huge Price",   1000.0f, 1000.0f, 0.05f, 0.2f, 1.0f}
    };

    // Pad with dummy cases to reach 32 to trigger AVX path
    size_t original_count = cases.size();
    while (cases.size() < 32) {
        cases.push_back({"Padding", 100.0f, 100.0f, 0.05f, 0.2f, 1.0f});
    }

    size_t count = cases.size();
    vector<float> S(count), K(count), r(count), v(count), T(count);
    
    for(size_t i=0; i<count; ++i) {
        S[i] = cases[i].S;
        K[i] = cases[i].K;
        r[i] = cases[i].r;
        v[i] = cases[i].v;
        T[i] = cases[i].T;
    }

    vector<float> naive_results(count);
    vector<float> optimized_results(count);
    vector<float> ai_results(count);

    // Run Implementations
    BlackScholesModelNaive::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), naive_results.data(), count);
    BlackScholesModelOptimized::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), optimized_results.data(), count);
    BlackScholesModelAIOptimized::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), ai_results.data(), count);

    bool all_passed = true;

    cout << fixed << setprecision(4);
    
    // Header
    cout << left << setw(15) << "Case"
         << right << setw(12) << "Naive"
         << setw(12) << "Optimized"
         << setw(12) << "AI Opt"
         << setw(12) << "Diff Opt"
         << setw(12) << "Diff AI"
         << setw(10) << "Status" << endl;
    cout << string(85, '-') << endl;

    for (size_t i = 0; i < original_count; ++i) {
        float n = naive_results[i];
        float o = optimized_results[i];
        float a = ai_results[i];
        
        float diff_o = std::abs(n - o);
        float diff_a = std::abs(n - a);
        
        bool pass_o = diff_o < 0.2f && !std::isnan(o) && !std::isinf(o);
        bool pass_a = diff_a < 0.2f && !std::isnan(a) && !std::isinf(a) && (a > 0.0001f || n < 0.001f);

        if (!pass_o || !pass_a) all_passed = false;
        
        cout << left << setw(15) << cases[i].name
             << right << setw(12) << n
             << setw(12) << o
             << setw(12) << a
             << setw(12) << diff_o
             << setw(12) << diff_a
             << setw(10) << (pass_o && pass_a ? "PASS" : "FAIL") << endl;
    }

    if (all_passed) {
        cout << "\n[SUCCESS] Tests passed." << endl;
        return 0;
    } else {
        cout << "\n[FAILURE] Significant deviations or NaNs detected." << endl;
        return 1;
    }
}
