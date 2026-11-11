# Use Qt's platform deployment tools and include dynamically loaded Hobbits plugins
# in dependency discovery. Scanning just the GUI misses QtNetwork and libusb.
function(pack_qt_libs)
    # Offscreen is not a default Qt plugin, but installed command-line and CI runs
    # need it when no desktop session is available.
    if(TARGET Qt6::QOffscreenIntegrationPlugin)
        qt_import_plugins(hobbits INCLUDE Qt6::QOffscreenIntegrationPlugin)
    endif()
    # WaylandClient is loaded through the platform plugin, so deployment's initial
    # library scan does not discover its shell and window-decoration plugin types.
    if(TARGET Qt6::WaylandClient)
        target_link_libraries(hobbits PRIVATE Qt6::WaylandClient)
    endif()
    foreach(plugin QWaylandIntegrationPlugin QWaylandXdgShellIntegrationPlugin
            QWaylandBradientDecorationPlugin QWaylandAdwaitaDecorationPlugin)
        if(TARGET Qt6::${plugin})
            qt_import_plugins(hobbits INCLUDE Qt6::${plugin})
        endif()
    endforeach()
    if(UNIX AND NOT APPLE)
        # An explicitly scanned library is not itself included in CMake's list of
        # resolved dependencies. Install these roots as well as their dependencies.
        install(IMPORTED_RUNTIME_ARTIFACTS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Concurrent
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT runtime)
    endif()
    get_property(pluginTargets GLOBAL PROPERTY HOBBITS_PLUGIN_TARGETS)
    set(modules)
    foreach(plugin IN LISTS pluginTargets)
        if(APPLE)
            # macdeployqt must modify the installed plugins' library references.
            get_target_property(pluginTypeDir "${plugin}" LIBRARY_OUTPUT_DIRECTORY)
            get_filename_component(pluginType "${pluginTypeDir}" NAME)
            string(APPEND modules "\n    \"\${QT_DEPLOY_PREFIX}/${HOBBITS_PLUGINS_INSTALL_PATH}/${pluginType}/$<TARGET_FILE_NAME:${plugin}>\"")
        else()
            string(APPEND modules "\n    \"$<TARGET_FILE:${plugin}>\"")
        endif()
    endforeach()

    if(APPLE)
        set(executable "hobbits.app")
        set(runner "\${QT_DEPLOY_PREFIX}/hobbits.app/Contents/MacOS/$<TARGET_FILE_NAME:hobbits-runner>")
        set(platformOptions "")
    else()
        set(executable "$<TARGET_FILE:hobbits>")
        set(runner "$<TARGET_FILE:hobbits-runner>")
        if(WIN32)
            set(binDir ".")
        else()
            set(binDir "${CMAKE_INSTALL_BINDIR}")
        endif()
        set(platformOptions "GENERATE_QT_CONF\n    BIN_DIR \"${binDir}\"\n    LIB_DIR \"${CMAKE_INSTALL_LIBDIR}\"\n    PLUGINS_DIR \"plugins\"")
    endif()

    # The application's own libraries are already installed with their final RPATH.
    # Do not overwrite them with build-tree copies during Qt's dependency scan.
    set(excludedLibraries)
    foreach(library hobbits-core hobbits-widgets hobbits-python)
        if(TARGET "${library}")
            string(APPEND excludedLibraries "\n    \"$<TARGET_FILE:${library}>\"")
        endif()
    endforeach()
    if(HOBBITS_WITH_PYTHON)
        string(APPEND excludedLibraries "\n    \"$<TARGET_FILE:Python3::Python>\"")
    endif()

    # Scan Qt libraries explicitly too: excluding an installed Hobbits library from
    # copying can also stop recursive scanning of its Qt dependencies.
    set(qtLibraries)
    if(NOT APPLE)
        foreach(library Core Gui Widgets Network Concurrent)
            string(APPEND qtLibraries "\n    \"$<TARGET_FILE:Qt6::${library}>\"")
        endforeach()
    endif()
    qt_generate_deploy_script(TARGET hobbits OUTPUT_SCRIPT deployScript CONTENT "
set(QT_DEPLOY_BIN_DIR \"${binDir}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${executable}\"
    ADDITIONAL_EXECUTABLES \"${runner}\"
    ADDITIONAL_MODULES ${modules}
    ADDITIONAL_LIBRARIES ${qtLibraries}
    POST_EXCLUDE_FILES ${excludedLibraries}
    ${platformOptions}
    NO_TRANSLATIONS)
")
    install(SCRIPT "${deployScript}" COMPONENT runtime)
endfunction()
