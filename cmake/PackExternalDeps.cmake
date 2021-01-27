
function(pack_qt_libs)

    get_target_property(QT_CORE_LIB Qt5::Core LOCATION)
    if (APPLE)
        get_filename_component(CORE_FRAMEWORK_DIR "${QT_CORE_LIB}" DIRECTORY)
        get_filename_component(QT_LIB_DIR "${CORE_FRAMEWORK_DIR}" DIRECTORY)
    else()
        get_filename_component(QT_LIB_DIR "${QT_CORE_LIB}" DIRECTORY)
    endif()
    get_filename_component(QT_ROOT_DIR "${QT_LIB_DIR}" DIRECTORY)
    set(QT_PLATFORM_DIR "${QT_ROOT_DIR}/plugins/platforms")

    if (LINUX)
        file(GLOB LIBICU "${QT_LIB_DIR}/libicu*.so*")
        file(GLOB LIBCORE "${QT_LIB_DIR}/libQt5Core.so*")
        file(GLOB LIBCON "${QT_LIB_DIR}/libQt5Concurrent.so*")
        file(GLOB LIBDBUS "${QT_LIB_DIR}/libQt5DBus.so*")
        file(GLOB LIBGUI "${QT_LIB_DIR}/libQt5Gui.so*")
        file(GLOB LIBNET "${QT_LIB_DIR}/libQt5Network.so*")
        file(GLOB LIBPRNT "${QT_LIB_DIR}/libQt5PrintSupport.so*")
        file(GLOB LIBWAY "${QT_LIB_DIR}/libQt5WaylandClient.so*")
        file(GLOB LIBWID "${QT_LIB_DIR}/libQt5Widgets.so*")
        file(GLOB LIBXCB "${QT_LIB_DIR}/libQt5XcbQpa.so*")

        list(APPEND QTLIBS
                    ${LIBICU}
                    ${LIBCORE}
                    ${LIBCON}
                    ${LIBDBUS}
                    ${LIBGUI}
                    ${LIBNET}
                    ${LIBPRNT}
                    ${LIBWID}
                    ${LIBWAY}
                    ${LIBXCB}
                    )

        list(FILTER QTLIBS EXCLUDE REGEX .*debug$)

        file(GLOB QTPLATFORMS "${QT_PLATFORM_DIR}/*.so*")
        list(FILTER QTPLATFORMS EXCLUDE REGEX .*debug$)

        install(FILES ${QTLIBS}
                DESTINATION "${CMAKE_INSTALL_LIBDIR}")

        install(FILES ${QTPLATFORMS}
                DESTINATION "${CMAKE_INSTALL_BINDIR}/platforms")

        foreach(PLATFILEFULL ${QTPLATFORMS})
            get_filename_component(PLATFILE "${PLATFILEFULL}" NAME)
            install(
                CODE
                "
                message(\"Updating RPATH of bin/platforms/${PLATFILE}\")
                execute_process(
                    COMMAND patchelf --remove-rpath \${CMAKE_INSTALL_PREFIX}/bin/platforms/${PLATFILE}
                )
                execute_process(
                    COMMAND patchelf --force-rpath --set-rpath \\$ORIGIN/../../lib64:\\$ORIGIN/../../lib \${CMAKE_INSTALL_PREFIX}/bin/platforms/${PLATFILE}
                )"
            )
        endforeach()
    elseif(APPLE)
        file(GLOB QTPLATFORMS "${QT_PLATFORM_DIR}/*.dylib")
        install(FILES ${QTPLATFORMS}
                DESTINATION "hobbits.app/Contents/PlugIns/platforms")
        install(DIRECTORY "${QT_LIB_DIR}/QtPrintSupport.framework"
                          "${QT_LIB_DIR}/QtDBus.framework"
                          "${QT_LIB_DIR}/QtNetwork.framework"
                          "${QT_LIB_DIR}/QtConcurrent.framework"
                DESTINATION "hobbits.app/Contents/Frameworks")
    endif()

endfunction(pack_qt_libs)

function(pack_python)
    get_target_property(PYBIN Python3::Interpreter LOCATION)
    get_filename_component(PYBIN_DIR "${PYBIN}" DIRECTORY)
    get_filename_component(PYROOT_DIR "${PYBIN_DIR}" DIRECTORY)

    set(PY_DEST_DIR "python")
    set(LIB_WILDCARD "*.so")
    if (APPLE)
        set(PY_DEST_DIR "hobbits.app/Contents/Frameworks/python")
        set(LIB_WILDCARD "*.dylib")
    elseif(WIN32)
        set(LIB_WILDCARD "*.dll")
    endif()

    install( DIRECTORY "${PYROOT_DIR}/bin"
                DESTINATION "${PY_DEST_DIR}"
                COMPONENT runtime
                PATTERN "*"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE)
    install( DIRECTORY "${PYROOT_DIR}/lib"
                DESTINATION "${PY_DEST_DIR}"
                COMPONENT runtime
                PATTERN "${LIB_WILDCARD}"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE)
endfunction(pack_python)

function(pack_fftw)
    if (LINUX)
        get_target_property(FFTWLIB FFTW::Double INTERFACE_LINK_LIBRARIES)
        string(REGEX REPLACE \.so\..*$ ".so" BASELIB ${FFTWLIB})
        file(GLOB FFTWLIBS "${BASELIB}*")
        install(FILES ${FFTWLIBS}
                DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    elseif(APPLE)
        get_target_property(FFTWLIB FFTW::Double INTERFACE_LINK_LIBRARIES)
        install(FILES ${FFTWLIBS}
                DESTINATION "hobbits.app/Contents/Frameworks")
    elseif(WIN32)
    endif()
endfunction(pack_fftw)

function(pack_pcap)
    if (LINUX)
        string(REGEX REPLACE \.so\..*$ ".so" BASELIB ${PCAP_LIBRARY})
        file(GLOB PCAPLIBS "${BASELIB}*")
        install(FILES ${PCAPLIBS}
                DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    elseif(APPLE)
        install(FILES ${PCAP_LIBRARY}
                DESTINATION "hobbits.app/Contents/Frameworks")
    elseif(WIN32)
    endif()
endfunction(pack_pcap)
