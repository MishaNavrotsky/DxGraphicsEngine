//
// Created by Misha on 2/3/2026.
//

#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include "engine/DxUtils.h"
#include "engine/core/EngineContext.h"
#include "engine/systems/SystemBase.h"
#include "engine/Globals.h"
#include <third_party/glfw/glfw3.h>
#include <third_party/glfw/glfw3native.h>

#include "engine/systems/render/RenderSystem.h"
#include "engine/systems/window/SwapChainSystem.h"

class WindowSystem: public SystemBase {
    GLFWwindow* window = nullptr;
    HWND windowHandle = nullptr;
public:
    explicit WindowSystem() = default;

    void Initialize(EngineContext& ctx) {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // important for DX12

        window = glfwCreateWindow(
            static_cast<int32_t>(ctx.windowConfig.width), static_cast<int32_t>(ctx.windowConfig.height), Globals::PROJ_NAME.data(), nullptr, nullptr
        );

        windowHandle = glfwGetWin32Window(window);
        ctx.windowConfig.windowHandle = windowHandle;
    }

    void StartPolling(EngineContext& ctx) const {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    [[nodiscard]] HWND GetWindowHandle() const {
        return windowHandle;
    }
};
