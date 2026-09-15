// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>

/**
 * @brief Target registry backed by an unbalanced binary search tree.
 *
 * Fast on random keys, but degenerates into a linked list (O(n) per
 * operation) when keys arrive already sorted.
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class BstRegistry {
 private:
  /// @brief A single tree node.
  struct Node {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
    Node* left;       ///< Left subtree (smaller keys).
    Node* right;      ///< Right subtree (larger keys).
  };

  /// @brief Recursively frees a subtree.
  void destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
  }

  Node* root;  ///< Tree root, or nullptr if empty.
  int count;   ///< Number of stored entries.

 public:
  /// @brief Creates an empty registry.
  BstRegistry() : root(nullptr), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  BstRegistry(const BstRegistry&) = delete;
  BstRegistry& operator=(const BstRegistry&) = delete;

  /// @brief Frees every node.
  ~BstRegistry() {
    destroy(root);
  }

  /**
   * @brief Inserts a key/value pair following BST ordering.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent walking down to the insertion point.
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    if (root == nullptr) {
      root = new Node{key, value, nullptr, nullptr};
      ++count;
      return 1;
    }

    int steps = 0;
    Node* current = root;
    while (true) {
      ++steps;
      if (key == current->key) {
        current->value = value;  // key already present, just update
        return steps;
      } else if (key < current->key) {
        if (current->left == nullptr) {
          current->left = new Node{key, value, nullptr, nullptr};
          ++steps;
          ++count;
          return steps;
        }
        current = current->left;
      } else {
        if (current->right == nullptr) {
          current->right = new Node{key, value, nullptr, nullptr};
          ++steps;
          ++count;
          return steps;
        }
        current = current->right;
      }
    }
  }

  /**
   * @brief Removes the entry with the given key, if present.
   *
   * Handles the classic three BST deletion cases (0, 1, or 2 children);
   * with 2 children, the in-order successor is promoted.
   *
   * @param key Key to remove.
   * @return Steps spent locating and unlinking the node.
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    Node* parent = nullptr;
    Node* current = root;

    while (current != nullptr && current->key != key) {
      ++steps;
      parent = current;
      current = (key < current->key) ? current->left : current->right;
    }

    if (current == nullptr) {
      return steps;  // not found: full path already walked
    }
    ++steps;  // the comparison that confirmed the match

    if (current->left != nullptr && current->right != nullptr) {
      // Two children: replace with the in-order successor.
      Node* successorParent = current;
      Node* successor = current->right;
      while (successor->left != nullptr) {
        ++steps;
        successorParent = successor;
        successor = successor->left;
      }

      current->key = successor->key;
      current->value = successor->value;

      Node* child = successor->right;  // successor has at most a right child
      if (successorParent->left == successor) {
        successorParent->left = child;
      } else {
        successorParent->right = child;
      }
      delete successor;
    } else {
      // 0 or 1 child: splice it in directly.
      Node* child = (current->left != nullptr) ? current->left : current->right;
      if (parent == nullptr) {
        root = child;
      } else if (parent->left == current) {
        parent->left = child;
      } else {
        parent->right = child;
      }
      delete current;
    }

    ++steps;  // the unlink itself
    --count;
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
