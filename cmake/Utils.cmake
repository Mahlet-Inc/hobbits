
function(all_lib_extensions ALLVAR LIBVAR)

    foreach(LIBFILE in ${LIBVAR})
        string(REGEX MATCH "${LIBFILE}" ^.*\.so.*$ THEMATCH)
        if (NOT THEMATCH)
        list(APPEND ALL_FILES ${LIBVAR})
        while(TRUE)

        endwhile()
    endforeach()

    set("${ALLVAR}" ${ALL_FILES} PARENT_SCOPE)
endfunction(all_lib_extensions)