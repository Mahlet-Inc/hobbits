pluginInDir("${pluginType}" "PythonRunner" "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries("hobbits-plugin-operators-PythonRunner" PRIVATE hobbits-python)
pass_through_cpack_vars()
