cmake_minimum_required(VERSION 3.10)
project(oop_lab1)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(task1_editor task1_editor/main.cpp)
add_executable(task2_scheduler task2_scheduler/main.cpp)
