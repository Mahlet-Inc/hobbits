function(install_hobbits_lib target headers)
	if (LINUX)
		install(TARGETS ${target}
				LIBRARY
				COMPONENT "runtime"
		)

		install(FILES ${headers}
				DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits"
				COMPONENT "dev"
		)
	elseif(APPLE)
		install(TARGETS ${target}
				LIBRARY
				COMPONENT "runtime"
				DESTINATION "hobbits.app/Contents/Frameworks"
		)
	elseif(WIN32)
		install(TARGETS ${target}
				COMPONENT "runtime"
				DESTINATION "."
		)
	endif()
endfunction(install_hobbits_lib)
