
if (NOT WIN32)
    pluginInDir("${pluginType}" "PacketCapture" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("hobbits-plugin-importerexporters-PacketCapture" PRIVATE CONAN_PKG::libpcap)
endif()
