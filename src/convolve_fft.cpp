#include "convolve_fft.h"
#include "fft.h"
#include <complex>
#include <algorithm>

static size_t next_pow2(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

std::vector<float> convolve_fft_ola(const std::vector<float>& x,
                                    const std::vector<float>& h,
                                    size_t blockSize) {
    size_t N = x.size();
    size_t M = h.size();

    size_t Lmin = blockSize + M - 1;
    size_t L = next_pow2(Lmin);

    std::vector<std::complex<double>> H(L);
    for (size_t i = 0; i < L; ++i)
        H[i] = (i < M) ? std::complex<double>(h[i], 0.0) : 0.0;
    fft_inplace(H, false);

    std::vector<float> y(N + M - 1, 0.0f);
    std::vector<std::complex<double>> X(L), Y(L);

    for (size_t offset = 0; offset < N; offset += blockSize) {
        size_t len = std::min(blockSize, N - offset);

        std::fill(X.begin(), X.end(), 0.0);
        for (size_t i = 0; i < len; ++i)
            X[i] = std::complex<double>(x[offset + i], 0.0);

        fft_inplace(X, false);
        for (size_t i = 0; i < L; ++i)
            Y[i] = X[i] * H[i];
        fft_inplace(Y, true);

        for (size_t i = 0; i < Lmin; ++i) {
            size_t out = offset + i;
            if (out < y.size())
                y[out] += static_cast<float>(Y[i].real());
        }
    }
    return y;
}
