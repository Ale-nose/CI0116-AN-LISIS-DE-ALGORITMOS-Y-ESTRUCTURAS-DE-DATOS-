// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Simulation.hpp"

#include <cstdlib>
#include <vector>

#include "CombatConstants.hpp"
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
    EnemyId id = spawnedEnemy->getId();
    auto inserted = active_enemies_.emplace(
      id, TrackedEnemy{*spawnedEnemy, {}});
    updateRangeMembership(id, inserted.first->second);
  }

  moveEnemies();

  auto results = slots_.tickAll();
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    const auto& r = results[i];
    total_steps_ += static_cast<std::uint64_t>(r.stepsUsed);
    if (!r.fired) {
      continue;
    }
    ++shots_fired_;

    auto target = active_enemies_.find(r.target);
    if (target == active_enemies_.end() || !target->second.enemy.isAlive()) {
      ++ghost_shots_[i];  // the registry was still tracking a dead enemy
      continue;
    }
    ++effective_shots_[i];

    Enemy& enemy = target->second.enemy;
    if (enemy.takeDamage(TOWER_DAMAGE)) {
      // The tower that fired the final shot collects the reward.
      economy_.rewardKill(enemy.getReward());
      removeEnemy(r.target);
    }
  }
  if (waves_.doneSpawning() && !waves_.allWavesComplete()
      && active_enemies_.empty()) {
    logWave(waves_.currentWave());
    waves_.advanceToNextWave();
  }

  refreshWorldState();

  // A defeat ends the match mid-wave: log that unfinished wave once so
  // the file still covers every tick that was played.
  if (world_state_.game_over && !waves_.allWavesComplete()
      && !final_wave_logged_) {
    logWave(waves_.currentWave());
    final_wave_logged_ = true;
  }
}

void Simulation::moveEnemies() {
  const bool stepThisTick =
    world_state_.ticks_elapsed % ENEMY_TICKS_PER_CELL == 0;

  std::vector<EnemyId> leaked;
  std::vector<EnemyId> expired;

  for (auto& [id, tracked] : active_enemies_) {
    Enemy& enemy = tracked.enemy;
    enemy.tick();  // Decoys count down their lifetime here
    if (!enemy.isAlive()) {
      expired.push_back(id);
      continue;
    }
    if (!stepThisTick) {
      continue;
    }
    enemy.advance(1);
    if (enemy.getDistanceToBase() <= 0) {
      leaked.push_back(id);
      continue;
    }
    updateRangeMembership(id, tracked);
  }

  for (EnemyId id : expired) {
    removeEnemy(id);  // a Decoy that timed out: no reward, no life lost
  }
  for (EnemyId id : leaked) {
    // A Decoy can't be killed, so letting it cost a life would make
    // every wave lose lives no tower could defend. It only exists to
    // inflate registry sizes: at the base it just leaves the map.
    if (!active_enemies_.at(id).enemy.isInvulnerable()) {
      economy_.registerEnemyEscaped();
    }
    removeEnemy(id);
  }
}

void Simulation::updateRangeMembership(EnemyId id, TrackedEnemy& tracked) {
  int pathIndex = static_cast<int>(route_.length())
    - tracked.enemy.getDistanceToBase();
  Point cell = route_.at(static_cast<std::size_t>(pathIndex));
  const auto& slotCells = towerSlotPositions();

  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    int dx = std::abs(cell.first - slotCells[i].first);
    int dy = std::abs(cell.second - slotCells[i].second);
    bool inside = dx <= TOWER_RANGE_CELLS && dy <= TOWER_RANGE_CELLS;

    if (inside && !tracked.inRange[i]) {
      tracked.inRange[i] = true;
      slots_.enqueueInsert(i, id, id);  // registries are keyed by id
    } else if (!inside && tracked.inRange[i]) {
      tracked.inRange[i] = false;
      slots_.enqueueErase(i, id);
    }
  }
}

void Simulation::removeEnemy(EnemyId id) {
  auto found = active_enemies_.find(id);
  if (found == active_enemies_.end()) {
    return;
  }
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    if (found->second.inRange[i]) {
      slots_.enqueueErase(i, id);
    }
  }
  active_enemies_.erase(found);
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

void Simulation::logWave(int waveNumber) {
  for (int i = 0; i < SlotManager::kSlotCount; ++i) {
    std::optional<CoreType> type = slots_.installedType(i);
    if (!type) {
      continue;  // empty slot: nothing to log
    }

    CombatLogRecord record;
    record.wave = waveNumber;
    record.slot = i;
    record.structure = coreTypeToName(*type);
    record.stats = slots_.takeWaveStats(i);
    record.effectiveShots = effective_shots_[i];
    record.ghostShots = ghost_shots_[i];
    combat_log_.push_back(record);

    effective_shots_[i] = 0;
    ghost_shots_[i] = 0;
  }
}