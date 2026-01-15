#pragma once
#include <string>
#include <vector>

struct WavData {
    int sampleRate = 44100;
    int channels = 1;
    std::vector<float> mono; // normalized samples in [-1,1]
};

WavData readWavPcm16ToMono(const std::string& path);
void writeWavPcm16Mono(const std::string& path, int sampleRate, const std::vector<float>& mono);
