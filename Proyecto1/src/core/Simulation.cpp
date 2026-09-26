// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Simulation.hpp"

#include "CoreFactory.hpp"
#include "HashTableRegistry.hpp"

Simulation::Simulation(std::uint32_t seed, const Config& config)
  : config_(config), ticks_engine_(seed) {
  buildDefaultMap(grid_);
  route_ = buildRoute(grid_, /* entrance */ {0, 1}, /* exit */ {19, 9});
  refreshWorldState();
}

void Simulation::tick() {
  ticks_engine_.tick();
  ++world_state_.ticks_elapsed;

  if (waves_.phase() == WavePhase::Construction) {
    waves_.startCombat();
  }

  int initialDistance = static_cast<int>(route_.length());
  // Attempt to spawn a new enemy for the current tick
  auto spawnedEnemy = waves_.tick(initialDistance,
    static_cast<std::size_t>(HashTableRegistry::kInitialBucketCount));

  if (spawnedEnemy.has_value()) {
    active_enemies_.push_back(*spawnedEnemy);

    for (int i = 0; i < SlotManager::kSlotCount; i++) {
      // TODO(Topic 3.1): determine which slot(s) currently have this
      // enemy within radius, based on its position along route_, and
      // enqueueInsert only on those. Right now the enemy exists in
      // active_enemies_ but isn't tracked by any tower yet.
    }
  }

  auto results = slots_.tickAll();
  for (const auto& r : results) {
    if (r.fired) {
      ++shots_fired_;
    }
    total_steps_ += static_cast<std::uint64_t>(r.stepsUsed);
  }

  if (waves_.doneSpawning() && !waves_.allWavesComplete()
      && active_enemies_.empty()) {
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

bool Simulation::purchaseCore(int slotIndex, CoreType type) {
  if (!economy_.buyCore(type)) {
    return false;  // can't afford it — nothing charged, nothing installed
  }
  slots_.installCore(slotIndex, type);
  return true;
}

void Simulation::refreshWorldState() {
  world_state_.game_over = waves_.allWavesComplete() || economy_.isDefeated();
  world_state_.current_wave = waves_.currentWave();
  world_state_.credits = economy_.getCredits();
  world_state_.lives = economy_.getLives();
  world_state_.next_wave_composition = waves_.composition();
}
