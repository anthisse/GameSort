cmake_minimum_required(VERSION 3.27)
project(GameSort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

add_executable(GameSort src/grailsort.h
        src/main.cpp)
