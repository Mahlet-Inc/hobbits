# This package deliberately uses Ubuntu 24.04's Python, independently of the
# Python 3.14 runtime embedded in standalone archives.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR HOBBITS_BUNDLE_PYTHON OR
   NOT HOBBITS_WITH_PYTHON OR NOT Python3_VERSION MATCHES "^3[.]12[.]")
    message(FATAL_ERROR "The Ubuntu DEB requires Linux, Python 3.12, and HOBBITS_BUNDLE_PYTHON=OFF")
endif()
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3.12, libpython3.12t64")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1ubuntu24.04")

# Launchers and the desktop entry are on the normal system paths; the binaries,
# Qt libraries, and Qt configuration remain in the application's private tree.
foreach(program hobbits hobbits-runner)
    set(launcher "${CMAKE_CURRENT_BINARY_DIR}/${program}-launcher")
    file(WRITE "${launcher}" "#!/bin/sh\nexec \"/usr/${CMAKE_INSTALL_BINDIR}/${program}\" \"$@\"\n")
    install(PROGRAMS "${launcher}" DESTINATION bin RENAME "${program}" COMPONENT runtime)
endforeach()
