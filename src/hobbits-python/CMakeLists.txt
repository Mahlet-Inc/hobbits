#
# Config Header
#

if ( MANUAL_PYTHON_PATH )
    message("Setting manual Python path")
    message("${MANUAL_PYTHON_PATH}")
    set ( Python3_ROOT_DIR "${MANUAL_PYTHON_PATH}" )
endif()
find_package (Python3 3.9 EXACT COMPONENTS Interpreter Development REQUIRED)

set(HOBBITS_PYTHON_VERSION "${BUILD_VERSION}")
set(HOBBITS_PYTHON_PYTHON_VERSION "${Python3_VERSION}")

set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitspythonconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")


#
# Build
#

file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library("hobbits-python" "${SRCFILES}" "${HDRFILES}" "${RCFILES}")

target_include_directories("hobbits-python"
                           INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
                           PRIVATE "${Python3_INCLUDE_DIRS}")

target_link_libraries("hobbits-python" hobbits-core hobbits-widgets "${Python3_LIBRARIES}")
target_compile_definitions("hobbits-python" PRIVATE "-DHOBBITSPYTHON_LIBRARY")
#harden("hobbits-python")


#
# CPack
#

set("CPACK_DEBIAN_HOBBITS-PYTHON_PACKAGE_NAME" "libhobbits-python")
set("CPACK_RPM_HOBBITS-PYTHON_PACKAGE_NAME" "libhobbits-python")
set("CPACK_DEBIAN_HOBBITS-PYTHON_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
set("CPACK_RPM_HOBBITS-PYTHON_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")

install(TARGETS "hobbits-python"
	LIBRARY
	COMPONENT "hobbits-python"
)

set("CPACK_DEBIAN_HOBBITS-PYTHON-DEV_PACKAGE_NAME" "${CPACK_DEBIAN_HOBBITS-PYTHON_PACKAGE_NAME}-dev")
set("CPACK_RPM_HOBBITS-PYTHON-DEV_PACKAGE_NAME" "${CPACK_RPM_HOBBITS-PYTHON_PACKAGE_NAME}-devel")
set("CPACK_DEBIAN_HOBBITS-PYTHON-DEV_PACKAGE_DEPENDS" "qtbase${Qt5Core_VERSION_MAJOR}-dev, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}, libpython${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}-dev")
set("CPACK_RPM_HOBBITS-PYTHON-DEV_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}, python-devel")

install(FILES ${HDRFILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits/python"
	COMPONENT "hobbits-python-dev"
)

pass_through_cpack_vars()
