include(ExternalProject)
set(HOBBITS_DEPS_PREFIX "${CMAKE_BINARY_DIR}/dependencies" CACHE PATH "Private dependency installation")
set(HOBBITS_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/downloads" CACHE PATH "Dependency archive cache")
set(HOBBITS_DEPENDENCY_JOBS 4 CACHE STRING "Parallel jobs per dependency build")
set(_deps)
set(_common -DCMAKE_INSTALL_PREFIX:PATH=${HOBBITS_DEPS_PREFIX}
    -DCMAKE_INSTALL_LIBDIR:STRING=lib -DCMAKE_BUILD_TYPE:STRING=Release
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON)
foreach(_var CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_TOOLCHAIN_FILE CMAKE_CONFIGURATION_TYPES
        CMAKE_OSX_ARCHITECTURES CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_OSX_SYSROOT)
    if(DEFINED ${_var})
        string(REPLACE ";" "|" _value "${${_var}}")
        list(APPEND _common "-D${_var}:STRING=${_value}")
    endif()
endforeach()
if(NOT HOBBITS_USE_SYSTEM_QT)
    set(_qt_deps)
    set(_qt_options -DQT_BUILD_TESTS=OFF -DQT_BUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=ON
        -DBUILD_WITH_PCH=OFF -DFEATURE_icu=OFF -DFEATURE_sql=OFF
        -DFEATURE_system_zlib=OFF -DFEATURE_system_pcre2=OFF
        -DFEATURE_system_png=OFF -DFEATURE_system_jpeg=OFF
        -DFEATURE_system_freetype=OFF -DFEATURE_system_harfbuzz=OFF)
    if(UNIX AND NOT APPLE)
        if(HOBBITS_ENABLE_WAYLAND AND NOT HOBBITS_HEADLESS)
            # These headers and the protocol scanner are the native display SDK.
            # Diagnose their absence before spending time compiling Qt itself.
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(HOBBITS_WAYLAND_SDK REQUIRED wayland-client wayland-cursor xkbcommon)
            find_program(HOBBITS_WAYLAND_SCANNER wayland-scanner REQUIRED)
        endif()
        # Linux has no native Qt TLS backend. Build OpenSSL so HTTPS support does
        # not depend on development libraries already installed on the host.
        find_program(HOBBITS_PERL perl REQUIRED)
        find_program(HOBBITS_MAKE NAMES gmake make REQUIRED)
        ExternalProject_Add(openssl_external
            URL https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz
            URL_HASH SHA256=603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a
            DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "CC=${CMAKE_C_COMPILER}"
                ${HOBBITS_PERL} <SOURCE_DIR>/Configure no-shared no-tests no-apps no-module -fPIC
                --prefix=${HOBBITS_DEPS_PREFIX} --libdir=lib --openssldir=/etc/ssl
            BUILD_COMMAND ${HOBBITS_MAKE} -j${HOBBITS_DEPENDENCY_JOBS}
            INSTALL_COMMAND ${HOBBITS_MAKE} install_sw)
        list(APPEND _qt_deps openssl_external)
        list(APPEND _qt_options -DINPUT_openssl=linked -DFEATURE_openssl=ON -DFEATURE_ssl=ON -DFEATURE_openssl_linked=ON
            -DOPENSSL_USE_STATIC_LIBS=TRUE -DOPENSSL_ROOT_DIR:PATH=${HOBBITS_DEPS_PREFIX})
    endif()
    if(HOBBITS_HEADLESS)
        list(APPEND _qt_options -DFEATURE_xcb=OFF -DFEATURE_wayland=OFF -DINPUT_opengl=no)
    elseif(UNIX AND NOT APPLE)
        if(HOBBITS_ENABLE_WAYLAND)
            # Native Wayland needs the separate, version-matched QtWayland module.
            # Also build XCB when its SDK is present. Clear previous forced values
            # so switching from headless or X11-only builds restores autodetection.
            list(APPEND _qt_options -DFEATURE_wayland=ON -DFEATURE_xkbcommon=ON
                -UFEATURE_xcb -UQT_FEATURE_xcb -DINPUT_opengl=no)
        else()
            list(APPEND _qt_options -DFEATURE_wayland=OFF -DFEATURE_xcb=ON -DINPUT_opengl=no)
        endif()
    endif()
    get_property(_qt_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(NOT _qt_multi_config)
        # Debug Qt and application binaries must agree on CRT and plugin metadata.
        list(APPEND _qt_options "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}")
    endif()
    ExternalProject_Add(qt6_external
        LIST_SEPARATOR "|"
        URL https://download.qt.io/archive/qt/6.8/6.8.3/submodules/qtbase-everywhere-src-6.8.3.tar.xz
        URL_HASH SHA256=56001b905601bb9023d399f3ba780d7fa940f3e4861e496a7c490331f49e0b80
        DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        DEPENDS ${_qt_deps}
        CMAKE_ARGS ${_common} ${_qt_options}
        BUILD_ALWAYS ${_qt_multi_config}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --parallel ${HOBBITS_DEPENDENCY_JOBS}
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>)
    list(APPEND _deps qt6_external)
    if(UNIX AND NOT APPLE AND HOBBITS_ENABLE_WAYLAND AND NOT HOBBITS_HEADLESS)
        set(_wayland_options -DQT_BUILD_TESTS=OFF -DQT_BUILD_EXAMPLES=OFF
            -DBUILD_WITH_PCH=OFF -DFEATURE_wayland_client=ON -DFEATURE_wayland_server=OFF
            -DCMAKE_PREFIX_PATH:PATH=${HOBBITS_DEPS_PREFIX})
        if(NOT _qt_multi_config)
            list(APPEND _wayland_options "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}")
        endif()
        ExternalProject_Add(qtwayland_external
            LIST_SEPARATOR "|"
            URL https://download.qt.io/archive/qt/6.8/6.8.3/submodules/qtwayland-everywhere-src-6.8.3.tar.xz
            URL_HASH SHA256=20fe385887d21190165a3180c17dcfc8b9a0e1da4ec76865b6334bdc709994b0
            DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            DEPENDS qt6_external
            CMAKE_ARGS ${_common} ${_wayland_options}
            BUILD_ALWAYS ${_qt_multi_config}
            BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --parallel ${HOBBITS_DEPENDENCY_JOBS}
            INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>)
        list(APPEND _deps qtwayland_external)
    endif()
endif()

if(NOT WIN32 AND NOT JUST_LIBS)
    find_program(HOBBITS_MAKE NAMES gmake make REQUIRED)
    find_program(HOBBITS_M4 m4)
    find_program(HOBBITS_FLEX flex)
    find_program(HOBBITS_BISON bison)
    set(_parser_deps)
    if(NOT HOBBITS_M4)
        ExternalProject_Add(m4_external
            URL https://ftp.gnu.org/gnu/m4/m4-1.4.19.tar.xz
            URL_HASH SHA256=63aede5c6d33b6d9b13511cd0be2cac046f2e70fd0a07aa9573a04a82783af96
            DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${HOBBITS_DEPS_PREFIX}
            BUILD_COMMAND ${HOBBITS_MAKE} -j${HOBBITS_DEPENDENCY_JOBS}
            INSTALL_COMMAND ${HOBBITS_MAKE} install)
        set(HOBBITS_M4 "${HOBBITS_DEPS_PREFIX}/bin/m4")
        list(APPEND _parser_deps m4_external)
    endif()
    if(NOT HOBBITS_BISON)
        ExternalProject_Add(bison_external
            URL https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.xz
            URL_HASH SHA256=9bba0214ccf7f1079c5d59210045227bcf619519840ebfa80cd3849cff5a5bf2
            DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            DEPENDS ${_parser_deps}
            CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env M4=${HOBBITS_M4}
                <SOURCE_DIR>/configure --prefix=${HOBBITS_DEPS_PREFIX} --disable-nls
            BUILD_COMMAND ${HOBBITS_MAKE} -j${HOBBITS_DEPENDENCY_JOBS}
            INSTALL_COMMAND ${HOBBITS_MAKE} install)
        set(HOBBITS_BISON "${HOBBITS_DEPS_PREFIX}/bin/bison")
        list(APPEND _parser_deps bison_external)
    endif()
    if(NOT HOBBITS_FLEX)
        ExternalProject_Add(flex_external
            URL https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz
            URL_HASH SHA256=e87aae032bf07c26f85ac0ed3250998c37621d95f8bd748b31f15b33c45ee995
            DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            DEPENDS ${_parser_deps}
            CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env M4=${HOBBITS_M4} YACC=${HOBBITS_BISON}
                <SOURCE_DIR>/configure --prefix=${HOBBITS_DEPS_PREFIX} --disable-shared --disable-nls --disable-libfl
            BUILD_COMMAND ${HOBBITS_MAKE} -j${HOBBITS_DEPENDENCY_JOBS}
            INSTALL_COMMAND ${HOBBITS_MAKE} install)
        set(HOBBITS_FLEX "${HOBBITS_DEPS_PREFIX}/bin/flex")
        list(APPEND _parser_deps flex_external)
    endif()
    ExternalProject_Add(pcap_external
        LIST_SEPARATOR "|"
        URL https://www.tcpdump.org/release/libpcap-1.10.5.tar.xz
        URL_HASH SHA256=84fa89ac6d303028c1c5b754abff77224f45eca0a94eb1a34ff0aa9ceece3925
        DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        DEPENDS ${_parser_deps}
        CMAKE_ARGS ${_common} -DBUILD_SHARED_LIBS=OFF -DBUILD_WITH_LIBNL=OFF
            -DDISABLE_DBUS=ON -DDISABLE_BLUETOOTH=ON -DDISABLE_RDMA=ON
            -DDISABLE_DPDK=ON -DDISABLE_NETMAP=ON -DDISABLE_DAG=ON
            -DDISABLE_SNF=ON -DLEX_EXECUTABLE=${HOBBITS_FLEX} -DYACC_EXECUTABLE=${HOBBITS_BISON}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --parallel ${HOBBITS_DEPENDENCY_JOBS})
    # libusb uses Autoconf, so CMake's Apple platform settings must be translated
    # to compiler/linker flags rather than passed as unused CMake arguments.
    set(_usb_platform_flags "")
    if(APPLE)
        foreach(_arch IN LISTS CMAKE_OSX_ARCHITECTURES)
            string(APPEND _usb_platform_flags " -arch ${_arch}")
        endforeach()
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            string(APPEND _usb_platform_flags " -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
        endif()
        if(CMAKE_OSX_SYSROOT)
            set(_usb_sysroot "${CMAKE_OSX_SYSROOT}")
            if(NOT IS_DIRECTORY "${_usb_sysroot}")
                execute_process(COMMAND xcrun --sdk "${CMAKE_OSX_SYSROOT}" --show-sdk-path
                    OUTPUT_VARIABLE _usb_sysroot OUTPUT_STRIP_TRAILING_WHITESPACE
                    COMMAND_ERROR_IS_FATAL ANY)
            endif()
            string(APPEND _usb_platform_flags " -isysroot \"${_usb_sysroot}\"")
        endif()
    endif()
    ExternalProject_Add(usb_external
        URL https://github.com/libusb/libusb/releases/download/v1.0.29/libusb-1.0.29.tar.bz2
        URL_HASH SHA256=5977fc950f8d1395ccea9bd48c06b3f808fd3c2c961b44b0c2e6e29fc3a70a85
        DOWNLOAD_DIR "${HOBBITS_DOWNLOAD_DIR}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "CC=${CMAKE_C_COMPILER}"
            "CFLAGS=${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} ${_usb_platform_flags}"
            "LDFLAGS=${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${_usb_platform_flags}"
            <SOURCE_DIR>/configure --prefix=${HOBBITS_DEPS_PREFIX} --libdir=${HOBBITS_DEPS_PREFIX}/lib
            --disable-udev --disable-static --enable-shared
        BUILD_COMMAND ${HOBBITS_MAKE} -j${HOBBITS_DEPENDENCY_JOBS}
        INSTALL_COMMAND ${HOBBITS_MAKE} install)
    list(APPEND _deps pcap_external usb_external)
endif()

foreach(_dep qt6 qtwayland pcap usb openssl)
    if(TARGET ${_dep}_external)
        ExternalProject_Add_Step(${_dep}_external licenses
            COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=<SOURCE_DIR>
                -DDEST_DIR=${HOBBITS_DEPS_PREFIX}/share/hobbits/licenses/${_dep}
                -P ${CMAKE_SOURCE_DIR}/cmake/CollectLicenses.cmake
            DEPENDEES install)
    endif()
endforeach()

set(_app_args -DHOBBITS_SUPERBUILD:BOOL=OFF -DHOBBITS_DEPS_PREFIX:PATH=${HOBBITS_DEPS_PREFIX}
    -DHOBBITS_OUTPUT_ROOT:PATH=${CMAKE_BINARY_DIR}
    -DCMAKE_PREFIX_PATH:STRING=${HOBBITS_DEPS_PREFIX})
# Forward normal CMake/user settings, including explicit Python selection and offline
# FetchContent source overrides, to the actual application build.
get_cmake_property(_cache_vars CACHE_VARIABLES)
foreach(_var IN LISTS _cache_vars)
    if(_var MATCHES "^(CMAKE_(BUILD_TYPE|CONFIGURATION_TYPES|INSTALL_.*|(C|CXX)_FLAGS.*|(EXE|SHARED|MODULE|STATIC)_LINKER_FLAGS.*|MSVC_RUNTIME_LIBRARY|C_COMPILER|CXX_COMPILER|TOOLCHAIN_FILE|DISABLE_FIND_PACKAGE_Python3|OSX_.*)|CPACK_.*|Qt6_DIR|Python3_.*|BUILD_VERSION|BUILD_TESTING|JUST_LIBS|SELF_CONTAINED_APP|DEVELOPMENT_BUILD|HOBBITS_ENABLE_PYTHON|HOBBITS_BUNDLE_PYTHON|HOBBITS_PYTHON_VERSION|HOBBITS_ENABLE_WAYLAND|HOBBITS_HEADLESS|HOBBITS_PLUGINS_PATHS|FETCHCONTENT_.*)$")
        string(REPLACE ";" "|" _value "${${_var}}")
        list(APPEND _app_args "-D${_var}:STRING=${_value}")
    endif()
endforeach()
if(CMAKE_PREFIX_PATH)
    string(REPLACE ";" "|" _prefix "${CMAKE_PREFIX_PATH}")
    list(APPEND _app_args "-DCMAKE_PREFIX_PATH:STRING=${HOBBITS_DEPS_PREFIX}|${_prefix}")
endif()
ExternalProject_Add(hobbits_app
    SOURCE_DIR "${CMAKE_SOURCE_DIR}" BINARY_DIR "${CMAKE_BINARY_DIR}/app"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND ""
    DEPENDS ${_deps} LIST_SEPARATOR "|"
    CMAKE_ARGS ${_app_args}
    BUILD_ALWAYS TRUE
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --parallel ${HOBBITS_DEPENDENCY_JOBS}
    INSTALL_COMMAND "")
# Installation belongs to the user-requested install step, never the default build.
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --install \"${CMAKE_BINARY_DIR}/app\" --prefix \"\${CMAKE_INSTALL_PREFIX}\" --config \"\${CMAKE_INSTALL_CONFIG_NAME}\" COMMAND_ERROR_IS_FATAL ANY)")
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/app" --target package --config $<CONFIG>
    DEPENDS hobbits_app USES_TERMINAL)
if(BUILD_TESTING)
    add_test(NAME hobbits-regressions
        COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}/app" -C $<CONFIG> --output-on-failure)
endif()
