//
// Created by Misha on 2/19/2026.
//

#pragma once
#include "SystemBase.h"
#include "engine/systems/window/WindowSystem.h"

class InputSystem : public SystemBase {
    WindowSystem& windowSystem;
public:
    explicit InputSystem(WindowSystem& windowSystem) : windowSystem(windowSystem) {}

    void Startup(EngineContextInternal& ctx, EngineConfigs& configs) override {
    }
};
