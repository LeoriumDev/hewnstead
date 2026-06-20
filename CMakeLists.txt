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

set(_saved_cxx_flags "${CMAKE_CXX_FLAGS}")
string(APPEND CMAKE_CXX_FLAGS " -Wno-ignored-attributes")

CPMAddPackage(
    NAME FastNoise2
    GITHUB_REPOSITORY Auburn/FastNoise2
    GIT_TAG v1.1.1
    OPTIONS
        "FASTNOISE2_NOISETOOL OFF"
        "FASTNOISE2_TESTS OFF"
)

set(CMAKE_CXX_FLAGS "${_saved_cxx_flags}")

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.92.7
    DOWNLOAD_ONLY YES
)
if(imgui_ADDED)
    add_library(hewnstead_imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(hewnstead_imgui SYSTEM PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_link_libraries(hewnstead_imgui PUBLIC glfw)
    if(APPLE)
        target_compile_definitions(hewnstead_imgui PRIVATE
            GL_SILENCE_DEPRECATION)
    endif()
    target_compile_options(hewnstead_imgui PRIVATE
        -Wno-conversion -Wno-sign-conversion -Wno-shadow
        -Wno-pedantic -Wno-old-style-cast)
endif()

add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_library(stb_image STATIC third_party/stb/src/stb_image_impl.cpp)
target_include_directories(stb_image PUBLIC third_party/stb/include)
# Disable strict warnings for this TU only
target_compile_options(stb_image PRIVATE
    -Wno-conversion
    -Wno-sign-conversion
    -Wno-shadow
    -Wno-cast-qual
    -Wno-cast-align
    -Wno-unused-but-set-variable
    -Wno-missing-field-initializers
)

add_library(hewnstead_core STATIC
    src/core/application.cpp
    src/core/gl_check.cpp
    src/core/input.cpp
    src/core/splash.cpp
    src/core/window.cpp
    src/physics/player.cpp
    src/render/camera.cpp
    src/render/chunk_mesh.cpp
    src/render/debug_overlay.cpp
    src/render/frustum.cpp
    src/render/imgui_runtime.cpp
    src/render/line_mesh.cpp
    src/render/mesher.cpp
    src/render/shader.cpp
    src/render/texture_array.cpp
    src/world/block_accessor.cpp
    src/world/block_id.cpp
    src/world/chunk_manager.cpp
    src/world/chunk.cpp
    src/world/raycast.cpp
    src/world/worldgen.cpp
)
target_include_directories(hewnstead_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(hewnstead_core PUBLIC cxx_std_20)
target_compile_definitions(hewnstead_core PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(hewnstead_core
    PUBLIC glm::glm spdlog::spdlog glfw glad hewnstead_imgui stb_image FastNoise
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
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)
add_dependencies(hewnstead copy_assets)
target_link_libraries(hewnstead PRIVATE hewnstead_core)

# tests subdirectory
enable_testing()
add_subdirectory(tests)
