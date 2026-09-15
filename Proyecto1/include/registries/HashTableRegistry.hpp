// Copyright 2026 Kevin Velásquez García
#pragma once

#include <functional>
#include <optional>
#include <utility>
#include <vector>

/**
 * @brief Target registry backed by a hash table with separate chaining.
 *
 * Amortized O(1) insert/erase/query; doubles its bucket count and
 * rehashes once the load factor passes kMaxLoadFactor.
 *
 * @tparam KeyType   Type used to look up entries (the enemy id).
 * @tparam ValueType Type stored alongside each key.
 */
template <typename KeyType, typename ValueType>
class HashTableRegistry {
 private:
  /// @brief A single chain entry.
  struct Entry {
    KeyType key;      ///< Lookup key.
    ValueType value;  ///< Stored value.
  };

  static constexpr int kInitialBucketCount = 8;      ///< Starting bucket count.
  static constexpr double kMaxLoadFactor = 0.75;     ///< Rehash threshold.

  std::vector<std::vector<Entry>> buckets;  ///< One chain per bucket.
  int count;                                ///< Number of stored entries.

  /// @brief Maps a key to its bucket index.
  int bucketIndex(const KeyType& key) const {
    return static_cast<int>(std::hash<KeyType>{}(key) % buckets.size());
  }

  /// @brief Whether inserting one more entry would exceed kMaxLoadFactor.
  bool loadFactorTooHigh() const {
    return static_cast<double>(count + 1) / buckets.size() > kMaxLoadFactor;
  }

  /**
   * @brief Doubles the bucket count and re-inserts every element.
   * @return Steps spent (one per element moved), like growing a dynamic array.
   */
  int rehash() {
    int steps = 0;
    std::vector<std::vector<Entry>> oldBuckets = std::move(buckets);
    buckets.assign(oldBuckets.size() * 2, {});

    for (std::vector<Entry>& chain : oldBuckets) {
      for (Entry& entry : chain) {
        int index = bucketIndex(entry.key);
        buckets[index].push_back(entry);
        ++steps;
      }
    }
    return steps;
  }

 public:
  /// @brief Creates an empty registry with kInitialBucketCount buckets.
  HashTableRegistry() : buckets(kInitialBucketCount), count(0) {}

  /**
   * @brief Inserts a key/value pair, rehashing first if needed.
   * @param key   Lookup key.
   * @param value Value to store.
   * @return Steps spent (rehash, if any, plus the bucket walk).
   */
  inline int insert(const KeyType& key, const ValueType& value) {
    int steps = 0;
    if (loadFactorTooHigh()) {
      steps += rehash();
    }

    int index = bucketIndex(key);
    ++steps;  // computing the hash

    std::vector<Entry>& chain = buckets[index];
    for (Entry& entry : chain) {
      ++steps;
      if (entry.key == key) {
        entry.value = value;  // key already exists, just update
        return steps;
      }
    }
    chain.push_back(Entry{key, value});
    ++steps;
    ++count;
    return steps;
  }

  /**
   * @brief Removes the entry with the given key from its bucket, if present.
   * @param key Key to remove.
   * @return Steps spent walking the bucket's chain.
   */
  inline int erase(const KeyType& key) {
    int steps = 0;
    int index = bucketIndex(key);
    ++steps;

    std::vector<Entry>& chain = buckets[index];
    for (auto it = chain.begin(); it != chain.end(); ++it) {
      ++steps;
      if (it->key == key) {
        chain.erase(it);
        ++steps;
        --count;
        return steps;
      }
    }
    return steps;  // not found: walked the whole bucket
  }

  /**
   * @brief Looks up the value stored under a key.
   * @param key Key to search for.
   * @return The value (if found) and the steps spent searching.
   */
  inline std::pair<std::optional<ValueType>, int> query(
    const KeyType& key) const {
    int steps = 0;
    int index = bucketIndex(key);
    ++steps;

    const std::vector<Entry>& chain = buckets[index];
    for (const Entry& entry : chain) {
      ++steps;
      if (entry.key == key) {
        return {entry.value, steps};
      }
    }
    return {std::nullopt, steps};
  }

  /// @brief Number of stored entries. Costs 0 steps.
  inline int size() const {
    return count;
  }
};
