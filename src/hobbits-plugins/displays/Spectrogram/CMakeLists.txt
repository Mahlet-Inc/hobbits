find_package(FFTW REQUIRED)
pluginInDir("${pluginType}" "Spectrogram" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE FFTW::Double)
target_include_directories("hobbits-plugin-displays-Spectrogram" PRIVATE "${FFTW_INCLUDE_DIRS}")
