#pragma once

#include <cstdint>

#include "KeyType.hpp"
#include "Wave.hpp"

/**
 * @brief Generates category-specific EnemyIds designed to stress-test data 
 * structures.
 * @param category Category of the enemy being spawned.
 * @param spawnIndex Sequential order/index of creation.
 * @param bucketCount Total number of buckets in the target Hash Table.
 * @return EnemyId unique ID value.
 */
inline EnemyId generateEnemyId(EnemyCategory category, std::uint64_t spawnIndex
  , std::size_t bucketCount) {
  switch (category){
    case EnemyCategory::Hive:
      // Multiplies by bucketCount to force Hash Table bucket 0 collisions.
      return static_cast<EnemyId>(spawnIndex * bucketCount);
    default:
      // Sequential IDs (0, 1, 2...)
      return static_cast<EnemyId>(spawnIndex);
  }
}
