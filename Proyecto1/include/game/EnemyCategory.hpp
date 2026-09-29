// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <cstddef>

/// @brief The 5 enemy categories.
enum class EnemyCategory {
  Swarm,     ///< Arrives in increasing id order: degenerates a BST.
  Wraith,    ///< Keeps entering and leaving radii: pure maintenance.
  Hive,      ///< Ids built to collide in the default hash function.
  Decoy,     ///< Invulnerable; inflates registry sizes, then expires.
  Colossus,  ///< Lots of life and a big reward.
  kCount     ///< Not a real category; used to size arrays.
};

/**
 * @brief Number of real enemy categories.
 */
constexpr size_t kCategoryCount = static_cast<size_t>(EnemyCategory::kCount);