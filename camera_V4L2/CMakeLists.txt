pkg_check_modules(V4L2 REQUIRED libv4l2)
include_directories(${V4L2_INCLUDE_DIRS})
link_directories(${V4L2_LIBRARY_DIRS})
link_libraries(${V4L2_LIBRARIES})

add_library(ccscamera_V4L2 SHARED
	AddCameraWidget.cpp
	CameraPlugin.cpp
	LiveCapture.cpp
	LiveCaptureParameters.cpp
)

target_link_libraries(ccscamera_V4L2 ccscommon)
install(TARGETS ccscamera_V4L2 DESTINATION .)
