//
// Created by Misha on 2/3/2026.
//

#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include <stdexcept>

template <typename Base>
class Registry {
public:
    Registry() = default;
    ~Registry() = default;
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(Registry&&) = delete;
    template<typename T>
    T& Get() {
        const auto it = items.find(typeid(T));
        if (it == items.end())
            throw std::runtime_error("System not registered");
        return *static_cast<T*>(it->second.get());
    }
    template<typename T, typename... Args>
    T& Register(Args &&... args) {
        const auto type = std::type_index(typeid(T));
        if (items.contains(type)) throw std::runtime_error("System already registered");

        auto sys = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *sys;
        items[type] = std::move(sys);
        return ref;
    }
    template<typename T>
    T& Register(std::unique_ptr<T> args) {
        const auto type = std::type_index(typeid(T));
        if (items.contains(type))
            throw std::runtime_error("System already registered");

        T& ref = *args;
        items[type] = std::move(args);
        return ref;
    }
private:
    std::unordered_map<std::type_index, std::unique_ptr<Base>> items;
};


