//
// Created by Misha on 2/3/2026.
//

#pragma once

#include "engine/DxUtils.h"

struct DxQueues {
public:
    void Initialize(ID3D12Device *device) {
        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        DX_CHECK(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&graphicsQueue)));

        desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        DX_CHECK(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&computeQueue)));

        desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        DX_CHECK(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&copyQueue)));
    }

    [[nodiscard]] ID3D12CommandQueue &GetGraphicsQueue() const {
        return *graphicsQueue.Get();
    };

    [[nodiscard]] ID3D12CommandQueue &GetComputeQueue() const {
        return *computeQueue.Get();
    };

    [[nodiscard]] ID3D12CommandQueue &GetCopyQueue() const {
        return *copyQueue.Get();
    };

private:
    dx::ComPtr<ID3D12CommandQueue> graphicsQueue;
    dx::ComPtr<ID3D12CommandQueue> computeQueue;
    dx::ComPtr<ID3D12CommandQueue> copyQueue;
};
