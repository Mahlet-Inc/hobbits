set(CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)

add_subdirectory(hobbits-core)
add_subdirectory(hobbits-python)
add_subdirectory(hobbits-gui)
add_subdirectory(hobbits-runner)
add_subdirectory(hobbits-plugins)

pass_through_cpack_vars()
