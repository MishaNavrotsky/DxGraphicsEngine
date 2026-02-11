//
// Created by Misha on 2/11/2026.
//

#ifndef DXGRAPHICSENGINE_RENDERSYSTEM_H
#define DXGRAPHICSENGINE_RENDERSYSTEM_H
#include "engine/EngineContext.h"
#include "engine/systems/SystemBase.h"

class RenderSystem: public SystemBase {
public:
    explicit RenderSystem() = default;

    void Initialize(EngineContext& ctx) {

    }

    void BeginFrame(EngineContext& ctx) {

    }
    void EndFrame(EngineContext& ctx) {

    }
};

#endif //DXGRAPHICSENGINE_RENDERSYSTEM_H