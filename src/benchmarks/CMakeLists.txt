cmake_minimum_required(VERSION 3.22)
project(incremental_watershed_bench)
enable_testing()

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)

# Where to find CMake modules and OpenCV
#================= NEED TO COMPILE AND LINK WITH OpenCV =====================
set(OpenCV_DIR "C:\\Lib\\OpenCV\\build_vs\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(OpenCV_INCLUDE_DIR "C:\\Lib\\OpenCV\\build_vs\\install\\include" )

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Externally provided libraries
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.12.x)

FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.6.1) # need master for benchmark::benchmark

FetchContent_MakeAvailable(
        googletest
        googlebenchmark)

set(COM_SOURCE ../framework/algorithms.cpp
        ../framework/algorithms.h
        ../framework/data.h
        ../framework/imageManager.cpp
        ../framework/imageManager.fwd.h
        ../framework/imageManager.h
        ../framework/dataStructures/graph.cpp
        ../framework/dataStructures/graph.h
        ../framework/dataStructures/Q.h
        ../framework/dataStructures/QBT.cpp
        ../framework/dataStructures/QBT.h
        ../framework/dataStructures/QEBT.cpp
        ../framework/dataStructures/QEBT.h
        ../framework/dataStructures/QT.cpp
        ../framework/dataStructures/QT.h
        ../framework/dataStructures/partition.cpp
        ../framework/dataStructures/partition.h
        ../framework/meyer_ws.cpp
        ../framework/meyer_ws.h
        ../markers.h)

add_executable (${PROJECT_NAME}
        test.cpp
        test.h
        ${COM_SOURCE})

add_executable (${PROJECT_NAME}-paper_tower
        paper_tower.cpp
        paper_tower.h
        ${COM_SOURCE})

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(${PROJECT_NAME} benchmark::benchmark OpenMP::OpenMP_CXX ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME}-paper_tower benchmark::benchmark OpenMP::OpenMP_CXX ${OpenCV_LIBS})