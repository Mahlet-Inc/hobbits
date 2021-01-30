
if (PCAP_FOUND)
    pluginInDir("${pluginType}" "PacketCapture" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("hobbits-plugin-importerexporters-PacketCapture" PRIVATE ${PCAP_LIBRARY} Qt5::Network)
    set("CPACK_DEBIAN_HOBBITS-PLUGIN-IMPORTEREXPORTERS-PACKETCAPTURE_PACKAGE_DEPENDS" "${CPACK_DEBIAN_HOBBITS-PLUGIN-IMPORTEREXPORTERS-PACKETCAPTURE_PACKAGE_DEPENDS}, libpcap, libqt${Qt5Network_VERSION_MAJOR}network${Qt5Network_VERSION_MAJOR}")
endif()
