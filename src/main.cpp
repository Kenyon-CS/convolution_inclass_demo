#include "wav.h"
#include "fir.h"
#include "convolve_naive.h"
#include "convolve_fft.h"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>

using Clock = std::chrono::steady_clock;

// Normalize to prevent clipping
static void normalize_if_needed(std::vector<float>& x) {
    float mx = 0.0f;
    for (float v : x) mx = std::max(mx, std::abs(v));
    if (mx > 1.0f && mx > 1e-9f) {
        float inv = 1.0f / mx;
        for (float& v : x) v *= inv;
    }
}

// Trim convolution output to original length (linear-phase FIR)
static std::vector<float> trim(const std::vector<float>& y_full,
                               size_t inputLen,
                               int taps) {
    size_t delay = (taps - 1) / 2;
    std::vector<float> y(inputLen, 0.0f);
    for (size_t i = 0; i < inputLen; ++i) {
        size_t src = i + delay;
        if (src < y_full.size()) y[i] = y_full[src];
    }
    return y;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./demo input.wav\n";
        return 1;
    }

    const int taps = 8193;           // long filter for algorithmic contrast
    const double cutoffHz = 9000.0;  // low-pass to reduce hiss
    const size_t blockSize = 16384;  // overlap-add block size

    auto w = readWavPcm16ToMono(argv[1]);
    std::cerr << "Loaded " << argv[1]
              << " (" << w.mono.size() << " samples)\n";

    auto h = designLowpassFIR(taps, cutoffHz, w.sampleRate);

    // Naïve convolution
    auto t0 = Clock::now();
    auto y_naive_full = convolve_naive(w.mono, h);
    auto y_naive = trim(y_naive_full, w.mono.size(), taps);
    normalize_if_needed(y_naive);
    auto t1 = Clock::now();

    // FFT convolution
    auto t2 = Clock::now();
    auto y_fft_full = convolve_fft_ola(w.mono, h, blockSize);
    auto y_fft = trim(y_fft_full, w.mono.size(), taps);
    normalize_if_needed(y_fft);
    auto t3 = Clock::now();

    writeWavPcm16Mono("out_naive.wav", w.sampleRate, y_naive);
    writeWavPcm16Mono("out_fft.wav", w.sampleRate, y_fft);

    double naiveS = std::chrono::duration<double>(t1 - t0).count();
    double fftS   = std::chrono::duration<double>(t3 - t2).count();

    std::cerr << "Naive time:   " << naiveS << " s\n";
    std::cerr << "FFT time:     " << fftS   << " s\n";
    std::cerr << "Speedup:      " << (naiveS / fftS) << "x\n";

    return 0;
}
