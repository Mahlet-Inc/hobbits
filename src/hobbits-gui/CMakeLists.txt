#
# Config Header
#

set(HOBBITS_GUI_VERSION "${BUILD_VERSION}")
set(CONFIG_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/hobbitsguiconfig.h")
configure_file("${CONFIG_FILE_NAME}.in" "${CONFIG_FILE_NAME}")


#
# Build
#

file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB RCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")


if (BUILDING_WITH_CONAN)
	set(ALL_LINK_LIBS hobbits-python hobbits-widgets hobbits-core CONAN_PKG::qt)
else()
	set(ALL_LINK_LIBS hobbits-python hobbits-widgets hobbits-core Qt5::Widgets)
endif()

add_executable(hobbits MACOSX_BUNDLE "${SRCFILES}" "${HDRFILES}" "${RCFILES}")
target_link_libraries(hobbits ${ALL_LINK_LIBS})
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
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor/624x624/apps")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor/624x624")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/icons")
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CMAKE_INSTALL_FULL_DATADIR}/applications")

    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/Hobbits.desktop"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
            COMPONENT "runtime"
    )
    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/images/icons/HobbitsRingSmall.png"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/624x624/apps/
            COMPONENT "runtime"
    )
endif()
