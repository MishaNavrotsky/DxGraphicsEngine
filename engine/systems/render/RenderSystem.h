//
// Created by Misha on 2/11/2026.
//

#pragma once

#include "engine/core/EngineContextInternal.h"
#include "engine/systems/SystemBase.h"

class RenderSystem : public SystemBase {
    CbvSrvUavDescriptorHeap descriptorHeap{};
    DxQueues dxQueues{};

public:
    explicit RenderSystem() = default;

    void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) override {
        dxQueues.Initialize(ctx.dx.device.Get());
        descriptorHeap.Initialize(
            ctx.dx.device.Get(), configs.bindlessHeapConfig, configs.engineConfig
        );
    }

    void BeginFrame(EngineContextInternal &ctx) {
    }

    void EndFrame(EngineContextInternal &ctx) {
    }

    CbvSrvUavDescriptorHeap &GetDescriptorHeap() {
        return descriptorHeap;
    }

    DxQueues &GetDxQueues() {
        return dxQueues;
    }
};
