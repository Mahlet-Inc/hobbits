# Preserve upstream paths so similarly named third-party notices are not lost.
if(IS_DIRECTORY "${SOURCE_DIR}/LICENSES")
    file(INSTALL "${SOURCE_DIR}/LICENSES" DESTINATION "${DEST_DIR}" TYPE DIRECTORY)
endif()
file(GLOB_RECURSE _notices RELATIVE "${SOURCE_DIR}"
    "${SOURCE_DIR}/*LICENSE*" "${SOURCE_DIR}/*LICENCE*"
    "${SOURCE_DIR}/*COPYING*" "${SOURCE_DIR}/*COPYRIGHT*"
    "${SOURCE_DIR}/qt_attribution.json" "${SOURCE_DIR}/REUSE.toml")
foreach(_notice IN LISTS _notices)
    if(NOT IS_DIRECTORY "${SOURCE_DIR}/${_notice}")
        get_filename_component(_dir "${_notice}" DIRECTORY)
        file(INSTALL "${SOURCE_DIR}/${_notice}" DESTINATION "${DEST_DIR}/${_dir}")
    endif()
endforeach()
