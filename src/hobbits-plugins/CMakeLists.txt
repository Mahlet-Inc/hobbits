macro(pluginInDir pluginType modName modDir)
	file(GLOB_RECURSE SRCFILES "${modDir}/*.c" "${modDir}/*.cpp")
        file(GLOB RCFILES "${modDir}/*.qrc" "${modDir}/*.ui")
        file(GLOB_RECURSE HDRFILES "${modDir}/*.h" "${modDir}/*.hpp") # this is not necessary for building, but groups the files beter in IDEs

	set(pluginName "hobbits-plugin-${pluginType}-${modName}")
        add_library("${pluginName}" "${SRCFILES}" "${RCFILES}" "${HDRFILES}")
		set_target_properties("${pluginName}"
							PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${HOBBITS_PLUGIN_OUT_DIR}/${pluginType}
                                       RUNTIME_OUTPUT_DIRECTORY ${HOBBITS_PLUGIN_OUT_DIR}/${pluginType})

		set(ALL_LINK_LIBS hobbits-core hobbits-widgets Qt6::Core Qt6::Widgets)

        foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
            string(TOUPPER "${config}" configUpper)
            set_target_properties("${pluginName}" PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY_${configUpper} "${HOBBITS_OUTPUT_ROOT}/${config}/plugins/${pluginType}"
                RUNTIME_OUTPUT_DIRECTORY_${configUpper} "${HOBBITS_OUTPUT_ROOT}/${config}/plugins/${pluginType}")
        endforeach()
        if(APPLE)
            set_target_properties("${pluginName}" PROPERTIES
                INSTALL_RPATH "@loader_path/../../../Frameworks")
        elseif(UNIX)
            file(RELATIVE_PATH pluginLibraryPath
                "${CMAKE_INSTALL_PREFIX}/${HOBBITS_PLUGINS_INSTALL_PATH}/${pluginType}"
                "${CMAKE_INSTALL_FULL_LIBDIR}")
            set_target_properties("${pluginName}" PROPERTIES
                INSTALL_RPATH "$ORIGIN/${pluginLibraryPath}")
        endif()
        set_property(GLOBAL APPEND PROPERTY HOBBITS_PLUGIN_TARGETS "${pluginName}")
        target_link_libraries("${pluginName}" PRIVATE ${ALL_LINK_LIBS})
        #harden("${pluginName}")

        string(TOUPPER "${pluginName}" pluginNameUpper)

	install(TARGETS "${pluginName}"
                DESTINATION "${HOBBITS_PLUGINS_INSTALL_PATH}/${pluginType}"
                COMPONENT "runtime"
        )
endmacro()

macro(processPluginsType pluginTypeDir)
        get_filename_component(pluginType "${pluginTypeDir}" NAME)

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
