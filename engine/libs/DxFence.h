//
// Created by Misha on 2/27/2026.
//

#pragma once

#include "engine/DxUtils.h"

struct DxFence {
    dx::ComPtr<ID3D12Fence> fence;
    UINT64 value = 0;
    HANDLE event = nullptr;

    void Initialize(ID3D12Device *device) {
        DX_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        value = 1;
        event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!event)
            DX_CHECK(HRESULT_FROM_WIN32(GetLastError()));
    }

    ~DxFence() {
        if (event) CloseHandle(event);
    }

    void Wait() const {
        if (fence->GetCompletedValue() < value - 1) {
            DX_CHECK(fence->SetEventOnCompletion(value - 1, event));
            WaitForSingleObject(event, INFINITE);
        }
    }

    void Signal(ID3D12CommandQueue *queue) {
        DX_CHECK(queue->Signal(fence.Get(), value));
        value++;
    }
};
