
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
                          "${QT_LIB_DIR}/QtCore.framework"
                          "${QT_LIB_DIR}/QtGui.framework"
                          "${QT_LIB_DIR}/QtWidgets.framework"
                DESTINATION "hobbits.app/Contents/Frameworks")
    elseif(WIN32)
        file(GLOB QTPLATFORMS "${QT_PLATFORM_DIR}/*.dll")
        install(FILES ${QTPLATFORMS}
                DESTINATION "platforms")
        file(GLOB QTTOOLS "${QT_ROOT_DIR}/../../Tools/*.dll")
        install(FILES ${QTTOOLS}
                DESTINATION ".")
        install(FILES "${QT_ROOT_DIR}/bin/Qt5Core.dll"
                      "${QT_ROOT_DIR}/bin/Qt5Gui.dll"
                      "${QT_ROOT_DIR}/bin/Qt5Widgets.dll"
                      "${QT_ROOT_DIR}/bin/Qt5Concurrent.dll"
                      "${QT_ROOT_DIR}/bin/Qt5Network.dll"
                      "${QT_ROOT_DIR}/bin/libEGL.dll"
                      "${QT_ROOT_DIR}/bin/opengl32sw.dll"
                DESTINATION ".")
    endif()

endfunction(pack_qt_libs)

function(pack_python)
    get_target_property(PYBIN Python3::Interpreter LOCATION)
    get_filename_component(PYBIN_DIR "${PYBIN}" DIRECTORY)
    get_filename_component(PYROOT_DIR "${PYBIN_DIR}" DIRECTORY)

	if (MANUAL_PYTHON_PATH)
		if (WIN32)
			file(GLOB PY_DIRS "${PYROOT_DIR}/bin/*")
			file(GLOB PY_FILES
				LIST_DIRECTORIES false
				"${PYROOT_DIR}/bin/*")
			list(REMOVE_ITEM PY_DIRS ${PY_FILES})

			install(FILES ${PY_FILES}
					DESTINATION ".")
			install(DIRECTORY ${PY_DIRS}
					DESTINATION ".")

		else()
			set(PY_DEST_DIR "hobbits-cpython")
			set(LIB_WILDCARD "*.so")
			if (APPLE)
				set(PY_DEST_DIR "hobbits.app/Contents/Frameworks/hobbits-cpython")
				set(LIB_WILDCARD "*.dylib")
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

			if(APPLE)
				file(GLOB INTLLIBS "/usr/local/opt/gettext/lib/libintl.*dylib")
				install(FILES ${INTLLIBS}
						DESTINATION "hobbits.app/Contents/Frameworks")
			endif()
		endif()
	endif()
endfunction(pack_python)

function(pack_pffft)
	install(FILES ${PFFFT_LIBRARIES}
			DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endfunction(pack_pcap)

function(pack_pcap)
    if (LINUX)
        string(REGEX REPLACE \.so\..*$ ".so" BASELIB ${PCAP_LIBRARY})
        file(GLOB PCAPLIBS "${BASELIB}*")
        install(FILES ${PCAPLIBS}
                DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    elseif(APPLE)
        file(GLOB PCAPLIBS "/usr/local/opt/libpcap/lib/libpcap.*dylib")
        install(FILES ${PCAPLIBS}
                DESTINATION "hobbits.app/Contents/Frameworks")
    elseif(WIN32)
    endif()
endfunction(pack_pcap)
