//
// Created by Misha on 2/11/2026.
//

#pragma once

#include <memory>

class Engine {
public:
    ~Engine();

    Engine(const Engine &) = delete;

    Engine &operator=(const Engine &) = delete;

    static void Initialize();

    static Engine &Get();

    static void Run();

    static void Shutdown();

private:
    Engine();

    struct Impl;
    const std::unique_ptr<Impl> pimpl;

    static std::unique_ptr<Engine> instance;
};
