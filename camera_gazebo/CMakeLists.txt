find_package(gazebo)

if(gazebo_FOUND)

include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
link_libraries(${GAZEBO_LIBRARIES})

add_library(ccscamera_gazebo SHARED
	AddCameraWidget.cpp
	CameraPlugin.cpp
	GazeboConnection.cpp
	LiveCapture.cpp
)

target_link_libraries(ccscamera_gazebo ccscommon)
install(TARGETS ccscamera_gazebo DESTINATION .)

endif(gazebo_FOUND)
