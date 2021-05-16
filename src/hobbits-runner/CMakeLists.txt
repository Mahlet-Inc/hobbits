#
# Config Header
#

set(HOBBITS_RUNNER_VERSION "${BUILD_VERSION}")
set(CONFIG_FILE_NAME "hobbitsrunnerconfig.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}.in" "${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}")

#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_executable("hobbits-runner" "${SRCFILES}"  "${HDRFILES}" "${RCFILES}")

if (BUILDING_WITH_CONAN)
	set(ALL_LINK_LIBS hobbits-python hobbits-widgets hobbits-core CONAN_PKG::qt CONAN_PKG::fftw)
else()
	set(ALL_LINK_LIBS hobbits-python hobbits-widgets hobbits-core Qt5::Widgets)
	if(WIN32)
        list(APPEND ALL_LINK_LIBS FFTW::Double)
    else()
        list(APPEND ALL_LINK_LIBS FFTW::Double FFTW::DoubleThreads)
    endif()
endif()

target_link_libraries("hobbits-runner" ${ALL_LINK_LIBS})
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

