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

if (BUILDING_WITH_CONAN)
	set(ALL_LINK_LIBS CONAN_PKG::qt)
else()
	set(ALL_LINK_LIBS Qt5::Core Qt5::Widgets)
endif()

target_link_libraries(hobbits-core ${ALL_LINK_LIBS})
target_compile_definitions(hobbits-core PRIVATE "-DHOBBITSCORE_LIBRARY")
#harden("hobbits-core")

set_target_properties(hobbits-core
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})

install_hobbits_lib(hobbits-core "${HDRFILES}")
