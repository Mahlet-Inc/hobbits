if (TARGET PCAP::PCAP)
    pluginInDir("${pluginType}" "PacketCapture" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(hobbits-plugin-importerexporters-PacketCapture PRIVATE PCAP::PCAP Qt6::Network)
endif()
