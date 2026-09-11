#pragma once

#include <cstdint>
#include "ticks.hpp"

/**
 * @brief Configuration parameters needed to initialize Simulation
 */
struct Config {
  // TODO: Tema 3 STARTING_CREDITS, STARTING_LIVES, WAVE_BASE, etc
};

/**
 * @brief Read-only snapshot of the game state for the UI
 */
struct WorldState {
  std::uint64_t ticks_elapsed = 0;  ///< Total ticks executed.
  bool game_over = false;
};

/**
 * @brief Consolidated summary of simulator metrics and statistics.
  */
struct Stats {
  std::uint64_t ticks_run = 0;  ///< Total processed ticks.
  std::uint64_t simulated_ms = 0;  ///< ticks_run * MS_PER_TICK
};

class Simulation {
 public:
  /**
   * @brief Constructs the simulation engine.
   */
  Simulation(std::uint32_t seed, const Config& cfg);

  /**
   * @brief Advances the simulation by one discrete tick. It asks the tick 
   * engine to process all towers/slots, increments the overall match tick 
   * counter, and refreshes the WorldState.
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

 private:
  Config config_;
  Ticks ticks_engine_;
  WorldState world_state_;

  int accumulator_ms_ = 0;  // Unprocessed real-time accumulator in ms.

  /**
   * @brief Syncs public WorldState with internal tick engine state. Checks 
   * whether the Ticks class reported the match as finished and reflects 
   * that in the world_state_.game_over flag.
   */
  void refreshWorldState();
};
