//
// Created by Misha on 2/11/2026.
//

#ifndef DXGRAPHICSENGINE_ENGINE_H
#define DXGRAPHICSENGINE_ENGINE_H

#include <memory>
#include <stop_token>
#include <thread>
#include <print>

class Engine {
public:
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static void Initialize();
    static Engine& Get();

    static void Run();
    static void Shutdown();

private:
    Engine();

    struct Impl;
    const std::unique_ptr<Impl> pimpl;

    static std::unique_ptr<Engine> instance;
};

#endif //DXGRAPHICSENGINE_ENGINE_H
