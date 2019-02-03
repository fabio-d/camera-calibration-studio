cmake_minimum_required(VERSION 3.0)
project(camera-calibration-studio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)

find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

pkg_check_modules(OPENCV3 REQUIRED opencv>=3)
include_directories(${OPENCV3_INCLUDE_DIRS})
link_directories(${OPENCV3_LIBRARY_DIRS})
link_libraries(${OPENCV3_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_SHARED_LIBRARY_PREFIX "") # no "lib" prefix (e.g. libXX.so -> XX.so)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")

# Core
add_subdirectory(common)
add_subdirectory(main)

# Camera plugins
add_subdirectory(camera_V4L2)
add_subdirectory(camera_gazebo)
