# High-Performance Black-Scholes Model in C++

This project implements the Black-Scholes option pricing model with varying levels of optimization, demonstrating the progression from standard C++ code to high-frequency trading (HFT) grade AVX2 intrinsics.

## Implementations

The project compares 5 implementations, ranging from readable standard C++ to aggressive hardware-specific optimizations:

1.  **Naive (`std::erfc`)**: Uses the Standard Library `std::erfc` and `std::exp`.
    *   *Pros*: Readable, portable, surprisingly fast on modern compilers.
    *   *Cons*: Scalar execution (one option at a time).
2.  **Reference**: Standard pointer-based implementation.
3.  **Value**: Pass-by-value semantics.
4.  **Optimized (AVX2 Basic)**: Uses explicitly vectorized approximations (`__m256`) for `exp`, `log`, and `cdf`.
5.  **AI Optimized (AVX2 + FMA + Unroll)**: "No-holds-barred" optimization.
    *   **Techniques**: FMA3 instructions, `rsqrt`/`rcp` approximations, 4x Loop Unrolling (32 options/iter).
    *   **Performance**: ~1.3 Billion options/sec (Single Thread).

## Prerequisites

*   **Compiler**: GCC (`g++`) or Clang with C++20 support.
*   **Hardware**: CPU with AVX2 and FMA3 support (Haswell or newer).
*   **OS**: Windows (MinGW), Linux, or macOS.

## Building and Running

### 1. Build
Use the provided `Makefile` to compile all implementations:
```bash
make
```

### 2. Run Benchmarks
Run the benchmark suite to compare throughput (10 Million options x 100 runs):
```bash
make run
```

### 3. Run Tests
Execute unit tests to verify correctness:
```bash
make test
```

## Performance Benchmark (Example Results)

| Implementation | Time per Option | Throughput | Speedup |
| :--- | :--- | :--- | :--- |
| **Naive (Std Lib)** | ~55.0 ns | ~18M/s | 1x |
| Reference | ~122.7 ns | ~8M/s | 0.45x |
| Optimized (AVX2) | ~0.91 ns | ~1.1B/s | 60x |
| **AI Optimized** | **~0.75 ns** | **~1.33B/s** | **73x** |

*Note: Results depend heavily on hardware capabilities.*
