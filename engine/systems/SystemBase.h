//
// Created by Misha on 2/3/2026.
//

#pragma once
#include "engine/core/EngineContextInternal.h"
#include "engine/core/EngineConfigs.h"

class SystemBase {
public:
    SystemBase() = default;

    virtual ~SystemBase() = default;

    SystemBase(const SystemBase &) = delete;

    SystemBase &operator=(const SystemBase &) = delete;

    SystemBase(SystemBase &&) = delete;

    SystemBase &operator=(SystemBase &&) = delete;

    virtual void Initialize(EngineContextInternal &ctx, EngineConfigs &configs) {
    };

    virtual void Shutdown() {
    };
};
