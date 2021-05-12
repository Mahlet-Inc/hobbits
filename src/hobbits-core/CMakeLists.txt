#
# Config Header
#

set(HOBBITS_CORE_VERSION "${BUILD_VERSION}")

set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitscoreconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")

#
# Build
#

# Gather Files
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")

# Configure target
add_library(hobbits-core "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories(hobbits-core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(hobbits-core CONAN_PKG::qt)
target_compile_definitions(hobbits-core PRIVATE "-DHOBBITSCORE_LIBRARY")
#harden("hobbits-core")

set_target_properties(hobbits-core
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})
