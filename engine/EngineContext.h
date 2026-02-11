//
// Created by Misha on 2/3/2026.
//

#ifndef DXGRAPHICSENGINE_ENGINECONTEXT_H
#define DXGRAPHICSENGINE_ENGINECONTEXT_H
#include <cstdint>
#include <d3d12.h>
#include <dxgiformat.h>
#include "Types.h"
#include "libs/Registry.h"
#include "systems/SystemBase.h"

struct WindowConfig {
    std::uint32_t width = 1280, height = 720;
    bool vsync = false;
    HWND windowHandle = nullptr;
};

struct DXContext {
    dx::ComPtr<ID3D12Device> device;
    dx::ComPtr<IDXGIFactory6> factory;
    std::uint32_t bufferCount = 3;
};

struct EngineContext {
    WindowConfig windowConfig;
    DXContext dx;
    Registry<SystemBase> systems;
    std::stop_token stopToken;
};

#endif //DXGRAPHICSENGINE_ENGINECONTEXT_H