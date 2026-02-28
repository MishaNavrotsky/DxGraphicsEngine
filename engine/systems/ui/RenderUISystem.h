//
// Created by Misha on 2/13/2026.
//

#pragma once

#include <d3d12.h>
#include <windows.h>

#include "engine/core/EngineContextInternal.h"
#include "engine/systems/SystemBase.h"
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_internal.h"
#include "third_party/imgui/imgui_impl_dx12.h"
#include "third_party/imgui/imgui_impl_win32.h"

#include "engine/DxUtils.h"
#include "engine/libs/DescriptorHeaps.h"
#include "engine/systems/window/WindowSystem.h"

inline ImGuiKey MapGlfwKeyToImGui(int32_t key) {
    // Direct mappings for letters and numbers
    if (key >= 48 && key <= 57) return (ImGuiKey) (ImGuiKey_0 + (key - 48)); // 0-9
    if (key >= 65 && key <= 90) return (ImGuiKey) (ImGuiKey_A + (key - 65)); // A-Z
    if (key >= 290 && key <= 301) return (ImGuiKey) (ImGuiKey_F1 + (key - 290)); // F1-F12

    switch (key) {
        // Special keys
        case 256: return ImGuiKey_Escape;
        case 257: return ImGuiKey_Enter;
        case 258: return ImGuiKey_Tab;
        case 259: return ImGuiKey_Backspace;
        case 260: return ImGuiKey_Insert;
        case 261: return ImGuiKey_Delete;
        case 262: return ImGuiKey_RightArrow;
        case 263: return ImGuiKey_LeftArrow;
        case 264: return ImGuiKey_DownArrow;
        case 265: return ImGuiKey_UpArrow;
        case 266: return ImGuiKey_PageUp;
        case 267: return ImGuiKey_PageDown;
        case 268: return ImGuiKey_Home;
        case 269: return ImGuiKey_End;
        case 280: return ImGuiKey_CapsLock;
        case 281: return ImGuiKey_ScrollLock;
        case 282: return ImGuiKey_NumLock;
        case 283: return ImGuiKey_PrintScreen;
        case 284: return ImGuiKey_Pause;

        // Modifiers
        case 340: return ImGuiKey_LeftShift;
        case 341: return ImGuiKey_LeftCtrl;
        case 342: return ImGuiKey_LeftAlt;
        case 343: return ImGuiKey_LeftSuper;
        case 344: return ImGuiKey_RightShift;
        case 345: return ImGuiKey_RightCtrl;
        case 346: return ImGuiKey_RightAlt;
        case 347: return ImGuiKey_RightSuper;

        // Symbols
        case 32: return ImGuiKey_Space;
        case 39: return ImGuiKey_Apostrophe;
        case 44: return ImGuiKey_Comma;
        case 45: return ImGuiKey_Minus;
        case 46: return ImGuiKey_Period;
        case 47: return ImGuiKey_Slash;
        case 59: return ImGuiKey_Semicolon;
        case 61: return ImGuiKey_Equal;
        case 91: return ImGuiKey_LeftBracket;
        case 92: return ImGuiKey_Backslash;
        case 93: return ImGuiKey_RightBracket;
        case 96: return ImGuiKey_GraveAccent;

        default: return ImGuiKey_None;
    }
}

class RenderUISystem : public SystemBase {
    WindowSystem &windowSystem;
    RenderSystem &renderSystem;
    InputSystem &inputSystem;
    ImGuiContext *imguiContext = nullptr;
    DescriptorHandle descriptorHandle{};
    BumpAllocator allocator{};
    const uint32_t DescriptorHeapSize = 256;

public:
    explicit RenderUISystem(WindowSystem &window, InputSystem &inputSystem, RenderSystem &renderSystem
    ) : windowSystem(window), inputSystem(inputSystem),

        renderSystem(renderSystem) {
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
    };

