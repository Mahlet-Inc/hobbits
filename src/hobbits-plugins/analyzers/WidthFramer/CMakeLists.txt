
pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")

if (BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE CONAN_PKG::pffft)
else()
	target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE PFFFT::PFFFT)
endif()


