include(FetchContent)
FetchContent_Declare(pocketfft
    URL https://codeload.github.com/mreineck/pocketfft/tar.gz/4d865cba24359b05b37da4e75aee24498c3b78e7
    URL_HASH SHA256=db236e44b1020dd0b415e5040ce10d8be604fefb7ce83ec0b32bc0f78975aa2f)
FetchContent_MakeAvailable(pocketfft)
add_library(hobbits-pocketfft INTERFACE)
add_library(PocketFFT::PocketFFT ALIAS hobbits-pocketfft)
target_include_directories(hobbits-pocketfft SYSTEM INTERFACE "${pocketfft_SOURCE_DIR}")
target_compile_definitions(hobbits-pocketfft INTERFACE POCKETFFT_NO_MULTITHREADING POCKETFFT_CACHE_SIZE=4)
install(FILES "${pocketfft_SOURCE_DIR}/LICENSE.md"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/hobbits/licenses/pocketfft")

if(NOT WIN32 AND NOT JUST_LIBS)
    find_path(PCAP_INCLUDE_DIR pcap/pcap.h HINTS "${HOBBITS_DEPS_PREFIX}/include" REQUIRED)
    find_library(PCAP_LIBRARY NAMES pcap HINTS "${HOBBITS_DEPS_PREFIX}/lib" REQUIRED)
    add_library(PCAP::PCAP UNKNOWN IMPORTED GLOBAL)
    set_target_properties(PCAP::PCAP PROPERTIES IMPORTED_LOCATION "${PCAP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PCAP_INCLUDE_DIR}")
    find_package(Threads REQUIRED)
    set_property(TARGET PCAP::PCAP PROPERTY INTERFACE_LINK_LIBRARIES Threads::Threads)

    find_path(LIBUSB_INCLUDE_DIR libusb.h HINTS "${HOBBITS_DEPS_PREFIX}/include/libusb-1.0"
        PATH_SUFFIXES libusb-1.0 REQUIRED)
    find_library(LIBUSB_LIBRARY NAMES usb-1.0 HINTS "${HOBBITS_DEPS_PREFIX}/lib" REQUIRED)
    add_library(libusb::libusb UNKNOWN IMPORTED GLOBAL)
    set_target_properties(libusb::libusb PROPERTIES IMPORTED_LOCATION "${LIBUSB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBUSB_INCLUDE_DIR}")
    if(HOBBITS_DEPS_PREFIX)
        if(APPLE)
            file(GLOB _usb_libs "${HOBBITS_DEPS_PREFIX}/lib/libusb-1.0*.dylib")
            install(FILES ${_usb_libs} DESTINATION "hobbits.app/Contents/Frameworks")
        else()
            file(GLOB _usb_libs "${HOBBITS_DEPS_PREFIX}/lib/libusb-1.0.so*")
            install(FILES ${_usb_libs} DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        endif()
    endif()
endif()

if(HOBBITS_DEPS_PREFIX)
    install(DIRECTORY "${HOBBITS_DEPS_PREFIX}/share/hobbits/licenses/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/hobbits/licenses" OPTIONAL)
endif()
