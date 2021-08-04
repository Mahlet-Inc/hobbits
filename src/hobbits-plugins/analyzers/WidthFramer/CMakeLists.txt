
pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")

if (BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE CONAN_PKG::fftw)
	target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE CONAN_PKG::pffft)
else()
	if(WIN32)
		target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE FFTW::Double)
    else()
		target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE FFTW::Double FFTW::DoubleThreads)
    endif()
endif()


