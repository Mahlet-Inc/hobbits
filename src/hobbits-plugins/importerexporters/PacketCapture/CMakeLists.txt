
if (NOT WIN32)
    pluginInDir("${pluginType}" "PacketCapture" "${CMAKE_CURRENT_SOURCE_DIR}")

	if (BUILDING_WITH_CONAN)
		target_link_libraries("hobbits-plugin-importerexporters-PacketCapture" PRIVATE CONAN_PKG::libpcap)
	else()
		target_link_libraries("hobbits-plugin-importerexporters-PacketCapture" PRIVATE ${PCAP_LIBRARY} Qt5::Network)
	endif()
    
endif()
