find_package(Qt5Sql REQUIRED)

add_library(ccscommon SHARED
	AddCameraWidget.cpp
	CalibrationParameters.cpp
	Camera.cpp
	CameraPlugin.cpp
	LiveCapture.cpp
	Project.cpp
	Sensor.cpp
	Shot.cpp
	SqliteDatabase.cpp
)

target_link_libraries(ccscommon PUBLIC Qt5::Widgets Qt5::Sql)
install(TARGETS ccscommon DESTINATION .)
