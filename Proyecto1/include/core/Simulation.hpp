#pragma once

#include <cstdint>
#include <vector>
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
  std::uint64_t shots_fired = 0;    ///< Successful tower shots across all slots.
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
   * @brief Provides read-only access to the slot manager.
   * @return Const reference to the underlying SlotManager instance.
   */
  const SlotManager& slots() const {
    return slots_;
  }

  const Economy& economy() const {
    return economy_;
  }

 private:
  Config config_;
  Ticks ticks_engine_;
  WorldState world_state_;
  SlotManager slots_;
  WaveManager waves_;
  Economy economy_;

  Grid grid_;                         ///< Spatial grid structure representing the map layout.
  RouteData route_;                   ///< Pathfinding route data used by enemies to reach the base.
  std::vector<Enemy> active_enemies_; ///< List of active enemies currently spawned on the map.

  std::uint64_t shots_fired_ = 0;
  std::uint64_t total_steps_ = 0;

  int accumulator_ms_ = 0;  // Unprocessed real-time accumulator in ms.

  /**
   * @brief Syncs public WorldState with internal engine state: pulls the
   * current wave and whether every wave has been completed.
   * @note game_over currently reflects only wave completion — there are
   * no live enemies yet (Topics 1.3/3.4 aren't wired into tick()), so
   * Economy's defeat condition can never trigger. Once enemies exist,
   * this should also check economy_.isDefeated().
   */
  void refreshWorldState();
};
