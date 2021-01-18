
function(pack_qt_libs)

    get_target_property(QT_CORE_LIB Qt5::Core LOCATION)
    get_filename_component(QT_LIB_DIR "${QT_CORE_LIB}" DIRECTORY)

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

    get_filename_component(QT_ROOT_DIR "${QT_LIB_DIR}" DIRECTORY)
    set(QT_PLATFORM_DIR "${QT_ROOT_DIR}/plugins/platforms")
    file(GLOB QTPLATFORMS "${QT_PLATFORM_DIR}/*.so*")
    list(FILTER QTPLATFORMS EXCLUDE REGEX .*debug$)

    install(FILES ${QTLIBS}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    install(FILES ${QTPLATFORMS}
            DESTINATION "${CMAKE_INSTALL_BINDIR}/platforms")

endfunction(pack_qt_libs)

function(pack_python)
    get_target_property(PYBIN Python3::Interpreter LOCATION)
    get_filename_component(PYBIN_DIR "${PYBIN}" DIRECTORY)
    get_filename_component(PYROOT_DIR "${PYBIN_DIR}" DIRECTORY)
    
    install( DIRECTORY "${PYROOT_DIR}/bin"
                DESTINATION "python"
                COMPONENT runtime)
    install( DIRECTORY "${PYROOT_DIR}/lib"
                DESTINATION "python"
                COMPONENT runtime)
endfunction(pack_python)

function(pack_fftw)
    get_target_property(FFTWLIB FFTW::Double INTERFACE_LINK_LIBRARIES)
    string(REGEX REPLACE \.so\..*$ ".so" BASELIB ${FFTWLIB})
    file(GLOB FFTWLIBS "${BASELIB}*")
    install(FILES ${FFTWLIBS}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endfunction(pack_fftw)

function(pack_pcap)
    string(REGEX REPLACE \.so\..*$ ".so" BASELIB ${PCAP_LIBRARY})
    file(GLOB PCAPLIBS "${BASELIB}*")
    install(FILES ${PCAPLIBS}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endfunction(pack_pcap)
