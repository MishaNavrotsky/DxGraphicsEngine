//
// Created by Misha on 2/15/2026.
//
#pragma once

#include <cstdint>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iterator>

static constexpr uint32_t InvalidIndex = UINT32_MAX;

struct Range {
    uint32_t offset;
    uint32_t size;

    [[nodiscard]] bool IsAdjacent(const Range &other) const {
        return offset + size == other.offset || other.offset + other.size == offset;
    }
};

struct RangeSlotAllocator {
    uint32_t capacity = 0;
    uint32_t startOffset = 0;

    std::vector<Range> freeRanges{};
    std::mutex allocationMutex{};

    void Initialize(const uint32_t offset, const uint32_t maxItems) {
        std::lock_guard<std::mutex> lock(allocationMutex);
        startOffset = offset;
        capacity = maxItems;
        freeRanges.clear();
        freeRanges.push_back({startOffset, capacity});
    }

    uint32_t Allocate(const uint32_t numSlots) {
        std::lock_guard<std::mutex> lock(allocationMutex);

        for (auto it = freeRanges.begin(); it != freeRanges.end(); ++it) {
            if (it->size >= numSlots) {
                const uint32_t allocatedOffset = it->offset;

                if (it->size == numSlots) {
                    freeRanges.erase(it);
                } else {
                    it->offset += numSlots;
                    it->size -= numSlots;
                }
                return allocatedOffset;
            }
        }
        return InvalidIndex;
    }

    void Free(const uint32_t offset, const uint32_t numSlots) {
        std::lock_guard<std::mutex> lock(allocationMutex);

        const Range newRange = {offset, numSlots};

        const auto it = std::ranges::lower_bound(freeRanges, newRange,
                                                 [](const Range &a, const Range &b) { return a.offset < b.offset; });

        const size_t idx = std::distance(freeRanges.begin(), it);
        freeRanges.insert(it, newRange);

        if (idx + 1 < freeRanges.size()) {
            if (freeRanges[idx].offset + freeRanges[idx].size == freeRanges[idx + 1].offset) {
                freeRanges[idx].size += freeRanges[idx + 1].size;
                freeRanges.erase(freeRanges.begin() + static_cast<uint32_t>(idx) + 1);
            }
        }

        if (idx > 0) {
            if (freeRanges[idx - 1].offset + freeRanges[idx - 1].size == freeRanges[idx].offset) {
                freeRanges[idx - 1].size += freeRanges[idx].size;
                freeRanges.erase(freeRanges.begin() + static_cast<uint32_t>(idx));
            }
        }
    }
};

struct BumpAllocator {
    uint32_t startOffset = 0;
    uint32_t capacity = 0;
    std::atomic<uint32_t> currentOffset{0};

    BumpAllocator() = default;

    BumpAllocator(BumpAllocator &&other) noexcept {
        startOffset = other.startOffset;
        capacity = other.capacity;
        currentOffset.store(other.currentOffset.load());
    };

    BumpAllocator &operator=(BumpAllocator &&other) noexcept {
        if (this != &other) {
            startOffset = other.startOffset;
            capacity = other.capacity;
            currentOffset.store(other.currentOffset.load());
        }
        return *this;
    }

    void Initialize(const uint32_t offset, const uint32_t maxItems) {
        startOffset = offset;
        capacity = maxItems;
        currentOffset.store(0);
    }

    void Reset() {
        currentOffset.store(0);
    }

    uint32_t Allocate(const uint32_t count) {
        const uint32_t oldOffset = currentOffset.fetch_add(count);

        if (oldOffset + count > capacity) return InvalidIndex;

        return startOffset + oldOffset;
    }
};
