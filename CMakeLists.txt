cmake_minimum_required(VERSION 3.28)

project(hewnstead
    VERSION 0.0.1
    DESCRIPTION "A medieval voxel sandbox where you build, settle, and outlast the seasons."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache enabled: ${CCACHE_PROGRAM}")
endif()

include(cmake/CPM.cmake)

add_executable(hewnstead
    src/main.cpp
)

target_compile_features(hewnstead PRIVATE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(hewnstead PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wconversion
        -Wsign-conversion
    )
endif()

target_include_directories(hewnstead PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
