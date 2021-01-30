#
# Config Header
#

set(HOBBITS_CORE_VERSION "${BUILD_VERSION}")

set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitscoreconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")

#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")

add_library("hobbits-core" "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories("hobbits-core" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-core" Qt5::Core Qt5::Widgets)
target_compile_definitions("hobbits-core" PRIVATE "-DHOBBITSCORE_LIBRARY")
#harden("hobbits-core")

if (SELF_CONTAINED_APP AND LINUX)
    set_target_properties("hobbits-core"
                          PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()

#
# CPack
#

if (LINUX)
    install(TARGETS "hobbits-core"
            LIBRARY
            COMPONENT "runtime"
    )

    install(FILES ${HDRFILES}
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits"
            COMPONENT "dev"
    )
elseif(APPLE)
    install(TARGETS "hobbits-core"
            LIBRARY
            COMPONENT "runtime"
            DESTINATION "hobbits.app/Contents/Frameworks"
    )
elseif(WIN32)
    install(TARGETS "hobbits-core"
            COMPONENT "runtime"
            DESTINATION "."
    )
endif()
