#pragma once
#include <cmath>
#include <cstdint>

namespace sysp::core::algorithm {

inline int64_t gcd(int64_t a, int64_t b) {
    while (b) { a %= b; std::swap(a, b); }
    return a;
}

inline int64_t lcm(int64_t a, int64_t b) {
    return a / gcd(a, b) * b;
}

inline bool is_prime(int64_t n) {
    if (n < 2) return false;
    for (int64_t i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

inline double newton_sqrt(double n) {
    double x = n;
    for (int i = 0; i < 50; i++)
        x = (x + n / x) / 2.0;
    return x;
}

} // namespace sysp::core::algorithm
