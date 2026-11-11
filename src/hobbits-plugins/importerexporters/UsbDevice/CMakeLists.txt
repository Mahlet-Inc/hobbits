if (TARGET libusb::libusb)
    pluginInDir("${pluginType}" "UsbDevice" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(hobbits-plugin-importerexporters-UsbDevice PRIVATE libusb::libusb)
endif()
