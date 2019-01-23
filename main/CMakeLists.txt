add_executable(ccs
	AddCameraDialog.cpp
	CalibrationDataDockWidget.cpp
	CentralWidget.cpp
	MainWindow.cpp
	ProjectTreeDockWidget.cpp
	ProjectTreeItems.cpp

	main.cpp
)

target_link_libraries(ccs ccscommon)
install(TARGETS ccs DESTINATION .)
