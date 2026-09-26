// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a sorted dynamic array.
 *
 * Search uses binary search; insertion/removal still shift elements.
 * Its cheap native query is "peek index 0", the minimum key, O(1).
 */
class SortedArrayRegistry : public ITargetRegistry {
 private:
  /// @brief A single array slot.
  struct Entry {
    EnemyId id;  ///< Enemy identifier.
    Key key;     ///< Ordering key (== id in this project).
  };

  /// @brief Result of a binary search.
  struct SearchResult {
    int index;   ///< Index of the key, or where it should be inserted.
    bool found;  ///< Whether the key was actually present.
    int steps;   ///< Steps spent searching.
  };

  /// @brief Doubles capacity and copies every element over.
  void grow();

  /**
   * @brief Binary search for a key.
   * @param key Key to search for.
   * @return Index (found or insertion point), found flag, and steps spent.
   */
  SearchResult binarySearch(Key key) const;

  Entry* data;   ///< Backing storage, kept sorted by key.
  int capacity;  ///< Allocated slots.
  int count;     ///< Slots actually in use.

  mutable StepCounter counter_;

 public:
  /// @brief Creates an empty registry with room for 4 entries.
  SortedArrayRegistry();

  // Raw pointers are owned here; copying would double-free them.
  SortedArrayRegistry(const SortedArrayRegistry&) = delete;
  SortedArrayRegistry& operator=(const SortedArrayRegistry&) = delete;

  /// @brief Frees the backing array.
  ~SortedArrayRegistry() override;

  /**
   * @brief Inserts a key/value pair, keeping the array sorted.
   * @param id Enemy identifier.
   * @param k  Key this array orders by (same value as id, per spec).
   * @return Steps spent (binary search plus shifting elements right).
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, if present.
   * @param id Enemy identifier to remove.
   * @return Steps spent (binary search plus shifting elements left).
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek index 0 (the minimum).
   * @param out Set to the smallest-key id, if the array is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;

  const StepCounter& stepBreakdown() const override { return counter_; }
};
