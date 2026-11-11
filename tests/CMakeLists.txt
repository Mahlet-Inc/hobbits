add_executable(hobbits-regression-tests regression_tests.cpp)
target_link_libraries(hobbits-regression-tests PRIVATE hobbits-widgets)
add_test(NAME hobbits-regressions COMMAND hobbits-regression-tests)

if(HOBBITS_WITH_PYTHON)
    find_package(Threads REQUIRED)
    add_executable(hobbits-python-tests test_python.cpp)
    target_link_libraries(hobbits-python-tests PRIVATE hobbits-python Threads::Threads)
    add_test(NAME hobbits-python COMMAND hobbits-python-tests)
    add_test(NAME hobbits-kaitai-stdlib
        COMMAND "${Python3_EXECUTABLE}" -B -S "${CMAKE_CURRENT_SOURCE_DIR}/test_kaitai_stdlib.py"
                "${CMAKE_SOURCE_DIR}/src/hobbits-plugins/analyzers/KaitaiStruct")
    add_test(NAME hobbits-python-worker-init COMMAND hobbits-python-tests --worker-init)
    add_test(NAME hobbits-python-invalid-home COMMAND hobbits-python-tests --invalid-home)
    set_tests_properties(hobbits-python hobbits-python-worker-init hobbits-python-invalid-home PROPERTIES TIMEOUT 30)
endif()

add_executable(hobbits-fft-tests fft_regression.cpp)
target_link_libraries(hobbits-fft-tests PRIVATE PocketFFT::PocketFFT)
add_test(NAME hobbits-fft COMMAND hobbits-fft-tests)

add_executable(hobbits-tls-tests tls_regression.cpp)
target_link_libraries(hobbits-tls-tests PRIVATE Qt6::Network)
hobbits_copy_runtime_dlls(hobbits-tls-tests)
add_test(NAME hobbits-tls COMMAND hobbits-tls-tests)
set_tests_properties(hobbits-tls PROPERTIES TIMEOUT 30)

if(TARGET hobbits-plugin-displays-Spectrogram)
    add_executable(hobbits-spectrogram-tests spectrogram_regression.cpp)
    target_link_libraries(hobbits-spectrogram-tests PRIVATE hobbits-widgets)
    add_dependencies(hobbits-spectrogram-tests hobbits-plugin-displays-Spectrogram)
    add_test(NAME hobbits-spectrogram COMMAND hobbits-spectrogram-tests
        "$<TARGET_FILE:hobbits-plugin-displays-Spectrogram>")
    set_tests_properties(hobbits-spectrogram PROPERTIES TIMEOUT 30)
endif()

if(TARGET hobbits-runner)
    add_test(NAME hobbits-plugin-discovery COMMAND hobbits-runner list
        --config "${CMAKE_CURRENT_BINARY_DIR}/discovery-settings-$<CONFIG>.ini")
    set_tests_properties(hobbits-plugin-discovery PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        PASS_REGULAR_EXPRESSION "Display: Bit Raster"
        FAIL_REGULAR_EXPRESSION "could not be loaded as a plugin"
        TIMEOUT 30)
endif()

add_executable(hobbits-preview-tests preview_regression.cpp)
target_link_libraries(hobbits-preview-tests PRIVATE hobbits-widgets Qt6::Concurrent)
hobbits_copy_runtime_dlls(hobbits-preview-tests)
add_test(NAME hobbits-preview COMMAND hobbits-preview-tests)
set_tests_properties(hobbits-preview PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 30)
