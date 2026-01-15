#include "fir.h"
#include <cmath>
#include <numeric>

static double sinc(double x) {
    if (std::abs(x) < 1e-12) return 1.0;
    return std::sin(M_PI * x) / (M_PI * x);
}

std::vector<float> designLowpassFIR(int taps,
                                    double cutoffHz,
                                    int sampleRate) {
    int M = taps;
    int c = (M - 1) / 2;
    double fc = cutoffHz / sampleRate;

    std::vector<double> h(M);
    for (int m = 0; m < M; ++m) {
        int n = m - c;
        double ideal = 2 * fc * sinc(2 * fc * n);
        double w = 0.54 - 0.46 * std::cos(2 * M_PI * m / (M - 1));
        h[m] = ideal * w;
    }

    double sum = std::accumulate(h.begin(), h.end(), 0.0);
    std::vector<float> out(M);
    for (int i = 0; i < M; ++i)
        out[i] = static_cast<float>(h[i] / sum);
    return out;
}
