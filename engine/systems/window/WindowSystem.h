//
// Created by Misha on 2/3/2026.
//

#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include "engine/DxUtils.h"
#include "engine/core/EngineContextInternal.h"
#include "engine/systems/SystemBase.h"
#include "engine/Globals.h"
#include <third_party/glfw/glfw3.h>
#include <third_party/glfw/glfw3native.h>


class WindowSystem: public SystemBase {
    GLFWwindow* window = nullptr;
    HWND windowHandle = nullptr;
public:
    explicit WindowSystem() = default;

    void Startup(EngineContextInternal& ctx, EngineConfigs& configs) override {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // important for DX12

        window = glfwCreateWindow(
            static_cast<int32_t>(configs.window.width), static_cast<int32_t>(configs.window.height), Globals::PROJ_NAME.data(), nullptr, nullptr
        );

        windowHandle = glfwGetWin32Window(window);
    }

    void StartPolling(EngineContextInternal& ctx) const {
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
