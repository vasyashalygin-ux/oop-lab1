cmake_minimum_required(VERSION 3.10)
project(oop_lab1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Задание 1 — двумерный графический редактор
add_executable(task1_editor
    task1_editor/main.cpp
    task1_editor/geometry.cpp
    task1_editor/shapes.cpp
    task1_editor/editor.cpp
    task1_editor/tests.cpp
)

# Задание 2 — планировщик задач на двоичной куче
add_executable(task2_scheduler
    task2_scheduler/main.cpp
    task2_scheduler/task.cpp
    task2_scheduler/tests.cpp
)
