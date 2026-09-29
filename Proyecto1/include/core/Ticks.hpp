// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once
#include <cstdint>
#include <random>

/**
 * @brief Section 6 — simulation ticks per second of game time.
 */
constexpr int TICKS_PER_SECOND  = 60;

/**
 * @brief Real milliseconds one tick represents.
 */
constexpr int MS_PER_TICK       = 1000 / TICKS_PER_SECOND;

/**
 * @brief Steps each tower may spend per tick (2400 per second).
 */
constexpr int STEPS_PER_TICK    = 40;

/**
 * @brief Most ticks advance() may run in one call, so a stalled machine
 * doesn't fall into a catch-up spiral.
 */
constexpr int MAX_CATCHUP_TICKS = 5;

/**
 * @brief Number of tower slots on the map.
 */
constexpr int SLOTS             = 8;

/**
 * @brief Section 1.1 — fixed-step tick engine: turns real elapsed time
 * into a whole number of simulation ticks, and owns the match's seeded
 * random number generator.
 */
class Ticks {
 public:
  /**
   * @brief Creates the engine.
   * @param seed Match seed. A fixed-width 32-bit type, so the generator
   * gets the same seed (and gives the same sequence) on every platform.
   */
  explicit Ticks(std::uint32_t seed);

  /**
   * @brief Adds real elapsed time and runs the ticks it covers, at most
   * MAX_CATCHUP_TICKS per call. Time left over stays in the accumulator
   * for the next call.
   * @param elapsed_ms Real milliseconds elapsed since the last call.
   */
  void advance(int elapsed_ms);

  /**
   * @brief Runs one simulation tick.
   */
  void tick();

  /**
   * @brief Whether the engine considers the match finished.
   * @return true once the match is over.
   */
  bool over() const;

 private:
  std::mt19937 rng_;             ///< Seeded generator, bit-for-bit repeatable.
  int accumulator_ms_ = 0;       ///< Real time not yet turned into ticks.
  int busy_ticks_[SLOTS] = {0};  ///< Legacy per-slot block counters.
  bool game_over_ = false;       ///< Whether the match ended.

  /**
   * @brief Ticks an operation of a given cost occupies:
   * ceil(steps / STEPS_PER_TICK), computed without floating point.
   * @param steps Cost of the operation.
   * @return Whole ticks it spans.
   */
  static int ticksToBlock(int steps) {
    return (steps + STEPS_PER_TICK - 1) / STEPS_PER_TICK;
  }
};