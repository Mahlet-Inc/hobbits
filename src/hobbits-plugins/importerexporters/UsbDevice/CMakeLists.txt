
pluginInDir("${pluginType}" "UsbDevice" "${CMAKE_CURRENT_SOURCE_DIR}")

if (BUILDING_WITH_CONAN)
	target_link_libraries("hobbits-plugin-importerexporters-UsbDevice" PRIVATE CONAN_PKG::libusb)
else()
	target_link_libraries("hobbits-plugin-importerexporters-UsbDevice" usb-1.0)
endif()