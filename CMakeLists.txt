cmake_minimum_required(VERSION 4.0)
project(DxGraphicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(DxGraphicsEngine
        main.cpp
        engine/systems/window/SwapChainSystem.h
        engine/Types.h
        engine/Globals.h
        engine/libs/Registry.h
        engine/systems/SystemBase.h
        engine/core/EngineContext.h
        engine/DxUtils.h
        engine/systems/QueueSystem.h
        engine/systems/window/WindowSystem.h
        engine/core/Engine.h
        engine/core/Engine.cpp
        engine/systems/render/RenderSystem.h
)

target_include_directories(DxGraphicsEngine
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        directx_headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG        main # Or a specific release tag
)
FetchContent_MakeAvailable(directx_headers)

FetchContent_Declare(
        d3d12ma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(d3d12ma)

target_link_libraries(DxGraphicsEngine
        PRIVATE
        glfw
        d3d12
        dxgi
        dxguid
        d3dcompiler
        Microsoft::DirectX-Headers
        D3D12MemoryAllocator
)

target_include_directories(DxGraphicsEngine BEFORE PRIVATE
        ${directx_headers_SOURCE_DIR}/include/directx
)

target_compile_definitions(DxGraphicsEngine
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)