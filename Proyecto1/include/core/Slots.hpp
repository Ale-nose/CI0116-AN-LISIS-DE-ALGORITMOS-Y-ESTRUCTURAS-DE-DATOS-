// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once
#include <array>
#include <memory>
#include <optional>
#include "CoreFactory.hpp"
#include "CorePrices.hpp"
#include "Tower.hpp"
#include "Ticks.hpp"

/**
 * @brief Section 3.1 — the board's 8 fixed tower slots. A slot starts
 * empty until the player buys a core for it; from then on, installCore()
 * replaces whatever was there ("reemplazo de núcleo").
 */
class SlotManager {
 public:
  static constexpr int kSlotCount = SLOTS;  ///< Number of slots.

  /**
   * @brief Whether a core is installed in this slot.
   * @param slotIndex Slot to check, 0..kSlotCount-1.
   * @return true if the slot has a tower.
   */
  bool hasTower(int slotIndex) const {
    return towers_[slotIndex] != nullptr;
  }

  /**
   * @brief Assigns a core to an empty slot, or replaces the one already
   * there. Maintenance still pending against the old core is dropped
   * (handled inside Tower::installRegistry).
   * @param slotIndex Slot to install into, 0..kSlotCount-1.
   * @param type Structure to install.
   * @param hashMode Bucket function; only used for CoreType::HashTable.
   */
  void installCore(int slotIndex, CoreType type,
      HashMode hashMode = HashMode::Default) {
    if (towers_[slotIndex]) {
      towers_[slotIndex]->installRegistry(createCore(type, hashMode));
    } else {
      towers_[slotIndex] =
        std::make_unique<Tower>(createCore(type, hashMode));
    }
    installedTypes_[slotIndex] = type;
  }

  /**
   * @brief Section 5.2 — read-only access for the UI to draw each slot's
   * indicators (installed structure, registry size, lag bar).
   * @param slotIndex Slot to read, 0..kSlotCount-1.
   * @return The slot's tower, or nullptr if the slot is empty.
   */
  const Tower* towerAt(int slotIndex) const {
    return towers_[slotIndex].get();
  }

  /**
   * @brief The structure installed in a slot.
   * @param slotIndex Slot to read, 0..kSlotCount-1.
   * @return Its CoreType, or std::nullopt if the slot is empty.
   */
  std::optional<CoreType> installedType(int slotIndex) const {
    return installedTypes_[slotIndex];
  }

  /**
   * @brief Section 2.4 — an enemy entered this slot's radius: schedules
   * an insert. No-op if the slot has no core installed yet.
   * @param slotIndex Slot whose radius the enemy entered.
   * @param id Enemy that entered.
   * @param key Key the core indexes it by (== id in this project).
   */
  void enqueueInsert(int slotIndex, EnemyId id, Key key) {
    if (towers_[slotIndex]) {
      towers_[slotIndex]->enqueueInsert(id, key);
    }
  }

  /**
   * @brief Section 2.4 — an enemy left this slot's radius or died:
   * schedules an erase. No-op if the slot has no core installed yet.
   * @param slotIndex Slot the enemy left.
   * @param id Enemy to remove.
   */
  void enqueueErase(int slotIndex, EnemyId id) {
    if (towers_[slotIndex]) {
      towers_[slotIndex]->enqueueErase(id);
    }
  }

  /**
   * @brief Section 6.1 — hands out (and resets) the metrics this slot's
   * tower gathered during the wave that just ended.
   * @param slotIndex Slot to read, 0..kSlotCount-1.
   * @return The wave's metrics; all zeros for an empty slot.
   */
  TowerWaveStats takeWaveStats(int slotIndex) {
    if (!towers_[slotIndex]) {
      return TowerWaveStats{};
    }
    return towers_[slotIndex]->takeWaveStats();
  }

  /**
   * @brief What one slot's tower did during one tick.
   */
  struct SlotTickResult {
      bool fired = false;    ///< Whether the tower shot this tick.
      EnemyId target = -1;   ///< Target of the shot, if any.
      int stepsUsed = 0;     ///< Steps the tower spent this tick.
  };

  /**
   * @brief Runs one tick for every occupied slot. Empty slots are skipped
   * entirely: no cost, no result, nothing to log for them.
   * @return One result per slot (default values for empty slots).
   */
  std::array<SlotTickResult, kSlotCount> tickAll() {
    std::array<SlotTickResult, kSlotCount> results{};
    for (int i = 0; i < kSlotCount; ++i) {
      if (!towers_[i]) {
        continue;
      }
      SlotTickResult r;
      r.fired = towers_[i]->tick(r.target, &r.stepsUsed);
      results[i] = r;
    }
    return results;
  }

 private:
  std::array<std::unique_ptr<Tower>, kSlotCount> towers_;  ///< Per slot.
  std::array<std::optional<CoreType>, kSlotCount> installedTypes_;  ///< Per slot.
};