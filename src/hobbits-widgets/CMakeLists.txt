#
# Build
#

file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB resource_files "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_library("hobbits-widgets" "${SRCFILES}" "${HDRFILES}" "${resource_files}")
target_include_directories("hobbits-widgets" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("hobbits-widgets" hobbits-core Qt5::Core Qt5::Widgets)
target_compile_definitions("hobbits-widgets" PRIVATE "-DHOBBITSWIDGETS_LIBRARY")
#harden("hobbits-widgets")

if (LINUX AND SELF_CONTAINED_APP)
    set_target_properties("hobbits-widgets"
                          PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()

#
# CPack
#

if (LINUX)
    install(TARGETS "hobbits-widgets"
            LIBRARY
            COMPONENT "runtime"
    )

    install(FILES ${HDRFILES}
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits"
            COMPONENT "dev"
    )
endif()
