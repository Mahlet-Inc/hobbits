cmake_minimum_required(VERSION 3.15)
project("hobbits")
include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CPackComponent)

SET(MANUAL_PYTHON_PATH "" CACHE STRING "user-specified python root path")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(Hardening)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/findFFTW")

macro(pass_through_cpack_vars)
	get_cmake_property(cpackVarsToPassthrough VARIABLES)
	foreach(varName ${cpackVarsToPassthrough})
		if(varName MATCHES "^CPACK_DEBIAN_")
			#message(STATUS "${varName}")
			set("${varName}" "${${varName}}" PARENT_SCOPE)
		endif()
	endforeach()
endmacro()


option(HOBBITS_DEVELOPMENT "Plugins directory is relative to the binary" OFF)

if(WIN32 OR HOBBITS_DEVELOPMENT)
	list(APPEND HOBBITS_PLUGINS_SYSTEM_PATH "../hobbits-plugins" "../plugins")
endif()

if(NOT WIN32)
	list(APPEND HOBBITS_PLUGINS_SYSTEM_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/hobbits/plugins")
endif()

set(CXX_STANDARD 20)
set(CMAKE_USE_RELATIVE_PATHS ON)
set(BUILD_SHARED_LIBS ON)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

set(CPACK_PACKAGE_NAME "hobbits-all")
set(CPACK_PACKAGE_VENDOR "hobbits Team")

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/docs/CPack.hobbits.description.txt")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/Mahlet-Inc/hobbits")
set(CPACK_HELP_LINK "https://github.com/Mahlet-Inc/hobbits/tree/master/docs")
set(CPACK_PACKAGE_CONTACT "${CPACK_PACKAGE_HOMEPAGE_URL}")
set(CPACK_PACKAGE_CHECKSUM "SHA512")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/hobbits-gui/images/icons/HobbitsRingSmall.png")
#set(CPACK_GENERATOR "TXZ")
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
set(CPACK_RPM_COMPRESSION_TYPE "xz")
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)

set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")

add_subdirectory(src)
add_subdirectory(wizards)

set(CPACK_CREATE_DESKTOP_LINKS ON)

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)

if(NOT HOBBITS_DEVELOPMENT)
	include(CPack)
endif()
