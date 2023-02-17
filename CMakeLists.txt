#
# CMake Setup
#

cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#
# Project Config
#

set(LINUX FALSE)
if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (DEFINED BUILD_VERSION)
    message("Using build version from command-line")
    set(VERSION_STRING "${BUILD_VERSION}")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/.version")
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
include(installhelper)

#
# Compile and Link Config
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS ON)

if (QT_NO_DEBUG)
    add_compile_definitions(QT_NO_DEBUG=1)
endif()

if (LINUX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-sized-deallocation")
    endif()
    add_link_options("LINKER:--disable-new-dtags")
endif()

if ((LINUX AND SELF_CONTAINED_APP) OR APPLE)
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    SET(CMAKE_INSTALL_RPATH "")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
	if (APPLE)
		set(MACOSX_RPATH TRUE)
		set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../Frameworks/hobbits-cpython/lib")
	elseif(LINUX)
		set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib;$ORIGIN/../lib64;$ORIGIN/../hobbits-cpython/lib;$ORIGIN/../hobbits-cpython/lib64")
	endif()
endif()

#
# Dependencies
#

if (BUILDING_WITH_CONAN)
	# add conan
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup(TARGETS KEEP_RPATHS NO_OUTPUT_DIRS)
	#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONAN_LIB_DIRS_QT}/cmake/Qt5Core")
	#include(conan_qt_executables_variables OPTIONAL)
	set(QT_CMAKE_EXPORT_NAMESPACE Qt5)
	set(QT_VERSION_MAJOR 5)
	set(QT_VERSION_MINOR 15)
	set(QT_VERSION_PATCH 2)
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::moc)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::moc IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::moc PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/moc${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5Core_MOC_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::moc)
	endif()
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::rcc)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::rcc IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::rcc PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/rcc${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5Core_RCC_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::rcc)
	endif()
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::qmake)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::qmake IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::qmake PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/qmake${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5Core_QMAKE_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::qmake)
	endif()
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::qdbuscpp2xml)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::qdbuscpp2xml IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::qdbuscpp2xml PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/qdbuscpp2xml${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5DBus_QDBUSCPP2XML_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::qdbuscpp2xml)
	endif()
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::qdbusxml2cpp)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::qdbusxml2cpp IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::qdbusxml2cpp PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/qdbusxml2cpp${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5DBus_QDBUSXML2CPP_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::qdbusxml2cpp)
	endif()
	if(NOT TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::uic)
		add_executable(${QT_CMAKE_EXPORT_NAMESPACE}::uic IMPORTED)
		set_target_properties(${QT_CMAKE_EXPORT_NAMESPACE}::uic PROPERTIES IMPORTED_LOCATION ${QT_PKG_ROOT_DIR}/bin/uic${CMAKE_EXECUTABLE_SUFFIX})
		set(Qt5Widgets_UIC_EXECUTABLE ${QT_CMAKE_EXPORT_NAMESPACE}::uic)
	endif()
else()
	find_package(PkgConfig)

	# Qt
	find_package(Qt5Core CONFIG REQUIRED)
	find_package(Qt5Widgets CONFIG REQUIRED)
	find_package(Qt5Network CONFIG REQUIRED)

	# libpcap
	if (NOT WIN32)
		include(FindPCAP)
		find_package(PCAP REQUIRED)
	endif()

	if(NOT WIN32)
		# Libusb
		find_package(libusb-1.0 REQUIRED)
	endif()

	# PFFFT
	option(USE_SYSTEM_PFFFT "Use a system-provided version of pffft" OFF)
	if(USE_SYSTEM_PFFFT)
		message(STATUS "Using system pffft")
		find_package(PFFFT REQUIRED)
	else()
		add_subdirectory(external)
	endif()

endif()

#
# Python Config
#

if (BUILDING_WITH_CONAN)
	add_compile_definitions(EMBEDDED_HOBBITS_CPYTHON=1)
	set ( Python3_ROOT_DIR "${CONAN_HOBBITS-CPYTHON_ROOT}" )
elseif( MANUAL_PYTHON_PATH )
	add_compile_definitions(EMBEDDED_HOBBITS_CPYTHON=1)
	message("Setting manual Python path")
	message("${MANUAL_PYTHON_PATH}")
	set ( Python3_ROOT_DIR "${MANUAL_PYTHON_PATH}" )
endif()

find_package (Python3 3.9 COMPONENTS Interpreter Development REQUIRED)
message("Python3 Libs: ${Python3_LIBRARIES}")

#
# Plugin installing and loading
#

if (LINUX)
	if (SELF_CONTAINED_APP)
		set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_INSTALL_BINDIR}/../plugins")
		list(APPEND HOBBITS_PLUGINS_PATHS "../plugins")
	else()
		set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_INSTALL_LIBDIR}/hobbits/plugins")
		list(APPEND HOBBITS_PLUGINS_PATHS "${CMAKE_INSTALL_FULL_LIBDIR}/hobbits/plugins")
		list(APPEND HOBBITS_PLUGINS_PATHS "/usr/lib/hobbits/plugins")
		list(APPEND HOBBITS_PLUGINS_PATHS "~/.local/share/hobbits/plugins")
	endif()
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
# CPack
#

if (NOT DEFINED CPACK_GENERATOR)
    if(LINUX)
        set(CPACK_GENERATOR "DEB")
    endif()
endif()

set(CPACK_PACKAGE_NAME "hobbits")
set(CPACK_PACKAGE_VENDOR "Hobbits Team")
set(CPACK_PACKAGE_DESCRIPTION "A multi-platform GUI for bit-based analysis, processing, and visualization.")
set(CPACK_HELP_LINK "https://mahlet-inc.github.io/")
set(CPACK_PACKAGE_CONTACT "adam@smr.llc")
set(CPACK_PACKAGE_CHECKSUM "SHA512")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/hobbits-gui/images/icons/HobbitsRingSmall.png")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_CREATE_DESKTOP_LINKS ON)

set(CPACK_DEBIAN_PACKAGE_NAME "hobbits")
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
set(CPACK_DEB_COMPONENT_INSTALL OFF)

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5widgets5, libqt5network5, libusb-1.0-0, libpcap0.8, python3")
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

#
# Add Files
#

add_subdirectory(src)

#
# Install
#

if (NOT BUILDING_WITH_CONAN)
	include(PackExternalDeps)
	if (SELF_CONTAINED_APP OR APPLE OR WIN32)
		pack_qt_libs()
		pack_python()
		pack_pffft()
		pack_pcap()
	endif()
	include(CPack)
endif()
