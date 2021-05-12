
pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")

add_definitions(-DFFTW_AUTOCORRELATION)
target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE CONAN_PKG::fftw)

