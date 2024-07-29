cmake_minimum_required (VERSION 3.27)
project (GameSort2)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++20")

find_package (CURL REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

add_executable (GameSort2
        #grailsort.h
        main.cpp
        simdjson.h
        simdjson.cpp
        timsort.cpp
        timsort.h
        Game.h
        Game.cpp
        buttons.h
        TextureManager.cpp
        TextureManager.h
        buttons.cpp)
#set (SFML_STATIC_LIBRARIES TRUE)
#set (SFML_DIR/opt/homebrew/lib/cmake/SFML/SFMLConfig.cmake)| include_directories (${SFML_INCLUDE_DIRS})


find_package (SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
include_directories (${SFML_INCLUDE_DIRS})
target_link_libraries (GameSort2 PRIVATE CURL::libcurl sfml-system sfml-window sfml-graphics sfml-audio sfml-network)