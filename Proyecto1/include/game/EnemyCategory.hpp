// Copyright 2026 Kevin Velásquez García
#pragma once

#include <cstddef>

/// @brief The 5 enemy categories.
enum class EnemyCategory {
  Swarm,
  Wraith,
  Hive,
  Decoy,
  Colossus,
  kCount  // not a real category; used to size arrays
};

constexpr size_t kCategoryCount = static_cast<size_t>(EnemyCategory::kCount);
