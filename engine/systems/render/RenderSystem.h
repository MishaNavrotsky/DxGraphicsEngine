//
// Created by Misha on 2/11/2026.
//

#pragma once

#include "engine/core/EngineContextInternal.h"
#include "engine/libs/CommandLists.h"
#include "engine/systems/SystemBase.h"
#include "engine/libs/DxFence.h"

class RenderSystem : public SystemBase {
    CbvSrvUavDescriptorHeap descriptorHeap{};
    DxQueues dxQueues{};
    CommandLists commandLists{};
    DxFence fence{};

public:
    explicit RenderSystem() = default;

    void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) override {
        auto *device = ctx.dx.device.Get();

        dxQueues.Initialize(device);
        descriptorHeap.Initialize(
            device, configs.bindlessHeapConfig, configs.engineConfig
        );
        commandLists.Initialize(device, configs.commandListsConfig);
        fence.Initialize(device);
    }

    void BeginFrame(EngineContextInternal &ctx) {
        commandLists.ResetDirect();
    }

    void EndFrame(EngineContextInternal &ctx) {
        auto *graphicsQueue = dxQueues.GetGraphicsQueue();

        commandLists.CloseDirect();
        ID3D12CommandList *lists[] = {commandLists.GetCommandListDirect().GetRawList()};
        graphicsQueue->ExecuteCommandLists(1, lists);
    }

    void WaitForFrame() {
        fence.Wait();
    }

    void SignalQueue() {
        fence.Signal(dxQueues.GetGraphicsQueue());
    }

    CommandLists &GetCommandLists() {
        return commandLists;
    }

    CbvSrvUavDescriptorHeap &GetDescriptorHeap() {
        return descriptorHeap;
    }

    DxQueues &GetDxQueues() {
        return dxQueues;
    }

    void Shutdown() override {
        auto *graphicsQueue = dxQueues.GetGraphicsQueue();

        SignalQueue();
        WaitForFrame();
    }
};
