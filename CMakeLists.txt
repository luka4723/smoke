cmake_minimum_required(VERSION 3.16)
project(smoke CXX)
set(CMAKE_CXX_STANDARD 20)

# Release unless a build type is given
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP REQUIRED)

add_executable(smoke main.cpp Grid2D.cpp MAC2D.cpp
    advection.cpp output.cpp projection.cpp recorder.cpp)
target_include_directories(smoke PRIVATE core)
target_link_libraries(smoke PRIVATE OpenMP::OpenMP_CXX)