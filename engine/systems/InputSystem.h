//
// Created by Misha on 2/19/2026.
//

#pragma once
#include "SystemBase.h"
#include "engine/systems/window/WindowSystem.h"

class InputSystem : public SystemBase {
    WindowSystem &windowSystem;

public:
    explicit InputSystem(WindowSystem &windowSystem) : windowSystem(windowSystem) {
    }

    void Startup(EngineContextInternal &ctx, EngineConfigs &configs) override {
        windowSystem.OnFramebufferSizeCallback = [this](uint32_t width, uint32_t height) {
            std::print("Width {}, Height {}\n", width, height);
        };
    }
};
