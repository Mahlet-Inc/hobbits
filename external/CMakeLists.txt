
# can't assume the target system has the required SIMD instructions
option(PFFFT_ENABLE_SIMD "Enable SIMD support for pffft" OFF)

message(STATUS "Using bundled pffft")
add_library(pffft STATIC pffft/pffft.c)
if(PFFFT_ENABLE_SIMD)
    message(STATUS "Enabling SIMD support for pffft")
else()
    message(STATUS "Disabling SIMD support for pffft")
    target_compile_definitions(pffft PUBLIC PFFFT_SIMD_DISABLE)
endif()
target_compile_options(pffft PUBLIC "${PFFFT_CFLAGS}")
target_link_libraries(pffft m)
target_include_directories(pffft INTERFACE pffft)

add_library(PFFFT::PFFFT ALIAS pffft)
