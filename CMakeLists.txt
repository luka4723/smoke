cmake_minimum_required(VERSION 3.16)
project(smoke CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(smoke main.cpp Grid2D.cpp MAC2D.cpp
    advection.cpp output.cpp projection.cpp recorder.cpp)
target_include_directories(smoke PRIVATE core)