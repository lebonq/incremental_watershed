cmake_minimum_required(VERSION 3.22)
project(incremental_watershed)

set(CMAKE_CXX_STANDARD 14)

file(
        GLOB_RECURSE
        SOURCE_FILES
        src/*.cpp
)

file(
        GLOB_RECURSE
        HEADER_FILES
        src/*.h
)

# Where to find CMake modules and OpenCV
set(OpenCV_DIR "C:\\Lib\\OpenCV\\build_vs\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenCV_INCLUDE_DIR "C:\\Lib\\OpenCV\\build_vs\\install\\include" )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable (${PROJECT_NAME} ${SOURCE_FILES}  ${HEADER_FILES})

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})