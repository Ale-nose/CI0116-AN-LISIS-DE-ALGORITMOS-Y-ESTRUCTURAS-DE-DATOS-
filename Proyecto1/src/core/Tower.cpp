// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include "Tower.hpp"

#include <chrono>
#include <utility>

Tower::Tower(std::unique_ptr<ITargetRegistry> registry)
  : registry_(std::move(registry)) {
}

void Tower::installRegistry(std::unique_ptr<ITargetRegistry> registry) {
  registry_ = std::move(registry);
  // Drop pending maintenance: it referred to the previous core.
  std::queue<MaintenanceOp> empty;
  std::swap(pending_, empty);
}

void Tower::enqueueInsert(EnemyId id, Key key) {
  pending_.push(MaintenanceOp{MaintOpType::Insert, id, key});
}

void Tower::enqueueErase(EnemyId id) {
  pending_.push(MaintenanceOp{MaintOpType::Erase, id, std::nullopt});
}

bool Tower::tick(EnemyId& firedTarget, int* stepsUsedOut) {
  if (stepsUsedOut) {
    *stepsUsedOut = 0;
  }

  if (busy_ticks_ > 0) {
    --busy_ticks_;
    sampleEndOfTick();
    return false;  // still paying for a previous operation
  }

  int steps = 0;
  bool fired = false;

  // The registry's StepCounter is cumulative; snapshot it so only the
  // steps of this tick's operation are added to the wave breakdown.
  StepCounter before = registry_->stepBreakdown();
  auto start = std::chrono::steady_clock::now();

  if (!pending_.empty()) {
    MaintenanceOp op = pending_.front();
    pending_.pop();
    if (op.type == MaintOpType::Insert) {
      steps = registry_->insert(op.id, *op.key);
      ++wave_.inserts;
      wave_.insertSteps += static_cast<std::uint64_t>(steps);
    } else {
      steps = registry_->erase(op.id);
      ++wave_.erases;
      wave_.eraseSteps += static_cast<std::uint64_t>(steps);
    }
  } else if (registry_->size() > 0) {
    // An empty registry has nothing to shoot at: querying it still
    // costs 0 steps and leaves firedTarget untouched, so don't count
    // it as a shot.
    steps = registry_->query(firedTarget);
    fired = true;
    ++wave_.queries;
    wave_.querySteps += static_cast<std::uint64_t>(steps);
  }

  auto end = std::chrono::steady_clock::now();
  std::uint64_t elapsedNs = static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
      .count());
  total_nanoseconds_ += elapsedNs;
  wave_.realNanoseconds += elapsedNs;
  addBreakdownDelta(before, registry_->stepBreakdown());

  // ceil(steps / STEPS_PER_TICK) without <cmath>.
  busy_ticks_ = (steps + STEPS_PER_TICK - 1) / STEPS_PER_TICK;

  if (stepsUsedOut) {
    *stepsUsedOut = steps;
  }

  sampleEndOfTick();
  return fired;
}

TowerWaveStats Tower::takeWaveStats() {
  TowerWaveStats finished = wave_;
  wave_ = TowerWaveStats{};
  return finished;
}

void Tower::addBreakdownDelta(const StepCounter& before,
    const StepCounter& after) {
  wave_.breakdown.comparisons += after.comparisons - before.comparisons;
  wave_.breakdown.pointerHops += after.pointerHops - before.pointerHops;
  wave_.breakdown.shifts += after.shifts - before.shifts;
  wave_.breakdown.rotations += after.rotations - before.rotations;
}

void Tower::sampleEndOfTick() {
  std::size_t size = registry_->size();
  int pending = pendingCount();

  ++wave_.ticks;
  if (size > wave_.maxSize) {
    wave_.maxSize = size;
  }
  wave_.sizeSum += static_cast<std::uint64_t>(size);
  if (pending > wave_.maxPending) {
    wave_.maxPending = pending;
  }
  wave_.pendingSum += static_cast<std::uint64_t>(pending);
  if (pending == 0) {
    ++wave_.emptyQueueTicks;
  }
}