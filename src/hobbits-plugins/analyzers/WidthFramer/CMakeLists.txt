
pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")
if (FFTW_FOUND)
    add_definitions(-DFFTW_AUTOCORRELATION)
    target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE FFTW::Double)
    target_include_directories("hobbits-plugin-analyzers-WidthFramer" PRIVATE "${FFTW_INCLUDE_DIRS}")
endif()
