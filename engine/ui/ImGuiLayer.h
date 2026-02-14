//
// Created by Misha on 2/13/2026.
//

#pragma once

#include <d3d12.h>
#include <windows.h>

class ImGuiLayer {
public:
    void Init(HWND hwnd, ID3D12Device* device, ID3D12DescriptorHeap* srvHeap, int frameCount);
    void BeginFrame();
    void EndFrame(ID3D12GraphicsCommandList* cmd);
    void Shutdown();

    [[nodiscard]] bool IsEnabled() const { return enabled; }

private:
    bool enabled = false;
};
