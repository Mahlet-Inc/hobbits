#
# Config Header
#

set(HOBBITS_PYTHON_VERSION "${BUILD_VERSION}")
set(HOBBITS_PYTHON_PYTHON_VERSION "${Python3_VERSION}")

configure_file(hobbitspythonconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/hobbitspythonconfig.h")


#
# Build
#

file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(FILTER HDRFILES EXCLUDE REGEX "/hobbitspythonconfig\\.h$")
list(APPEND HDRFILES "${CMAKE_CURRENT_BINARY_DIR}/hobbitspythonconfig.h")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library(hobbits-python "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories(hobbits-python
							PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
							PRIVATE "${Python3_INCLUDE_DIRS}")

target_link_libraries(hobbits-python PUBLIC hobbits-core hobbits-widgets PRIVATE Python3::Python Qt6::Concurrent)
target_compile_definitions(hobbits-python PRIVATE "-DHOBBITSPYTHON_LIBRARY")
if(MSVC)
    get_target_property(_python_debug_library Python3::Python IMPORTED_IMPLIB_DEBUG)
    if(NOT _python_debug_library)
        get_target_property(_python_debug_library Python3::Python IMPORTED_LOCATION_DEBUG)
    endif()
    if(NOT _python_debug_library)
        target_compile_definitions(hobbits-python PRIVATE HOBBITS_PYTHON_RELEASE_ABI)
    endif()
endif()
#harden("hobbits-python")

set_target_properties(hobbits-python
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})

install_hobbits_lib(hobbits-python "${HDRFILES}")
