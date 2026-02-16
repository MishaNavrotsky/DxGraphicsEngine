//
// Created by Misha on 2/15/2026.
//

#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

struct SlotAllocator {
    std::vector<size_t> freeIndices;
    size_t capacity;

    void Init(const size_t maxItems) {
        capacity = maxItems;
        freeIndices.resize(capacity);

        for (size_t i = 0; i < capacity; ++i) {
            freeIndices[i] = capacity - 1 - i;
        }
    }

    size_t Allocate() {
        if (freeIndices.empty()) {
            return static_cast<size_t>(-1);
        }
        size_t index = freeIndices.back();
        freeIndices.pop_back();
        return index;
    }

    void Free(size_t index) {
        freeIndices.push_back(index);
    }

    void Reset() {
        Init(capacity);
    }
};
struct LinearAllocator {
    size_t capacity = 0;
    size_t currentIndex = 0;

    void Init(const size_t maxItems) {
        capacity = maxItems;
        currentIndex = 0;
    }

    size_t Allocate(const size_t count = 1) {
        if (currentIndex + count > capacity) {
            return static_cast<size_t>(-1);
        }
        const size_t allocatedIndex = currentIndex;
        currentIndex += count;
        return allocatedIndex;
    }

    void Reset() {
        currentIndex = 0;
    }
};

struct RingAllocator {
    size_t capacity = 0;
    size_t head = 0;
    size_t tail = 0;
    bool isFull = false;

    void Init(const size_t maxItems) {
        capacity = maxItems;
        head = 0;
        tail = 0;
        isFull = false;
    }

    size_t Allocate(const size_t count = 1) {
        if (count == 0 || capacity == 0) return static_cast<size_t>(-1);

        size_t availableSpace = 0;
        if (!isFull) {
            if (head >= tail) {
                availableSpace = capacity - head + tail;
            } else {
                availableSpace = tail - head;
            }
        }

        if (count > availableSpace) return static_cast<size_t>(-1);

        const size_t allocatedIndex = head;
        head = (head + count) % capacity;

        if (head == tail) isFull = true;

        return allocatedIndex;
    }

    void Free(const size_t count) {
        if (count == 0) return;
        tail = (tail + count) % capacity;
        isFull = false;
    }
};
