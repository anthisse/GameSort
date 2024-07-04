cmake_minimum_required(VERSION 3.27)
project(GameSort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17")

find_package(CURL REQUIRED)

add_executable(GameSort src/grailsort.h
        src/main.cpp)
target_link_libraries(GameSort PRIVATE CURL::libcurl)
