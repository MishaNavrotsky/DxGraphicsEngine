//
// Created by Misha on 2/3/2026.
//

#pragma once

#include <algorithm>

#include "engine/systems/SystemBase.h"
#include "engine/core/EngineContextInternal.h"
#include "engine/DxUtils.h"
#include "../../libs/DxQueues.h"
#include "engine/systems/render/RenderSystem.h"

class SwapChainSystem : public SystemBase {
    RenderSystem &renderSystem;
    HWND windowHandle;

    dx::ComPtr<IDXGISwapChain4> swapChain;
    const DXGI_FORMAT SwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_SWAP_CHAIN_DESC1 scDesc{};

    void buildSwapChainDesc(const EngineConfigs &configs) {
        scDesc = {
            .Width = std::max(1u, configs.window.width),
            .Height = std::max(1u, configs.window.height),
            .Format = SwapChainFormat,
            .SampleDesc = {.Count = 1, .Quality = 0},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = configs.swapChain.bufferCount,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        };
    }

    uint32_t currentBackBufferIndex = 0;

    size_t rtvDescriptorSize = 0;
    dx::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    std::vector<dx::ComPtr<ID3D12Resource> > renderTargets;

public:
    explicit SwapChainSystem(RenderSystem &renderSystem, HWND windowHandle) : renderSystem(renderSystem),
                                                                              windowHandle(windowHandle) {
    }

    void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) override {
        buildSwapChainDesc(configs);

        dx::ComPtr<IDXGISwapChain1> swapChain1;

        DX_CHECK(ctx.dx.factory->CreateSwapChainForHwnd(
            renderSystem.GetDxQueues().GetGraphicsQueue(),
            windowHandle,
            &scDesc,
            nullptr,
            nullptr,
            &swapChain1
        ));

        DX_CHECK(swapChain1.As(&swapChain));

        DX_CHECK(ctx.dx.factory->MakeWindowAssociation(windowHandle, 0));

        InitializeRtvDescriptorHeap(ctx.dx.device.Get());
    }

    void Present(const WindowConfig &config) {
        DX_CHECK(swapChain->Present(config.vsync, 0));
        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    }

    void Resize(ID3D12Device *device, uint32_t width, uint32_t height) {
        width = std::max(1u, width);
        height = std::max(1u, height);

        for (auto &target: renderTargets) {
            target.Reset();
        }

        DX_CHECK(swapChain->ResizeBuffers(
            scDesc.BufferCount,
            width, height,
            SwapChainFormat,
            0));

        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
        scDesc.Width = width;
        scDesc.Height = height;

        InitializeRtvDescriptorHeap(device);
    }

    [[nodiscard]] ID3D12Resource *GetCurrentBackBuffer() const {
        return renderTargets[currentBackBufferIndex].Get();
    }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtv() const {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(
            rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            static_cast<int32_t>(currentBackBufferIndex),
            rtvDescriptorSize
        );
    }

    void Shutdown() override {
        if (swapChain) {
            DX_CHECK(swapChain->SetFullscreenState(FALSE, nullptr));
        }
        for (auto &renderTarget: renderTargets) {
            renderTarget.Reset();
        }

        swapChain.Reset();
    }

private:
    void InitializeRtvDescriptorHeap(ID3D12Device *device) {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = scDesc.BufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DX_CHECK(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

        rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        renderTargets.resize(scDesc.BufferCount);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        uint32_t i = 0;
        for (auto &target: renderTargets) {
            DX_CHECK(swapChain->GetBuffer(i++, IID_PPV_ARGS(&target)));
            DX_CHECK(target->SetName(L"SwapChain_BackBuffer"));
            device->CreateRenderTargetView(target.Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }
};
