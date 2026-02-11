//
// Created by Misha on 2/3/2026.
//

#ifndef DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H
#define DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H

#include <algorithm>

#include "engine/systems/SystemBase.h"
#include "engine/core/EngineContext.h"
#include "engine/DxUtils.h"
#include "engine/systems/QueueSystem.h"

class SwapChainSystem : public SystemBase {
    dx::ComPtr<IDXGISwapChain4> swapChain;
    DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_SWAP_CHAIN_DESC1 scDesc{};

    void buildSwapChainDesc(const EngineContext &ctx) {
        scDesc = {
            .Width = std::max(1u, ctx.windowConfig.width),
            .Height = std::max(1u, ctx.windowConfig.height),
            .Format = swapChainFormat,
            .SampleDesc = {.Count = 1, .Quality = 0},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = ctx.swapChainConfig.bufferCount,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        };
    }

    uint32_t currentBackBufferIndex = 0;

    size_t rtvDescriptorSize = 0;
    dx::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    std::vector<dx::ComPtr<ID3D12Resource>> renderTargets;
public:
    explicit SwapChainSystem() = default;

    void Initialize(EngineContext &ctx) {
        buildSwapChainDesc(ctx);

        dx::ComPtr<IDXGISwapChain1> swapChain1;
        const auto &commandQueue = ctx.systems.Get<QueueSystem>();
        const auto windowHandler = ctx.windowConfig.windowHandle;

        DX_CHECK(ctx.dx.factory->CreateSwapChainForHwnd(
            &commandQueue.GetGraphicsQueue(),
            windowHandler,
            &scDesc,
            nullptr,
            nullptr,
            &swapChain1
        ));

        DX_CHECK(swapChain1.As(&swapChain));

        DX_CHECK(ctx.dx.factory->MakeWindowAssociation(windowHandler, 0));

        InitializeRtvDescriptorHeap(ctx.dx.device.Get());
    }

    void Present(const EngineContext &ctx) {
        DX_CHECK(swapChain->Present(ctx.windowConfig.vsync, 0));
        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    }

    [[nodiscard]] ID3D12Resource* GetCurrentBackBuffer() const {
        return renderTargets[currentBackBufferIndex].Get();
    }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(
            rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            currentBackBufferIndex,
            rtvDescriptorSize
        );
    }
private:
    void InitializeRtvDescriptorHeap(ID3D12Device* device) {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = scDesc.BufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DX_CHECK(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

        rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        renderTargets.resize(scDesc.BufferCount);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        uint32_t i = 0;
        for (auto& target : renderTargets) {
            DX_CHECK(swapChain->GetBuffer(i++, IID_PPV_ARGS(&target)));
            device->CreateRenderTargetView(target.Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }
};

#endif //DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H
