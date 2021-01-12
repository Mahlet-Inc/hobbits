file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB resource_files "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

if ( MANUAL_PYTHON_PATH )
    message("Setting manual Python path")
    message("${MANUAL_PYTHON_PATH}")
    set ( Python3_ROOT_DIR "${MANUAL_PYTHON_PATH}" )
endif()
find_package (Python3 3.9 EXACT COMPONENTS Interpreter Development REQUIRED)

add_library("hobbits-python" "${SRCFILES}")

target_include_directories("hobbits-python" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}" PRIVATE "${CORE_INCLUDE_DIR}" "${WIDGETS_INCLUDE_DIR}" "${Python3_INCLUDE_DIRS}")

get_target_property(PYHOBBITS_INCLUDE_DIR "hobbits-python" INTERFACE_INCLUDE_DIRECTORIES)
set(PYHOBBITS_INCLUDE_DIR "${PYHOBBITS_INCLUDE_DIR}" PARENT_SCOPE)

target_link_libraries("hobbits-python" hobbits-core hobbits-widgets "${Python3_LIBRARIES}")
target_compile_definitions("hobbits-python" PRIVATE "-DQT_DEPRECATED_WARNINGS" "-DHOBBITSPYTHON_LIBRARY")
#target_compile_definitions("hobbits-python" PRIVATE "-DQT_DEPRECATED_WARNINGS" "-DHOBBITSPYTHON_LIBRARY" "-DPYTHON_LINK_LIB="\"\""")
#harden("hobbits-python")

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
