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

    std::function<void(int32_t key, int32_t scancode, int32_t action, int32_t mods)> OnKeyCallback;
    std::function<void(uint32_t codepoint)> OnCharCallback;
    std::function<void(double xpos, double ypos)> OnCursorPosCallback;
    std::function<void(int32_t button, int32_t action, int32_t mods)> OnMouseButtonCallback;
    std::function<void(double xoffset, double yoffset)> OnScrollCallback;
    std::function<void(int32_t width, int32_t height)> OnFramebufferSizeCallback;
    std::function<void()> OnWindowCloseCallback;
    std::function<void(int32_t focused)> OnWindowFocusCallback;

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

        glfwSetKeyCallback(window, [](GLFWwindow *window, const int32_t key, const int32_t scancode, const int32_t action,
                                      const int32_t mods) {
            auto *instance = static_cast<WindowSystem *>(
                glfwGetWindowUserPointer(window)
            );

            if (instance && instance->OnKeyCallback) instance->OnKeyCallback(key, scancode, action, mods);
        });
        glfwSetCharCallback(window,
                            [](GLFWwindow *window, const uint32_t codepoint) {
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
                                   [](GLFWwindow *window, const int32_t button, const int32_t action, const int32_t mods) {
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
                                       [](GLFWwindow *window, const int32_t width, const int32_t height) {
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
                                   [](GLFWwindow *window, const int32_t focused) {
                                       const auto* instance = static_cast<WindowSystem *>(glfwGetWindowUserPointer(window));
                                       if (instance && instance->OnWindowFocusCallback)
                                           instance->OnWindowFocusCallback(focused);
                                   });
    }

    void PollEvents() {
        glfwPollEvents();
    }

    [[nodiscard]] int32_t ShouldWindowClose() const {
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