    void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) override {
        const HWND windowHandle = windowSystem.GetWindowHandle();
        assert(windowHandle != nullptr);
        auto *device = ctx.dx.device.Get();
        assert(device != nullptr);

        allocator.Initialize(0, DescriptorHeapSize);
        descriptorHandle = renderSystem.GetDescriptorHeap().AllocateStatic(DescriptorHeapSize);

        ImGui_ImplWin32_Init(windowSystem.GetWindowHandle());

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device;
        init_info.CommandQueue = renderSystem.GetDxQueues().GetGraphicsQueue();
        init_info.NumFramesInFlight = static_cast<int32_t>(configs.swapChain.bufferCount);
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        init_info.UserData = this;

        init_info.SrvDescriptorHeap = renderSystem.GetDescriptorHeap().Get();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo *info, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle,
                                            D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle) {
            auto &uiSystem = *static_cast<RenderUISystem *>(info->UserData);

            *out_cpu_handle = {
                uiSystem.descriptorHandle.startCpu.ptr + uiSystem.allocator.currentOffset * uiSystem.renderSystem.
                GetDescriptorHeap().GetDescriptorHandleIncrementSize()
            };
            *out_gpu_handle = {
                uiSystem.descriptorHandle.startGpu.ptr + uiSystem.allocator.currentOffset * uiSystem.renderSystem.
                GetDescriptorHeap().GetDescriptorHandleIncrementSize()
            };

            const uint32_t index = uiSystem.allocator.Allocate(1);

            assert(index != InvalidIndex);
        };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo *info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                                           D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
            auto &context = *static_cast<EngineContextInternal *>(info->UserData);
            //TODO: for now there is no way to clear allocated descriptor heap slots and I'm not sure if it is needed if SrvDescriptorAllocFn runs per frame for each required resource
        };
        ImGui_ImplDX12_Init(&init_info);

        inputSystem.OnKey.Subscribe([](int32_t key, int32_t scancode, int32_t action, int32_t mods) {
            ImGuiIO &io = ImGui::GetIO();
            ImGuiKey imguiKey = MapGlfwKeyToImGui(key);

            io.AddKeyEvent(imguiKey, (action != 0));

            io.AddKeyEvent(ImGuiMod_Ctrl, (mods & 0x0002) != 0);
            io.AddKeyEvent(ImGuiMod_Shift, (mods & 0x0001) != 0);
            io.AddKeyEvent(ImGuiMod_Alt, (mods & 0x0004) != 0);
            io.AddKeyEvent(ImGuiMod_Super, (mods & 0x0008) != 0);
        });

        inputSystem.OnCursorPos.Subscribe([](double xpos, double ypos) {
            ImGui::GetIO().AddMousePosEvent((float) xpos, (float) ypos);
        });

        inputSystem.OnMouseButton.Subscribe([](int32_t button, int32_t action, int32_t mods) {
            ImGui::GetIO().AddMouseButtonEvent(button, action != 0);
        });

        inputSystem.OnScroll.Subscribe([](double xoffset, double yoffset) {
            ImGui::GetIO().AddMouseWheelEvent((float) xoffset, (float) yoffset);
        });

        inputSystem.OnChar.Subscribe([](uint32_t codepoint) {
            ImGui::GetIO().AddInputCharacter(codepoint);
        });
    }


    void BeginFrame(EngineContextInternal &ctx) {
        allocator.Reset();
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        bool open = true;
        ImGui::ShowDemoWindow(&open);
    }

    void EndFrame(EngineContextInternal &ctx, ID3D12GraphicsCommandList *commandList) {
        ImGui::Render();

        ID3D12DescriptorHeap *heaps[] = {renderSystem.GetDescriptorHeap().Get()};
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    }

    void Shutdown() override {
        if (imguiContext) {
            ImGui_ImplWin32_Shutdown();
            ImGui_ImplDX12_Shutdown();
            ImGui::DestroyContext(imguiContext);
        }
    }

    [[nodiscard]] ImGuiContext *GetContext() const { return imguiContext; }
};
