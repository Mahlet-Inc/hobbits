pluginInDir("${pluginType}" "KaitaiStruct" "${CMAKE_CURRENT_SOURCE_DIR}")

# Doesn't work out of the box, likely a bug in CMake
get_target_property(INTERFACE_INCLUDE_DIRECTORIES_HobbitsPython "hobbits-python" INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories("hobbits-plugin-analyzers-KaitaiStruct" PRIVATE "${INTERFACE_INCLUDE_DIRECTORIES_HobbitsPython}")

target_link_libraries("hobbits-plugin-analyzers-KaitaiStruct" PRIVATE hobbits-python)
set("CPACK_DEBIAN_HOBBITS-PLUGIN-ANALYZERS-KAITAISTRUCT_PACKAGE_DEPENDS" "${CPACK_DEBIAN_HOBBITS-PLUGIN-ANALYZERS-KAITAISTRUCT_PACKAGE_DEPENDS}, ${CPACK_DEBIAN_HOBBITS-PYTHON_PACKAGE_NAME}")
set("CPACK_DEBIAN_HOBBITS-PLUGIN-ANALYZERS-KAITAISTRUCT_PACKAGE_SUGGESTS" "kaitai-struct-compiler, python3-kaitaistruct")
pass_through_cpack_vars()
