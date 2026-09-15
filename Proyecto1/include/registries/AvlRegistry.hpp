// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <tuple>
#include <utility>

/**
 * @brief Target registry backed by a self-balancing AVL tree.
 *
 * Keeps O(log n) height at all times via rotations, at the cost of
 * extra steps spent rebalancing after insert/erase.
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class AvlRegistry {
 private:
  /// @brief A single tree node.
  struct Node {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
    Node* left;       ///< Left subtree (smaller keys).
    Node* right;      ///< Right subtree (larger keys).
    int height;       ///< Height of this subtree.
  };

  Node* root;  ///< Tree root, or nullptr if empty.
  int count;   ///< Number of stored entries.

  /// @brief Height of a subtree (0 for nullptr).
  static int heightOf(Node* node) {
    return node == nullptr ? 0 : node->height;
  }

  /// @brief Recomputes a node's height from its children.
  static void updateHeight(Node* node) {
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
  }

  /// @brief Left height minus right height; outside [-1, 1] means unbalanced.
  static int balanceFactor(Node* node) {
    return node == nullptr ? 0 : heightOf(node->left) - heightOf(node->right);
  }

  /// @brief Standard AVL right rotation. Counted as 1 step by the caller.
  static Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* t2 = x->right;
    x->right = y;
    y->left = t2;
    updateHeight(y);
    updateHeight(x);
    return x;
  }

  /// @brief Standard AVL left rotation. Counted as 1 step by the caller.
  static Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* t2 = y->left;
    y->left = x;
    x->right = t2;
    updateHeight(x);
    updateHeight(y);
    return y;
  }

  /**
   * @brief Recursive insert with rebalancing on the way back up.
   * @param node  Subtree root to insert into.
   * @param key   Lookup key.
   * @param value Value to store.
   * @param steps Accumulator, incremented as work happens.
   * @return New subtree root (may differ from `node` if a rotation occurred).
   */
  Node* insertRec(Node* node, const KeyType& key,
    const ValueType& value, int& steps) {
    if (node == nullptr) {
      ++count;
      ++steps;
      return new Node{key, value, nullptr, nullptr, 1};
    }

    ++steps;  // comparison at this level
    if (key == node->key) {
      node->value = value;
      return node;
    } else if (key < node->key) {
      node->left = insertRec(node->left, key, value, steps);
    } else {
      node->right = insertRec(node->right, key, value, steps);
    }

    updateHeight(node);
    int balance = balanceFactor(node);

    if (balance > 1 && key < node->left->key) {          // left-left
      ++steps;
      return rotateRight(node);
    }
    if (balance < -1 && key > node->right->key) {         // right-right
      ++steps;
      return rotateLeft(node);
    }
    if (balance > 1 && key > node->left->key) {           // left-right
      node->left = rotateLeft(node->left);
      ++steps;
      ++steps;
      return rotateRight(node);
    }
    if (balance < -1 && key < node->right->key) {         // right-left
      node->right = rotateRight(node->right);
      ++steps;
      ++steps;
      return rotateLeft(node);
    }

    return node;
  }

  /**
   * @brief Finds the minimum-key node of a subtree.
   * @param node  Subtree to search (must not be nullptr).
   * @param steps Accumulator, one step per node visited.
   * @return The leftmost (minimum) node.
   */
  Node* findMin(Node* node, int& steps) {
    while (node->left != nullptr) {
      ++steps;
      node = node->left;
    }
    return node;
  }

  /**
   * @brief Recursive erase with rebalancing on the way back up.
   * @param node  Subtree root to erase from.
   * @param key   Key to remove.
   * @param steps Accumulator, incremented as work happens.
   * @param found Set to true if the key was actually present.
   * @return New subtree root (may differ from `node`).
   */
  Node* eraseRec(Node* node, const KeyType& key, int& steps, bool& found) {
    if (node == nullptr) {
      return nullptr;
    }

    ++steps;
    if (key < node->key) {
      node->left = eraseRec(node->left, key, steps, found);
    } else if (key > node->key) {
      node->right = eraseRec(node->right, key, steps, found);
    } else {
      found = true;
      if (node->left == nullptr || node->right == nullptr) {
        Node* child = (node->left != nullptr) ? node->left : node->right;
        delete node;
        --count;
        return child;  // 0 or 1 child: splice it in directly
      }

      // Two children: pull up the in-order successor's data, then remove it.
      Node* successor = findMin(node->right, steps);
      node->key = successor->key;
      node->value = successor->value;
      bool dummy = false;
      node->right = eraseRec(node->right, successor->key, steps, dummy);
    }

    if (node == nullptr) {
      return nullptr;
    }

    updateHeight(node);
    int balance = balanceFactor(node);

    if (balance > 1 && balanceFactor(node->left) >= 0) {
      ++steps;
      return rotateRight(node);
    }
    if (balance > 1 && balanceFactor(node->left) < 0) {
      node->left = rotateLeft(node->left);
      ++steps;
      ++steps;
      return rotateRight(node);
    }
    if (balance < -1 && balanceFactor(node->right) <= 0) {
      ++steps;
      return rotateLeft(node);
    }
    if (balance < -1 && balanceFactor(node->right) > 0) {
      node->right = rotateRight(node->right);
      ++steps;
      ++steps;
      return rotateLeft(node);
    }

    return node;
  }

  /// @brief Recursively frees a subtree.
  void destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
  }

 public:
  /// @brief Creates an empty registry.
  AvlRegistry() : root(nullptr), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  AvlRegistry(const AvlRegistry&) = delete;
  AvlRegistry& operator=(const AvlRegistry&) = delete;

  /// @brief Frees every node.
  ~AvlRegistry() {
    destroy(root);
  }

  /**
   * @brief Inserts a key/value pair, rebalancing as needed.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent (tree walk plus any rotations).
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    int steps = 0;
    root = insertRec(root, key, value, steps);
    return steps;
  }

  /**
   * @brief Removes the entry with the given key, rebalancing as needed.
   * @param key Key to remove.
   * @return Steps spent (tree walk plus any rotations).
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    bool found = false;
    root = eraseRec(root, key, steps, found);
    return steps;
  }

  /**
   * @brief Looks up the value stored under a key.
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    int steps = 0;
    Node* current = root;
    while (current != nullptr) {
      ++steps;
      if (key == current->key) {
        return {current->value, steps};
      }
      current = (key < current->key) ? current->left : current->right;
    }
    return {std::nullopt, steps};
  }

  /// @brief Number of stored entries. Costs 0 steps.
  inline int size() const {
    return count;
  }
};
