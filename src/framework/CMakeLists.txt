cmake_minimum_required(VERSION 3.22)
project(incremental_watershed_framework)

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)

file(
        GLOB_RECURSE
        SOURCE_FILES
        *.cpp
)

list(FILTER SOURCE_FILES EXCLUDE REGEX "main.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX "interface.cpp")

file(
        GLOB_RECURSE
        HEADER_FILES
        *.h
)

FetchContent_Declare(matplotlib-cpp
        GIT_REPOSITORY https://github.com/lava/matplotlib-cpp.git
        GIT_TAG master) # need master for benchmark::benchmark

FetchContent_MakeAvailable(matplotlib-cpp)

option(TBB_TEST OFF)

# Where to find CMake modules and OpenCV
#================= NEED TO COMPILE AND LINK WITH OpenCV =====================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenCV_DIR "C:\\Lib\\OpenCV\\build_vs\\install")
set(OpenCV_INCLUDE_DIR "C:\\Lib\\OpenCV\\build_vs\\install\\include" )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(OpenMP REQUIRED)

add_executable (${PROJECT_NAME}-interface ${SOURCE_FILES}  ${HEADER_FILES} ../markers.h "interface.cpp")
add_executable (${PROJECT_NAME} ${SOURCE_FILES}  ${HEADER_FILES} ../markers.h "main.cpp")

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(${PROJECT_NAME}-interface ${OpenCV_LIBS} OpenMP::OpenMP_CXX)
target_link_libraries(${PROJECT_NAME} matplotlib_cpp ${OpenCV_LIBS} OpenMP::OpenMP_CXX segm)