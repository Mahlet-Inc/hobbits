set(QT_CREATOR_WIZARDS_BASE "${CMAKE_INSTALL_DATADIR}/qtcreator/templates/wizards/projects/")

list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/${CMAKE_INSTALL_DATADIR}/qtcreator/templates/wizards/projects")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/${CMAKE_INSTALL_DATADIR}/qtcreator/templates/wizards")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/${CMAKE_INSTALL_DATADIR}/qtcreator/templates")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/${CMAKE_INSTALL_DATADIR}/qtcreator")
pass_through_cpack_vars()

macro(qtCreatorWizardInDir wizardName wizDir)
	set(wizardDirName "hobbits-plugin-${wizardName}")
	set(wizardPackageName "qtcreator-wizard-${wizardDirName}")
	file(GLOB_RECURSE SRCFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${wizDir}/*")
	add_custom_target(
		"${wizardPackageName}"
		SOURCES "${SRCFILES}"
	)

        string(TOUPPER "${wizardPackageName}" wizardPackageNameUpper)
	
	install(FILES ${SRCFILES}
		DESTINATION "${QT_CREATOR_WIZARDS_BASE}/${wizardDirName}"
                COMPONENT "dev"
        )
endmacro()

file(GLOB wizardsDirzInThisDir
	LIST_DIRECTORIES true
	"${CMAKE_CURRENT_SOURCE_DIR}/*"
)
foreach(wizDir IN LISTS wizardsDirzInThisDir)
	if(IS_DIRECTORY "${wizDir}")
		get_filename_component(wizardName "${wizDir}" NAME)
		message(STATUS "Processing ${wizardName} module ${wizardName}")
		if(EXISTS "${wizDir}/CMakeLists.txt")
			add_subdirectory("${wizDir}")
		else()
			qtCreatorWizardInDir("${wizardName}" "${wizDir}" )
		endif()
	endif()
endforeach()

