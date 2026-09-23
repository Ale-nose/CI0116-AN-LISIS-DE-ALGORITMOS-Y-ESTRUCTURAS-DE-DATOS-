// Copyright 2026 Kevin Velásquez García
#pragma once

#include <vector>

#include "TargetRegistry.hpp"

/**
 * @brief Target registry backed by a hash table with separate chaining.
 *
 * Amortized O(1) insert/erase by id. Has no natural order, so its
 * cheap native query just scans buckets from the start until it finds
 * any entry — cost is not guaranteed, unlike the other 7 structures.
 */
class HashTableRegistry : public ITargetRegistry {
 private:
  /// @brief A single chain entry.
  struct Entry {
    EnemyId id;  ///< Enemy identifier.
    Key key;     ///< Hashing key (== id in this project).
  };
  
  static constexpr double kMaxLoadFactor = 0.75;  ///< Rehash threshold.
  
  /// @brief Maps a key to its bucket index.
  int bucketIndex(Key key) const;
  
  /// @brief Whether inserting one more entry would exceed kMaxLoadFactor.
  bool loadFactorTooHigh() const;
  
  /**
   * @brief Doubles the bucket count and re-inserts every element.
   * @return Steps spent (one per element moved), like growing a dynamic array.
   */
  int rehash();
  
  std::vector<std::vector<Entry>> buckets;  ///< One chain per bucket.
  int count;                                ///< Number of stored entries.

 public:
  static constexpr int kInitialBucketCount = 8;   ///< Starting bucket count.

  /// @brief Creates an empty registry with kInitialBucketCount buckets.
  HashTableRegistry();

  /**
   * @brief Inserts a key/value pair, rehashing first if needed.
   * @param id Enemy identifier.
   * @param k  Key this table hashes on (same value as id, per spec).
   * @return Steps spent (rehash, if any, plus the bucket walk).
   */
  int insert(EnemyId id, Key k) override;

  /**
   * @brief Removes the entry with the given id from its bucket, if present.
   * @param id Enemy identifier to remove.
   * @return Steps spent walking the bucket's chain.
   */
  int erase(EnemyId id) override;

  /**
   * @brief Answers the cheap native question: 
   * first entry found scanning buckets.
   * @param out Set to that entry's id, if the table is non-empty.
   * @return Steps spent scanning buckets until one is non-empty.
   */
  int query(EnemyId& out) const override;

  /// @brief Number of stored entries. Costs 0 steps.
  size_t size() const override;
};
