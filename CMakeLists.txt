cmake_minimum_required(VERSION 4.0)
project(DxGraphicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(DxGraphicsEngine
        main.cpp
)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(DxGraphicsEngine
        PRIVATE
        glfw
        d3d12
        dxgi
        dxguid
        d3dcompiler
)

target_compile_definitions(DxGraphicsEngine
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)