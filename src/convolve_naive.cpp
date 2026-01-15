#include "convolve_naive.h"

std::vector<float> convolve_naive(const std::vector<float>& x,
                                  const std::vector<float>& h) {
    size_t N = x.size();
    size_t M = h.size();

    std::vector<float> y(N + M - 1, 0.0f);

    for (size_t n = 0; n < y.size(); ++n) {
        double sum = 0.0;
        size_t kmin = (n >= M - 1) ? (n - (M - 1)) : 0;
        size_t kmax = (n < N - 1) ? n : (N - 1);
        for (size_t k = kmin; k <= kmax; ++k) {
            sum += x[k] * h[n - k];
        }
        y[n] = static_cast<float>(sum);
    }
    return y;
}
