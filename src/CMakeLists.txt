
#
# Add Subdirectories
#

add_subdirectory(hobbits-core)
add_subdirectory(hobbits-widgets)
add_subdirectory(hobbits-python)
if (NOT JUST_LIBS)
	add_subdirectory(hobbits-plugins)
	add_subdirectory(hobbits-gui)
	add_subdirectory(hobbits-runner)
endif()
