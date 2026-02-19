list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(Utils)

add_subdirectory(thirdParty)

add_library(Engine STATIC
        systems/window/SwapChainSystem.h
        Types.h
        Globals.h
        libs/Registry.h
        systems/SystemBase.h
        core/EngineContextInternal.h
        DxUtils.h
        systems/DxQueueSystem.h
        systems/window/WindowSystem.h
        core/Engine.h
        core/Engine.cpp
        systems/render/RenderSystem.h
        systems/ui/EditorUISystem.h
        libs/Allocators.h
        libs/DescriptorHeaps.h
        core/EngineConfigs.h
        core/EngineContext.h
        systems/InputSystem.h
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(Engine
        PRIVATE
        dxheaders_wrapper
        glfw_wrapper
        d3d12ma_wrapper
        ImGuiDX12
        enkiTS_wrapper
        entt_wrapper

        d3d12
        dxgi
        dxguid
        d3dcompiler
)

target_compile_definitions(Engine
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)