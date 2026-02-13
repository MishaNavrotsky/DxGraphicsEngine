//
// Created by Misha on 2/11/2026.
//

#include "engine/core/Engine.h"

#include "engine/DxUtils.h"
#include "engine/core/EngineContext.h"

#include "engine/systems/QueueSystem.h"
#include "engine/systems/window/SwapChainSystem.h"
#include "engine/systems/window/WindowSystem.h"
#include "engine/systems/render/RenderSystem.h"

struct Engine::Impl {
    EngineContext context;
    std::stop_source stopSource;

    void InitializeContext() {
        context.stopToken = stopSource.get_token();
    }
    void InitializeAdapter() {
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&context.dx.factory)));

        dx::ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; context.dx.factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            DX_CHECK(adapter->GetDesc1(&desc));

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
                D3D_FEATURE_LEVEL_12_2,
                __uuidof(ID3D12Device),
                nullptr))) {
                break;
                }
        }
        DX_CHECK(adapter.As(&context.dx.adapter));
        DX_CHECK(D3D12CreateDevice(context.dx.adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&context.dx.device)));
    }
    void InitializeSystems() {
        auto &queueSystem = context.systems.Register<QueueSystem>();
        auto &renderSystem = context.systems.Register<RenderSystem>();
        auto &windowSystem = context.systems.Register<WindowSystem>();
        auto &swapChainSystem = context.systems.Register<SwapChainSystem>();

        queueSystem.Initialize(context);
        renderSystem.Initialize(context);
        windowSystem.Initialize(context);
        swapChainSystem.Initialize(context);
    }
    void InitializeAllocator() {
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = context.dx.device.Get();
        allocatorDesc.pAdapter = context.dx.adapter.Get();

        DX_CHECK(D3D12MA::CreateAllocator(&allocatorDesc, &context.dx.allocator));
    }
#ifdef _DEBUG
    static void InitializeDebug() {
            dx::ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                std::print("[DX12] Debug layer enabled\n");
            } else {
                std::print("[DX12] Debug layer not available (Graphics Tools missing)\n");
            }
    }

    std::thread adapterDebugThread;
    std::stop_source adapterDebugThreadStopSource;
    std::stop_token adapterDebugThreadStopToken = adapterDebugThreadStopSource.get_token();
    void InitializeAdapterDebug() {
        DX12DebugSetup(context.dx.device.Get());
        std::atomic<bool> debugThreadStarted = false;
        adapterDebugThread = std::thread([&](const std::stop_token &st) {
            std::print("[DX12] Debug polling thread started\n");
            debugThreadStarted.store(true, std::memory_order_release);

            while (!st.stop_requested()) {
                DX12DebugPoll();
            }

            std::print("[DX12] Debug polling thread stopped\n");
        }, adapterDebugThreadStopToken);
        while (!debugThreadStarted.load(std::memory_order_acquire)) {
            std::this_thread::yield(); // or sleep_for(1ms)
        }
    }
#endif
};

std::unique_ptr<Engine> Engine::instance = nullptr;

Engine::Engine() : pimpl(std::make_unique<Engine::Impl>()) {}
Engine::~Engine() = default;

void Engine::Initialize() {
#ifdef _DEBUG
    Engine::Impl::InitializeDebug();
#endif

    if (instance) return;

    instance = std::unique_ptr<Engine>(new Engine());
    instance->pimpl->InitializeAdapter();
    instance->pimpl->InitializeAllocator();
#ifdef _DEBUG
    instance->pimpl->InitializeAdapterDebug();
#endif

    instance->pimpl->InitializeSystems();
}

Engine &Engine::Get() {
    return *instance;
}

void Engine::Run() {
    const WindowSystem &windowSystem = instance->pimpl->context.systems.Get<WindowSystem>();
    windowSystem.StartPolling(instance->pimpl->context);
}

void Engine::Shutdown() {
#ifdef _DEBUG
    instance->pimpl->adapterDebugThreadStopSource.request_stop();
    if (instance->pimpl->adapterDebugThread.joinable()) instance->pimpl->adapterDebugThread.join();
#endif
    instance->pimpl->stopSource.request_stop();
}