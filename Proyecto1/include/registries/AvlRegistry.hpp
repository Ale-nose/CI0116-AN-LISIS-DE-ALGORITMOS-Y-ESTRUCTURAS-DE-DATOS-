// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a self-balancing AVL tree.
 *
 * Keeps O(log n) height via rotations. Its cheap native query is the
 * true minimum key, found by walking left — O(log n), guaranteed by
 * the balance invariant.
 */
class AvlRegistry : public ITargetRegistry {
 private:
  /// @brief A single tree node.
  struct Node {
    EnemyId id;   ///< Enemy identifier.
    Key key;      ///< Ordering key (== id in this project).
    Node* left;   ///< Left subtree (smaller keys).
    Node* right;  ///< Right subtree (larger keys).
    int height;   ///< Height of this subtree.
  };

  /// @brief Height of a subtree (0 for nullptr).
  static int heightOf(Node* node);

  /// @brief Recomputes a node's height from its children.
  static void updateHeight(Node* node);

  /// @brief Left height minus right height; outside [-1, 1] means unbalanced.
  static int balanceFactor(Node* node);

  /// @brief Standard AVL right rotation. Counted as 1 step by the caller.
  static Node* rotateRight(Node* y);

  /// @brief Standard AVL left rotation. Counted as 1 step by the caller.
  static Node* rotateLeft(Node* x);

  /**
   * @brief Recursive insert with rebalancing on the way back up.
   * @param node  Subtree root to insert into.
   * @param id    Enemy identifier.
   * @param k     Key to insert.
   * @return New subtree root (may differ from `node` if a rotation occurred).
   */
  Node* insertRec(Node* node, EnemyId id, Key k);

  /**
   * @brief Finds the minimum-key node of a subtree.
   * @param node  Subtree to search (must not be nullptr).
   * @return The leftmost (minimum) node.
   */
  Node* findMin(Node* node) const;

  /**
   * @brief Recursive erase with rebalancing on the way back up.
   * @param node  Subtree root to erase from.
   * @param id    Enemy identifier to remove.
   * @return New subtree root (may differ from `node`).
   */
  Node* eraseRec(Node* node, EnemyId id);

  /// @brief Recursively frees a subtree.
  void destroy(Node* node);

  Node* root;         ///< Tree root, or nullptr if empty.
  size_t count;       ///< Number of stored entries.

  mutable StepCounter counter_;

 public:
  /// @brief Creates an empty registry.
  AvlRegistry();

  // Raw pointers are owned here; copying would double-free them.
  AvlRegistry(const AvlRegistry&) = delete;
  AvlRegistry& operator=(const AvlRegistry&) = delete;

  /// @brief Frees every node.
  ~AvlRegistry() override;

  /**
   * @brief Inserts a key/value pair, rebalancing as needed.
   * @param id Enemy identifier.
   * @param k  Key this tree orders by (same value as id, per spec).
   * @return Steps spent (tree walk plus any rotations).
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id, rebalancing as needed.
   * @param id Enemy identifier to remove.
   * @return Steps spent (tree walk plus any rotations).
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: the true minimum key.
   * @param out Set to the minimum id, if the tree is non-empty.
   * @return Steps spent walking left (0 if empty).
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;

  const StepCounter& stepBreakdown() const override { return counter_; }
};