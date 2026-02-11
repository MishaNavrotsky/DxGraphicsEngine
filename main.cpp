#include "engine/Engine.h"

int main() {
    Engine::Initialize();
    Engine& engine = Engine::Get();
    engine.Run();
    engine.Shutdown();
    return 0;
}
