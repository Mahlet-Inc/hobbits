
pluginInDir("${pluginType}" "Spectrogram" "${CMAKE_CURRENT_SOURCE_DIR}")


if (BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE CONAN_PKG::fftw)
else()
	if(WIN32)
		target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE FFTW::Double)
    else()
		target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE FFTW::Double FFTW::DoubleThreads)
    endif()
endif()
