#
# Config Header
#

set(HOBBITS_CORE_VERSION "${BUILD_VERSION}")

configure_file(hobbitscoreconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/hobbitscoreconfig.h")

#
# Build
#

# Gather Files
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(FILTER HDRFILES EXCLUDE REGEX "/hobbitscoreconfig\\.h$")
list(APPEND HDRFILES "${CMAKE_CURRENT_BINARY_DIR}/hobbitscoreconfig.h")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")

# Configure target
add_library(hobbits-core "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories(hobbits-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(hobbits-core BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(hobbits-core Qt6::Core Qt6::Widgets Qt6::Concurrent)
target_compile_definitions(hobbits-core PRIVATE "-DHOBBITSCORE_LIBRARY")
#harden("hobbits-core")

set_target_properties(hobbits-core
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})

install_hobbits_lib(hobbits-core "${HDRFILES}")
