include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_Declare(
        directx_headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG main
)
FetchContent_Declare(
        d3d12ma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG master
)
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw directx_headers d3d12ma entt)

add_library(Engine STATIC
        systems/window/SwapChainSystem.h
        Types.h
        Globals.h
        libs/Registry.h
        systems/SystemBase.h
        core/EngineContext.h
        DxUtils.h
        systems/QueueSystem.h
        systems/window/WindowSystem.h
        core/Engine.h
        core/Engine.cpp
        systems/render/RenderSystem.h
        ui/ImGuiLayer.h
)

add_subdirectory(thirdParty/ImGuiDx12)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_include_directories(Engine BEFORE PRIVATE
        ${directx_headers_SOURCE_DIR}/include/directx
)

target_link_libraries(Engine
        PRIVATE
        glfw
        d3d12
        dxgi
        dxguid
        d3dcompiler
        Microsoft::DirectX-Headers
        D3D12MemoryAllocator
        EnTT::EnTT
        ImGuiDX12
)

target_compile_definitions(Engine
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)