#
# Config Header
#

set(HOBBITS_GUI_VERSION "${BUILD_VERSION}")
set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitsguiconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")


#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_executable("hobbits" "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_link_libraries("hobbits" hobbits-python hobbits-widgets hobbits-core Qt5::Widgets)
#harden("hobbits")


#
# CPack
#

cpack_add_component("gui")
set("CPACK_DEBIAN_GUI_PACKAGE_NAME" "hobbits")
set("CPACK_RPM_GUI_PACKAGE_NAME" "hobbits")
set("CPACK_DEBIAN_GUI_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
set("CPACK_RPM_GUI_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")

list(APPEND CPACK_PACKAGE_EXECUTABLES "hobbits;Hobbits")

install(TARGETS "hobbits" COMPONENT "gui")
install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/Hobbits.desktop"
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
	COMPONENT "gui"
)
install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/images/icons/HobbitsRingSmall.png"
	DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/624x624/apps/
	COMPONENT "gui"
)

pass_through_cpack_vars()
