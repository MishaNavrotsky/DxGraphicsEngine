//
// Created by Misha on 2/11/2026.
//

#pragma once

#include "engine/core/EngineContextInternal.h"
#include "engine/systems/SystemBase.h"
#include "engine/systems/window/SwapChainSystem.h"

class RenderSystem : public SystemBase {
    SwapChainSystem &swapChainSystem;

public:
    explicit RenderSystem(SwapChainSystem &swapChainSystem) : swapChainSystem(swapChainSystem) {
    };

    void Startup(EngineContextInternal &ctx, EngineConfigs &configs) override {
    }

    void BeginFrame(EngineContextInternal &ctx) {
    }

    void EndFrame(EngineContextInternal &ctx) {
    }
};
