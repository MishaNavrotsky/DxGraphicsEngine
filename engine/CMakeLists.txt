list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(Utils)

add_subdirectory(thirdParty)

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