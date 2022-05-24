cmake_minimum_required(VERSION 3.22)
project(incremental_watershed)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

# Where to find CMake modules and OpenCV
set(OpenCV_DIR "C:\\Lib\\OpenCV\\build_vs\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenCV_INCLUDE_DIR "C:\\Lib\\OpenCV\\build_vs\\install\\include" )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(incremental_watershed
        graph.cpp
        graph.h main.cpp imageManager.cpp imageManager.h dataStructures/Q.h dataStructures/QBT.cpp dataStructures/QBT.h algorithms.cpp algorithms.h data.h dataStructures/QT.cpp dataStructures/QT.h dataStructures/QEBT.cpp dataStructures/QEBT.h)

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(incremental_watershed ${OpenCV_LIBS})