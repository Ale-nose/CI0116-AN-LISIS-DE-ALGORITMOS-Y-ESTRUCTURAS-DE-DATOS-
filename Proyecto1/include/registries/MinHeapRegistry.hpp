// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <vector>

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a binary min-heap.
 *
 * The minimum-key element is always at the root. Its cheap native
 * query is "peek the root" — the true minimum, O(1).
 */
class MinHeapRegistry : public ITargetRegistry {
 private:
  /// @brief A single heap slot.
  struct Entry {
    EnemyId id;  ///< Enemy identifier.
    Key key;     ///< Ordering key (== id in this project).
  };

  /**
   * @brief Moves data_[index] up while it's smaller than its parent.
   * @param index Starting index.
   * @return The index where the element finally landed.
   */
  int siftUp(int index);

  /**
   * @brief Moves data_[index] down while it's bigger than a child.
   * @param index Starting index.
   * @return The index where the element finally landed.
   */
  int siftDown(int index);

  std::vector<Entry> data;  ///< Backing array, kept in heap order.
  int count;                ///< Number of stored entries.

  mutable StepCounter counter_;

 public:
  /// @brief Creates an empty registry.
  MinHeapRegistry();

  /**
   * @brief Inserts a key/value pair and restores the heap property.
   * @param id Enemy identifier.
   * @param k  Key this heap orders by (same value as id, per spec).
   * @return Steps spent sifting the new entry up.
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, if present.
   *
   * Requires a linear search first, since the heap has no order
   * besides "the root is the minimum".
   *
   * @param id Enemy identifier to remove.
   * @return Steps spent searching plus restoring the heap property.
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek the root.
   * @param out Set to the minimum id, if the heap is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;

  const StepCounter& stepBreakdown() const override { return counter_; }
};