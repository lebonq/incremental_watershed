cmake_minimum_required(VERSION 3.16)
project(incremental_watershed_framework)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

file(
        GLOB_RECURSE
        SOURCE_FILES
        *.cpp
)

list(FILTER SOURCE_FILES EXCLUDE REGEX "main.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX "iws_bench.cpp")
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
find_package(DCMTK REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(OpenMP REQUIRED)

add_executable (${PROJECT_NAME}-interface ${SOURCE_FILES}  ${HEADER_FILES} "interface.cpp")
add_executable (${PROJECT_NAME} ${SOURCE_FILES}  ${HEADER_FILES} "main.cpp")
add_executable (${PROJECT_NAME}-3d-iws ${SOURCE_FILES}  ${HEADER_FILES} "iws_bench.cpp")
add_executable (${PROJECT_NAME}-3d-niws ${SOURCE_FILES}  ${HEADER_FILES} "niws_bench.cpp")

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(${PROJECT_NAME}-interface ${OpenCV_LIBS} OpenMP::OpenMP_CXX)
target_link_libraries(${PROJECT_NAME} matplotlib_cpp ${OpenCV_LIBS} OpenMP::OpenMP_CXX ${DCMTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}-3d-iws matplotlib_cpp ${OpenCV_LIBS} OpenMP::OpenMP_CXX ${DCMTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}-3d-niws matplotlib_cpp ${OpenCV_LIBS} OpenMP::OpenMP_CXX ${DCMTK_LIBRARIES})
