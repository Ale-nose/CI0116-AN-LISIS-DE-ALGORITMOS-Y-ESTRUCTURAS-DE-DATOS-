#include "Simulation.hpp"

Simulation::Simulation(std::uint32_t seed, const Config& config)
  : config_(config), ticks_engine_(seed) {
}

void Simulation::tick() {
  ticks_engine_.tick();
  ++world_state_.ticks_elapsed;

  // ### Enemy spawning/movement (Topics 1.3/3.4) belongs here: place the
  // ### Enemy waves_.tick() returns via Pathfinding, and enqueueInsert/
  // ### enqueueErase on whichever slots it enters/leaves. initialDistance
  // ### and hiveBucketCount below are placeholders (0 and 8) until the
  // ### map exposes a real entrance-to-base distance and the hash core's
  // ### real bucket count. Until that's wired in, every occupied slot
  // ### just repeatedly fires query() against an empty registry.
  if (waves_.phase() == WavePhase::Construction) {
    waves_.startCombat();
  }
  waves_.tick(/*initialDistance=*/0, /*hiveBucketCount=*/8);

  auto results = slots_.tickAll();
  for (const auto& r : results) {
    if (r.fired) {
      ++shots_fired_;
    }
    total_steps_ += static_cast<std::uint64_t>(r.stepsUsed);
  }

  if (waves_.doneSpawning() && !waves_.allWavesComplete()) {
    waves_.advanceToNextWave();
  }

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
  s.shots_fired = shots_fired_;
  s.total_steps = total_steps_;
  s.waves_completed = world_state_.current_wave - 1;
  return s;
}

void Simulation::installCoreEverywhere(CoreType type) {
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    slots_.installCore(i, type);
  }
}

void Simulation::refreshWorldState() {
  // ### Should also check an Economy defeat condition once enemies can
  // ### actually reach the base (Topics 1.3/3.3/3.4).
  world_state_.game_over = waves_.allWavesComplete();
  world_state_.current_wave = waves_.currentWave();
}