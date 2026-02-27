//
// Created by Misha on 2/19/2026.
//

#pragma once
#include "SystemBase.h"
#include "engine/libs/Event.h"
#include "engine/systems/window/WindowSystem.h"

class InputSystem : public SystemBase {
    WindowSystem &windowSystem;
    std::function<void(int32_t key, int32_t scancode, int32_t action, int32_t mods)> OnKeyCallback;
    std::function<void(uint32_t codepoint)> OnCharCallback;
    std::function<void(double xpos, double ypos)> OnCursorPosCallback;
    std::function<void(int32_t button, int32_t action, int32_t mods)> OnMouseButtonCallback;
    std::function<void(double xoffset, double yoffset)> OnScrollCallback;
    std::function<void(int32_t width, int32_t height)> OnFramebufferSizeCallback;
    std::function<void()> OnWindowCloseCallback;
    std::function<void(int32_t focused)> OnWindowFocusCallback;

public:
    Event<int32_t, int32_t, int32_t, int32_t> OnKey{};
    Event<uint32_t> OnChar{};
    Event<double, double> OnCursorPos{};
    Event<int32_t, int32_t, int32_t> OnMouseButton{};
    Event<double, double> OnScroll{};
    Event<int32_t, int32_t> OnFramebufferSize{};
    Event<> OnWindowClose{};
    Event<int32_t> OnWindowFocus{};

    explicit InputSystem(WindowSystem &windowSystem) : windowSystem(windowSystem) {
    }

    void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) override {
        windowSystem.OnKeyCallback = [this](int32_t k, int32_t s, int32_t a, int32_t m) {
            OnKey.Invoke(k, s, a, m);
        };

        windowSystem.OnCharCallback = [this](uint32_t c) {
            OnChar.Invoke(c);
        };

        windowSystem.OnCursorPosCallback = [this](double x, double y) {
            OnCursorPos.Invoke(x, y);
        };

        windowSystem.OnMouseButtonCallback = [this](int32_t b, int32_t a, int32_t m) {
            OnMouseButton.Invoke(b, a, m);
        };

        windowSystem.OnScrollCallback = [this](double x, double y) {
            OnScroll.Invoke(x, y);
        };

        windowSystem.OnFramebufferSizeCallback = [this](int32_t w, int32_t h) {
            OnFramebufferSize.Invoke(w, h);
        };

        windowSystem.OnWindowCloseCallback = [this]() {
            OnWindowClose.Invoke();
        };

        windowSystem.OnWindowFocusCallback = [this](int32_t f) {
            OnWindowFocus.Invoke(f);
        };
    }
};
