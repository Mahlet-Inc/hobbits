# A Qt installation with only offscreen/minimal plugins can compile Hobbits but
# cannot open its desktop window. Check the selected Qt, including system Qt,
# before configuring the rest of the application.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR HOBBITS_HEADLESS OR JUST_LIBS)
    return()
endif()

# QtGui discovers the desktop platform plugins. Load the optional WaylandClient
# module too for its shell/decoration plugins, keeping the lookup at directory
# scope so Qt's deployment helpers also see their metadata.
find_package(Qt6 6.8 QUIET OPTIONAL_COMPONENTS WaylandClient)

set(_hobbits_desktop_plugin "")
foreach(_plugin Qt6::QWaylandIntegrationPlugin
                Qt6::QWaylandEglPlatformIntegrationPlugin
                Qt6::QXcbIntegrationPlugin)
    if(NOT TARGET "${_plugin}")
        continue()
    endif()
    get_target_property(_plugin_type "${_plugin}" QT_PLUGIN_TYPE)
    get_target_property(_plugin_imported "${_plugin}" IMPORTED)
    if(NOT _plugin_imported OR NOT _plugin_type STREQUAL "platforms")
        continue()
    endif()

    get_target_property(_plugin_configs "${_plugin}" IMPORTED_CONFIGURATIONS)
    set(_plugin_location_properties IMPORTED_LOCATION)
    foreach(_config IN LISTS _plugin_configs)
        string(TOUPPER "${_config}" _config_upper)
        list(APPEND _plugin_location_properties "IMPORTED_LOCATION_${_config_upper}")
    endforeach()
    foreach(_property IN LISTS _plugin_location_properties)
        get_target_property(_plugin_location "${_plugin}" "${_property}")
        if(_plugin_location AND EXISTS "${_plugin_location}")
            set(_hobbits_desktop_plugin "${_plugin}")
            break()
        endif()
    endforeach()
    if(_hobbits_desktop_plugin)
        break()
    endif()
endforeach()

if(NOT _hobbits_desktop_plugin)
    message(FATAL_ERROR
        "The selected Qt ${Qt6_VERSION} (${Qt6_DIR}) has no installed Wayland or XCB "
        "desktop platform plugin. Hobbits cannot open a desktop window with only "
        "offscreen/minimal plugins. Install or build Qt's WaylandClient or XCB "
        "platform plugin and configure again. For intentional headless testing "
        "only, explicitly configure with -DHOBBITS_HEADLESS=ON.")
endif()
message(STATUS "Qt desktop platform plugin: ${_hobbits_desktop_plugin}")
