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

class UISystem : public SystemBase {
    WindowSystem& windowSystem;
    QueueSystem& queueSystem;
    CbvSrvUavDescriptorHeap* descriptorHeap = nullptr;
    ImGuiContext* imguiContext = nullptr;
    DescriptorHandle descriptorHandle{};
    uint32_t descriptorHeapOffset = 0;
    const uint32_t DescriptorHeapSize = 256;
public:
    explicit UISystem(WindowSystem& window, QueueSystem& queueSystem) : windowSystem(window), queueSystem(queueSystem) {
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
    };
    void Startup(EngineContextInternal& ctx, EngineConfigs& configs) override {
        descriptorHeap = &ctx.dx.descriptorHeap;
        const HWND windowHandle = windowSystem.GetWindowHandle();
        assert(windowHandle != nullptr);
        auto* device = ctx.dx.device.Get();
        assert(device != nullptr);

        descriptorHandle = ctx.dx.descriptorHeap.AllocateStatic(DescriptorHeapSize);

        ImGui_ImplWin32_Init(windowSystem.GetWindowHandle());

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device;
        init_info.CommandQueue = &queueSystem.GetGraphicsQueue();
        init_info.NumFramesInFlight = static_cast<int32_t>(configs.swapChain.bufferCount);
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        init_info.UserData = this;

        init_info.SrvDescriptorHeap = ctx.dx.descriptorHeap.Get();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
            auto& uiSystem = *static_cast<UISystem*>(info->UserData);

            *out_cpu_handle = {uiSystem.descriptorHandle.startCpu.ptr + uiSystem.descriptorHeapOffset * uiSystem.descriptorHeap->GetDescriptorHandleIncrementSize()};;
            *out_gpu_handle = {uiSystem.descriptorHandle.startGpu.ptr + uiSystem.descriptorHeapOffset * uiSystem.descriptorHeap->GetDescriptorHandleIncrementSize()};;

            uiSystem.descriptorHeapOffset++;

            assert(uiSystem.descriptorHeapOffset < uiSystem.DescriptorHeapSize);
        };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
            auto& context = *static_cast<EngineContextInternal*>(info->UserData);
            //TODO: for now there is no way to clear allocated descriptor heap slots and I'm not sure if it is needed if SrvDescriptorAllocFn runs per frame for each required resource
        };
        ImGui_ImplDX12_Init(&init_info);
    }



    void BeginFrame(EngineContextInternal& ctx) {
        descriptorHeapOffset = 0;
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        bool open = true;
        ImGui::ShowDemoWindow(&open);
    }

    void EndFrame(EngineContextInternal& ctx, ID3D12GraphicsCommandList* commandList) {
        ImGui::Render();

        ID3D12DescriptorHeap* heaps[] = { descriptorHeap->Get() };
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

    [[nodiscard]] ImGuiContext* GetContext() const { return imguiContext; }
};
