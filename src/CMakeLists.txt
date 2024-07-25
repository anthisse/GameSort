cmake_minimum_required (VERSION 3.27)
project (GameSort)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++20")

find_package (CURL REQUIRED)

add_executable (GameSort
        #src/grailsort.h
        src/main.cpp
        src/sindison.h
        src/sindison.cpp)
#set (SFML_STATIC_LIBRARIES TRUE)
#set (SFML_DIR/opt/homebrew/lib/cmake/SFML/SFMLConfig.cmake)| include_directories (${SFML_INCLUDE_DIRS})


find_package (SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories (${SFML_INCLUDE_DIRS})
target_link_libraries (GameSort PRIVATE CURL: : libcurl sfm-system sfml-window sfml-graphics sfml-audio sml-network)