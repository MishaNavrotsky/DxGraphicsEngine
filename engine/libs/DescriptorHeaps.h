//
// Created by Misha on 2/15/2026.
//

#pragma once

#include "engine/libs/Allocators.h"
#include "engine/DxUtils.h"
#include "engine/core/EngineConfigs.h"


struct CbvSrvUavDescriptorHeap {
    void Init(ID3D12Device* device, BindlessHeapConfig& config) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1000000,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };
        DX_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));
        allocator.Init(desc.NumDescriptors);
    }

    [[nodiscard]] ID3D12DescriptorHeap* Get() const {
        return heap.Get();
    }

    SlotAllocator allocator{};
    dx::ComPtr<ID3D12DescriptorHeap> heap;
};
