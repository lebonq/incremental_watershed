cmake_minimum_required(VERSION 3.22)
project(incremental_watershed_framework)

set(CMAKE_CXX_STANDARD 14)

file(
        GLOB_RECURSE
        SOURCE_FILES
        *.cpp
)

file(
        GLOB_RECURSE
        HEADER_FILES
        *.h
)

option(TBB_TEST OFF)

# Where to find CMake modules and OpenCV
#================= NEED TO COMPILE AND LINK WITH OpenCV =====================
set(OpenCV_DIR "C:\\Lib\\OpenCV\\build_vs\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenCV_INCLUDE_DIR "C:\\Lib\\OpenCV\\build_vs\\install\\include" )
set(TBB_DIR "C:\\Lib\\oneTBB\\lib\\cmake\\tbb" )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(OpenMP REQUIRED)
find_package(TBB REQUIRED)

add_executable (${PROJECT_NAME} ${SOURCE_FILES}  ${HEADER_FILES} ../markers.h)

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} OpenMP::OpenMP_CXX TBB::tbb)