add_executable(ccs
	AddCameraDialog.cpp
	AddPatternDialog.cpp
	CalibrationDataDockWidget.cpp
	CentralWidget.cpp
	MainWindow.cpp
	PatternDetector.cpp
	PrintPatternDialog.cpp
	ProjectTreeDockWidget.cpp
	ProjectTreeItems.cpp

	main.cpp
)
target_link_libraries(ccs ccscommon Qt5::PrintSupport)
install(TARGETS ccs DESTINATION .)
