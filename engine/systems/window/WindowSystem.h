//
// Created by Misha on 2/3/2026.
//

#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include <functional>

#include "engine/DxUtils.h"
#include "engine/core/EngineContextInternal.h"
#include "engine/systems/SystemBase.h"
#include "engine/Globals.h"
#include <third_party/glfw/glfw3.h>
#include <third_party/glfw/glfw3native.h>


class WindowSystem : public SystemBase {
    GLFWwindow *window = nullptr;
    HWND windowHandle = nullptr;

public:
    explicit WindowSystem() = default;

    std::function<void(int key, int scancode, int action, int mods)> OnKeyCallback;
    std::function<void(unsigned int codepoint)> OnCharCallback;
    std::function<void(double xpos, double ypos)> OnCursorPosCallback;
    std::function<void(int button, int action, int mods)> OnMouseButtonCallback;
    std::function<void(double xoffset, double yoffset)> OnScrollCallback;
    std::function<void(int width, int height)> OnFramebufferSizeCallback;
    std::function<void()> OnWindowCloseCallback;
    std::function<void(int focused)> OnWindowFocusCallback;

    void Startup(EngineContextInternal &ctx, EngineConfigs &configs) override {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // important for DX12

        window = glfwCreateWindow(
            static_cast<int32_t>(configs.window.width), static_cast<int32_t>(configs.window.height),
            Globals::PROJ_NAME.data(), nullptr, nullptr
        );

        windowHandle = glfwGetWin32Window(window);
        glfwSetWindowUserPointer(window, this);

        glfwSetKeyCallback(window, [](GLFWwindow *window, const int key, const int scancode, const int action,
                                      const int mods) {
            auto *instance = static_cast<WindowSystem *>(
                glfwGetWindowUserPointer(window)
            );

            if (instance && instance->OnKeyCallback) instance->OnKeyCallback(key, scancode, action, mods);
        });
        glfwSetCharCallback(window,
                            [](GLFWwindow *window, const unsigned int codepoint) {
                                const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                if (instance && instance->OnCharCallback)
                                    instance->OnCharCallback(codepoint);
                            });
        glfwSetCursorPosCallback(window,
                                 [](GLFWwindow *window, const double xpos, const double ypos) {
                                     const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                     if (instance && instance->OnCursorPosCallback)
                                         instance->OnCursorPosCallback(xpos, ypos);
                                 });
        glfwSetMouseButtonCallback(window,
                                   [](GLFWwindow *window, const int button, const int action, const int mods) {
                                       const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                       if (instance && instance->OnMouseButtonCallback)
                                           instance->OnMouseButtonCallback(button, action, mods);
                                   });
        glfwSetScrollCallback(window,
                              [](GLFWwindow *window, const double xoffset, const double yoffset) {
                                  const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                  if (instance && instance->OnScrollCallback)
                                      instance->OnScrollCallback(xoffset, yoffset);
                              });
        glfwSetFramebufferSizeCallback(window,
                                       [](GLFWwindow *window, const int width, const int height) {
                                           const auto* instance = static_cast<WindowSystem *>(
                                               glfwGetWindowUserPointer(window));
                                           if (instance && instance->OnFramebufferSizeCallback)
                                               instance->OnFramebufferSizeCallback(width, height);
                                       });
        glfwSetWindowCloseCallback(window,
                                   [](GLFWwindow *window) {
                                       const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                       if (instance && instance->OnWindowCloseCallback)
                                           instance->OnWindowCloseCallback();
                                   });
        glfwSetWindowFocusCallback(window,
                                   [](GLFWwindow *window, const int focused) {
                                       const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                       if (instance && instance->OnWindowFocusCallback)
                                           instance->OnWindowFocusCallback(focused);
                                   });
    }

    void PollEvents() {
        glfwPollEvents();
    }

    [[nodiscard]] int ShouldWindowClose() const {
        return glfwWindowShouldClose(window);
    }

    void CloseWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    [[nodiscard]] HWND GetWindowHandle() const {
        return windowHandle;
    }
};
