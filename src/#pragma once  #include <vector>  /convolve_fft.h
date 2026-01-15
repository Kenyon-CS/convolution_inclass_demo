#pragma once

#include <vector>

// FFT-based overlap-add convolution
std::vector<float> convolve_fft_ola(const std::vector<float>& x,
                                    const std::vector<float>& h,
                                    size_t blockSize);
