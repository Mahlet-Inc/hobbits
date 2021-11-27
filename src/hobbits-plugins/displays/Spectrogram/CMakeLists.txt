
pluginInDir("${pluginType}" "Spectrogram" "${CMAKE_CURRENT_SOURCE_DIR}")


if (BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE CONAN_PKG::pffft)
else()
	target_link_libraries("hobbits-plugin-displays-Spectrogram" PRIVATE PFFFT::PFFFT)
endif()
