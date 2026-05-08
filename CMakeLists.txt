cmake_minimum_required(VERSION 3.28)

project(hewnstead
    VERSION 0.0.1
    DESCRIPTION "A medieval voxel sandbox where you build, settle, and outlast the seasons."
    LANGUAGES CXX C
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

CPMAddPackage(
    NAME glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)

CPMAddPackage(
    NAME glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    OPTIONS
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_INSTALL OFF"
)

CPMAddPackage(
    NAME spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)

CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY doctest/doctest
    VERSION 2.5.2
)


add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_library(hewnstead_core STATIC
    src/glcheck.cpp
    src/window.cpp
    src/shader.cpp
    src/mesh.cpp
    src/input.cpp
    src/camera.cpp
    src/chunk.cpp
    src/chunk_manager.cpp
)
target_include_directories(hewnstead_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(hewnstead_core PUBLIC cxx_std_20)
target_compile_definitions(hewnstead_core PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(hewnstead_core
    PUBLIC glm::glm spdlog::spdlog glfw glad
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(hewnstead_core PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wconversion
        -Wsign-conversion
    )
endif()

add_executable(hewnstead src/main.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hewnstead PRIVATE hewnstead_core)

# tests subdirectory
enable_testing()
add_subdirectory(tests)
