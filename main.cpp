#include "engine/core/Engine.h"

int main() {
    Engine::Initialize();
    Engine::Run();
    Engine::Shutdown();
    return 0;
}
