#pragma once

#include <complex>
#include <vector>

// In-place radix-2 FFT
// If inverse == true, computes inverse FFT and scales by 1/N
void fft_inplace(std::vector<std::complex<double>>& a, bool inverse);
