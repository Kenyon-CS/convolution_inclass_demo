#pragma once
#include <vector>

// Windowed-sinc low-pass FIR (use odd taps)
std::vector<float> designLowpassFIR(int taps, double cutoffHz, int sampleRate);
