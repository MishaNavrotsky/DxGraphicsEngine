//
// Created by Misha on 2/16/2026.
//

#pragma once
#include <cstdint>


constexpr const uint32_t FramesInFlightCount = 3;

struct WindowConfig {
    std::uint32_t width = 1280, height = 720;
    bool vsync = false;
};

struct SwapChainConfig {
    std::uint32_t bufferCount = 3;
};

struct UIConfig {
    bool enabled = true;
};

struct BindlessHeapConfig {
    uint32_t staticCapacity = 500000;
    uint32_t reusableCapacity = 200000;
    uint32_t dynamicCapacityPerFrame = 100000;
};

struct CommandListsConfig {
};

struct EngineConfig {
};

struct EngineConfigs {
    WindowConfig window;
    SwapChainConfig swapChain;
    UIConfig uiConfig;
    BindlessHeapConfig bindlessHeapConfig;
    EngineConfig engineConfig;
    CommandListsConfig commandListsConfig;
};
