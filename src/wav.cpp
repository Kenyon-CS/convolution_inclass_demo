#include "wav.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cmath>

static uint32_t read_u32_le(std::ifstream& in) {
    uint8_t b[4]; in.read(reinterpret_cast<char*>(b), 4);
    if (!in) throw std::runtime_error("Unexpected EOF");
    return uint32_t(b[0]) | (uint32_t(b[1])<<8) | (uint32_t(b[2])<<16) | (uint32_t(b[3])<<24);
}
static uint16_t read_u16_le(std::ifstream& in) {
    uint8_t b[2]; in.read(reinterpret_cast<char*>(b), 2);
    if (!in) throw std::runtime_error("Unexpected EOF");
    return uint16_t(b[0]) | (uint16_t(b[1])<<8);
}
static void write_u32_le(std::ofstream& out, uint32_t v) {
    uint8_t b[4] = { uint8_t(v & 0xFF), uint8_t((v>>8)&0xFF), uint8_t((v>>16)&0xFF), uint8_t((v>>24)&0xFF) };
    out.write(reinterpret_cast<const char*>(b), 4);
}
static void write_u16_le(std::ofstream& out, uint16_t v) {
    uint8_t b[2] = { uint8_t(v & 0xFF), uint8_t((v>>8)&0xFF) };
    out.write(reinterpret_cast<const char*>(b), 2);
}

WavData readWavPcm16ToMono(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open WAV: " + path);

    char riff[4]; in.read(riff, 4);
    if (std::memcmp(riff, "RIFF", 4) != 0) throw std::runtime_error("Not a RIFF file");
    (void)read_u32_le(in);
    char wave[4]; in.read(wave, 4);
    if (std::memcmp(wave, "WAVE", 4) != 0) throw std::runtime_error("Not a WAVE file");

    uint16_t audioFormat = 0, numChannels = 0, bitsPerSample = 0;
    uint32_t sampleRate = 0;
    std::vector<int16_t> pcm;
    bool gotFmt=false, gotData=false;

    while (in && !(gotFmt && gotData)) {
        char chunkId[4];
        in.read(chunkId, 4);
        if (!in) break;
        uint32_t chunkSize = read_u32_le(in);

        if (std::memcmp(chunkId, "fmt ", 4) == 0) {
            audioFormat = read_u16_le(in);
            numChannels = read_u16_le(in);
            sampleRate  = read_u32_le(in);
            (void)read_u32_le(in); // byteRate
            (void)read_u16_le(in); // blockAlign
            bitsPerSample = read_u16_le(in);

            if (chunkSize > 16) in.seekg(static_cast<std::streamoff>(chunkSize - 16), std::ios::cur);
            gotFmt = true;
        } else if (std::memcmp(chunkId, "data", 4) == 0) {
            if (!gotFmt) throw std::runtime_error("WAV missing fmt before data");
            if (audioFormat != 1) throw std::runtime_error("Only PCM WAV supported");
            if (bitsPerSample != 16) throw std::runtime_error("Only 16-bit PCM supported");
            if (numChannels < 1 || numChannels > 2) throw std::runtime_error("Only mono/stereo supported");

            size_t samples = chunkSize / 2;
            pcm.resize(samples);
            in.read(reinterpret_cast<char*>(pcm.data()), static_cast<std::streamsize>(samples * sizeof(int16_t)));
            if (!in) throw std::runtime_error("Failed reading WAV data");
            gotData = true;
        } else {
            in.seekg(static_cast<std::streamoff>(chunkSize), std::ios::cur);
        }

        // pad byte
        if (chunkSize % 2 == 1) in.seekg(1, std::ios::cur);
    }

    if (!(gotFmt && gotData)) throw std::runtime_error("Incomplete WAV file");

    WavData w;
    w.sampleRate = static_cast<int>(sampleRate);
    w.channels   = static_cast<int>(numChannels);

    if (numChannels == 1) {
        w.mono.resize(pcm.size());
        for (size_t i=0;i<pcm.size();++i)
            w.mono[i] = std::clamp(pcm[i] / 32768.0f, -1.0f, 1.0f);
    } else {
        size_t frames = pcm.size()/2;
        w.mono.resize(frames);
        for (size_t i=0;i<frames;++i) {
            float L = pcm[2*i]   / 32768.0f;
            float R = pcm[2*i+1] / 32768.0f;
            w.mono[i] = std::clamp(0.5f*(L+R), -1.0f, 1.0f);
        }
    }
    return w;
}

void writeWavPcm16Mono(const std::string& path, int sampleRate, const std::vector<float>& mono) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open for write: " + path);

    uint16_t numChannels = 1;
    uint16_t bitsPerSample = 16;
    uint16_t blockAlign = numChannels * (bitsPerSample/8);
    uint32_t byteRate = uint32_t(sampleRate) * blockAlign;

    uint32_t dataBytes = uint32_t(mono.size()) * 2;
    uint32_t riffSize  = 4 + (8+16) + (8+dataBytes);

    out.write("RIFF",4);
    write_u32_le(out, riffSize);
    out.write("WAVE",4);

    out.write("fmt ",4);
    write_u32_le(out, 16);
    write_u16_le(out, 1); // PCM
    write_u16_le(out, numChannels);
    write_u32_le(out, uint32_t(sampleRate));
    write_u32_le(out, byteRate);
    write_u16_le(out, blockAlign);
    write_u16_le(out, bitsPerSample);

    out.write("data",4);
    write_u32_le(out, dataBytes);

    for (float s : mono) {
        float x = std::clamp(s, -1.0f, 1.0f);
        int32_t v = (int32_t)std::lrintf(x * 32767.0f);
        if (v < -32768) v = -32768;
        if (v >  32767) v =  32767;
        int16_t vv = (int16_t)v;
        out.write(reinterpret_cast<const char*>(&vv), sizeof(vv));
    }
    if (!out) throw std::runtime_error("Write failed: " + path);
}
