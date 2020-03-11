set(QT_CREATOR_WIZARDS_BASE "${CMAKE_INSTALL_DATADIR}/qtcreator/templates/wizards/projects/")

macro(qtCreatorWizardInDir wizardName wizDir)
	set(wizardDirName "hobbits-plugin-${wizardName}")
	set(wizardPackageName "qtcreator-wizard-${wizardDirName}")
	file(GLOB_RECURSE SRCFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${wizDir}/*")
	add_custom_target(
		"${wizardPackageName}"
		SOURCES "${SRCFILES}"
	)

	string(TOUPPER "${wizardPackageName}" wizardPackageNameUpper)
	cpack_add_component("${wizardPackageName}" GROUP "qtcreator_wizards" DISPLAY_NAME "${wizardName}")

	set("CPACK_DEBIAN_${wizardPackageNameUpper}_PACKAGE_NAME" "${wizardPackageName}")
	set("CPACK_RPM_${wizardPackageNameUpper}_PACKAGE_NAME" "${wizardPackageName}")
	
	install(FILES ${SRCFILES}
		DESTINATION "${QT_CREATOR_WIZARDS_BASE}/${wizardDirName}"
		COMPONENT "${wizardPackageName}"
	)
	pass_through_cpack_vars()
endmacro()

cpack_add_component_group("qtcreator_wizards"
	DISPLAY_NAME "Qt Creator Wizards"
	DESCRIPTION "wizards to Qt Creator to make hobbits plugins"
	PARENT_GROUP "dev-all"
)
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

pass_through_cpack_vars()
