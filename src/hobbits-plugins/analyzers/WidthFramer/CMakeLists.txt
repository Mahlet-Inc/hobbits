pluginInDir("${pluginType}" "WidthFramer" "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries("hobbits-plugin-analyzers-WidthFramer" PRIVATE PocketFFT::PocketFFT)
