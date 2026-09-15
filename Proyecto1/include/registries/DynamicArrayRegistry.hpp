// Copyright 2026 Kevin Velásquez García
#pragma once

#include <optional>
#include <utility>

/**
 * @brief Target registry backed by a dynamic array (amortized growth).
 *
 * Insertion is O(1) amortized (doubles capacity when full); removal
 * swaps the target with the last element instead of shifting.
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class DynamicArrayRegistry {
 private:
  /// @brief A single array slot.
  struct Entry {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
  };

  Entry* data;    ///< Backing storage.
  int capacity;   ///< Allocated slots.
  int count;      ///< Slots actually in use.

 public:
  /// @brief Creates an empty registry with room for 4 entries.
  DynamicArrayRegistry() : data(new Entry[4]), capacity(4), count(0) {}

  // Raw pointers are owned here; copying would double-free them.
  DynamicArrayRegistry(const DynamicArrayRegistry&) = delete;
  DynamicArrayRegistry& operator=(const DynamicArrayRegistry&) = delete;

  /// @brief Frees the backing array.
  ~DynamicArrayRegistry() {
    delete[] data;
  }

  /**
   * @brief Appends a key/value pair, growing the array if needed.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent (copies made while growing, plus 1).
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    int steps = 0;
    if (count == capacity) {
      int newCapacity = capacity * 2;
      Entry* newData = new Entry[newCapacity];
      for (int i = 0; i < count; ++i) {
        newData[i] = data[i];
        ++steps;  // one step per element copied while growing
      }
      delete[] data;
      data = newData;
      capacity = newCapacity;
    }
    data[count] = Entry{key, value};
    ++count;
    ++steps;  // the final write
    return steps;
  }

  /**
   * @brief Removes the entry with the given key by swapping with the last one.
   * @param key Key to remove.
   * @return Steps spent searching (plus the swap, if found).
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
      return steps;  // not found: walked the whole array
    }
    data[index] = data[count - 1];  // swap with the last element
    ++steps;
    --count;
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
