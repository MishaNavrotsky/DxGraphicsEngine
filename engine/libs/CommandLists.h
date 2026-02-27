//
// Created by Misha on 2/19/2026.
//

#pragma once

#include "engine/DxUtils.h"

struct CommandContext {
private:
    dx::ComPtr<ID3D12GraphicsCommandList7> m_commandList;

public:
    CommandContext() = default;

    ~CommandContext() = default;

    CommandContext(const CommandContext &) = delete;

    CommandContext &operator=(const CommandContext &) = delete;

    CommandContext(CommandContext &&) = default;

    CommandContext &operator=(CommandContext &&) = default;

    void Initialize(ID3D12Device10 *device, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *initialAllocator) {
        assert(device != nullptr && initialAllocator != nullptr);

        DX_CHECK(device->CreateCommandList(
            0,
            type,
            initialAllocator,
            nullptr,
            IID_PPV_ARGS(&m_commandList)
        ));

        DX_CHECK(m_commandList->Close());
    }

    void Begin(ID3D12CommandAllocator *currentFrameAllocator) const {
        assert(currentFrameAllocator != nullptr);

        DX_CHECK(m_commandList->Reset(currentFrameAllocator, nullptr));
    }

    void End() const {
        DX_CHECK(m_commandList->Close());
    }

    void TransitionTexture(ID3D12Resource *resource,
                           const D3D12_BARRIER_SYNC syncBefore, const D3D12_BARRIER_SYNC syncAfter,
                           const D3D12_BARRIER_ACCESS accessBefore, const D3D12_BARRIER_ACCESS accessAfter,
                           const D3D12_BARRIER_LAYOUT layoutBefore, const D3D12_BARRIER_LAYOUT layoutAfter) const {
        D3D12_TEXTURE_BARRIER barrier = {};
        barrier.SyncBefore = syncBefore;
        barrier.SyncAfter = syncAfter;
        barrier.AccessBefore = accessBefore;
        barrier.AccessAfter = accessAfter;
        barrier.LayoutBefore = layoutBefore;
        barrier.LayoutAfter = layoutAfter;
        barrier.pResource = resource;

        barrier.Subresources.IndexOrFirstMipLevel = 0xffffffff;
        barrier.Subresources.NumMipLevels = 0;
        barrier.Subresources.FirstArraySlice = 0;
        barrier.Subresources.NumArraySlices = 0;

        D3D12_BARRIER_GROUP group = {};
        group.Type = D3D12_BARRIER_TYPE_TEXTURE;
        group.NumBarriers = 1;
        group.pTextureBarriers = &barrier;

        m_commandList->Barrier(1, &group);
    }

    void TransitionBackBuffer(ID3D12Resource *resource, const bool toRenderTarget) const {
        if (toRenderTarget) {
            TransitionTexture(resource,
                              D3D12_BARRIER_SYNC_NONE, D3D12_BARRIER_SYNC_RENDER_TARGET,
                              D3D12_BARRIER_ACCESS_NO_ACCESS, D3D12_BARRIER_ACCESS_RENDER_TARGET,
                              D3D12_BARRIER_LAYOUT_PRESENT, D3D12_BARRIER_LAYOUT_RENDER_TARGET);
        } else {
            TransitionTexture(resource,
                              D3D12_BARRIER_SYNC_RENDER_TARGET, D3D12_BARRIER_SYNC_NONE,
                              D3D12_BARRIER_ACCESS_RENDER_TARGET, D3D12_BARRIER_ACCESS_NO_ACCESS,
                              D3D12_BARRIER_LAYOUT_RENDER_TARGET, D3D12_BARRIER_LAYOUT_PRESENT);
        }
    }

    void ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float color[4]) const {
        m_commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
    }

    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv) const {
        m_commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
    }

    [[nodiscard]] ID3D12GraphicsCommandList7 *GetRawList() const {
        return m_commandList.Get();
    }
};

struct CommandLists {
private:
    dx::ComPtr<ID3D12CommandAllocator> commandAllocatorDirect;
    CommandContext commandListDirect{};

public:
    void Initialize(ID3D12Device10 *device, const CommandListsConfig &config) {
        DX_CHECK(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocatorDirect)));
        commandListDirect.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocatorDirect.Get());
    };

    void ResetDirect() const {
        DX_CHECK(commandAllocatorDirect->Reset());
        commandListDirect.Begin(commandAllocatorDirect.Get());
    }

    void CloseDirect() const {
        commandListDirect.End();
    }

    CommandContext &GetCommandListDirect() {
        return commandListDirect;
    }
};
