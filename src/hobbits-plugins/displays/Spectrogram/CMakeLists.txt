
pluginInDir("${pluginType}" "Spectrogram" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE CONAN_PKG::fftw)
