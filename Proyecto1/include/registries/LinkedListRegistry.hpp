// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>

/**
 * @brief Target registry backed by an unsorted singly linked list.
 *
 * Insertion is O(1) at the front; search and removal are O(n).
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class LinkedListRegistry {
 private:
  /// @brief A single list element.
  struct Node {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
    Node* next;       ///< Next node, or nullptr at the tail.
  };

  Node* head;  ///< First node, or nullptr if empty.
  int count;   ///< Number of stored entries.

 public:
  /// @brief Creates an empty registry.
  LinkedListRegistry() : head(nullptr), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  LinkedListRegistry(const LinkedListRegistry&) = delete;
  LinkedListRegistry& operator=(const LinkedListRegistry&) = delete;

  /// @brief Frees every node.
  ~LinkedListRegistry() {
    Node* current = head;
    while (current != nullptr) {
      Node* next = current->next;
      delete current;
      current = next;
    }
  }

  /**
   * @brief Inserts a key/value pair at the front of the list.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent (always 1).
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    Node* node = new Node{key, value, head};
    head = node;
    ++count;
    return 1;
  }

  /**
   * @brief Removes the entry with the given key, if present.
   * @param key Key to remove.
   * @return Steps spent walking the list.
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    Node* previous = nullptr;
    Node* current = head;

    while (current != nullptr) {
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
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    int steps = 0;
    Node* current = head;
    while (current != nullptr) {
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
