//
// Created by Misha on 2/11/2026.
//

#include "engine/core/Engine.h"

#include <ranges>

#include "EngineContext.h"
#include "engine/DxUtils.h"
#include "engine/core/EngineContextInternal.h"

#include "engine/Logger.h"
#include "engine/systems/InputSystem.h"
#include "engine/systems/window/SwapChainSystem.h"
#include "engine/systems/window/WindowSystem.h"
#include "engine/systems/render/RenderSystem.h"
#include "engine/systems/ui/EditorUISystem.h"

struct Engine::Impl {
    EngineContext context;
    EngineContextInternal contextInternal;
    std::stop_source stopSource;

    void InitializeContext() {
        contextInternal.stopToken = stopSource.get_token();
    }

    void InitializeAdapter() {
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&contextInternal.dx.factory)));

        dx::ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; contextInternal.dx.factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
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
        DX_CHECK(adapter.As(&contextInternal.dx.adapter));
        DX_CHECK(
            D3D12CreateDevice(contextInternal.dx.adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&contextInternal.dx
                .device)));
    }

    void InitializeSystems() {
        auto &windowSystem = context.systems.Register<WindowSystem>();
        windowSystem.Initialize(contextInternal, context.configs);

        auto &inputSystem = context.systems.Register<InputSystem>(windowSystem);
        inputSystem.Initialize(contextInternal, context.configs);

        auto &renderSystem = context.systems.Register<RenderSystem>();
        renderSystem.Initialize(contextInternal, context.configs);

        auto &swapChainSystem = context.systems.Register<SwapChainSystem>(renderSystem, windowSystem.GetWindowHandle());
        swapChainSystem.Initialize(contextInternal, context.configs);

        if (context.configs.uiConfig.enabled) {
            auto &uiSystem = context.systems.Register<EditorUISystem>(windowSystem, renderSystem);
            uiSystem.Initialize(contextInternal, context.configs);
        }
    }

    void InitializeAllocator() {
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = contextInternal.dx.device.Get();
        allocatorDesc.pAdapter = contextInternal.dx.adapter.Get();

        DX_CHECK(D3D12MA::CreateAllocator(&allocatorDesc, &contextInternal.dx.allocator));
    }

#ifdef _DEBUG
    static void InitializeDebug() {
        dx::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            ENG_LOG_INFO("[DX12] Debug layer enabled\n");
        } else {
            ENG_LOG_INFO("[DX12] Debug layer not available (Graphics Tools missing)\n");
        }
    }

    std::thread adapterDebugThread;
    std::stop_source adapterDebugThreadStopSource;
    std::stop_token adapterDebugThreadStopToken = adapterDebugThreadStopSource.get_token();

    void InitializeAdapterDebug() {
        DX12DebugSetup(contextInternal.dx.device.Get());
        std::atomic<bool> debugThreadStarted = false;
        adapterDebugThread = std::thread([&](const std::stop_token &st) {
            ENG_LOG_INFO("[DX12] Debug polling thread started\n");
            debugThreadStarted.store(true, std::memory_order_release);

            while (!st.stop_requested()) {
                DX12DebugPoll();
            }

            ENG_LOG_INFO("[DX12] Debug polling thread stopped\n");
        }, adapterDebugThreadStopToken);
        while (!debugThreadStarted.load(std::memory_order_acquire)) {
            std::this_thread::yield(); // or sleep_for(1ms)
        }
    }
#endif
};

std::unique_ptr<Engine> Engine::instance = nullptr;

Engine::Engine() : pimpl(std::make_unique<Engine::Impl>()) {
}

Engine::~Engine() = default;

void Engine::Initialize() {
    Logger::Initialize();
#ifdef _DEBUG
    Engine::Impl::InitializeDebug();
#endif

    if (instance) return;

    instance = std::unique_ptr<Engine>(new Engine());
    instance->pimpl->InitializeAdapter();
    instance->pimpl->InitializeAllocator();
    instance->pimpl->InitializeContext();
#ifdef _DEBUG
    instance->pimpl->InitializeAdapterDebug();
#endif

    instance->pimpl->InitializeSystems();
}

Engine &Engine::Get() {
    return *instance;
}

void Engine::Run() {
    auto &windowSystem = instance->pimpl->context.systems.Get<WindowSystem>();
    auto *editorUISystem = instance->pimpl->context.systems.GetMaybe<EditorUISystem>();

    auto &engine = instance->pimpl;

    while (!engine->contextInternal.stopToken.stop_requested()) {
        if (windowSystem.ShouldWindowClose()) {
            engine->stopSource.request_stop();
            //TODO: may have to do cleanup here
            continue;
        }
        windowSystem.PollEvents();

        if (editorUISystem) {
            // editorUISystem->BeginFrame(engine->contextInternal);
        }
    }
    windowSystem.CloseWindow();
}

void Engine::Shutdown() {
#ifdef _DEBUG
    instance->pimpl->adapterDebugThreadStopSource.request_stop();
    if (instance->pimpl->adapterDebugThread.joinable()) instance->pimpl->adapterDebugThread.join();
#endif
    instance->pimpl->stopSource.request_stop();
    for (const auto &val: instance->pimpl->context.systems.GetItems() | std::views::values) val->Shutdown();
}
