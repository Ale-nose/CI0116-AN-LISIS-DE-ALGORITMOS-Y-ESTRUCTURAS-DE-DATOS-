// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include "KeyType.hpp"
#include "EnemyCategory.hpp"
#include "Ticks.hpp"

/**
 * @brief Duration in seconds that a Decoy enemy remains active on the board 
 * before expiring.
 */
constexpr int DECOY_LIFETIME_S = 15;

/**
 * @struct EnemyStats
 * @brief Represents the base stats (health and bounty) assigned to an enemy 
 * upon creation.
 */
struct EnemyStats {
  int life;
  int reward;
};

/**
 * @brief Returns the base life/reward for a given category.
 * @param category The category classification of the enemy.
 * @return EnemyStats Structure containing the base life points and reward 
 * value.
 */
inline EnemyStats getEnemyStats(EnemyCategory category);

class Enemy {
 public:
  /**
   * @brief Constructs an Enemy instance initialized with stats based on its 
   * category.
   */
  Enemy(EnemyId id, EnemyCategory category, int initialDistance);

  /**
   * @brief Gets the unique identifier of the enemy.
   * @return EnemyId Unique ID.
   */
  EnemyId getId() const {
    return id_;
  }

  /**
   * @brief Gets the category classification of the enemy.
   * @return EnemyCategory Category enum value.
   */
  EnemyCategory getCategory() const {
    return category_;
  }

  /**
   * @brief Gets the current health points of the enemy.
   * @return int Current health points remaining.
   */
  int getLife() const {
    return life_;
  }

  /**
   * @brief Gets the credit reward granted when this enemy is killed.
   * @return int Reward amount in credits.
   */
  int getReward() const {
    return reward_;
  }

  /**
   * @brief Gets the current remaining distance to the player's base.
   * @return int Distance value in grid units/steps.
   */
  int getDistanceToBase() const {
    return distanceToBase_;
  }

  /**
   * @brief Checks if the enemy is still alive.
   * @return true if health points are strictly greater than 0; false otherwise.
   */
  bool isAlive() const {
    return life_ > 0;
  }

  /**
   * @brief Checks whether the enemy is completely invulnerable to all incoming 
   * damage.
   * @return true if the enemy is a Decoy; false otherwise.
   */
  bool isInvulnerable() const {
    return category_ == EnemyCategory::Decoy;
  }

  /**
   * @brief Advances the internal lifecycle clock of the enemy by one 
   * simulation tick.
   * @note If the enemy is a Decoy, increments its lifetime counter and causes
   * it to expire once it hits its maximum lifetime.
   */
  void tick();

  /**
   * @brief Processes incoming damage applied to the enemy based on 
   * category-specific rules.
   * @param amount Hit points of damage to inflict.
   * @return true if this damage instance fatalizes the enemy (reducing life 
   * to 0); false otherwise.
   * @note Decoys take no damage. Colossus has no special damage rule
   * anymore — its resistance comes entirely from its higher base life.
   */
  bool takeDamage(int amount);

  /**
   * @brief Advances the enemy position closer toward the base.
   * @param delta Distance reduction to apply in grid units.
   */
  void advance(int delta) {
    distanceToBase_ -= delta;
  }

 private:
  EnemyId id_;
  EnemyCategory category_;
  int life_;
  int reward_;           ///< Credit payout awarded upon defeat.
  int distanceToBase_;   ///< Distance steps remaining to reach the base.
  int lifetimeTicks_;    ///< Accumulated active ticks
};
