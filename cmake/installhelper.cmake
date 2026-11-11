function(hobbits_copy_runtime_dlls target)
    if(WIN32)
        # Build-tree executables and tests must run without adding the private Qt
        # installation to PATH. Installed Qt DLLs are handled by Qt deployment.
        set(copyScript "${CMAKE_CURRENT_BINARY_DIR}/copy-runtime-${target}-$<CONFIG>.cmake")
        file(GENERATE OUTPUT "${copyScript}" CONTENT "
set(runtimeDlls \"$<TARGET_RUNTIME_DLLS:${target}>\")
foreach(runtimeDll IN LISTS runtimeDlls)
    get_filename_component(runtimeName \"\${runtimeDll}\" NAME)
    if(runtimeName MATCHES \"^[Pp][Yy][Tt][Hh][Oo][Nn][0-9].*[.]dll$\")
        continue()
    endif()
    if(NOT \"\${runtimeDll}\" STREQUAL \"$<TARGET_FILE_DIR:${target}>/\${runtimeName}\")
        file(COPY_FILE \"\${runtimeDll}\" \"$<TARGET_FILE_DIR:${target}>/\${runtimeName}\" ONLY_IF_DIFFERENT)
    endif()
endforeach()
")
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -P "${copyScript}" VERBATIM)
    endif()
endfunction()

function(install_hobbits_lib target headers)
    hobbits_copy_runtime_dlls(${target})
    if(APPLE)
        set(libraryDir "hobbits.app/Contents/Frameworks")
    else()
        set(libraryDir "${CMAKE_INSTALL_LIBDIR}")
    endif()
    if(WIN32)
        set(runtimeDir ".")
    else()
        set(runtimeDir "${CMAKE_INSTALL_BINDIR}")
    endif()
    install(TARGETS ${target}
        RUNTIME DESTINATION "${runtimeDir}" COMPONENT runtime
        LIBRARY DESTINATION "${libraryDir}" COMPONENT runtime
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev)
    install(FILES ${headers}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hobbits" COMPONENT dev)
endfunction()
