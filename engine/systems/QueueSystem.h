//
// Created by Misha on 2/3/2026.
//

#ifndef DXGRAPHICSENGINE_QUEUESYSTEM_H
#define DXGRAPHICSENGINE_QUEUESYSTEM_H

#include "SystemBase.h"
#include "engine/DxUtils.h"
#include "engine/EngineContext.h"

class QueueSystem : public SystemBase {
public:
    explicit QueueSystem() = default;
    void Initialize(EngineContext& ctx) {
        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        DX_CHECK(ctx.dx.device->CreateCommandQueue(&desc, IID_PPV_ARGS(&graphicsQueue)));

        desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        DX_CHECK(ctx.dx.device->CreateCommandQueue(&desc, IID_PPV_ARGS(&computeQueue)));

        desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        DX_CHECK(ctx.dx.device->CreateCommandQueue(&desc, IID_PPV_ARGS(&copyQueue)));
    }

    [[nodiscard]] ID3D12CommandQueue& GetGraphicsQueue() const {
        return *graphicsQueue.Get();
    };
    [[nodiscard]] ID3D12CommandQueue& GetComputeQueue() const {
        return *computeQueue.Get();
    };
    [[nodiscard]] ID3D12CommandQueue& GetCopyQueue() const {
        return *copyQueue.Get();
    };

private:
    dx::ComPtr<ID3D12CommandQueue> graphicsQueue;
    dx::ComPtr<ID3D12CommandQueue> computeQueue;
    dx::ComPtr<ID3D12CommandQueue> copyQueue;
};

#endif //DXGRAPHICSENGINE_QUEUESYSTEM_H