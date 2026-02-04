//
// Created by Misha on 2/3/2026.
//

#ifndef DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H
#define DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H
#include <dxgi1_6.h>

#include "engine/systems/window/WindowSystem.h"
#include "engine/systems/SystemBase.h"
#include "engine/EngineContext.h"
#include "engine/DxUtils.h"
#include "engine/systems/QueueSystem.h"

class SwapChainSystem: public SystemBase {
    dx::ComPtr<IDXGISwapChain4> swapChain;
    DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_SWAP_CHAIN_DESC1 scDesc {};
    void buildSwapChainDesc(const EngineContext& ctx) {
        scDesc = {
            .Width = ctx.windowConfig.width,
            .Height = ctx.windowConfig.height,
            .Format = swapChainFormat,
            .SampleDesc = {.Count = 1, .Quality = 0 },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = ctx.dx.bufferCount,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        };
    }

public:
    explicit SwapChainSystem(const EngineContext& ctx) {
        buildSwapChainDesc(ctx);
    }

    void Initialize(EngineContext& ctx) {
        dx::ComPtr<IDXGIFactory6> factory;
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

        dx::ComPtr<IDXGISwapChain1> swapChain1;
        const auto& commandQueue = ctx.systems.Get<QueueSystem>();
        const auto windowHandler = ctx.systems.Get<WindowSystem>().GetWindowHandle();

        DX_CHECK(factory->CreateSwapChainForHwnd(
            &commandQueue.GetGraphicsQueue(),
            windowHandler,
            &scDesc,
            nullptr,
            nullptr,
            &swapChain1
        ));

        DX_CHECK(swapChain1.As(&swapChain));

        DX_CHECK(factory->MakeWindowAssociation(windowHandler, 0));
    }

    IDXGISwapChain4& GetSwapChain() const {
        return *swapChain.Get();
    }

    DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() {
        return scDesc;
    }
};

#endif //DXGRAPHICSENGINE_SWAPCHAINSYSTEM_H