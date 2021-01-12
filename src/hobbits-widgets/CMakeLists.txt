file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB resource_files "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library("hobbits-widgets" "${SRCFILES}" "${HDRFILES}" "${resource_files}")

target_include_directories("hobbits-widgets" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}" PRIVATE "${CORE_INCLUDE_DIR}")
get_target_property(WIDGETS_INCLUDE_DIR "hobbits-widgets" INTERFACE_INCLUDE_DIRECTORIES)
set(WIDGETS_INCLUDE_DIR "${WIDGETS_INCLUDE_DIR}" PARENT_SCOPE)

target_link_libraries("hobbits-widgets" hobbits-core Qt5::Core Qt5::Widgets)
target_compile_definitions("hobbits-widgets" PRIVATE "-DQT_DEPRECATED_WARNINGS" "-DHOBBITSWIDGETS_LIBRARY")
#harden("hobbits-widgets")

cpack_add_component("widgets_lib")
set("CPACK_DEBIAN_HOBBITS-WIDGETS_PACKAGE_NAME" "libhobbits-widgets")
set("CPACK_RPM_HOBBITS-WIDGETS_PACKAGE_NAME" "libhobbits-widgets")
set("CPACK_DEBIAN_HOBBITS-WIDGETS_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
set("CPACK_RPM_HOBBITS-WIDGETS_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")

install(TARGETS "hobbits-widgets"
	LIBRARY
        COMPONENT "widgets_lib"
)

cpack_add_component("widgets_lib_dev")
set("CPACK_DEBIAN_HOBBITS-WIDGETS-DEV_PACKAGE_NAME" "${CPACK_DEBIAN_HOBBITS-WIDGETS_PACKAGE_NAME}-dev")
set("CPACK_RPM_HOBBITS-WIDGETS-DEV_PACKAGE_NAME" "${CPACK_RPM_HOBBITS-WIDGETS_PACKAGE_NAME}-devel")
set("CPACK_DEBIAN_HOBBITS-WIDGETS-DEV_PACKAGE_DEPENDS" "qtbase${Qt5Core_VERSION_MAJOR}-dev, ${CPACK_DEBIAN_CORE_LIB_DEV_PACKAGE_NAME}")
set("CPACK_RPM_HOBBITS-WIDGETS-DEV_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_DEV_PACKAGE_NAME}")

install(FILES ${HDRFILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits/widgets"
        COMPONENT "widgets_lib_dev"
)

pass_through_cpack_vars()
