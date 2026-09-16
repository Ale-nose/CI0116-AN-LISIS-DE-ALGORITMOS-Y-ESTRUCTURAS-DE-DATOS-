// Copyright 2026 Kevin Velásquez García
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a dynamic array (amortized growth).
 *
 * Insertion is O(1) amortized; removal swaps the target with the last
 * element. Its cheap native query is "peek the last slot", O(1).
 */
class DynamicArrayRegistry : public ITargetRegistry {
 private:
  /// @brief A single array slot.
  struct Entry {
    EnemyId id;  ///< Enemy identifier.
    Key key;     ///< Ordering key (== id in this project).
  };
  
  Entry* data;   ///< Backing storage.
  int capacity;  ///< Allocated slots.
  int count;     ///< Slots actually in use.

 public:
  /// @brief Creates an empty registry with room for 4 entries.
  DynamicArrayRegistry();

  // Raw pointers are owned here; copying would double-free them.
  DynamicArrayRegistry(const DynamicArrayRegistry&) = delete;
  DynamicArrayRegistry& operator=(const DynamicArrayRegistry&) = delete;

  /// @brief Frees the backing array.
  ~DynamicArrayRegistry() override;

  /**
   * @brief Appends a key/value pair, growing the array if needed.
   * @param id Enemy identifier.
   * @param k  Key this array orders by (same value as id, per spec).
   * @return Steps spent (copies made while growing, plus 1).
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id by swapping with the last one.
   * @param id Enemy identifier to remove.
   * @return Steps spent searching (plus the swap, if found).
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek the last slot.
   * @param out Set to the last-inserted id, if the array is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;
};
