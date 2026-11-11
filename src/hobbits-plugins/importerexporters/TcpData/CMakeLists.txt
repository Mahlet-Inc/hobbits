pluginInDir("${pluginType}" "TcpData" "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries("hobbits-plugin-importerexporters-TcpData" PRIVATE Qt6::Network)
