# Bundle the same Python installation that supplies Python3::Python.
# The DLL alone is insufficient: initialization needs encodings, and plugins use
# both the standard library and its native extension modules.
function(pack_windows_python)
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" -I -S -c "import sys; print(sys.base_prefix)"
        OUTPUT_VARIABLE pythonPrefix OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY)
    file(TO_CMAKE_PATH "${pythonPrefix}" pythonPrefix)
    if(NOT EXISTS "${Python3_STDLIB}/encodings/__init__.py" OR
       NOT IS_DIRECTORY "${pythonPrefix}/DLLs" OR
       NOT EXISTS "${pythonPrefix}/LICENSE.txt")
        message(FATAL_ERROR
            "Cannot bundle Python ${Python3_VERSION} from ${pythonPrefix}. "
            "Use a standard Windows Python installation or set HOBBITS_ENABLE_PYTHON=OFF.")
    endif()

    # Keep the linked DLL beside the executables for the Windows loader. Native
    # modules may also use the stable-ABI DLL and Python's MSVC runtime DLLs.
    install(FILES "$<TARGET_FILE:Python3::Python>" DESTINATION "." COMPONENT runtime)
    file(GLOB pythonSupportDlls "${pythonPrefix}/python3.dll" "${pythonPrefix}/vcruntime*.dll")
    install(FILES ${pythonSupportDlls} DESTINATION "." COMPONENT runtime)
    install(DIRECTORY "${Python3_STDLIB}/" DESTINATION "python/Lib" COMPONENT runtime
        PATTERN "site-packages" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "test" EXCLUDE
        PATTERN "tests" EXCLUDE
        PATTERN "*.pyc" EXCLUDE)
    install(DIRECTORY "${pythonPrefix}/DLLs/" DESTINATION "python/DLLs" COMPONENT runtime
        FILES_MATCHING PATTERN "*.pyd" PATTERN "*.dll")
    if(IS_DIRECTORY "${pythonPrefix}/tcl")
        install(DIRECTORY "${pythonPrefix}/tcl" DESTINATION "python" COMPONENT runtime)
    endif()
    install(FILES "${pythonPrefix}/LICENSE.txt" DESTINATION "python" COMPONENT runtime)
endfunction()

function(pack_linux_python)
    find_program(HOBBITS_PATCHELF patchelf REQUIRED)
    set(pythonNativeDir "${Python3_STDARCH}/lib-dynload")
    if(NOT EXISTS "${Python3_STDLIB}/encodings/__init__.py" OR NOT IS_DIRECTORY "${pythonNativeDir}")
        message(FATAL_ERROR "Cannot locate the standard library and native modules for Python ${Python3_VERSION}")
    endif()
    find_file(pythonLicense NAMES LICENSE.txt copyright
        PATHS "${Python3_STDLIB}" "${Python3_STDLIB}/../../share/doc/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}"
        NO_DEFAULT_PATH NO_CACHE REQUIRED)
    set(pythonHome "${CMAKE_INSTALL_LIBDIR}/hobbits/python")
    set(pythonStdlib "${pythonHome}/lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
    install(DIRECTORY "${Python3_STDLIB}/" DESTINATION "${pythonStdlib}" COMPONENT runtime
        PATTERN "site-packages" EXCLUDE
        PATTERN "dist-packages" EXCLUDE
        PATTERN "lib-dynload" EXCLUDE
        PATTERN "config-*" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "test" EXCLUDE
        PATTERN "tests" EXCLUDE
        PATTERN "*.pyc" EXCLUDE)
    install(DIRECTORY "${pythonNativeDir}/" DESTINATION "${pythonStdlib}/lib-dynload" COMPONENT runtime
        FILES_MATCHING PATTERN "*.so")
    install(FILES "${pythonLicense}" DESTINATION "${pythonHome}" RENAME LICENSE.txt COMPONENT runtime)

    file(GLOB tkinterModules "${pythonNativeDir}/_tkinter*.so")
    if(tkinterModules)
        execute_process(COMMAND "${Python3_EXECUTABLE}" -I -S -c
            "import tkinter; print(tkinter.Tcl().eval('info library'))"
            OUTPUT_VARIABLE tclLibrary OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY)
        get_filename_component(tclDataDir "${tclLibrary}" DIRECTORY)
        file(GLOB tclData "${tclDataDir}/tcl[0-9]*" "${tclDataDir}/tk[0-9]*")
        # PythonInterpreter selects these data directories for bundled Tcl/Tk.
        install(DIRECTORY ${tclData} DESTINATION "${pythonHome}/lib" COMPONENT runtime)
    endif()

    # Native modules may depend on libraries outside the Python installation
    # (OpenSSL, libffi, compression libraries, etc.). Deploy those privately and
    # replace absolute tool-cache RPATHs with paths relative to the installed files.
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/DeployPythonLinux.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/DeployPythonLinux.cmake.in" @ONLY)
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/DeployPythonLinux-$<CONFIG>.cmake"
        INPUT "${CMAKE_CURRENT_BINARY_DIR}/DeployPythonLinux.cmake.in")
    install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/DeployPythonLinux-$<CONFIG>.cmake" COMPONENT runtime)
endfunction()

function(pack_python)
    if(WIN32)
        pack_windows_python()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        pack_linux_python()
    endif()
endfunction()
