// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>

/**
 * @brief Target registry backed by a sorted dynamic array.
 *
 * Search uses binary search (O(log n)); insertion and removal still
 * need to shift elements to keep the array sorted (O(n)).
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class SortedArrayRegistry {
 private:
  /// @brief A single array slot.
  struct Entry {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
  };

  /// @brief Result of a binary search.
  struct SearchResult {
    int index;   ///< Index of the key, or where it should be inserted.
    bool found;  ///< Whether the key was actually present.
    int steps;   ///< Steps spent searching.
  };

  /// @brief Doubles capacity and copies every element over.
  inline void grow() {
    int newCapacity = capacity * 2;
    Entry* newData = new Entry[newCapacity];
    for (int i = 0; i < count; ++i) {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
  }

  /**
   * @brief Binary search for a key.
   * @param key Key to search for.
   * @return Index (found or insertion point), found flag, and steps spent.
   */
  inline SearchResult binarySearch(const KeyType& key) const {
    int steps = 0;
    int low = 0;
    int high = count - 1;

    while (low <= high) {
      ++steps;
      int mid = low + (high - low) / 2;
      if (data[mid].key == key) {
        return {mid, true, steps};
      } else if (data[mid].key < key) {
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }
    return {low, false, steps};
  }

  Entry* data;    ///< Backing storage, kept sorted by key.
  int capacity;   ///< Allocated slots.
  int count;      ///< Slots actually in use.

 public:
  /// @brief Creates an empty registry with room for 4 entries.
  SortedArrayRegistry() : data(new Entry[4]), capacity(4), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  SortedArrayRegistry(const SortedArrayRegistry&) = delete;
  SortedArrayRegistry& operator=(const SortedArrayRegistry&) = delete;

  /// @brief Frees the backing array.
  ~SortedArrayRegistry() {
    delete[] data;
  }

  /**
   * @brief Inserts a key/value pair, keeping the array sorted.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent (binary search plus shifting elements right).
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    SearchResult result = binarySearch(key);
    int steps = result.steps;

    if (count == capacity) {
      grow();
    }

    for (int i = count; i > result.index; --i) {
      data[i] = data[i - 1];
      ++steps;  // one step per shifted element
    }

    data[result.index] = Entry{key, value};
    ++count;
    ++steps;  // the final write
    return steps;
  }

  /**
   * @brief Removes the entry with the given key, if present.
   * @param key Key to remove.
   * @return Steps spent (binary search plus shifting elements left).
   */
  inline int erase(const KeyType& key) {
    SearchResult result = binarySearch(key);
    if (!result.found) {
      return result.steps;  // just the search, nothing else to do
    }

    int steps = result.steps;
    for (int i = result.index; i < count - 1; ++i) {
      data[i] = data[i + 1];
      ++steps;  // one step per shifted element
    }
    --count;
    return steps;
  }

  /**
   * @brief Looks up the value stored under a key (binary search).
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    SearchResult result = binarySearch(key);
    if (result.found) {
      return {data[result.index].value, result.steps};
    }
    return {std::nullopt, result.steps};
  }

  /// @brief Number of stored entries. Costs 0 steps.
  inline int size() const {
    return count;
  }
};
