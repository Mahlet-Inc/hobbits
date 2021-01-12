file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB resource_files "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library("hobbits-runner" "${SRCFILES}")
target_include_directories("hobbits-runner" PUBLIC "../hobbits-core" "../hobbits-python")
target_link_libraries("hobbits-runner" Qt5::Gui Qt5::Widgets)
target_compile_definitions("hobbits-runner" PRIVATE "-DQT_DEPRECATED_WARNINGS" "-DHOBBITS_RUNNER_VERSION=\"Extra Good Developer Version\"")
#harden("hobbits-runner")

cpack_add_component("runner")
set("CPACK_DEBIAN_RUNNER_PACKAGE_NAME" "hobbits-runner")
set("CPACK_RPM_RUNNER_PACKAGE_NAME" "hobbits-runner")
set("CPACK_DEBIAN_RUNNER_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt5gui5, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
set("CPACK_RPM_RUNNER_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtbase, qt${Qt5Core_VERSION_MAJOR}-qtgui, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")

install(TARGETS "hobbits-runner" COMPONENT "runner")

pass_through_cpack_vars()
