pluginInDir("${pluginType}" "UdpData" "${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-importerexporters-UdpData" PRIVATE Qt5::Network)
endif()
