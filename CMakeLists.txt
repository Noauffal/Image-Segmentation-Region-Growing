cmake_minimum_required(VERSION 3.1)
project(mif17_tp1)

set(CMAKE_CXX_FLAGS "-O2")
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(tp1 main.cpp)
target_link_libraries(tp1 ${OpenCV_LIBS})