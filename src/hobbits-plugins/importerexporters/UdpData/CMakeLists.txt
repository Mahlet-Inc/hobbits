pluginInDir("${pluginType}" "UdpData" "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries("hobbits-plugin-importerexporters-UdpData" PRIVATE Qt6::Network)
