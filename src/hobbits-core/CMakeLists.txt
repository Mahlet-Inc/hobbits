#
# Config Header
#

if(WIN32)
	list(APPEND HOBBITS_PLUGINS_DEFAULT_PATH "%APPDATA%\\hobbits\\plugins")
else()
	list(APPEND HOBBITS_PLUGINS_DEFAULT_PATH "~/.local/share/hobbits/plugins")
endif()

list(APPEND HOBBITS_PLUGINS_DEFAULT_PATH "${HOBBITS_PLUGINS_SYSTEM_PATH}")
string(REPLACE ";" ":" HOBBITS_PLUGINS_DEFAULT_PATH "${HOBBITS_PLUGINS_DEFAULT_PATH}")

set(HOBBITS_CORE_VERSION "${BUILD_VERSION}")

set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitscoreconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")

#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")

add_library("hobbits-core" "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories("hobbits-core" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-core" Qt5::Core Qt5::Widgets)
target_compile_definitions("hobbits-core" PRIVATE "-DHOBBITSCORE_LIBRARY")
#harden("hobbits-core")

#
# CPack
#

cpack_add_component("core_lib")
set("CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME" "libhobbits")
set("CPACK_RPM_CORE_LIB_PACKAGE_NAME" "libhobbits")
set("CPACK_DEBIAN_CORE_LIB_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}")
set("CPACK_RPM_CORE_LIB_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets")

install(TARGETS "hobbits-core"
	LIBRARY
	COMPONENT "core_lib"
)

cpack_add_component("core_lib_dev")
set("CPACK_DEBIAN_CORE_LIB_DEV_PACKAGE_NAME" "${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}-dev")
set("CPACK_RPM_CORE_LIB_DEV_PACKAGE_NAME" "${CPACK_RPM_CORE_LIB_PACKAGE_NAME}-devel")
set("CPACK_DEBIAN_CORE_LIB_DEV_PACKAGE_DEPENDS" "qtbase${Qt5Core_VERSION_MAJOR}-dev")
set("CPACK_RPM_CORE_LIB_DEV_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore-devel, qt${Qt5Core_VERSION_MAJOR}-qtwidgets-devel")

#install(TARGETS "hobbits-core"
#	PUBLIC_HEADER
#	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits"
#	COMPONENT "core_lib_dev"
#)

install(FILES ${HDRFILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits"
	COMPONENT "core_lib_dev"
)

pass_through_cpack_vars()
