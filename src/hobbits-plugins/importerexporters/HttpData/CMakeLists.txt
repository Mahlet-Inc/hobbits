
find_package(Qt5Network CONFIG REQUIRED)
pluginInDir("${pluginType}" "HttpData" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-plugin-importerexporters-HttpData" PRIVATE Qt5::Network)
set("CPACK_DEBIAN_HOBBITS-PLUGIN-IMPORTEREXPORTERS-HTTPDATA_PACKAGE_DEPENDS" "${CPACK_DEBIAN_HOBBITS-PLUGIN-IMPORTEREXPORTERS-HTTPDATA_PACKAGE_DEPENDS}, libqt${Qt5Network_VERSION_MAJOR}network${Qt5Network_VERSION_MAJOR}")
