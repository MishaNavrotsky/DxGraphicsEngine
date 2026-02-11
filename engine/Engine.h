//
// Created by Misha on 2/11/2026.
//

#ifndef DXGRAPHICSENGINE_ENGINE_H
#define DXGRAPHICSENGINE_ENGINE_H
#include <dxgi1_6.h>
#include <print>
#include <thread>
#include <chrono>
#include <atomic>

#include "DxUtils.h"
#include "EngineContext.h"

#include "systems/QueueSystem.h"
#include "systems/window/SwapChainSystem.h"
#include "systems/window/WindowSystem.h"
#include "systems/render/RenderSystem.h"

class Engine {
public:
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    EngineContext context;

    static void Initialize();

    static Engine& Get();

    void Run();

    void Shutdown();

private:
    Engine() = default;
    static std::unique_ptr<Engine> instance;
    void InitializeContext();
    void InitializeAdapter();
    void InitializeSystems();

    std::stop_source stopSource;
#ifdef _DEBUG
    static void InitializeDebug();

    std::thread adapterDebugThread;
    std::stop_source adapterDebugThreadStopSource;
    std::stop_token adapterDebugThreadStopToken = adapterDebugThreadStopSource.get_token();
    void InitializeAdapterDebug();
#endif

};
#endif //DXGRAPHICSENGINE_ENGINE_H
