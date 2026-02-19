//
// Created by Misha on 2/3/2026.
//

#pragma once

#include "engine/DxUtils.h"
#include "engine/Types.h"
#include "engine/libs/DescriptorHeaps.h"


struct DxContext {
    dx::ComPtr<ID3D12Device> device;
    dx::ComPtr<IDXGIFactory6> factory;
    dx::ComPtr<IDXGIAdapter4> adapter;
    dx::ComPtr<D3D12MA::Allocator> allocator;
};

struct EngineContextInternal {
    DxContext dx;
    std::stop_token stopToken;
};
