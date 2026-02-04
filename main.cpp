#include "engine/EngineContext.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdexcept>

#include "engine/DxUtils.h"

#include <print>
#include <thread>
#include <chrono>
#include <atomic>

#include "engine/systems/window/SwapChainSystem.h"
#include "engine/systems/window/WindowSystem.h"


int main() {
#if defined(_DEBUG)
    dx::ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
        std::print("[DX12] Debug layer enabled\n");
    } else {
        std::print("[DX12] Debug layer not available (Graphics Tools missing)\n");
    }
#endif

    EngineContext engineContext;


    {
        dx::ComPtr<IDXGIFactory6> factory;
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

        dx::ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
                D3D_FEATURE_LEVEL_12_2,
                __uuidof(ID3D12Device),
                nullptr))) {
                break;
            }
        }
        DX_CHECK(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&engineContext.dx.device)));
    }
#ifdef _DEBUG
    DX12DebugSetup(engineContext.dx.device.Get());
    std::atomic<bool> debugThreadStarted = false;
    std::jthread t([&] {
        std::print("[DX12] Debug polling thread started\n");
        debugThreadStarted.store(true, std::memory_order_release);

        while (engineContext.running) {
            DX12DebugPoll();
        }
    });
    while (!debugThreadStarted.load(std::memory_order_acquire)) {
        std::this_thread::yield(); // or sleep_for(1ms)
    }
#endif


    auto &windowSystem = engineContext.systems.Register<WindowSystem>();
    windowSystem.Initialize(engineContext);

    auto &queueSystem = engineContext.systems.Register<QueueSystem>();
    queueSystem.Initialize(engineContext);

    auto &swapChainSystem = engineContext.systems.Register<SwapChainSystem>(engineContext);
    swapChainSystem.Initialize(engineContext);

    windowSystem.StartPolling();

    engineContext.running = false;

    return 0;
}
