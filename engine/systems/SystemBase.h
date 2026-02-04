//
// Created by Misha on 2/3/2026.
//

#ifndef DXGRAPHICSENGINE_SYSTEMBASE_H
#define DXGRAPHICSENGINE_SYSTEMBASE_H

class SystemBase {
public:
    SystemBase() = default;
    ~SystemBase() = default;

    SystemBase(const SystemBase&) = delete;
    SystemBase& operator=(const SystemBase&) = delete;
    SystemBase(SystemBase&&) = delete;
    SystemBase& operator=(SystemBase&&) = delete;
};

#endif //DXGRAPHICSENGINE_SYSTEMBASE_H