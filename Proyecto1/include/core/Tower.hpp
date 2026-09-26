// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#pragma once
#include <cstddef>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <queue>
#include <vector>
#include "TargetRegistry.hpp"
#include "Ticks.hpp"  // for STEPS_PER_TICK

/**
 * @brief Kind of pending maintenance operation queued for a tower.
 */
enum class MaintOpType {
  Insert,  ///< An enemy entered the tower's radius.
  Erase    ///< An enemy left the radius or died.
};

/**
 * @brief One queued maintenance operation.
 */
struct MaintenanceOp {
  MaintOpType type;        ///< Insert or erase.
  EnemyId id;              ///< Enemy the operation refers to.
  std::optional<Key> key;  ///< Set only when type == Insert.
};

/**
 * @brief Section 6.1 — everything the combat log needs about one tower
 * during one wave. Filled by Tower::tick() and handed out (and reset)
 * by Tower::takeWaveStats() when the wave ends.
 */
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
  int peakHeight = -1;             ///< Tree height at maxSize; -1 if no tree.
  std::vector<int> peakBuckets;    ///< Chain lengths at maxSize; hash only.
};

/**
 * @brief A slot's tower: one core (ITargetRegistry) plus a FIFO queue of
 * pending maintenance, driven by a per-tick step budget.
 *
 * Sections 2.3 / 3.3 — every tick the tower earns STEPS_PER_TICK steps
 * and spends them on pending maintenance (FIFO), then, if the queue is
 * empty, on at most one shot. Cheap operations let several run in one
 * tick; one that costs more than the budget left drives the credit
 * negative, and the tower stays blocked the ticks needed to pay it off
 * (ceil(cost / STEPS_PER_TICK) in total). Unused budget is not saved.
 */
class Tower {
 public:
  /**
   * @brief Builds a tower around a core.
   * @param registry Structure the tower tracks its enemies in.
   */
  explicit Tower(std::unique_ptr<ITargetRegistry> registry);

  /**
   * @brief Swaps the structure installed in this slot. Operations still
   * queued for the old core are dropped, since they no longer apply.
   * @param registry New structure to install.
   */
  void installRegistry(std::unique_ptr<ITargetRegistry> registry);

  /**
   * @brief Schedules an insert: an enemy entered this tower's radius.
   * @param id Enemy that entered.
   * @param key Key the core indexes it by (== id in this project).
   */
  void enqueueInsert(EnemyId id, Key key);

  /**
   * @brief Schedules an erase: an enemy left the radius or died.
   * @param id Enemy to remove from the core.
   */
  void enqueueErase(EnemyId id);

  /**
   * @brief Runs one tick: adds STEPS_PER_TICK to the credit and, while
   * it is positive, runs pending maintenance, then at most one query
   * once the queue is empty.
   * @param firedTarget Set to the target only when a shot was taken.
   * @param stepsUsedOut If non-null, receives the steps spent this tick
   * (0 if the tower was blocked and did nothing).
   * @return true if the tower fired this tick.
   */
  bool tick(EnemyId& firedTarget, int* stepsUsedOut = nullptr);

  /**
   * @brief Whether the tower is still paying off an operation that cost
   * more than its budget.
   * @return true while the credit is negative.
   */
  bool isBusy() const {
    return credit_ < 0;
  }

  /**
   * @brief Number of enemies currently tracked by the core.
   * @return The core's size().
   */
  std::size_t registrySize() const {
    return registry_->size();
  }

  /**
   * @brief Whether any maintenance is still queued.
   * @return true if the FIFO is not empty.
   */
  bool hasPendingMaintenance() const {
    return !pending_.empty();
  }

  /**
   * @brief Section 5.2 — size of the maintenance FIFO, for the lag bar.
   * @return Number of queued operations.
   */
  int pendingCount() const {
    return static_cast<int>(pending_.size());
  }

  /**
   * @brief Total real time spent inside registry operations since the
   * tower was built. Kept in nanoseconds internally so single fast
   * operations don't truncate to 0 us.
   * @return Elapsed time in microseconds.
   */
  std::uint64_t totalMicroseconds() const {
    return total_nanoseconds_ / kNanosecondsPerMicrosecond;
  }

  /**
   * @brief Section 6.1 — hands out the metrics gathered since the last
   * call (the wave that just ended) and resets them for the next wave.
   * @return The finished wave's metrics.
   */
  TowerWaveStats takeWaveStats();

 private:
  static constexpr std::uint64_t kNanosecondsPerMicrosecond = 1000;

  /**
   * @brief Adds the per-category steps spent between two snapshots of
   * the core's cumulative StepCounter to this wave's breakdown.
   * @param before Snapshot taken before the operation.
   * @param after Counter after the operation.
   */
  void addBreakdownDelta(const StepCounter& before, const StepCounter& after);

  /**
   * @brief Samples registry size and queue length at the end of a tick,
   * and keeps the core's shape when a new size peak is reached.
   */
  void sampleEndOfTick();

  /**
   * @brief Runs one queued insert or erase and records its metrics.
   * @param op Operation taken from the front of the FIFO.
   * @return Steps the operation cost.
   */
  int runMaintenance(const MaintenanceOp& op);

  /**
   * @brief Runs one query (a shot) and records its metrics.
   * @param firedTarget Set to the core's native answer.
   * @return Steps the query cost.
   */
  int runQuery(EnemyId& firedTarget);

  /**
   * @brief Adds one operation's real time and per-category steps to the
   * wave stats.
   * @param before StepCounter snapshot taken before the operation.
   * @param start Clock reading taken before the operation.
   */
  void recordOperation(const StepCounter& before,
    std::chrono::steady_clock::time_point start);

  std::unique_ptr<ITargetRegistry> registry_;  ///< Installed core.
  std::queue<MaintenanceOp> pending_;          ///< Maintenance FIFO.
  int credit_ = 0;  ///< Steps left this tick; negative while in debt.
  std::uint64_t total_nanoseconds_ = 0;  ///< Real time in all operations.
  TowerWaveStats wave_;                  ///< Metrics of the current wave.
};