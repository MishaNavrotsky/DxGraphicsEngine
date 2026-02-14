//
// Created by Misha on 2/3/2026.
//

#pragma once

#include <cstdint>
#include "engine/DxUtils.h"
#include <dxgiformat.h>
#include "engine/Types.h"
#include "engine/libs/Registry.h"
#include "engine/systems/SystemBase.h"

struct WindowConfig {
    std::uint32_t width = 1280, height = 720;
    bool vsync = false;
    HWND windowHandle = nullptr;
};

struct DxContext {
    dx::ComPtr<ID3D12Device> device;
    dx::ComPtr<IDXGIFactory6> factory;
    dx::ComPtr<IDXGIAdapter4> adapter;
    dx::ComPtr<D3D12MA::Allocator> allocator;
};

struct SwapChainConfig {
    std::uint32_t bufferCount = 3;
};

struct EngineContext {
    WindowConfig windowConfig;
    SwapChainConfig swapChainConfig;
    DxContext dx;
    Registry<SystemBase> systems;
    std::stop_token stopToken;
};
