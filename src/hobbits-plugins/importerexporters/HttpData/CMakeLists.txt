pluginInDir("${pluginType}" "HttpData" "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries("hobbits-plugin-importerexporters-HttpData" PRIVATE Qt6::Network)
