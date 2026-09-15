// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>
#include <vector>

/**
 * @brief Target registry backed by a binary min-heap.
 *
 * The minimum-key element is always at the root, but there is no
 * order guarantee beyond that: searching for an arbitrary key is O(n).
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class MinHeapRegistry {
 private:
  /// @brief A single heap slot.
  struct Entry {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
  };

  std::vector<Entry> data;  ///< Backing array, kept in heap order.
  int count;                ///< Number of stored entries.

  /**
   * @brief Moves data[index] up while it's smaller than its parent.
   * @param index Starting index.
   * @param steps Accumulator, one step per comparison.
   * @return The index where the element finally landed.
   */
  inline int siftUp(int index, int& steps) {
    while (index > 0) {
      int parent = (index - 1) / 2;
      ++steps;
      if (data[parent].key <= data[index].key) break;
      std::swap(data[parent], data[index]);
      index = parent;
    }
    return index;
  }

  /**
   * @brief Moves data[index] down while it's bigger than a child.
   * @param index Starting index.
   * @param steps Accumulator, one step per comparison.
   * @return The index where the element finally landed.
   */
  inline int siftDown(int index, int& steps) {
    while (true) {
      int left = 2 * index + 1;
      int right = 2 * index + 2;
      int smallest = index;

      if (left < count) {
        ++steps;
        if (data[left].key < data[smallest].key) smallest = left;
      }
      if (right < count) {
        ++steps;
        if (data[right].key < data[smallest].key) smallest = right;
      }
      if (smallest == index) break;
      std::swap(data[index], data[smallest]);
      index = smallest;
    }
    return index;
  }

 public:
  /// @brief Creates an empty registry.
  MinHeapRegistry() : count(0) {}

  /**
   * @brief Inserts a key/value pair and restores the heap property.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent sifting the new entry up.
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    data.push_back(Entry{key, value});
    ++count;
    int steps = 1;  // the append itself
    siftUp(count - 1, steps);
    return steps;
  }

  /**
   * @brief Removes the entry with the given key, if present.
   *
   * Requires a linear search first, since the heap has no order
   * besides "the root is the minimum".
   *
   * @param key Key to remove.
   * @return Steps spent searching plus restoring the heap property.
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    int index = -1;
    for (int i = 0; i < count; ++i) {
      ++steps;
      if (data[i].key == key) {
        index = i;
        break;
      }
    }
    if (index == -1) {
      return steps;
    }

    data[index] = data[count - 1];
    data.pop_back();
    --count;
    ++steps;  // move the last element into the hole

    if (index < count) {
      int afterUp = siftUp(index, steps);
      if (afterUp == index) {
        siftDown(index, steps);
      }
    }
    return steps;
  }

  /**
   * @brief Looks up the value stored under a key (linear search).
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    int steps = 0;
    for (int i = 0; i < count; ++i) {
      ++steps;
      if (data[i].key == key) {
        return {data[i].value, steps};
      }
    }
    return {std::nullopt, steps};
  }

  /// @brief Number of stored entries. Costs 0 steps.
  inline int size() const {
    return count;
  }
};
