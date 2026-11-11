#include "../src/hobbits-plugins/common/ffttransform.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {

void requireNear(std::complex<float> actual, std::complex<float> expected,
                 float tolerance, const char *message)
{
    if (std::abs(actual - expected) > tolerance || !std::isfinite(std::abs(actual))) {
        throw std::runtime_error(message);
    }
}

void verifyTransform(size_t size)
{
    const double pi = std::acos(-1.0);
    std::vector<std::complex<float>> input(size);
    for (size_t i = 0; i < size; ++i) {
        input[i] = {float(std::cos(6 * pi * i / size)), float(std::sin(10 * pi * i / size))};
    }
    std::vector<std::complex<float>> actual;
    HobbitsDsp::transform(input, actual, true);
    // Independent direct DFT checks bin ordering, phase sign and normalization.
    for (size_t bin = 0; bin < size; ++bin) {
        std::complex<double> expected;
        for (size_t i = 0; i < size; ++i) {
            expected += std::complex<double>(input[i])
                      * std::polar(1.0, -2 * pi * bin * i / size);
        }
        requireNear(actual[bin], std::complex<float>(expected), 1e-4f * size,
                    "Forward FFT differs from the direct DFT");
    }
    HobbitsDsp::transform(actual, actual, false, 1.0f / size);
    for (size_t i = 0; i < size; ++i) {
        requireNear(actual[i], input[i], 2e-5f, "FFT round trip differs from the input");
    }
}

void verifyAutocorrelation()
{
    std::vector<std::complex<float>> input(64);
    // Leave a partial frame zero-padded, as the width detector does.
    for (size_t i = 0; i < 47; ++i) {
        input[i] = (i % 7 < 3) ? 1.0f : -1.0f;
    }
    const auto actual = HobbitsDsp::autocorrelation(input);
    for (size_t lag = 0; lag < input.size(); ++lag) {
        float expected = 0;
        for (size_t i = 0; i < input.size(); ++i) {
            expected += input[i].real() * input[(i + lag) % input.size()].real();
        }
        expected = std::abs(expected / input.size());
        requireNear(actual[lag], expected, 2e-5f,
                    "Autocorrelation differs from the direct circular correlation");
    }
}

}

int main()
{
    try {
        for (size_t size : {1, 2, 31, 32, 63, 256}) {
            verifyTransform(size);
        }
        verifyAutocorrelation();
        std::vector<std::complex<float>> empty;
        HobbitsDsp::transform(empty, empty, true);
        if (!HobbitsDsp::autocorrelation(empty).empty()) {
            throw std::runtime_error("An empty input must have an empty autocorrelation");
        }
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "FFT and autocorrelation regressions passed\n";
}
