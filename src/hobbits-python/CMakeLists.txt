#
# Config Header
#

set(HOBBITS_PYTHON_VERSION "${BUILD_VERSION}")
set(HOBBITS_PYTHON_PYTHON_VERSION "${Python3_VERSION}")

set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitspythonconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")


#
# Build
#

if (WIN32)
    # this helps when using the hobbits windows python builds
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library(hobbits-python "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories(hobbits-python
							INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
							PRIVATE "${Python3_INCLUDE_DIRS}")

target_link_libraries(hobbits-python hobbits-core hobbits-widgets ${Python3_LIBRARIES})
target_compile_definitions(hobbits-python PRIVATE "-DHOBBITSPYTHON_LIBRARY")
#harden("hobbits-python")

set_target_properties(hobbits-python
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})
