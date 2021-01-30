pluginInDir("${pluginType}" "PythonRunner" "${CMAKE_CURRENT_SOURCE_DIR}")

get_target_property(INTERFACE_INCLUDE_DIRECTORIES_HobbitsPython "hobbits-python" INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories("hobbits-plugin-operators-PythonRunner" PRIVATE "${INTERFACE_INCLUDE_DIRECTORIES_HobbitsPython}")
target_link_libraries("hobbits-plugin-operators-PythonRunner" PRIVATE hobbits-python)
set("CPACK_DEBIAN_HOBBITS-PLUGIN-OPERATORS-PYTHONRUNNER_PACKAGE_DEPENDS" "${CPACK_DEBIAN_HOBBITS-PLUGIN-OPERATORS-PYTHONRUNNER_PACKAGE_DEPENDS}, ${CPACK_DEBIAN_HOBBITS-PYTHON_PACKAGE_NAME}")
pass_through_cpack_vars()
