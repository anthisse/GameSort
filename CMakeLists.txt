cmake_minimum_required(VERSION 3.16)
project(GameSort LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}../../bin/)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(GameSort
        src/main.cpp
        lib/simdjson.h
        lib/simdjson.cpp
        src/timsort.hpp
        src/Game.cpp
        src/Game.hpp
        src/timsort.cpp
        src/TextureManager.h
        src/TextureManager.cpp
        src/buttons.h
        src/buttons.cpp
        src/Title.h
        src/Title.cpp

)

target_link_libraries(GameSort PRIVATE sfml-graphics)
target_compile_features(GameSort PRIVATE cxx_std_20)

if(WIN32)
    add_custom_command(
            TARGET GameSort
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:GameSort>
            VERBATIM)
endif()