// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a sorted singly linked list.
 *
 * Kept sorted ascending by key, with early exit on search. Its cheap
 * native query is "peek the head", which is also the minimum key, O(1).
 */
class SortedListRegistry : public ITargetRegistry {
 private:
  /// @brief A single list element.
  struct Node {
    EnemyId id;   ///< Enemy identifier.
    Key key;      ///< Ordering key (== id in this project).
    Node* next;   ///< Next node, or nullptr at the tail.
  };

  Node* head;         ///< First (smallest-key) node, or nullptr if empty.
  size_t count;       ///< Number of stored entries.

  StepCounter counter_;

 public:
  /// @brief Creates an empty registry.
  SortedListRegistry();

  // Raw pointers are owned here; copying would double-free them.
  SortedListRegistry(const SortedListRegistry&) = delete;
  SortedListRegistry& operator=(const SortedListRegistry&) = delete;

  /// @brief Frees every node.
  ~SortedListRegistry() override;

  /**
   * @brief Inserts a key/value pair, keeping the list sorted by key.
   * @param id Enemy identifier.
   * @param k  Key this list orders by (same value as id, per spec).
   * @return Steps spent finding the insertion point.
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, if present.
   *
   * Stops walking as soon as a key greater than id is seen.
   *
   * @param id Enemy identifier to remove.
   * @return Steps spent searching (and unlinking, if found).
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek the head (the minimum).
   * @param out Set to the head's id, if the list is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;

  const StepCounter& stepBreakdown() const override { return counter_; }
};
