#ifndef HOBBITS_FFTTRANSFORM_H
#define HOBBITS_FFTTRANSFORM_H

#include <complex>
#include <vector>
#include <pocketfft_hdronly.h>

namespace HobbitsDsp {

// Both directions are unnormalized, matching the convention used by the
// spectrogram and width detector. Callers explicitly choose any scaling.
inline void transform(const std::vector<std::complex<float>> &input,
                      std::vector<std::complex<float>> &output,
                      bool forward, float scale = 1.0f)
{
    output.resize(input.size());
    if (input.empty()) {
        return;
    }
    const pocketfft::shape_t shape{input.size()};
    const pocketfft::stride_t stride{sizeof(std::complex<float>)};
    pocketfft::c2c(shape, stride, stride, {0}, forward,
                  input.data(), output.data(), scale);
}

// Circular autocorrelation, normalized by the transform length. Zero-padding
// the input before calling this also supports short bit streams.
inline std::vector<float> autocorrelation(const std::vector<std::complex<float>> &input)
{
    if (input.empty()) {
        return {};
    }
    const float inverseSize = 1.0f / float(input.size());
    std::vector<std::complex<float>> spectrum;
    transform(input, spectrum, true);
    for (auto &sample : spectrum) {
        sample = std::complex<float>(std::norm(sample) * inverseSize, 0.0f);
    }
    transform(spectrum, spectrum, false, inverseSize);
    std::vector<float> result(input.size());
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = std::abs(spectrum[i].real());
    }
    return result;
}

} // namespace HobbitsDsp

#endif
