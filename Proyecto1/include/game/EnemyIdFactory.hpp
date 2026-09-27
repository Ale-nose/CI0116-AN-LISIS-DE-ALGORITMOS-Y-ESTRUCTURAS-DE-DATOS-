// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstddef>
#include <cstdint>

#include "KeyType.hpp"
#include "EnemyCategory.hpp"

/**
 * @brief Stride between Hive ids. A power of two larger than any bucket
 * count a hash table reaches in a match, so with the default hash
 * (identity modulo the bucket count) every Hive id lands in bucket 0 no
 * matter how many times the table rehashes (section 3.5).
 */
constexpr std::size_t HIVE_ID_STRIDE = 1024;

/**
 * @brief Generates category-specific EnemyIds designed to stress-test data
 * structures. Ids are unique across every category:
 *  - Hive:    spawnIndex * hiveIdStride (always even), all colliding in
 *             bucket 0 of the default hash.
 *  - others:  2 * spawnIndex + 1 (always odd), strictly increasing, so a
 *             Swarm arrives in increasing id order and degenerates an
 *             unbalanced BST.
 * @param category Category of the enemy being spawned.
 * @param spawnIndex Sequential order/index of creation.
 * @param hiveIdStride Stride for Hive ids; must be even (HIVE_ID_STRIDE).
 * @return EnemyId unique ID value.
 */
inline EnemyId generateEnemyId(EnemyCategory category, std::uint64_t spawnIndex
  , std::size_t hiveIdStride) {
  switch (category) {
    case EnemyCategory::Hive:
      return static_cast<EnemyId>(spawnIndex * hiveIdStride);
    default:
      return static_cast<EnemyId>(2 * spawnIndex + 1);
  }
}
