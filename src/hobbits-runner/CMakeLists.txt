#
# Config Header
#

set(HOBBITS_RUNNER_VERSION "${BUILD_VERSION}")
configure_file(hobbitsrunnerconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/hobbitsrunnerconfig.h")

#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(FILTER HDRFILES EXCLUDE REGEX "/hobbitsrunnerconfig\\.h$")
list(APPEND HDRFILES "${CMAKE_CURRENT_BINARY_DIR}/hobbitsrunnerconfig.h")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_executable("hobbits-runner" "${SRCFILES}"  "${HDRFILES}" "${RCFILES}")

set(ALL_LINK_LIBS hobbits-widgets hobbits-core Qt6::Widgets)
if (HOBBITS_WITH_PYTHON)
    list(APPEND ALL_LINK_LIBS hobbits-python)
endif()

target_include_directories(hobbits-runner PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries("hobbits-runner" PRIVATE ${ALL_LINK_LIBS})
target_compile_definitions(hobbits-runner PRIVATE HOBBITS_WITH_PYTHON=$<BOOL:${HOBBITS_WITH_PYTHON}>)
hobbits_copy_runtime_dlls(hobbits-runner)
if (UNIX)
	target_compile_options(hobbits-runner PRIVATE -fPIC)
endif()
#harden("hobbits-runner")

set_target_properties(hobbits-runner
			PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${HOBBITS_BIN_OUT_DIR})


#
# CPack
#
if (LINUX)
    install(TARGETS "hobbits-runner" COMPONENT "runtime")
elseif(APPLE)
    install(TARGETS "hobbits-runner"
            COMPONENT "runtime"
            DESTINATION "hobbits.app/Contents/MacOS"
    )
elseif(WIN32)
    install(TARGETS "hobbits-runner"
            COMPONENT "runtime"
            DESTINATION "."
    )
endif()

