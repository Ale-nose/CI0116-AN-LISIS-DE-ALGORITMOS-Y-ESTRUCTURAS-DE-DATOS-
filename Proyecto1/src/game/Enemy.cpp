// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Enemy.hpp"

#include "Ticks.hpp"

EnemyStats getEnemyStats(EnemyCategory category) {
  switch (category) {
    case EnemyCategory::Swarm:
      return {1, 2};
    case EnemyCategory::Wraith:
      return {8, 25};
    case EnemyCategory::Hive:
      return {20, 60};
    case EnemyCategory::Decoy:
      return {0, 0};
    case EnemyCategory::Colossus:
      return {800, 400};  // life raised from 400 to 800
    case EnemyCategory::kCount:
      return {0, 0};  // not a real category
  }
  return {0, 0};
}

Enemy::Enemy(EnemyId id, EnemyCategory category, int initialDistance)
    : id_(id),
      category_(category),
      distanceToBase_(initialDistance),
      lifetimeTicks_(0) {
  EnemyStats stats = getEnemyStats(category);
  life_ = stats.life;
  reward_ = stats.reward;
}

void Enemy::tick() {
  if (category_ == EnemyCategory::Decoy) {
    ++lifetimeTicks_;

    // Expire and remove Decoy when reaching its maximum time limit (15s)
    if (lifetimeTicks_ >= DECOY_LIFETIME_S * TICKS_PER_SECOND) {
      life_ = 0;
    }
  }
}

bool Enemy::takeDamage(int amount) {
  if (isInvulnerable()) {
    return false;  // Decoys are invulnerable and take no damage
  }

  life_ -= amount;

  // Clamp health to 0 and return true to report a fatal shot (enemy defeat)
  if (life_ <= 0) {
    life_ = 0;
    return true;
  }

  return false;
}
