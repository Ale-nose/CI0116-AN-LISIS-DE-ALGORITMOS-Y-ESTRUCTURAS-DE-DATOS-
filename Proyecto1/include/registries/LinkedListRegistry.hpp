// Copyright 2026 Kevin Velásquez García
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by an unsorted singly linked list.
 *
 * Insertion is O(1) at the front. Its cheap native query is "peek the
 * head" (the most recently inserted enemy), also O(1).
 */
class LinkedListRegistry : public ITargetRegistry {
 private:
  /// @brief A single list element.
  struct Node {
    EnemyId id;   ///< Enemy identifier.
    Key key;      ///< Ordering key (== id in this project).
    Node* next;   ///< Next node, or nullptr at the tail.
  };
  
  Node* head;         ///< First node, or nullptr if empty.
  size_t count;       ///< Number of stored entries.

 public:
  /// @brief Creates an empty registry.
  LinkedListRegistry();

  // Raw pointers are owned here; copying would double-free them.
  LinkedListRegistry(const LinkedListRegistry&) = delete;
  LinkedListRegistry& operator=(const LinkedListRegistry&) = delete;

  /// @brief Frees every node.
  ~LinkedListRegistry() override;

  /**
   * @brief Inserts a key/value pair at the front of the list.
   * @param id Enemy identifier.
   * @param k  Key this list orders by (same value as id, per spec).
   * @return Steps spent (always 1).
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, if present.
   * @param id Enemy identifier to remove.
   * @return Steps spent walking the list.
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek the head.
   * @param out Set to the head's id, if the list is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;
};
