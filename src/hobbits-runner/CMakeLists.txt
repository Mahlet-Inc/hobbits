#
# Config Header
#

set(HOBBITS_RUNNER_VERSION "${BUILD_VERSION}")
set(CONFIG_FILE_NAME "hobbitsrunnerconfig.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}.in" "${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}")

#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_executable("hobbits-runner" "${SRCFILES}"  "${HDRFILES}" "${RCFILES}")
target_link_libraries("hobbits-runner" hobbits-python hobbits-widgets hobbits-core Qt5::Widgets)
#harden("hobbits-runner")


#
# CPack
#

cpack_add_component("runner")
set("CPACK_DEBIAN_RUNNER_PACKAGE_NAME" "hobbits-runner")
set("CPACK_RPM_RUNNER_PACKAGE_NAME" "hobbits-runner")
set("CPACK_DEBIAN_RUNNER_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt5gui5, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
set("CPACK_RPM_RUNNER_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtbase, qt${Qt5Core_VERSION_MAJOR}-qtgui, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")

install(TARGETS "hobbits-runner" COMPONENT "runner")

pass_through_cpack_vars()
