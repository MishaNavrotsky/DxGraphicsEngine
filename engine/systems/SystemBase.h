//
// Created by Misha on 2/3/2026.
//

#pragma once

class SystemBase {
public:
    SystemBase() = default;
    ~SystemBase() = default;

    SystemBase(const SystemBase&) = delete;
    SystemBase& operator=(const SystemBase&) = delete;
    SystemBase(SystemBase&&) = delete;
    SystemBase& operator=(SystemBase&&) = delete;
};
