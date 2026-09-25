#pragma once
#include <array>
#include <memory>
#include <optional>
#include "CoreFactory.hpp"
#include "CorePrices.hpp"
#include "Tower.hpp"
#include "Ticks.hpp"

// Section 3.1 — models the board's 8 fixed tower slots. A slot starts
// empty (no tower installed) until the player buys a core for it; from
// then on, installCore() replaces whatever was there ("reemplazo de
// núcleo").
class SlotManager
{
public:
    static constexpr int kSlotCount = SLOTS;

    bool hasTower(int slotIndex) const
    {
        return towers_[slotIndex] != nullptr;
    }

    // Assigns a core to an empty slot, or replaces the one already there.
    // Any maintenance still pending against the old core is dropped
    // (handled inside Tower::installRegistry).
    void installCore(int slotIndex, CoreType type)
    {
        if (towers_[slotIndex])
        {
            towers_[slotIndex]->installRegistry(createCore(type));
        }
        else
        {
            towers_[slotIndex] = std::make_unique<Tower>(createCore(type));
        }
        installedTypes_[slotIndex] = type;
    }

    // Section 5.2 — read-only access for the UI to draw each slot's
    // indicators (installed structure, registry size, lag bar).
    const Tower* towerAt(int slotIndex) const
    {
        return towers_[slotIndex].get();
    }

    // The CoreType installed in this slot, or std::nullopt if empty.
    std::optional<CoreType> installedType(int slotIndex) const
    {
        return installedTypes_[slotIndex];
    }

    // Section 2.4 — an enemy entered this slot's radius: schedule an
    // insert. No-op if the slot has no core installed yet.
    void enqueueInsert(int slotIndex, EnemyId id, Key key)
    {
        if (towers_[slotIndex])
        {
            towers_[slotIndex]->enqueueInsert(id, key);
        }
    }

    // Section 2.4 — an enemy left this slot's radius or died: schedule
    // an erase. No-op if the slot has no core installed yet.
    void enqueueErase(int slotIndex, EnemyId id)
    {
        if (towers_[slotIndex])
        {
            towers_[slotIndex]->enqueueErase(id);
        }
    }

    struct SlotTickResult
    {
        bool fired = false;
        EnemyId target = -1;
        int stepsUsed = 0;
    };

    // Runs one tick for every occupied slot. Empty slots are skipped
    // entirely: no cost, no result, nothing to log for them.
    std::array<SlotTickResult, kSlotCount> tickAll()
    {
        std::array<SlotTickResult, kSlotCount> results{};
        for (int i = 0; i < kSlotCount; ++i)
        {
            if (!towers_[i])
            {
                continue;
            }
            SlotTickResult r;
            r.fired = towers_[i]->tick(r.target, &r.stepsUsed);
            results[i] = r;
        }
        return results;
    }

private:
    std::array<std::unique_ptr<Tower>, kSlotCount> towers_;
    std::array<std::optional<CoreType>, kSlotCount> installedTypes_;
};