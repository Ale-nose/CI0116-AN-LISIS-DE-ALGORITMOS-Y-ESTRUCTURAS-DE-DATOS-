// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by an unbalanced binary search tree.
 *
 * Fast on random keys, degenerates on sorted input. Its cheap native
 * query is "peek the root" — O(1), but not necessarily meaningful,
 * since an unbalanced tree's root can be any previously inserted key.
 */
class BstRegistry : public ITargetRegistry {
 private:
  /// @brief A single tree node.
  struct Node {
    EnemyId id;   ///< Enemy identifier.
    Key key;      ///< Ordering key (== id in this project).
    Node* left;   ///< Left subtree (smaller keys).
    Node* right;  ///< Right subtree (larger keys).
  };

  /// @brief Recursively frees a subtree.
  void destroy(Node* node);

  Node* root;         ///< Tree root, or nullptr if empty.
  size_t count;  ///< Number of stored entries.

  mutable StepCounter counter_;

 public:
  /// @brief Creates an empty registry.
  BstRegistry();

  // Raw pointers are owned here; copying would double-free them.
  BstRegistry(const BstRegistry&) = delete;
  BstRegistry& operator=(const BstRegistry&) = delete;

  /// @brief Frees every node.
  ~BstRegistry() override;

  /**
   * @brief Inserts a key/value pair following BST ordering.
   * @param id Enemy identifier.
   * @param k  Key this tree orders by (same value as id, per spec).
   * @return Steps spent walking down to the insertion point.
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, if present.
   *
   * Handles the classic three BST deletion cases (0, 1, or 2 children);
   * with 2 children, the in-order successor is promoted.
   *
   * @param id Enemy identifier to remove.
   * @return Steps spent locating and unlinking the node.
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: peek the root.
   * @param out Set to the root's id, if the tree is non-empty.
   * @return Steps spent (0 if empty, 1 otherwise).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;

  const StepCounter& stepBreakdown() const override { return counter_; }
};