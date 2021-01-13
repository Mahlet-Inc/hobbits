macro(pluginInDir pluginType modName modDir)
	file(GLOB_RECURSE SRCFILES "${modDir}/*.c" "${modDir}/*.cpp")
        file(GLOB RCFILES "${modDir}/*.qrc" "${modDir}/*.ui")

	set(pluginName "hobbits-plugin-${pluginType}-${modName}")
        add_library("${pluginName}" "${SRCFILES}" "${RCFILES}")

        target_link_libraries("${pluginName}" PRIVATE hobbits-core hobbits-widgets Qt5::Core Qt5::Widgets)
        #harden("${pluginName}")

	string(TOUPPER "${pluginName}" pluginNameUpper)
	cpack_add_component("${pluginName}" GROUP "${pluginType}")
	
	set("CPACK_DEBIAN_${pluginNameUpper}_PACKAGE_NAME" "lib${pluginName}")
	set("CPACK_RPM_${pluginNameUpper}_PACKAGE_NAME" "lib${pluginName}")
	set("CPACK_DEBIAN_${pluginNameUpper}_PACKAGE_DEPENDS" "libqt${Qt5Core_VERSION_MAJOR}core${Qt5Core_VERSION_MAJOR}a, libqt${Qt5Widgets_VERSION_MAJOR}widgets${Qt5Widgets_VERSION_MAJOR}, ${CPACK_DEBIAN_CORE_LIB_PACKAGE_NAME}")
	set("CPACK_RPM_${pluginNameUpper}_PACKAGE_DEPENDS" "qt${Qt5Core_VERSION_MAJOR}-qtcore, qt${Qt5Core_VERSION_MAJOR}-qtwidgets, ${CPACK_RPM_CORE_LIB_PACKAGE_NAME}")
	
	install(TARGETS "${pluginName}"
		DESTINATION "${HOBBITS_PLUGINS_SYSTEM_PATH}/${pluginType}"
		COMPONENT "${pluginName}"
	)
	pass_through_cpack_vars()
endmacro()

macro(processPluginsType pluginTypeDir)
	get_filename_component(pluginType "${pluginTypeDir}" NAME)
	cpack_add_component_group("${pluginType}"
		PARENT_GROUP "plugins"
	)
	file(GLOB pluginsDirzInThisDir
		LIST_DIRECTORIES true
		"${pluginTypeDir}/*"
	)
	foreach(modP IN LISTS pluginsDirzInThisDir)
		if(IS_DIRECTORY "${modP}")
			get_filename_component(modN "${modP}" NAME)
			message(STATUS "Processing ${pluginType} module ${modN}")
			if(EXISTS "${modP}/CMakeLists.txt")
				add_subdirectory("${modP}")
			else()
				# doesn't work IDK why, probably Qt machinery relies on entering into a dir
				pluginInDir("${pluginType}" "${modN}" "${modP}" )
			endif()
		endif()
	endforeach()

	pass_through_cpack_vars()
endmacro()

cpack_add_component_group("plugins"
	DESCRIPTION "Plugins to Hobbits GUI middleware actually doing the work"
)

file(GLOB pluginGroupDirzInThisDir
	LIST_DIRECTORIES true
	"${CMAKE_CURRENT_SOURCE_DIR}/*"
)
foreach(pluginTypeDir IN LISTS pluginGroupDirzInThisDir)
	if(IS_DIRECTORY "${pluginTypeDir}")
		processPluginsType("${pluginTypeDir}")
	endif()
endforeach()

pass_through_cpack_vars()
