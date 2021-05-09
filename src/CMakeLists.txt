#
# CMake Setup
#

cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#
# Project Config
#

set(LINUX FALSE)
if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/.version")
    message("Getting build version from .version file")
    file (STRINGS "${CMAKE_SOURCE_DIR}/.version" BUILD_VERSION)
    string(REGEX REPLACE "\n$" "" BUILD_VERSION "${BUILD_VERSION}")
    string(STRIP "${BUILD_VERSION}" BUILD_VERSION)
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

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CPackComponent)
include(Hardening)
include(passthroughvars)

#
# Compile and Link Config
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS ON)


if (LINUX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-sized-deallocation")
    endif()
    add_link_options("LINKER:--disable-new-dtags")
endif()

if (UNIX)
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    SET(CMAKE_INSTALL_RPATH "")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
	if (APPLE)
		set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../Frameworks/hobbits-cpython/lib")
	elseif(LINUX)
		set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib;$ORIGIN/../lib64;$ORIGIN/../hobbits-cpython/lib;$ORIGIN/../hobbits-cpython/lib64")
	endif()
endif()

#
# Dependencies
#

# add conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS KEEP_RPATHS NO_OUTPUT_DIRS)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONAN_LIB_DIRS_QT}/cmake/Qt5Core")
include(conan_qt_executables_variables)

#
# Python Config
#

set ( Python3_ROOT_DIR "${CONAN_HOBBITS-CPYTHON_ROOT}" )
find_package (Python3 3.9 EXACT COMPONENTS Interpreter Development REQUIRED)
message("Python3 Libs: ${Python3_LIBRARIES}")

#
# Plugin installing and loading
#

if (LINUX)
    set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_INSTALL_BINDIR}/../plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "../plugins")
elseif(WIN32)
    set(HOBBITS_PLUGINS_INSTALL_PATH "plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "plugins")
elseif(APPLE)
    set(HOBBITS_PLUGINS_INSTALL_PATH "hobbits.app/Contents/PlugIns/hobbits")
    list(APPEND HOBBITS_PLUGINS_PATHS "../PlugIns/hobbits")
endif()

string(REPLACE ";" ":" HOBBITS_PLUGINS_PATH "${HOBBITS_PLUGINS_PATHS}")

#
# Binary build locations
#

set(HOBBITS_BIN_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(HOBBITS_LIB_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(HOBBITS_PLUGIN_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/plugins)


#
# Add Files
#

add_subdirectory(hobbits-core)
add_subdirectory(hobbits-widgets)
add_subdirectory(hobbits-python)
add_subdirectory(hobbits-plugins)
add_subdirectory(hobbits-gui)
