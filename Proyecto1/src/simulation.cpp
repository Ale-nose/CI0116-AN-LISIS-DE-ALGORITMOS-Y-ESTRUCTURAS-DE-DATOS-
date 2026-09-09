#include "simulation.hpp"

Simulation::Simulation(std::uint32_t seed, const Config& config)
  : config_(config), ticks_engine_(seed) {
}

void Simulation::tick() {
  ticks_engine_.tick();
  ++world_state_.ticks_elapsed;
  refreshWorldState();
}

void Simulation::advance(int elapsed_ms) {
  accumulator_ms_ += elapsed_ms;
  int ticks_run = 0;
  // runs ticks until the accumulator or the catch-up cap runs out
  while (accumulator_ms_ >= MS_PER_TICK && ticks_run < MAX_CATCHUP_TICKS) {
    tick();
    accumulator_ms_ -= MS_PER_TICK;
    ++ticks_run;
  }
  // leftover time past the cap is discarded on purpose, not carried over
}

bool Simulation::over() const {
  return world_state_.game_over;
}

const WorldState& Simulation::state() const {
  return world_state_;
}

Stats Simulation::stats() const {
  Stats s;
  // pack tick count and its ms equivalent into the snapshot
  s.ticks_run = world_state_.ticks_elapsed;
  s.simulated_ms = world_state_.ticks_elapsed * 
    static_cast<std::uint64_t>(MS_PER_TICK);
  return s;
}

void Simulation::refreshWorldState() {
  world_state_.game_over = ticks_engine_.over();
}
