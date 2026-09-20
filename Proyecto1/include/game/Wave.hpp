// Copyright 2026 Kevin Velásquez García
#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <vector>
#include <cstdint>
#include "Enemy.hpp"
#include "EnemyCategory.hpp"

constexpr int TOTAL_WAVES = 20;
constexpr int WAVE_BASE = 20;           // enemies in wave 1
constexpr double WAVE_FACTOR = 1.3;     // growth per wave: base * factor^(w-1)
constexpr int WAVE_SPAWN_INTERVAL_TICKS = 10; // ticks between spawns during combat

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
 * @return A composition whose categories sum to totalSize.
 */
WaveComposition buildComposition(int totalSize);

/// @brief Which of the two phases the current wave is in.
enum class WavePhase {
  Construction, // unlimited time; player previews composition, buys/swaps cores
  Combat        // wave is running; slot cores are fixed
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
  int ticksSinceLastSpawn;
  uint64_t totalSpawnedCount;

  public:
  /// @brief Starts at wave 1, in the construction phase.
  WaveManager();
  
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
   * @return The category to spawn this tick, or std::nullopt if no
   *         spawn is due (either the timer hasn't elapsed yet, or the
   *         whole wave has already finished spawning).
   */
  std::optional<Enemy> tick(int initialDistance, std::size_t hiveBucketCount);

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
