#
# Config Header
#

set(HOBBITS_GUI_VERSION "${BUILD_VERSION}")
configure_file(hobbitsguiconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/hobbitsguiconfig.h")


#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(FILTER HDRFILES EXCLUDE REGEX "/hobbitsguiconfig\\.h$")
list(APPEND HDRFILES "${CMAKE_CURRENT_BINARY_DIR}/hobbitsguiconfig.h")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")


set(ALL_LINK_LIBS hobbits-widgets hobbits-core Qt6::Widgets)
if (HOBBITS_WITH_PYTHON)
    list(APPEND ALL_LINK_LIBS hobbits-python)
endif()

qt_add_executable(hobbits MACOSX_BUNDLE "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_include_directories(hobbits PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
# Include network plugin metadata in Qt deployment, even when networking is used by plugins only.
target_link_libraries(hobbits PRIVATE ${ALL_LINK_LIBS} Qt6::Network)
target_compile_definitions(hobbits PRIVATE HOBBITS_WITH_PYTHON=$<BOOL:${HOBBITS_WITH_PYTHON}>)
hobbits_copy_runtime_dlls(hobbits)
if (UNIX)
	target_compile_options(hobbits PRIVATE -fPIC)
endif()
#harden("hobbits")

set_target_properties(hobbits
			PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${HOBBITS_BIN_OUT_DIR})

if(WIN32)
    install(TARGETS "hobbits"
            COMPONENT "runtime"
            DESTINATION "."
    )
elseif(APPLE)
    install(TARGETS "hobbits" COMPONENT "runtime" BUNDLE DESTINATION ".")
else()
    install(TARGETS "hobbits" COMPONENT "runtime")
endif()

if (LINUX)
    list(APPEND CPACK_PACKAGE_EXECUTABLES "hobbits;Hobbits")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/applications")

    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/Hobbits.desktop"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
            COMPONENT "runtime"
    )
    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/images/icons/HobbitsRingSmall.png"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
            COMPONENT "runtime"
    )
endif()
