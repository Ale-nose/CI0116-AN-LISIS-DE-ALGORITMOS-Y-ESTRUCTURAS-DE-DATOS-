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
  // Section 2.3 — the tower earns STEPS_PER_TICK steps every tick. A
  // negative credit is debt left by an operation that cost more than
  // the budget it had: the tower stays blocked until it is paid off.
  credit_ += STEPS_PER_TICK;

  int stepsThisTick = 0;
  bool fired = false;

  // Spend the budget: maintenance first (FIFO); once the queue is empty,
  // at most one shot. Cheap operations let several run in one tick.
  while (credit_ > 0) {
    if (!pending_.empty()) {
      MaintenanceOp op = pending_.front();
      pending_.pop();
      int steps = runMaintenance(op);
      credit_ -= steps;
      stepsThisTick += steps;
    } else if (!fired && registry_->size() > 0) {
      // An empty registry has nothing to shoot at, so no shot then.
      int steps = runQuery(firedTarget);
      credit_ -= steps;
      stepsThisTick += steps;
      fired = true;
    } else {
      break;  // nothing left to do this tick
    }
  }

  // Unused budget doesn't carry over; debt does.
  if (credit_ > 0) {
    credit_ = 0;
  }

  if (stepsUsedOut) {
    *stepsUsedOut = stepsThisTick;
  }

  sampleEndOfTick();
  return fired;
}

int Tower::runMaintenance(const MaintenanceOp& op) {
  // The registry's StepCounter is cumulative; snapshot it so only this
  // operation's steps are added to the wave breakdown.
  StepCounter before = registry_->stepBreakdown();
  auto start = std::chrono::steady_clock::now();

  int steps = 0;
  if (op.type == MaintOpType::Insert) {
    steps = registry_->insert(op.id, *op.key);
    ++wave_.inserts;
    wave_.insertSteps += static_cast<std::uint64_t>(steps);
  } else {
    steps = registry_->erase(op.id);
    ++wave_.erases;
    wave_.eraseSteps += static_cast<std::uint64_t>(steps);
  }

  recordOperation(before, start);
  return steps;
}

int Tower::runQuery(EnemyId& firedTarget) {
  StepCounter before = registry_->stepBreakdown();
  auto start = std::chrono::steady_clock::now();

  int steps = registry_->query(firedTarget);
  ++wave_.queries;
  wave_.querySteps += static_cast<std::uint64_t>(steps);

  recordOperation(before, start);
  return steps;
}

void Tower::recordOperation(const StepCounter& before,
    std::chrono::steady_clock::time_point start) {
  auto end = std::chrono::steady_clock::now();
  std::uint64_t elapsedNs = static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
      .count());
  total_nanoseconds_ += elapsedNs;
  wave_.realNanoseconds += elapsedNs;
  addBreakdownDelta(before, registry_->stepBreakdown());
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
    // New peak for this wave: keep the registry's shape at this moment.
    RegistryShape peak = registry_->shape();
    wave_.peakHeight = peak.height;
    wave_.peakBuckets = std::move(peak.bucketLengths);
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