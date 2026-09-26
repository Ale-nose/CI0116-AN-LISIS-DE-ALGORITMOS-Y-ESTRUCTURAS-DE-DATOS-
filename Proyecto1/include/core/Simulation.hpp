// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <map>
#include <vector>
#include "CombatLog.hpp"
#include "CorePrices.hpp"
#include "Slots.hpp"
#include "Ticks.hpp"
#include "Wave.hpp"
#include "Grid.hpp"
#include "MapBuilder.hpp"
#include "Pathfinding.hpp"
#include "Enemy.hpp"
#include "Economy.hpp"

/**
 * @brief Configuration parameters needed to initialize Simulation
 */
struct Config {
  // ### Room for future match-setup knobs (Topic 3) beyond seed. ###
};

/**
 * @brief Read-only snapshot of the game state for the UI
 */
struct WorldState {
  std::uint64_t ticks_elapsed = 0;  ///< Total ticks executed.
  bool game_over = false;
  int current_wave = 1;  ///< 1-based wave currently being previewed/fought.
  int credits = STARTING_CREDITS;
  int lives = STARTING_LIVES;
  WaveComposition next_wave_composition{};
};

/**
 * @brief Consolidated summary of simulator metrics and statistics.
 */
struct Stats {
  std::uint64_t ticks_run = 0;      ///< Total processed ticks.
  std::uint64_t simulated_ms = 0;   ///< ticks_run * MS_PER_TICK
  std::uint64_t shots_fired = 0;    ///< Successful tower shots across all slots
  std::uint64_t total_steps = 0;    ///< Steps consumed across all slots.
  int waves_completed = 0;          ///< Waves fully finished spawning.
};

class Simulation {
 public:
  /**
   * @brief Constructs the simulation engine.
   */
  Simulation(std::uint32_t seed, const Config& cfg);

  /**
   * @brief Advances the simulation by one discrete tick. Drives the wave
   * spawn timer, asks every slot's tower to act, increments the overall
   * match tick counter, and refreshes the WorldState.
   */
  void tick();

  /**
   * @brief Checks if the match has ended.
   * @return true if game is over; false otherwise.
   */
  bool over() const;

  /**
   * @brief Provides read-only access to the current world state. The rendering
   * layer uses this method to obtain whatever it needs to draw on screen.
   * @return Const reference to WorldState.
   */
  const WorldState& state() const;

  /**
   * @brief Builds a snapshot of the simulation's aggregated statistics.
   * @return Stats object with accumulated metrics.
   */
  Stats stats() const;

  /**
   * @brief Advances simulation based on real time delta.
   * @param elapsed_ms Real-world time elapsed in milliseconds.
   * @note If the machine falls behind suddenly, it applies the maximum
   * catch-up tick cap.
   */
  void advance(int elapsed_ms);

  /**
   * @brief Section 4.2 — installs the same core on every one of the 8
   * slots. Used by --core in headless mode; the windowed UI will
   * instead call SlotManager per slot as the player buys/upgrades
   * individual towers (Topic 5.3).
   * @param type Structure to install everywhere.
   */
  void installCoreEverywhere(CoreType type);

  /**
   * @brief Section 5.3 — buys a core from Economy and, only if the
   * purchase succeeds, installs it in the given slot. Atomic: never
   * charges credits without installing, never installs without paying.
   * @param slotIndex Slot to install the purchased core into.
   * @param type Structure the player chose in the upgrade dialog.
   * @return true if the purchase went through.
   */
  bool purchaseCore(int slotIndex, CoreType type);

  /**
   * @brief Provides read-only access to the slot manager, for the UI to
   * read per-slot state (5.2's panels).
   * @note Named slotManager(), not slots() — Qt defines `slots` as a
   * macro, which breaks a method with that exact name in any file that
   * also includes Qt headers.
   * @return Const reference to the underlying SlotManager instance.
   */
  const SlotManager& slotManager() const {
    return slots_;
  }

  /**
   * @brief Provides read-only access to the economy, for the upgrade
   * dialog (5.3) to show current credits and grey out unaffordable
   * options.
   * @return Const reference to the underlying Economy instance.
   */
  const Economy& economy() const {
    return economy_;
  }

  /**
   * @brief Section 6.1 — every combat log row produced so far: one per
   * occupied slot per finished wave (plus the unfinished wave, once the
   * match is over).
   * @return Const reference to the accumulated records.
   */
  const std::vector<CombatLogRecord>& combatLog() const {
    return combat_log_;
  }

 private:
  Config config_;
  Ticks ticks_engine_;
  WorldState world_state_;
  SlotManager slots_;
  WaveManager waves_;
  Economy economy_;

  Grid grid_;          ///< Spatial grid structure representing the map layout.
  // Pathfinding route data used by enemies to reach the base.
  RouteData route_;

  /**
   * @brief An enemy on the map plus which slots currently see it, so
   * entering/leaving a radius can be turned into insert/erase ops.
   */
  struct TrackedEnemy {
    Enemy enemy;
    std::bitset<SlotManager::kSlotCount> inRange;
  };

  // Enemies currently on the map, keyed by id. std::map (not
  // unordered_map) so iteration order — and therefore the whole
  // match — is identical on every machine for the same seed.
  std::map<EnemyId, TrackedEnemy> active_enemies_;

  std::uint64_t shots_fired_ = 0;
  std::uint64_t total_steps_ = 0;

  int accumulator_ms_ = 0;  // Unprocessed real-time accumulator in ms.

  // Section 6.1 — per-slot shot classification for the current wave.
  std::array<int, SlotManager::kSlotCount> effective_shots_{};
  std::array<int, SlotManager::kSlotCount> ghost_shots_{};
  std::vector<CombatLogRecord> combat_log_;
  bool final_wave_logged_ = false;

  /**
   * @brief Moves every enemy along the route, expires Decoys, leaks the
   * ones that reach the base, and syncs each slot's radius membership.
   */
  void moveEnemies();

  /**
   * @brief Recomputes which slots see this enemy at its current path
   * cell, queueing an insert on each slot it entered and an erase on
   * each slot it left.
   */
  void updateRangeMembership(EnemyId id, TrackedEnemy& tracked);

  /**
   * @brief Takes an enemy off the map (killed, expired or leaked) and
   * queues an erase on every slot that was still tracking it.
   */
  void removeEnemy(EnemyId id);

  /**
   * @brief Closes the combat log for a wave: appends one record per
   * occupied slot and resets the per-wave counters.
   * @param waveNumber 1-based wave being closed.
   */
  void logWave(int waveNumber);

  /**
   * @brief Syncs public WorldState with internal engine state: pulls the
   * current wave, credits, lives, next wave composition, and whether
   * the match has ended (all waves complete or the player was
   * defeated).
   */
  void refreshWorldState();
};