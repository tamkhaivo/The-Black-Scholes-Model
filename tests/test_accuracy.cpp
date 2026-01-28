#include "../include/The_Black_Scholes_Model_Naive.h"
#include "../include/The_Black_Scholes_Model_Optimized.h"
#include "../include/The_Black_Scholes_Model_AI_Optimized.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

bool is_close(float a, float b, float epsilon = 0.5f) { // Approximations are rough
    return std::abs(a - b) < epsilon;
}

int main() {
    // Test parameters
    size_t count = 5;
    vector<float> S = {100.0f, 50.0f, 200.0f, 100.0f, 100.0f};
    vector<float> K = {100.0f, 50.0f, 200.0f, 120.0f, 80.0f};
    vector<float> r = {0.05f, 0.05f, 0.03f, 0.1f, 0.01f};
    vector<float> v = {0.2f, 0.3f, 0.15f, 0.5f, 0.1f};
    vector<float> T = {1.0f, 0.5f, 2.0f, 1.0f, 0.1f};

    vector<float> naive_results(count);
    vector<float> optimized_results(count);
    vector<float> ai_results(count);

    // 1. Run Naive (Reference)
    BlackScholesModelNaive::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), naive_results.data(), count);

    // 2. Run Optimized
    BlackScholesModelOptimized::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), optimized_results.data(), count);

    // 3. Run AI Optimized
    BlackScholesModelAIOptimized::black_scholes_batch(S.data(), K.data(), r.data(), v.data(), T.data(), ai_results.data(), count);

    bool all_passed = true;

    cout << fixed << setprecision(4);
    cout << "Accuracy Test Results:" << endl;
    cout << "Idx | Naive    | Optimized | AI Opt   | Diff Opt | Diff AI | Status" << endl;
    cout << "---------------------------------------------------------------------" << endl;

    for (size_t i = 0; i < count; ++i) {
        float n = naive_results[i];
        float o = optimized_results[i];
        float a = ai_results[i];
        
        float diff_o = std::abs(n - o);
        float diff_a = std::abs(n - a);
        
        // Epsilon: 0.10 seems fair for these approximations (e.g. 10.45 vs 10.40)
        bool pass_o = diff_o < 0.1f; 
        bool pass_a = diff_a < 0.1f;

        if (!pass_o || !pass_a) all_passed = false;

        cout << i << "   | " << n << "  | " << o << "  | " << a << "  | " << diff_o << "   | " << diff_a << "  | ";
        if (pass_o && pass_a) cout << "PASS";
        else cout << "FAIL";
        cout << endl;
    }

    if (all_passed) {
        cout << "\n[SUCCESS] All implementations match within tolerance." << endl;
        return 0;
    } else {
        cout << "\n[FAILURE] Significant deviation detected." << endl;
        return 1;
    }
}
