//
// Created by Misha on 2/15/2026.
//

#pragma once

#include "engine/libs/Allocators.h"
#include "engine/DxUtils.h"
#include "engine/core/EngineConfigs.h"

static constexpr uint32_t UnknownNumberDescriptors = UINT32_MAX;

struct DescriptorHandle {
    uint32_t startIndex = InvalidIndex;
    uint32_t numDescriptors = UnknownNumberDescriptors;
    D3D12_CPU_DESCRIPTOR_HANDLE startCpu{};
    D3D12_GPU_DESCRIPTOR_HANDLE startGpu{};

    [[nodiscard]] bool IsValid() const { return startIndex != InvalidIndex; }
};

struct CbvSrvUavDescriptorHeap {
private:
    ID3D12Device *device = nullptr;
    const uint32_t MaxGpuDescriptors = 1000000;
    uint32_t numDescriptors = 0;
    uint32_t descriptorSize = 0;

    dx::ComPtr<ID3D12DescriptorHeap> heap;

    RangeSlotAllocator staticAllocator;
    RangeSlotAllocator reusableAllocator;
    std::array<BumpAllocator, FramesInFlightCount> dynamicAllocators;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuStart{};
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuStart{};

public:
    void Initialize(ID3D12Device *dv, const BindlessHeapConfig &config, const EngineConfig &engineConfig) {
        device = dv;
        numDescriptors = config.staticCapacity + config.reusableCapacity + (
                             config.dynamicCapacityPerFrame * FramesInFlightCount);
        if (numDescriptors > MaxGpuDescriptors) {
            throw std::runtime_error("[CbvSrvUavDescriptorHeap] Too many descriptors per GPU descriptor heap");
        }

        const D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = numDescriptors,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };
        DX_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));

        descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        cpuStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
        gpuStart = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart());

        uint32_t currentOffset = 0;

        staticAllocator.Initialize(currentOffset, config.staticCapacity);
        currentOffset += config.staticCapacity;

        reusableAllocator.Initialize(currentOffset, config.reusableCapacity);
        currentOffset += config.reusableCapacity;

        for (uint32_t i = 0; i < FramesInFlightCount; ++i) {
            dynamicAllocators[i].Initialize(currentOffset, config.dynamicCapacityPerFrame);
            currentOffset += config.dynamicCapacityPerFrame;
        }
    }

    [[nodiscard]] ID3D12DescriptorHeap *Get() const {
        return heap.Get();
    }

    [[nodiscard]] DescriptorHandle
    GetHandle(const uint32_t index, const uint32_t size = UnknownNumberDescriptors) const {
        if (index == InvalidIndex) return {};

        return {
            index,
            size,
            {cpuStart.ptr + index * descriptorSize},
            {gpuStart.ptr + index * descriptorSize},
        };
    }

    DescriptorHandle AllocateStatic(ID3D12DescriptorHeap *cpuStagingHeap, const uint32_t count) {
#ifdef _DEBUG
        const auto desc = cpuStagingHeap->GetDesc();
        if (desc.Flags != D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {
            std::printf("[CbvSrvUavDescriptorHeap] Source heap must be NON-shader visible (CPU-only).\n");
        }
        if (count > desc.NumDescriptors) {
            std::printf("[CbvSrvUavDescriptorHeap] Count (%u) exceeds staging heap size (%u).\n", count,
                        desc.NumDescriptors);
        }
#endif

        const uint32_t startIndex = staticAllocator.Allocate(count);
        if (startIndex == InvalidIndex) return {};

        const auto handle = GetHandle(startIndex, count);

        device->CopyDescriptorsSimple(
            count,
            handle.startCpu,
            cpuStagingHeap->GetCPUDescriptorHandleForHeapStart(),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );
        return handle;
    }

    DescriptorHandle AllocateStatic(const uint32_t count) {
        const uint32_t startIndex = staticAllocator.Allocate(count);
        if (startIndex == InvalidIndex) return {};

        return GetHandle(startIndex, count);;
    }

    DescriptorHandle AllocateDynamic(const uint32_t frameIndex, const uint32_t count = 1) {
        return GetHandle(dynamicAllocators[frameIndex].Allocate(count), count);
    }

    [[nodiscard]] uint32_t GetDescriptorHandleIncrementSize() const {
        return descriptorSize;
    }
};
