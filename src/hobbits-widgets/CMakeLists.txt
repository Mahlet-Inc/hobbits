#
# Build
#

file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB resource_files "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library(hobbits-widgets ${SRCFILES} ${HDRFILES} ${resource_files})
target_include_directories(hobbits-widgets INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(hobbits-widgets hobbits-core)
target_compile_definitions(hobbits-widgets PRIVATE "-DHOBBITSWIDGETS_LIBRARY")
#harden("hobbits-widgets")

set_target_properties(hobbits-widgets
			PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_LIB_OUT_DIR})

install_hobbits_lib(hobbits-widgets "${HDRFILES}")
