// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <queue>
#include "TargetRegistry.hpp"
#include "Ticks.hpp"  // for STEPS_PER_TICK

// Kind of pending maintenance operation queued for a tower.
enum class MaintOpType {
  Insert,
  Erase
};

struct MaintenanceOp {
  MaintOpType type;
  EnemyId id;
  std::optional<Key> key;  // set only when type == Insert
};

// Section 6.1 — everything the combat log needs about one tower during
// one wave. Filled by Tower::tick() and handed out (and reset) by
// Tower::takeWaveStats() when the wave ends.
struct TowerWaveStats {
  int ticks = 0;                   ///< Ticks this tower existed in the wave.
  std::size_t maxSize = 0;         ///< Largest registry size seen.
  std::uint64_t sizeSum = 0;       ///< Sum of per-tick sizes (for the mean).
  int maxPending = 0;              ///< Longest maintenance queue seen.
  std::uint64_t pendingSum = 0;    ///< Sum of per-tick queue lengths.
  int emptyQueueTicks = 0;         ///< Ticks that ended with an empty queue.
  int inserts = 0;                 ///< Insert operations executed.
  int erases = 0;                  ///< Erase operations executed.
  int queries = 0;                 ///< Query operations executed (shots).
  std::uint64_t insertSteps = 0;   ///< Steps returned by insert().
  std::uint64_t eraseSteps = 0;    ///< Steps returned by erase().
  std::uint64_t querySteps = 0;    ///< Steps returned by query().
  StepCounter breakdown;           ///< Per-category steps during the wave.
  std::uint64_t realNanoseconds = 0;  ///< Real time spent inside operations.
};

// Within each tick, a FREE tower takes exactly ONE pending maintenance
// operation if any exist; otherwise it fires a query. It computes the
// step cost and becomes blocked for ceil(cost / STEPS_PER_TICK) ticks.
// A busy tower does nothing and just counts down. There is no per-tick
// budget that resets.
class Tower {
 public:
  explicit Tower(std::unique_ptr<ITargetRegistry> registry);

  // Swap the structure installed in this slot. Any operations still
  // queued for the old core are dropped, since they no longer apply
  // to the new one.
  void installRegistry(std::unique_ptr<ITargetRegistry> registry);

  // Called by Simulation when an enemy enters range, to schedule an
  // insert for this tower's core.
  void enqueueInsert(EnemyId id, Key key);

  // Called by Simulation when an enemy leaves range or dies, to
  // schedule an erase for this tower's core.
  void enqueueErase(EnemyId id);

  // Runs one tick for this tower.
  // - If still blocked from a previous operation, decrements the block
  //   counter and does nothing else this tick.
  // - If free, takes one pending maintenance op if any, otherwise
  //   fires via query(). Computes the step cost and blocks for
  //   ceil(cost / STEPS_PER_TICK) ticks.
  // Returns true and sets firedTarget only when a shot was taken THIS
  // tick (a maintenance tick or a blocked tick never fires).
  // stepsUsedOut, if non-null, receives the steps spent this tick (0 if
  // the tower was blocked and did nothing).
  bool tick(EnemyId& firedTarget, int* stepsUsedOut = nullptr);

  bool isBusy() const {
    return busy_ticks_ > 0;
  }

  std::size_t registrySize() const {
    return registry_->size();
  }

  bool hasPendingMaintenance() const {
    return !pending_.empty();
  }

  // Section 5.2 — size of the maintenance FIFO, for the lag bar.
  int pendingCount() const {
    return static_cast<int>(pending_.size());
  }

  // Total real time spent inside registry operations since the tower
  // was built. Accumulated in nanoseconds so single fast operations
  // don't truncate to 0 us.
  std::uint64_t totalMicroseconds() const {
    return total_nanoseconds_ / kNanosecondsPerMicrosecond;
  }

  // Section 6.1 — metrics gathered since the last call (i.e. during the
  // wave that just ended). Resets them so the next wave starts clean.
  TowerWaveStats takeWaveStats();

 private:
  static constexpr std::uint64_t kNanosecondsPerMicrosecond = 1000;

  // Adds the per-category steps spent between two snapshots of the
  // registry's cumulative StepCounter to this wave's breakdown.
  void addBreakdownDelta(const StepCounter& before, const StepCounter& after);

  // Samples registry size and queue length at the end of a tick.
  void sampleEndOfTick();

  std::unique_ptr<ITargetRegistry> registry_;
  std::queue<MaintenanceOp> pending_;
  int busy_ticks_ = 0;
  std::uint64_t total_nanoseconds_ = 0;
  TowerWaveStats wave_;
};