pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE FFTW::Double)
target_include_directories("hobbits-plugin-analyzers-WidthFramer" PRIVATE "${FFTW_INCLUDE_DIRS}")
