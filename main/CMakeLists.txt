add_executable(ccs
	AddCameraDialog.cpp
	AddPatternDialog.cpp
	CalibrationAlgorithmConfigurationDialog.cpp
	CalibrationDataDockWidget.cpp
	CaptureParametersDockWidget.cpp
	CaptureParameterWidget.cpp
	CentralWidget.cpp
	CornerRefiner.cpp
	CornerRefinerConfigurationWidget.cpp
	ImageImporter.cpp
	MainWindow.cpp
	PatternDetector.cpp
	PatternDetectorConfigurationDialog.cpp
	PrintPatternDialog.cpp
	ProjectTreeDockWidget.cpp
	ProjectTreeItems.cpp
	TermCriteriaConfigurationWidget.cpp

	main.cpp
)
target_link_libraries(ccs ccscommon Qt5::PrintSupport)
install(TARGETS ccs DESTINATION .)
