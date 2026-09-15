// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>

/**
 * @brief Target registry backed by a sorted singly linked list.
 *
 * Every operation walks the list, but stops as soon as it passes the
 * point where the key would be (early exit), instead of always
 * scanning to the end like LinkedListRegistry.
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class SortedLinkedListRegistry {
 private:
  /// @brief A single list element.
  struct Node {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
    Node* next;       ///< Next node, or nullptr at the tail.
  };

  Node* head;  ///< First (smallest-key) node, or nullptr if empty.
  int count;   ///< Number of stored entries.

 public:
  /// @brief Creates an empty registry.
  SortedLinkedListRegistry() : head(nullptr), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  SortedLinkedListRegistry(const SortedLinkedListRegistry&) = delete;
  SortedLinkedListRegistry& operator=(const SortedLinkedListRegistry&) = delete;

  /// @brief Frees every node.
  ~SortedLinkedListRegistry() {
    Node* current = head;
    while (current != nullptr) {
      Node* next = current->next;
      delete current;
      current = next;
    }
  }

  /**
   * @brief Inserts a key/value pair, keeping the list sorted by key.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent finding the insertion point.
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    int steps = 0;
    Node* previous = nullptr;
    Node* current = head;

    while (current != nullptr && current->key < key) {
      ++steps;
      previous = current;
      current = current->next;
    }

    Node* node = new Node{key, value, current};
    if (previous == nullptr) {
      head = node;
    } else {
      previous->next = node;
    }
    ++steps;
    ++count;
    return steps;
  }

  /**
   * @brief Removes the entry with the given key, if present.
   *
   * Stops walking as soon as a key greater than the target is seen.
   *
   * @param key Key to remove.
   * @return Steps spent searching (and unlinking, if found).
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    Node* previous = nullptr;
    Node* current = head;

    while (current != nullptr && !(current->key > key)) {
      ++steps;
      if (current->key == key) {
        if (previous == nullptr) {
          head = current->next;
        } else {
          previous->next = current->next;
        }
        ++steps;
        delete current;
        --count;
        return steps;
      }
      previous = current;
      current = current->next;
    }
    return steps;
  }

  /**
   * @brief Looks up the value stored under a key.
   *
   * Stops walking as soon as a key greater than the target is seen.
   *
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    int steps = 0;
    Node* current = head;
    while (current != nullptr && !(current->key > key)) {
      ++steps;
      if (current->key == key) {
        return {current->value, steps};
      }
      current = current->next;
    }
    return {std::nullopt, steps};
  }

  /// @brief Number of stored entries. Costs 0 steps.
  inline int size() const {
    return count;
  }
};
