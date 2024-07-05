cmake_minimum_required(VERSION 3.27)
project(GameSort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17")

find_package(CURL REQUIRED)

add_executable(GameSort src/grailsort.h
        src/main.cpp
        src/simdjson.h
        src/simdjson.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(c:/SFML/include/SFML)
target_link_libraries(GameSort PRIVATE CURL::libcurl sfml-system sfml-window sfml-graphics sfml-audio)
