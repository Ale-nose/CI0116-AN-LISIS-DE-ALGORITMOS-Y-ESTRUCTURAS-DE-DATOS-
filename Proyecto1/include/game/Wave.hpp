// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <random>
#include <vector>
#include <cstdint>
#include "Enemy.hpp"
#include "EnemyCategory.hpp"

constexpr int TOTAL_WAVES = 20;
constexpr int WAVE_BASE = 20;           // enemies in wave 1
constexpr double WAVE_FACTOR = 1.3;     // growth per wave: base * factor^(w-1)
/**
 * @brief Ticks between spawns during combat, for waves small enough to
 * finish spawning within WAVE_MAX_SPAWN_TICKS at this pace.
 */
constexpr int WAVE_SPAWN_INTERVAL_TICKS = 10;

/**
 * @brief Longest a wave may take to finish spawning (20 s). Bigger waves
 * are spread evenly over this window instead, several enemies per tick,
 * so towers on busy slots really face many enemies at once (section 2.1)
 * instead of a steady trickle of one every 10 ticks.
 */
constexpr int WAVE_MAX_SPAWN_TICKS = 1200;

/// @brief How many enemies of each category make up a wave.
struct WaveComposition {
  std::array<int, kCategoryCount> perCategory{};

  /// @brief Total enemies across all categories.
  int total() const;
};

/**
 * @brief Computes how many enemies wave `waveNumber` should have.
 * @param waveNumber 1-based wave number (1..TOTAL_WAVES).
 * @return Enemy count, following WAVE_BASE * WAVE_FACTOR^(waveNumber-1).
 */
int waveSize(int waveNumber);

/**
 * @brief Splits a wave's total size evenly across the 5 categories.
 * @param totalSize Total enemy count for the wave (from waveSize()).
 * @param onlyCategory If set, the whole wave goes to this category.
 * @return A composition whose categories sum to totalSize.
 */
WaveComposition buildComposition(int totalSize,
  std::optional<EnemyCategory> onlyCategory = std::nullopt);

/// @brief Which of the two phases the current wave is in.
enum class WavePhase {
  Construction,  // unlimited time; player previews composition, buys cores
  Combat         // wave is running; slot cores are fixed
};

/**
 * @brief Tracks wave number, phase, and enemy spawn timing.
 *
 * Owns nothing about towers, credits, or enemies themselves — it only
 * knows "what wave are we on" and "should an enemy spawn this tick,
 * and of which category".
 */
class WaveManager {
 private:
  int currentWave_;
  WavePhase phase_;
  WaveComposition composition_;

  std::vector<EnemyCategory> spawnQueue;  // flattened, one entry per enemy
  size_t nextToSpawn;
  int combatTicks;           ///< Ticks elapsed in the current combat phase.
  int spawnWindowTicks;      ///< Ticks this wave takes to finish spawning.
  uint64_t totalSpawnedCount;  ///< Enemies spawned so far in the match.

  /**
   * @brief When set, every wave is made only of this category (report
   * experiments, Topic 7); std::nullopt keeps the normal even split.
   */
  std::optional<EnemyCategory> onlyCategory_;

  /**
   * @brief Seeded RNG: decides the order enemies spawn within each wave,
   * so matches with different seeds differ (bit-for-bit reproducible for
   * the same seed).
   */
  std::mt19937 rng_;

 public:
  /**
   * @brief Starts at wave 1, in the construction phase.
   * @param seed Match seed; decides the spawn order within each wave.
   * @param onlyCategory If set, every wave is made only of this category
   *        (headless experiments for the report); otherwise waves are
   *        split evenly across the 5 categories.
   */
  explicit WaveManager(std::uint32_t seed = 0,
    std::optional<EnemyCategory> onlyCategory = std::nullopt);

  /// @brief The wave currently being previewed or fought (1-based).
  int currentWave() const { return currentWave_; }

  /// @brief Which phase the current wave is in.
  WavePhase phase() const { return phase_; }

  /// @brief Composition of the current wave, valid in both phases.
  const WaveComposition& composition() const { return composition_; }

  /**
   * @brief Ends the construction phase and starts combat.
   *
   * Builds the spawn queue from the current wave's composition and
   * resets the spawn timer. Only valid while phase() == Construction.
   */
  void startCombat();

  /**
   * @brief Advances the spawn timer by one simulation tick.
   * @param initialDistance Route length every new enemy starts from.
   * @param hiveIdStride Stride used to build colliding Hive ids.
   * @return The enemies due this tick: none, one, or several (large
   *         waves spread over WAVE_MAX_SPAWN_TICKS spawn several per
   *         tick). Empty once the whole wave has spawned.
   */
  std::vector<Enemy> tick(int initialDistance, std::size_t hiveIdStride);

  /// @brief Whether every enemy in this wave has been handed off to tick().
  bool doneSpawning() const;

  /**
   * @brief Moves on to the next wave and returns to the construction phase.
   *
   * Call once combat is fully resolved (doneSpawning() and every spawned
   * enemy is dead or has reached the exit — that bookkeeping lives
   * outside WaveManager, since it doesn't track live enemies).
   */
  void advanceToNextWave();

  /// @brief Whether wave TOTAL_WAVES has already been completed.
  bool allWavesComplete() const { return currentWave_ > TOTAL_WAVES; }
};