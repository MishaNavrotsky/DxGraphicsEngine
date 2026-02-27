//
// Created by Misha on 2/27/2026.
//

#pragma once

#include <functional>
#include <unordered_map>

template<typename... Args>
class Event {
    using Callback = std::function<void(Args...)>;
    std::unordered_map<size_t, Callback> subscribers;
    size_t nextId = 0;

public:
    size_t Subscribe(Callback cb) {
        subscribers[nextId] = cb;
        return nextId++;
    }

    void Unsubscribe(size_t id) { subscribers.erase(id); }

    void Invoke(Args... args) {
        for (auto const &[id, cb]: subscribers) {
            cb(args...);
        }
    }
};
