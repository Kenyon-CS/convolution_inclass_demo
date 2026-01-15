#include "fft.h"
#include <cmath>
#include <stdexcept>

static inline bool is_pow2(size_t n){ return n && ((n & (n-1)) == 0); }

void fft_inplace(std::vector<std::complex<double>>& a, bool inverse) {
    size_t n = a.size();
    if (!is_pow2(n)) throw std::runtime_error("FFT size must be power of two");

    for (size_t i=1, j=0; i<n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }

    for (size_t len=2; len<=n; len<<=1) {
        double ang = 2.0 * M_PI / (double)len * (inverse ? 1.0 : -1.0);
        std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (size_t i=0; i<n; i+=len) {
            std::complex<double> w(1.0, 0.0);
            for (size_t j=0; j<len/2; ++j) {
                auto u = a[i+j];
                auto v = a[i+j+len/2] * w;
                a[i+j] = u + v;
                a[i+j+len/2] = u - v;
                w *= wlen;
            }
        }
    }

    if (inverse) {
        for (auto& x : a) x /= (double)n;
    }
}
