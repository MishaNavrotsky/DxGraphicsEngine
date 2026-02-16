//
// Created by Misha on 2/16/2026.
//

#pragma once

#include "engine/core/EngineConfigs.h"
#include "engine/systems/SystemBase.h"
#include "engine/libs/Registry.h"

struct EngineContext {
    EngineConfigs configs;
    Registry<SystemBase> systems;
};