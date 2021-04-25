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
set(ALL_LINK_LIBS hobbits-python hobbits-widgets hobbits-core Qt5::Widgets)
if (FFTW_FOUND)
    add_definitions(-DFFTW_AVAILABLE)

    if(WIN32)
        list(APPEND ALL_LINK_LIBS FFTW::Double)
    else()
        list(APPEND ALL_LINK_LIBS FFTW::Double FFTW::DoubleThreads)
    endif()
    
    target_include_directories("hobbits-runner" PRIVATE "${FFTW_INCLUDE_DIRS}")
endif()
target_link_libraries("hobbits-runner" ${ALL_LINK_LIBS})
#harden("hobbits-runner")

if (APPLE)
    set_target_properties("hobbits-runner"
                          PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../Frameworks/python/lib")
elseif (LINUX AND SELF_CONTAINED_APP)
    set_target_properties("hobbits-runner"
                          PROPERTIES INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib;$ORIGIN/../lib64;$ORIGIN/../python/lib;$ORIGIN/../python/lib64")
endif()

# in dev mode, make sure dynamic link is to targetted python
if(DEVELOPMENT_BUILD)
    set_target_properties("hobbits-runner"
                          PROPERTIES BUILD_RPATH "${Python3_LIBRARY_DIRS}")
endif()


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

