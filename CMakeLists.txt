cmake_minimum_required(VERSION 3.15)
include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CPackComponent)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/findFFTW")
include(Hardening)
include(passthroughvars)


#
# Project Config
#

if (EXISTS "${CMAKE_SOURCE_DIR}/.version")
    message("Getting build version from .version file")
    file (STRINGS "${CMAKE_SOURCE_DIR}/.version" BUILD_VERSION)
    set(VERSION_STRING "${BUILD_VERSION}")
else()
    message("Attempting to get build version from Git")
    include(gitversion)
    gitversion(BUILD_VERSION)
    set(VERSION_STRING "0.0.0")
endif()

message("Building hobbits version: ${BUILD_VERSION}")

project("hobbits"
        VERSION "${VERSION_STRING}"
        HOMEPAGE_URL "https://github.com/Mahlet-Inc/hobbits")


#
# Dynamic Config
#

SET(MANUAL_PYTHON_PATH "" CACHE STRING "user-specified python root path")

option(HOBBITS_DEVELOPMENT "Plugins directory is relative to the binary" OFF)

if(WIN32 OR HOBBITS_DEVELOPMENT)
	list(APPEND HOBBITS_PLUGINS_SYSTEM_PATH "../hobbits-plugins" "../plugins")
endif()

if(NOT WIN32)
	list(APPEND HOBBITS_PLUGINS_SYSTEM_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/hobbits/plugins")
endif()


#
# CPack
#

if (NOT DEFINED CPACK_GENERATOR)
    if(WIN32)
        set(CPACK_GENERATOR "ZIP;NSIS")
    else()
        set(CPACK_GENERATOR "TXZ;RPM;DEB")
    endif()
endif()

set(CPACK_PACKAGE_NAME "hobbits")
set(CPACK_PACKAGE_VENDOR "Hobbits Team")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/docs/CPack.hobbits.description.txt")
set(CPACK_HELP_LINK "https://mahlet-inc.github.io/")
set(CPACK_PACKAGE_CONTACT "adam@mahletconsulting.com")
set(CPACK_PACKAGE_CHECKSUM "SHA512")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/hobbits-gui/images/icons/HobbitsRingSmall.png")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_CREATE_DESKTOP_LINKS ON)

set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")

set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_RPM_COMPRESSION_TYPE "xz")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_COMPONENT_INSTALL ON)
list(APPEND CPACK_RPM_RELOCATION_PATHS "${HOBBITS_PLUGINS_SYSTEM_PATH}")

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

#
# Add Files
#

add_subdirectory(src)
add_subdirectory(wizards)

include(CPack)
